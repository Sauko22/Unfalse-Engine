#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "GameObject.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	fps_lasttime = SDL_GetTicks();
	fps_current = 0;
	fps_frames = 0;
	FRAMES_PER_SECOND = 120;
	frame = 0;
	root = nullptr;
	SelectedGameObject = nullptr;
	camera = nullptr;
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

	root = new GameObject(nullptr);

	// Scene Camera
	camera = new GameObject(App->scene_intro->root);
	camera->name.append("Scene Camera");
	camera->AddComponent(Component::compType::TRANSFORM);
	camera->AddComponent(Component::compType::CAMERA);
	App->renderer3D->main_camera = ((CompCamera*)camera->GetComponent(Component::compType::CAMERA));

	// Load street
	std::string file_path = "Assets/Models/Street environment_V02.fbx";
	char* buffer = nullptr;
	uint fileSize = 0;
	fileSize = App->filesys->Load(file_path.c_str(), &buffer);
	App->fbxload->LoadFBX(buffer, fileSize, root);

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

void ModuleSceneIntro::GetAllGameObjects()
{
	// Clear list with every click (gameobjects can be added or deleted)
	gameobject_list.clear();

	AllGameObjects(App->scene_intro->root, gameobject_list);
}

void ModuleSceneIntro::AllGameObjects(GameObject* gameObject, std::vector<GameObject*>& gameObjects)
{
	if (gameObject->name != "Plane001" && gameObject->name != "Line002")
	{
		gameObjects.push_back(gameObject);


		for (int i = 0; i < gameObject->children_list.size(); i++)
		{
			AllGameObjects(gameObject->children_list[i], gameObjects);
		}
	}
}



