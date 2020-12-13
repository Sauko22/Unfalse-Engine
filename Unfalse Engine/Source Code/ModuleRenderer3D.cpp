#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#include "Glew\include\glew.h"

#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	frameBuffer = 0;
	depthBuffer = 0;
	renderTexture = 0;
	
	showlines = false;

	img_size = { 0,0 };
	win_size = { 0,0 };

	j = 0;

	main_camera = nullptr;
	culling = false;
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
	if (App->camera->scene_camera != nullptr)
	{
		UpdateCameraView();
	}

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrixCamera());

	// light 0 on cam pos
	lights[0].SetPos(5, 5, 5);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

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

void ModuleRenderer3D::UpdateCameraView()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	static float4x4 viewMatrix;

	viewMatrix = App->camera->scene_camera->frustum.ProjectionMatrix();
	viewMatrix.Transpose();

	glLoadMatrixf((GLfloat*)viewMatrix.v);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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
	
	ImGui::SetCursorPos( ImVec2(img_offset.x, img_offset.y));
	img_corner = Vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y) + Vec2(0, img_size.y);
	img_corner.y = App->window->windowSize.y - img_corner.y; 

	ImGui::Image((ImTextureID)App->renderer3D->renderTexture, ImVec2(img_size.x, img_size.y), ImVec2(0, 1), ImVec2(1, 0));

	 WinSize = ImGui::GetWindowSize();
	if (WinSize.x != App->window->windowSize.x || WinSize.y != App->window->windowSize.y)
	{
		WinResize(Vec2(WinSize.x, WinSize.y));
	}

	// Draw any Meshes loaded into scene
	UpdateGameObjects(App->scene_intro->root);

	// Draw Guizmo
	App->scene_intro->EditTransform();

	ImGui::End();
}

void ModuleRenderer3D::UpdateGameObjects(GameObject* gameobject)
{
	if (gameobject != nullptr )
	{
		if (gameobject->Objdelete == true)
		{
			std::string name;
			name.append("GameObject_0");
			if (gameobject->name != name)
			{
				DeleteGameObjects(gameobject);
			}
			else
			{
				_DeleteGameObjects(gameobject);
			}
		}

		if (gameobject->ObjrenderActive == true)
		{
			gameobject->update();

			for (int i = 0; i < gameobject->children_list.size(); i++)
			{
				UpdateGameObjects(gameobject->children_list[i]);
			}
		}
	}
}

void ModuleRenderer3D::_DeleteGameObjects(GameObject* gameobject)
{
	for (int i = 0; i < gameobject->children_list.size(); i++)
	{
		_DeleteGameObjects(gameobject->children_list[i]);
	}
	
	if (gameobject != nullptr)
	{
		if (gameobject->parentGameObject != nullptr)
		{
			for (int i = 0; i < gameobject->parentGameObject->children_list.size(); i++)
			{
				if (gameobject->parentGameObject->children_list[i] == gameobject)
				{
					gameobject->parentGameObject->children_list.erase(gameobject->parentGameObject->children_list.begin() + i);
					delete gameobject;
					gameobject = nullptr;
				}
				break;
			}
		}
	}

	App->scene_intro->SelectedGameObject = nullptr;
}

void ModuleRenderer3D::DeleteGameObjects(GameObject* gameobject)
{
	for (int i = 0; i < gameobject->children_list.size(); i++)
	{
		DeleteGameObjects(gameobject->children_list[i]);
	}

	if (gameobject->parentGameObject != nullptr)
	{
		for (int i = 0; i < gameobject->parentGameObject->children_list.size(); i++)
		{

			if (gameobject->parentGameObject->children_list[i] == gameobject)
			{
				gameobject->parentGameObject->children_list.erase(gameobject->parentGameObject->children_list.begin() + i);
				i--;
			}
		}
	}
	if (gameobject != nullptr)
	{
		if (App->scene_intro->SelectedGameObject = gameobject)
		{
			App->scene_intro->SelectedGameObject = nullptr;
		}

		delete gameobject;
		gameobject = nullptr;
	}
}

void ModuleRenderer3D::DeleteAllGameObjects()
{
	App->scene_intro->SelectedGameObject = nullptr;
	for (int i = 0; i < App->scene_intro->root->children_list.size(); i++)
	{
		std::string scenecamera;
		scenecamera.append(" Scene Camera");
		if (App->scene_intro->root->children_list[i]->name != scenecamera)
		{
			if (App->scene_intro->root->children_list[i] != nullptr)
			{
				for (int j = 0; i < App->scene_intro->root->children_list[i]->children_list.size(); j++)
				{
					if (App->scene_intro->root->children_list[i]->children_list[j] != nullptr)
					{
						delete App->scene_intro->root->children_list[i]->children_list[j];
						App->scene_intro->root->children_list[i]->children_list[j] = nullptr;
						App->scene_intro->root->children_list[i]->children_list.erase(App->scene_intro->root->children_list[i]->children_list.begin() + j);
						j--;
					}
				}
				delete App->scene_intro->root->children_list[i];
				App->scene_intro->root->children_list[i] = nullptr;
				App->scene_intro->root->children_list.erase(App->scene_intro->root->children_list.begin() + i);
				i--;
			}
		}
	}
	App->serialization->gameobject_list.clear();
}

void ModuleRenderer3D::WinResize(Vec2 newSize)
{
	win_size = newSize;

	img_size = App->window->windowSize;

	if (img_size.x > win_size.x - 10.0f)
	{
		img_size /= (img_size.x / (win_size.x - 10.0f));
	}
	if (img_size.y > win_size.y - 10.0f)
	{
		img_size /= (img_size.y / (win_size.y - 10.0f));
	}
	img_offset = Vec2(win_size.x - 5.0f - img_size.x, win_size.y - 5.0f - img_size.y) / 2;
}

bool ModuleRenderer3D::ContainsAaBox_2(AABB aabb)
{
	bool ret = false;

	ret = camera_culling->ContainsAaBox(aabb);

	return ret;
}

// LOCAL SPACE
void ModuleRenderer3D::GenerateAABB(CompMesh* compmesh)
{
	compmesh->bbox.SetNegativeInfinity();
	compmesh->bbox.Enclose((float3*)compmesh->vertex, compmesh->num_vertex);
}

void ModuleRenderer3D::GenerateLines(CompMesh* compmesh)
{
	float3 cube_vertex[8];

	compmesh->bbox.GetCornerPoints(cube_vertex);

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


