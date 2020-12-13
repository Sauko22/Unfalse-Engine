#include "Globals.h"
#include "Application.h"
#include "ModuleFBXLoad.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "MathGeoLib/include/MathGeoLib.h"

ModuleFBXLoad::ModuleFBXLoad(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	k = 0;
	texinlibrary = true;
	onlibrary = false;
	parentid = 0;
}

// Destructor
ModuleFBXLoad::~ModuleFBXLoad()
{}

// Called before render is available
bool ModuleFBXLoad::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}

// Called before quitting
bool ModuleFBXLoad::CleanUp()
{
	LOG("Destroying 3D Renderer");

	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

void ModuleFBXLoad::Import_Model(ResModel* resource, uint fileSize, char* buffer)
{
	const aiScene* scene = aiImportFileFromMemory(buffer, fileSize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (scene != nullptr && scene->HasMeshes())
	{
		uint id = App->resource->GenerateNewUID();
		App->serialization->initjson();
		ImportModel(scene->mRootNode, resource, scene, id);
		k += 1;
		aiReleaseImport(scene);
	}
}

void ModuleFBXLoad::ImportModel(aiNode* node, ResModel* resmodel, const aiScene* scene, uint id)
{
	ResModel* parent = (ResModel*)App->resource->CreateNewResource(Resource::ResType::MODEL);

	parent->parentResource = resmodel;

	if (parent->parentResource != nullptr)
	{
		parent->parentResource->children_list.push_back(parent);
	}

	// Name
	std::string obj = std::to_string(k);

	std::string rootnode = "RootNode";

	if (node->mName.C_Str() == rootnode)
	{
		parent->name.append("GameObject_").append(obj);
		parentid = parent->UID;
	}
	else
	{
		parent->name = node->mName.C_Str();
		parent->parentid = parentid;
	}

	// Transforms
	aiVector3D translation, scaling;
	aiQuaternion rotation;

	float3 posate, scalete;
	Quat rotate;

	node->mTransformation.Decompose(scaling, rotation, translation);

	posate.Set(translation.x, translation.y, translation.z);
	rotate.Set(rotation.x, rotation.y, rotation.z, rotation.w);
	scalete.Set(scaling.x, scaling.y, scaling.z);

	parent->pos.Set(posate.x, posate.y, posate.z);
	parent->rot = rotate;
	parent->scl.Set(scalete.x, scalete.y, scalete.z);

	parent->local_transform = float4x4::FromTRS(parent->pos, parent->rot, parent->scl);

	// Meshes
	// Mesh import
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		// Check if the mesh is already on the library or needs to be saved
		ResMesh* resmesh = nullptr;
		onlibrary = false;

		aiMesh* ourMesh = scene->mMeshes[node->mMeshes[i]];

		std::map<uint, Resource*> resources = App->resource->resources;

		for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); it++)
		{
			if (it->second->Type == Resource::ResType::MESH)
			{
				resmesh = (ResMesh*)it->second;
				if (resmesh->name == node->mName.C_Str())
				{
					// Mesh is already in library
					onlibrary = true;
					LOG("Using mesh %s already loaded on library", resmesh->name.c_str());
					break;
				}
			}
		}

		if (onlibrary == false)
		{
			resmesh = (ResMesh*)App->resource->CreateNewResource(Resource::ResType::MESH);

			resmesh->name = node->mName.C_Str();

			// copy vertices
			resmesh->num_vertex = ourMesh->mNumVertices;
			resmesh->vertex = new float[resmesh->num_vertex * 3];
			memcpy(resmesh->vertex, ourMesh->mVertices, sizeof(float) * resmesh->num_vertex * 3);
			LOG("New mesh with %d vertices", resmesh->num_vertex);

			// copy faces
			if (ourMesh->HasFaces())
			{
				resmesh->num_faces = ourMesh->mNumFaces;
				resmesh->num_index = ourMesh->mNumFaces * 3;
				resmesh->index = new uint[resmesh->num_index]; // assume each face is a triangle
				for (uint i = 0; i < ourMesh->mNumFaces; ++i)
				{
					if (ourMesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&resmesh->index[i * 3], ourMesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
			if (ourMesh->HasNormals())
			{
				resmesh->num_normals = ourMesh->mNumVertices;
				resmesh->normals = new float[resmesh->num_normals * 3];
				memcpy(resmesh->normals, ourMesh->mNormals, sizeof(float) * resmesh->num_normals * 3);
				LOG("New mesh with %d normal", resmesh->num_normals);
				LOG("New mesh with %d idnormal", resmesh->id_normals);
			}
			if (ourMesh->HasTextureCoords(0))
			{
				resmesh->num_tex = ourMesh->mNumVertices;
				resmesh->tex = new float[ourMesh->mNumVertices * 2];

				for (unsigned int i = 0; i < resmesh->num_tex; i++)
				{
					resmesh->tex[i * 2] = ourMesh->mTextureCoords[0][i].x;
					resmesh->tex[i * 2 + 1] = ourMesh->mTextureCoords[0][i].y;

				}
				LOG("New mesh with %d uvs", resmesh->num_tex);
			}

			// Mesh save
			resmesh->SaveResource(resmesh);
		}
		parent->component_list.push_back(resmesh);

		// Textures
		if (ourMesh->HasTextureCoords(0))
		{
			// Material
			if (scene->HasMaterials())
			{
				// Check if the texture is already on the library or needs to be saved
				ResTexture* restexture = nullptr;
				onlibrary = false;

				aiMaterial* texture = nullptr;
				aiString texture_path;

				texture = scene->mMaterials[ourMesh->mMaterialIndex];
				texture->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);

				resources = App->resource->resources;

				for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); it++)
				{
					if (it->second->Type == Resource::ResType::TEXTURE)
					{
						restexture = (ResTexture*)it->second;
						if (restexture->texture_path == texture_path.C_Str())
						{
							// Texture is already in library
							onlibrary = true;
							LOG("Using texture %s already loaded on library", restexture->texname.c_str());
							break;
						}
					}
				}

				if (onlibrary == false)
				{
					// The texture is not in library and needs to be saved
					restexture = (ResTexture*)App->resource->CreateNewResource(Resource::ResType::TEXTURE);

					// Default texture
					restexture->defaultex = App->renderer3D->texchec;
					restexture->deftexname = "Checkers";

					// Texture name
					std::string texname;
					std::string texname_2;
					std::string texname_3;

					App->filesys->SplitFilePath(texture_path.C_Str(), &texname, &texname_2, &texname_3);

					restexture->texname = texname_2;
					restexture->texture_path = texture_path.C_Str();
					restexture->assetsFile.append("Assets/Textures/").append(texname_2).append(".").append(texname_3);

					// Texture Import
					uint filesize = 0;
					char* buffer = nullptr;
					restexture->ImportResource(restexture, filesize, buffer);

					// Texture Save
					filesize = restexture->SaveResource(&buffer);

					if (filesize > 0)
					{
						App->filesys->Save(restexture->libraryFile.c_str(), buffer, filesize);
					}
				}
				parent->component_list.push_back(restexture);
			}
		}
	}

	// Resource Model importer (import and save)
	App->serialization->Import_GameObject(parent, id);

	for (int i = 0; i < node->mNumChildren; i++)
	{
		ImportModel(node->mChildren[i], parent, scene, id);
	}
}

void ModuleFBXLoad::ResImport_Texture(const char* assetspath)
{
	// Check if the texture is already on the library or needs to be saved
	ResTexture* restexture = nullptr;
	bool onlibrary = false;

	// Texture path without extension
	std::string texname;
	std::string texname_2;
	std::string texname_3;

	App->filesys->SplitFilePath(assetspath, &texname, &texname_2, &texname_3);
	std::string _path;
	_path.append(texname).append(texname_2);

	std::map<uint, Resource*> resources = App->resource->resources;
	resources = App->resource->resources;

	for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); it++)
	{
		if (it->second->Type == Resource::ResType::TEXTURE)
		{
			restexture = (ResTexture*)it->second;
			std::string path;
			path.append("Assets/Textures/").append(restexture->texname);
			
			if (path == _path || path.append(" ") == _path)
			{
				// Texture is already in library
				onlibrary = true;
				App->serialization->textuid = restexture->UID;
				LOG("Using texture %s already loaded on library", restexture->texname.c_str());
				break;
			}
		}
	}

	if (onlibrary == false)
	{
		// The texture is not in library and needs to be saved
		restexture = (ResTexture*)App->resource->CreateNewResource(Resource::ResType::TEXTURE);
		App->serialization->textuid = restexture->UID;

		// Default texture
		restexture->defaultex = App->renderer3D->texchec;
		restexture->deftexname = "Checkers";

		// Texture name
		std::string texname;
		std::string texname_2;
		std::string texname_3;

		App->filesys->SplitFilePath(assetspath, &texname, &texname_2, &texname_3);

		restexture->texname = texname_2;
		restexture->texture_path = assetspath;
		restexture->assetsFile.append("Assets/Textures/").append(texname_2).append(".").append(texname_3);

		// Texture Import 
		uint filesize = 0;
		char* buffer = nullptr;
		restexture->ImportResource(restexture, filesize, buffer);

		// Texture Save
		filesize = restexture->SaveResource(&buffer);

		if (filesize > 0)
		{
			App->filesys->Save(restexture->libraryFile.c_str(), buffer, filesize);
		}
	}
}