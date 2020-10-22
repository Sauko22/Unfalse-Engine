#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleFBXLoad.h"

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
	mesh = nullptr;

	frameBuffer = 0;
	depthBuffer = 0;
	renderTexture = 0;
	
	showlines = false;

	img_corner = { 0,0 };
	img_size = { 0,0 };;
	cornerPos = { 0,0 };;
	win_size = { 0,0 };;
	img_offset = { 0,0 };;
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
		//Use Vsync
		if (VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

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

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

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
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

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

	//glBindRenderbuffer(GL_RENDERBUFFER, 0);

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
	ImGui::Begin("Test1", NULL);

	ImGui::Image((ImTextureID)App->renderer3D->renderTexture, ImVec2(win_size.x, win_size.y), ImVec2(0, 1), ImVec2(1, 0));

	ImVec2 winSize = ImGui::GetWindowSize();
	if (winSize.x != App->window->windowSize.x || winSize.y != App->window->windowSize.y)
		FitWinScene(Vec2(winSize.x, winSize.y));

	ImGui::SetCursorPos(/*ImGui::GetCursorPos() +*/ ImVec2(img_offset.x, img_offset.y));
	img_corner = Vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y) + Vec2(0, img_size.y);
	img_corner.y = App->window->screen_surface->h - img_corner.y; //ImGui 0y is on top so we need to convert 0y on botton

	// Draw any Meshes loaded into scene
	App->renderer3D->Draw_Mesh();

	// Draw lines on all the normal faces of the mesh
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) showlines = !showlines;
	if (showlines)DrawNormalLines(&showlines);
	


	ImGui::End();
}

void ModuleRenderer3D::FitWinScene(Vec2 newSize)
{
	//Getting window size - some margins - separator (7)
	win_size = newSize;

	//Calculating the image size according to the window size.
	img_size = App->window->windowSize;// -Vec2(0.0f, 25.0f); //Removing the tab area
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


void ModuleRenderer3D::Draw_Mesh()
{
	//glScaled(0.1f, 0.1f, 0.1f);
	//glRotated(-90, 1, 0, 0);
	
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, App->fbxload->impmesh->imgID);
	//Draw Mesh
	glEnableClientState(GL_VERTEX_ARRAY);
	

	glBindBuffer(GL_ARRAY_BUFFER, App->fbxload->impmesh->id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//Normals
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, App->fbxload->impmesh->id_normals);
	glNormalPointer(GL_FLOAT, 0, NULL);


	//Uvs
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, App->fbxload->impmesh->id_tex);
	glTexCoordPointer(2,GL_FLOAT, 0, NULL);


	glBindBuffer(GL_ARRAY_BUFFER, App->fbxload->impmesh->id_normals);
	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, App->fbxload->impmesh->id_index);

	glDrawElements(GL_TRIANGLES, App->fbxload->impmesh->num_index, GL_UNSIGNED_INT, NULL);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void ModuleRenderer3D::Load_Mesh()
{
	// Our mesh
	mesh = App->fbxload->impmesh;

	//Vertex of the mesh
	glGenBuffers(1, (GLuint*)&mesh->id_vertex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_vertex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 3, &mesh->vertex[0], GL_STATIC_DRAW);

	//Normal faces of the mesh
	glGenBuffers(1, (GLuint*)&mesh->id_normals);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_normals * 3, &mesh->normals[0], GL_STATIC_DRAW);

	//Indices of the mesh
	glGenBuffers(1, (GLuint*)&mesh->id_index);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_index);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * mesh->num_index, &mesh->index[0], GL_STATIC_DRAW);

	//Uvs of the mesh
	glGenBuffers(1, (GLuint*)&mesh->id_tex);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_tex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_tex * 2, &mesh->tex[0], GL_STATIC_DRAW);



}



void ModuleRenderer3D::DrawNormalLines(bool* p_open) {

	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);

	for (size_t i = 0; i < App->fbxload->impmesh->num_vertex * 3; i += 3)
	{
		float v_x = App->fbxload->impmesh->vertex[i];
		float v_y = App->fbxload->impmesh->vertex[i + 1];
		float v_z = App->fbxload->impmesh->vertex[i + 2];

		float n_x = App->fbxload->impmesh->normals[i];
		float n_y = App->fbxload->impmesh->normals[i + 1];
		float n_z = App->fbxload->impmesh->normals[i + 2];

		glVertex3f(v_x, v_y, v_z);
		glVertex3f(v_x + n_x, v_y + n_y, v_z + n_z);
	}

	glEnd();









}

