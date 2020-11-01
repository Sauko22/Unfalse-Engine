#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleFBXLoad.h"
#include "ModuleWindow.h"
#include "Component.h"
#include "ModuleGameObject.h"
#include "p2Defs.h"

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

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	frameBuffer = 0;
	depthBuffer = 0;
	renderTexture = 0;
	
	showlines = false;

	img_size = { 0,0 };
	win_size = { 0,0 };

	j = 0;
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	if (ret == true)
	{
		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
	}

	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texchec);
	glBindTexture(GL_TEXTURE_2D, texchec);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(5, 5, 5);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		App->fbxload->ResizeFBX = !App->fbxload->ResizeFBX;
		LOG("RESIZED");
	}

	if (App->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
	{
		j = 1;
	}
	
	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate()
{
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	return true;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GenerateSceneBuffers();
}

void ModuleRenderer3D::Draw_Axis()
{
	glPushMatrix();
	glTranslatef(0, 0, 0);
	glScalef(1.5, 1.5, 1.5);
	glLineWidth(3.0);

	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3fv(init);
	glVertex3fv(X);
	glColor3f(0, 1, 0);
	glVertex3fv(init);
	glVertex3fv(Y);
	glColor3f(0, 0, 1);
	glVertex3fv(init);
	glVertex3fv(Z);
	glEnd();

	glColor3f(1.f, 1.f, 1.f);

	glPopMatrix();
}

void ModuleRenderer3D::GenerateSceneBuffers()
{
	//Generating buffers for scene render
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	//Generating texture to render to
	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, App->window->screen_surface->w, App->window->screen_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Generating the depth buffer
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, App->window->screen_surface->w, App->window->screen_surface->h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	

	//Configuring frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG("Error creating screen buffer");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleRenderer3D::Draw()
{
	// Window 1
	ImGui::Begin("Scene", NULL);
	
	ImGui::Image((ImTextureID)App->renderer3D->renderTexture, ImVec2(img_size.x, img_size.y), ImVec2(0, 1), ImVec2(1, 0));

	ImVec2 WinSize = ImGui::GetWindowSize();

	if (WinSize.x != App->window->windowSize.x || WinSize.y != App->window->windowSize.y)
	{
		WinResize(Vec2(WinSize.x, WinSize.y));
	}

	// Draw any Meshes loaded into scene
	for (int i = 0; i < App->gameobject->emptygameobject_list.size(); i++)
	{
		if (App->gameobject->emptygameobject_list[i]->emptyrenderActive == true)
		{
			for (int j = 0; j < App->gameobject->emptygameobject_list[i]->gameobject_list.size(); j++)
			{
				// Render active
				if (App->gameobject->emptygameobject_list[i]->gameobject_list[j]->ObjrenderActive == true)
				{
					// Normals active
					if (App->gameobject->emptygameobject_list[i]->gameobject_list[j]->ObjnormActive == true)
					{
						for (int k = 0; k < App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list.size(); k++)
						{
							App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list[k]->normactive = true;
						}
					}
					else
					{
						for (int k = 0; k < App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list.size(); k++)
						{
							App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list[k]->normactive = false;
						}
					}

					// Texture active
					if (App->gameobject->emptygameobject_list[i]->gameobject_list[j]->ObjtexActive == true)
					{
						if (App->gameobject->emptygameobject_list[i]->gameobject_list[j]->ObjdefauTex == true)
						{
							for (int k = 0; k < App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list.size(); k++)
							{
								App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list[k]->deftexactive = true;
							}
						}
						else
						{
							for (int k = 0; k < App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list.size(); k++)
							{
								App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list[k]->deftexactive = false;
							}
							for (int k = 0; k < App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list.size(); k++)
							{
								App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list[k]->texactive = true;
							}
						}
					}
					else
					{
						if (App->gameobject->emptygameobject_list[i]->gameobject_list[j]->ObjdefauTex == true)
						{
							for (int k = 0; k < App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list.size(); k++)
							{
								App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list[k]->deftexactive = true;
							}
						}
						else
						{
							for (int k = 0; k < App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list.size(); k++)
							{
								App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list[k]->deftexactive = false;
							}
						}
						for (int k = 0; k < App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list.size(); k++)
						{
							App->gameobject->emptygameobject_list[i]->gameobject_list[j]->component_list[k]->texactive = false;
						}
					}

					App->gameobject->emptygameobject_list[i]->gameobject_list[j]->update();
				}
			}
		}
		
	}
	
	
	
	ImGui::End();
}

void ModuleRenderer3D::WinResize(Vec2 newSize)
{
	win_size = newSize;

	img_size = App->window->windowSize;

	if (img_size.x > win_size.x)
	{
		img_size /= (img_size.x / (win_size.x));
	}
	if (img_size.y > win_size.y)
	{
		img_size /= (img_size.y / (win_size.y));
	}
}


