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
	pos = pos.zero;
	rot = rot.identity;
	scl = scl.one;
	transform = transform.zero;
}

CompTransform::~CompTransform()
{}

void CompTransform::update()
{
	int a = 1;
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
}

CompMesh::~CompMesh()
{}

void CompMesh::update()
{
	RenderMesh();
}

void CompMesh::RenderMesh()
{
	/*glPushMatrix();
	glMultMatrixf(mesh_list[i]->transform.Transposed().ptr());*/

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
	//glPopMatrix();
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