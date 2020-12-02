#include "Component.h"
#include "Application.h"
#include "GameObject.h"
#include "Glew\include\glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "MathGeoLib/include/MathGeoLib.h"

#include "ImGui/imgui.h"

Component::Component(compType type, GameObject*) : type(type), gameObject(gameObject)
{
	normactive = false;
	texactive = false;
	texactive = false;
	deftexactive = false;
	newtexgl = 0;
	gameobject_selected = false;
	renderactive = true;
	texture_h = 0;
	texture_w = 0;
	texname = " ";
}

Component::~Component()
{}

CompTransform::CompTransform(GameObject* gameobject) : Component(compType::TRANSFORM, gameobject)
{
	pos = pos.zero;
	rot = rot.identity;
	scl = scl.one;
	local_transform = local_transform.identity;
	gameobject_selected = false;
	gameObject = gameobject;
	first_it = false;
}

CompTransform::~CompTransform()
{}

void CompTransform::update()
{
	if(first_it == false)
	{
		UpdateTrans();
		first_it = true;
	}
}

void CompTransform::inspector()
{
	if (gameobject_selected == true)
	{
		if (ImGui::CollapsingHeader("Local Transformation", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float position[4] = { pos.x, pos.y, pos.z, 1.0f };
			if (ImGui::DragFloat3("Position", position, 0.1f, -500.0f, 500.0f))
			{
				pos.x = position[0];
				pos.y = position[1];
				pos.z = position[2];
			
				// Update position
				UpdateTrans();
			}

			ImGui::Text("Rotations");

			float3 _rot = rot.ToEulerXYZ();
			_rot *= RADTODEG;
			float anglex = _rot.x;
			if (ImGui::DragFloat("x", &anglex, 0.1f, -1000.0f, 1000.0f))
			{
				float newrot = anglex - _rot.x;
				float3 axis(1, 0, 0);
				Quat _newrot = Quat::RotateAxisAngle(axis, newrot * DEGTORAD);
				rot = rot * _newrot;

				// Update rotation
				UpdateTrans();
			}

			float angley = _rot.y;
			if (ImGui::DragFloat("y", &angley, 0.1f, -1000.0f, 1000.0f))
			{
				float newrot = angley - _rot.y;
				float3 axis(0, -1, 0);
				Quat _newrot = Quat::RotateAxisAngle(axis, newrot * DEGTORAD);
				rot = rot * _newrot;

				// Update rotation
				UpdateTrans();
			}

			float anglez = _rot.z;
			if (ImGui::DragFloat("z", &anglez, 0.1f, -1000.0f, 1000.0f))
			{
				float newrot = anglez - _rot.z;
				float3 axis(0, 0, 1);
				Quat _newrot = Quat::RotateAxisAngle(axis, newrot * DEGTORAD);
				rot = rot * _newrot;

				// Update rotation
				UpdateTrans();
			}

			float scale[4] = { scl.x, scl.y, scl.z, 1.0f };
			if (ImGui::DragFloat3("Scale", scale, 0.1f, 0.0f, 500.0f))
			{
				scl.x = scale[0];
				scl.y = scale[1];
				scl.z = scale[2];

				// Update scale
				UpdateTrans();
			}
		}
	}
}

void CompTransform::UpdateTrans()
{
	if (gameObject->children_list.size() == 0)
	{
		CompTransform* parent_transform = (CompTransform*)gameObject->parentGameObject->GetComponent(Component::compType::TRANSFORM);
		local_transform = float4x4::FromTRS(pos, rot, scl);
		if (parent_transform != nullptr)
		{
			local_transform = parent_transform->local_transform * local_transform;
		}
	}
	else
	{
		local_transform = float4x4::FromTRS(pos, rot, scl);

		for (int i = 0; i < gameObject->children_list.size(); i++)
		{
			for (int j = 0; j < gameObject->children_list[i]->component_list.size(); j++)
			{
				if (gameObject->children_list[i]->component_list[j]->type == Component::compType::TRANSFORM) 
				{
					CompTransform* children_transform = (CompTransform*)gameObject->children_list[i]->component_list[j];
					children_transform->UpdateTrans();
				}
			}
		}
	}
}

CompMesh::CompMesh(GameObject* gameobject) : Component(compType::MESH, gameobject)
{
	normactive = false;
	texactive = false;
	deftexactive = false;
	newtexgl = 0;
	id_index = 0;
	num_index = 0;
	id_vertex = 0;
	num_vertex = 0;
	vertex = nullptr;
	id_normals = 0;
	num_normals = 0;
	normals = nullptr;
	id_tex = 0;
	num_tex = 0;
	tex = nullptr;
	num_faces = 0;
	hastext = false;
	meshactive = true;
	texactive = false;
	deftexactive = false;
	name = " ";
	deftexname = " ";
	gameobject_selected = false;
	gameObject = gameobject;
	mpath = "";
	tpath = "";
}

CompMesh::~CompMesh()
{
	if (index != nullptr)
	{
		delete[] index;
		index = nullptr;
		glDeleteBuffers(1, &id_index);
	}

	if (vertex != nullptr)
	{
		delete[] vertex;
		vertex = nullptr;
		glDeleteBuffers(1, &id_vertex);
	}

	if (normals != nullptr)
	{
		delete[] normals;
		normals = nullptr;
		glDeleteBuffers(1, &id_normals);
	}

	if (tex != nullptr)
	{
		delete[] tex;
		tex = nullptr;
		glDeleteBuffers(1, &id_tex);
	}
}

void CompMesh::update()
{
	if (meshactive == true)
	{
		RenderMesh();
	}
}

void CompMesh::inspector()
{
	if (gameobject_selected == true)
	{
		if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Active Mesh", &meshactive);
			ImGui::Checkbox("ActiveNormals", &normactive);
			ImGui::Text("Index: %i", num_index);
			ImGui::Text("Normals: %i", num_normals);
			ImGui::Text("Vertex: %i", num_vertex);
			ImGui::Text("Faces: %i", num_faces);
			ImGui::Text("Text coords: %i", num_tex);
			ImGui::Text("Gameobject: %s", name.c_str());
		}

		if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("ActiveMat", &texactive);
			ImGui::Text("%s", texname.c_str());

			ImGui::Text("Width: %i", texture_w); ImGui::SameLine();
			ImGui::Text("Height: %i", texture_h);

			ImGui::Image((ImTextureID)textgl, ImVec2(128, 128));
		}
		if (ImGui::CollapsingHeader("Default Text", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Defaultext", &deftexactive);
			ImGui::Text("%s", deftexname.c_str());

			ImGui::Image((ImTextureID)defaultex, ImVec2(128, 128));
		}
	}
}

void CompMesh::RenderMesh()
{
	CompTransform* transform = nullptr;

	for (int i = 0; i < gameObject->component_list.size(); i++)
	{

		if (gameObject->component_list[i]->type == Component::compType::TRANSFORM)
		{
			transform = (CompTransform*)gameObject->component_list[i];
		}
	}
	
	glPushMatrix();
	glMultMatrixf(transform->local_transform.Transposed().ptr());

	// Draw textures
	if (newtexgl != 0)
	{
		textgl = newtexgl;
		hastext = true;
	}
	if (hastext == true)
	{
		glEnable(GL_TEXTURE_2D);
		/*glEnable(GL_CULL_FACE);
		glActiveTexture(GL_TEXTURE0);*/

		if (texactive == true)
		{
			if (deftexactive == true)
			{
				glBindTexture(GL_TEXTURE_2D, defaultex);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, textgl);
			}
		}
		else
		{
			if (deftexactive == true)
			{
				glBindTexture(GL_TEXTURE_2D, defaultex);
			}
		}
	}
	if (hastext == false && deftexactive == true)
	{
		glBindTexture(GL_TEXTURE_2D, defaultex);
	}

	//Draw Mesh
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//Normals
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, id_normals);
	glNormalPointer(GL_FLOAT, 0, NULL);

	//Uvs
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, id_tex);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, id_normals);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);

	glDrawElements(GL_TRIANGLES, num_index, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (hastext == true)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}
	else if (hastext == false && deftexactive == true)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	// Show normals
	if (normactive == true)
	{
		glBegin(GL_LINES);
		//glColor3f(1.0f, 0.0f, 0.0f);

		for (size_t k = 0; k < num_vertex * 3; k += 3)
		{
			GLfloat v_x = vertex[k];
			GLfloat v_y = vertex[k + 1];
			GLfloat v_z = vertex[k + 2];

			GLfloat n_x = normals[k];
			GLfloat n_y = normals[k + 1];
			GLfloat n_z = normals[k + 2];

			glVertex3f(v_x, v_y, v_z);
			glVertex3f(v_x + n_x, v_y + n_y, v_z + n_z);
		}
		glEnd();
	}
	glPopMatrix();
}

AABB CompMesh::GetAABB()
{
	return bbox;
}

CompMaterial::CompMaterial(GameObject* gameobject) : Component(compType::MATERIAL, gameobject)
{

}

CompMaterial::~CompMaterial()
{}

void CompMaterial::update()
{
	RenderTexture();
}

void CompMaterial::inspector()
{
}

void CompMaterial::RenderTexture()
{}

CompCamera::CompCamera(GameObject* gameobject) : Component(compType::CAMERA, gameobject)
{
	gameObject = gameobject;

	rot = rot.zero;
	rotation = rotation.identity;
	aspectRatio = 0;
	initpos = initpos.zero;
	initrot = initrot.zero;
	horizontalFOV = 0;

	init();
}

CompCamera::~CompCamera()
{}

void CompCamera::init()
{
	frustum.type = FrustumType::PerspectiveFrustum;

	aspectRatio = 1.77777777f;
	frustum.nearPlaneDistance = 4;
	frustum.farPlaneDistance = 500;

	float3 cpose;
	cpose.Set(0.f, 0.f, 0.f);
	frustum.pos = cpose;

	rot = initrot;
	rot *= DEGTORAD;
	rotation = float4x4::FromEulerXYZ(rot.x, rot.y, rot.z);
	frustum.SetWorldMatrix(rotation.Float3x4Part());

	horizontalFOV = 65 * DEGTORAD;
	frustum.horizontalFov = horizontalFOV;
	frustum.verticalFov = horizontalFOV / aspectRatio;
}

void CompCamera::update()
{
	UpdateTransform();

	if (gameobject_selected == true)
	{
		Render();
	}
}

void CompCamera::UpdateTransform()
{
	// Update camera position
	CompTransform* transform = (CompTransform*)gameObject->GetComponent(Component::compType::TRANSFORM);

	frustum.pos = transform->pos;

	frustum.up = transform->local_transform.WorldY();
	frustum.front = transform->local_transform.WorldZ();
}

void CompCamera::inspector()
{
	if (gameobject_selected == true)
	{
		if (ImGui::CollapsingHeader("Camera Properties", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static bool culling = false;
			ImGui::Checkbox("Culling", &culling);
			if (culling == true)
			{
				App->renderer3D->culling = true;
			}
			else
			{
				App->renderer3D->culling = false;
			}

			ImGui::Text("Planes");

			float nearplane[1] = { frustum.nearPlaneDistance };
			if (ImGui::DragFloat("Near", nearplane, 0.1f, 0.f, 2000.0f))
			{
				frustum.nearPlaneDistance = nearplane[0];
			}
			float farplane[1] = { frustum.farPlaneDistance };
			if (ImGui::DragFloat("Far", farplane, 0.1f, 0.f, 2000.0f))
			{
				frustum.farPlaneDistance = farplane[0];
			}

			ImGui::Text("FOV");
			float FOV[1] = { frustum.verticalFov };
			if (ImGui::DragFloat(" ", FOV, 0.01f, 0.f, 2000.0f))
			{
				frustum.verticalFov = FOV[0];
				frustum.horizontalFov = 2 * Atan(tan((frustum.verticalFov / 2)) * aspectRatio);
			}
			ImGui::Text("Horizontal FOV: %f", frustum.horizontalFov * RADTODEG);
			ImGui::Text("Vertical FOV: %f", frustum.verticalFov * RADTODEG);
			ImGui::Text("Aspect ratio: %f", aspectRatio);
		}
	}
}

void CompCamera::Render()
{
	float3 cube_vertex[8];

	frustum.GetCornerPoints(cube_vertex);
		
	glBegin(GL_LINES);
		
	// Base
	glVertex3fv(cube_vertex[0].ptr());
	glVertex3fv(cube_vertex[1].ptr());

	glVertex3fv(cube_vertex[0].ptr());
	glVertex3fv(cube_vertex[4].ptr());

	glVertex3fv(cube_vertex[4].ptr());
	glVertex3fv(cube_vertex[5].ptr());

	glVertex3fv(cube_vertex[5].ptr());
	glVertex3fv(cube_vertex[1].ptr());

	// Pilars
	glVertex3fv(cube_vertex[0].ptr());
	glVertex3fv(cube_vertex[2].ptr());

	glVertex3fv(cube_vertex[4].ptr());
	glVertex3fv(cube_vertex[6].ptr());

	glVertex3fv(cube_vertex[5].ptr());
	glVertex3fv(cube_vertex[7].ptr());

	glVertex3fv(cube_vertex[1].ptr());
	glVertex3fv(cube_vertex[3].ptr());

	// Top
	glVertex3fv(cube_vertex[2].ptr());
	glVertex3fv(cube_vertex[6].ptr());

	glVertex3fv(cube_vertex[2].ptr());
	glVertex3fv(cube_vertex[3].ptr());

	glVertex3fv(cube_vertex[6].ptr());
	glVertex3fv(cube_vertex[7].ptr());

	glVertex3fv(cube_vertex[3].ptr());
	glVertex3fv(cube_vertex[7].ptr());

	glEnd();
}

// tests if a AaBox is within the frustrum
bool CompCamera::ContainsAaBox(const AABB& refBox) const
{
	float3 vCorner[8];
	Plane* m_plane = new Plane [6];
	int iTotalIn = 0;
	refBox.GetCornerPoints(vCorner); // get the corners of the box into the vCorner array
	frustum.GetPlanes(m_plane);

	// test all 8 corners against the 6 sides
	// if all points are behind 1 specific plane, we are out
	// if we are in with all points, then we are fully in
	for (int p = 0; p < 6; ++p) 
	{
		int iInCount = 8;
		int iPtIn = 1;

		for (int i = 0; i < 8; ++i) 
		{
			// test this point against the planes
			if (m_plane[p].IsOnPositiveSide(vCorner[i]) == true) //<-- “IsOnPositiveSide” from MathGeoLib
			{ 
				iPtIn = 0;
				--iInCount;
			}
		}
	
		// were all the points outside of plane p?
		if (iInCount == 0)
		{
			delete m_plane;
			m_plane = nullptr;
			return false;
		}
			
		// check if they were all on the right side of the plane
		iTotalIn += iPtIn;
	}

	// so if iTotalIn is 6, then all are inside the view
	if (iTotalIn == 6)
	{
		delete m_plane;
		m_plane = nullptr;
		return true;
	}

	// we must be partly in then otherwise
	delete m_plane;
	m_plane = nullptr;
	return true;
}