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
	local_transform = local_transform.zero;
	gameobject_selected = false;
	gameObject = gameobject;
}

CompTransform::~CompTransform()
{}

void CompTransform::update()
{
	
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

			float angle[4] = { rot.x, rot.y, rot.z };
			if (ImGui::DragFloat3("Degrees", angle, 0.1f, -180.0f, 180.0f))
			{
				rot.x = angle[0];
				rot.y = angle[1];
				rot.z = angle[2];

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
		local_transform = parent_transform->local_transform * local_transform;

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