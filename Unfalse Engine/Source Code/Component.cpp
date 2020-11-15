#include "Component.h"

#include "Glew\include\glew.h"
#pragma comment (lib, "Glew/libx86/glew32.lib") /* link Microsoft OpenGL lib   */

#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )

#include "MathGeoLib/include/MathGeoLib.h"

Component::Component(compType type, GameObject*) : type(type), gameObject(gameObject)
{
	normactive = false;
	texactive = false;
	texactive = false;
	deftexactive = false;
	newtexgl = 0;
}

Component::~Component()
{}

CompTransform::CompTransform(GameObject* gameobject) : Component(compType::TRANSFORM, gameobject)
{
	//newtransform = nullptr;
}

CompTransform::~CompTransform()
{}

void CompTransform::update()
{
	
}

CompMesh::CompMesh(GameObject* gameobject) : Component(compType::MESH, gameobject)
{
	newmesh = nullptr;
	normactive = false;
	texactive = false;
	deftexactive = false;
	newtexgl = 0;
}

CompMesh::~CompMesh()
{}

void CompMesh::update()
{
	RenderMesh();
}

void CompMesh::RenderMesh()
{
	for (int i = 0; i < mesh_list.size(); i++)
	{
		glPushMatrix();
		glMultMatrixf(mesh_list[i]->transform.Transposed().ptr());

		if (newtexgl != 0)
		{
			mesh_list[i]->textgl = newtexgl;
			mesh_list[i]->hastext = true;
		}
		if (mesh_list[i]->hastext == true)
		{
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_CULL_FACE);
			glActiveTexture(GL_TEXTURE0);

			if (texactive == true)
			{
				if (deftexactive == true)
				{
					glBindTexture(GL_TEXTURE_2D, mesh_list[i]->defaultex);
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, mesh_list[i]->textgl);
				}	
			}
			else
			{
				if (deftexactive == true)
				{
					glBindTexture(GL_TEXTURE_2D, mesh_list[i]->defaultex);
				}
			}
		}
		if (mesh_list[i]->hastext == false && deftexactive == true)
		{
			glBindTexture(GL_TEXTURE_2D, mesh_list[i]->defaultex);
		}
		//Draw Mesh
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_list[i]->id_vertex);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		//Normals
		glEnableClientState(GL_NORMAL_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_list[i]->id_normals);
		glNormalPointer(GL_FLOAT, 0, NULL);

		//Uvs
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_list[i]->id_tex);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, mesh_list[i]->id_normals);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_list[i]->id_index);

		glDrawElements(GL_TRIANGLES, mesh_list[i]->num_index, GL_UNSIGNED_INT, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		if (mesh_list[i]->hastext == true)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
		}
		else if (mesh_list[i]->hastext == false && deftexactive == true)
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

			for (size_t k = 0; k < mesh_list[i]->num_vertex * 3; k += 3)
			{
				GLfloat v_x = mesh_list[i]->vertex[k];
				GLfloat v_y = mesh_list[i]->vertex[k + 1];
				GLfloat v_z = mesh_list[i]->vertex[k + 2];

				GLfloat n_x = mesh_list[i]->normals[k];
				GLfloat n_y = mesh_list[i]->normals[k + 1];
				GLfloat n_z = mesh_list[i]->normals[k + 2];

				glVertex3f(v_x, v_y, v_z);
				glVertex3f(v_x + n_x, v_y + n_y, v_z + n_z);
			}
			glEnd();
		}
		glPopMatrix();
	}
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

void CompMaterial::RenderTexture()
{}