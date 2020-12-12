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

	root = new GameObject(nullptr);

	// Scene Camera
	camera = new GameObject(App->scene_intro->root);
	camera->name.append("Scene Camera");
	camera->AddComponent(Component::compType::TRANSFORM);
	camera->AddComponent(Component::compType::CAMERA);
	App->renderer3D->main_camera = ((CompCamera*)camera->GetComponent(Component::compType::CAMERA));

	// Load street
	std::string file_path = "Assets/Models/Street environment_V03.fbx";
	//std::string file_path = "Assets/Models/BakerHouse.fbx";
	/*char* buffer = nullptr;
	uint fileSize = 0;
	fileSize = App->filesys->Load(file_path.c_str(), &buffer);
	App->fbxload->LoadFBX(buffer, fileSize, root);*/
	std::string path;
	std::string texname;
	std::string texname_2;
	std::string texname_3;

	App->filesys->SplitFilePath(file_path.c_str(), &texname, &texname_2, &texname_3);
	path.append(texname).append(texname_2).append(".meta");

	if (App->filesys->Exists(path.c_str()))
	{
		App->serialization->LoadGameObject(path.c_str());
	}
	else
	{
		LOG("%s doesn't have a meta file", path.c_str());
	}
	
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

GameObject* ModuleSceneIntro::SearchGameObjectID(uint id)
{
	for (int i = 0; i < App->serialization->gameobject_list.size(); i++)
	{
		if (App->serialization->gameobject_list[i]->guid == id)
		{
			return App->serialization->gameobject_list[i];
		}
	}

	return nullptr;
}



