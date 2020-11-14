#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "ModuleFileSystem.h"


ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	fps_lasttime = SDL_GetTicks();
	fps_current = 0;
	fps_frames = 0;
	FRAMES_PER_SECOND = 120;
	frame = 0;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	// TODO 2: Place the camera one unit up in Y and one unit to the right
	// experiment with different camera placements, then use LookAt()
	// to make it look at the center
	App->camera->Move(vec3(-3, 2, 1));
	App->camera->LookAt(vec3(0.f, 0.f, 0.f));

	

	std::string file_path = "Assets/Models/street2.fbx";
	//std::string file_path = "Assets/Models/BakerHouse.fbx";
	//std::string file_path = "Assets/Models/Blitzcrank.fbx";
	char* buffer = nullptr;
	uint fileSize = 0;
	fileSize = App->filesys->Load(file_path.c_str(), &buffer);
	App->fbxload->Import(buffer, fileSize);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	fps.Start();
	frame++;
	// Cap fps
	if ((fps.Read() < 1000 / FRAMES_PER_SECOND))
	{
		SDL_Delay((1000 / FRAMES_PER_SECOND) - fps.Read());
	}

	// Read fps
	fps_frames++;
	if (fps_lasttime < SDL_GetTicks() - FPS_INTERVAL * 1000)
	{
		fps_lasttime = SDL_GetTicks();
		fps_current = fps_frames;
		fps_frames = 0;
	}

	// Create initial plane
	Plane_Primitive plane;
	plane.Render();

	// Create XYZ Axis
	App->renderer3D->Draw_Axis();

	return UPDATE_CONTINUE;
}

