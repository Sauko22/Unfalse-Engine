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
	savescene = false;
	sceneuid = 0;
	mCurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	mCurrentGizmoMode = ImGuizmo::MODE::WORLD;

	ImGuizmo::Enable(true);
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
	//std::string file_path = "Assets/Models/Megaman.fbx";
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
	if (ImGuizmo::IsUsing() == false)
		HandleInput();
	
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

void ModuleSceneIntro::SaveScene()
{
	savescene = true;

	App->serialization->initjson();
	for (int i = 0; i < root->children_list.size(); i++)
	{
		std::string name;
		name.append(" Scene Camera");
		if (root->children_list[i]->name != name)
		{
			sceneuid = App->resource->GenerateNewUID();
			App->serialization->parentuid = sceneuid;
			App->serialization->SaveSceneGameObject(root->children_list[i], sceneuid);

			LOG("Saving GameObject %s", root->children_list[i]->name.c_str());
		}
	}
	savescene = false;
}

void ModuleSceneIntro::LoadScene()
{
	App->UI->showSceneLoad = true;
}

void ModuleSceneIntro::HandleInput()
{
	if ((App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN))
		mCurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

	else if ((App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN))
		mCurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;

	else if ((App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN))
		mCurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
}

void ModuleSceneIntro::EditTransform()
{
	if (SelectedGameObject == nullptr)
		return;

	float4x4 viewMatrix = App->camera->GetViewMatrixM().Transposed();
	float4x4 projectionMatrix = App->camera->GetProjectionMatrixM().Transposed();
	float4x4 marcburru = dynamic_cast<CompTransform*>(SelectedGameObject->GetComponent(Component::compType::TRANSFORM))->global_transform.Transposed();
	//objectTransform.Transposed();
	ImGuizmo::SetDrawlist();
	cornerPos = Vec2(App->renderer3D->img_corner.x, App->window->windowSize.y - App->renderer3D->img_corner.y - App->renderer3D->img_size.y);
	ImGuizmo::SetRect(App->renderer3D->img_corner.x, cornerPos.y, App->renderer3D->img_size.x, App->renderer3D->img_size.y);

	//CompTransform* ty = dynamic_cast<CompTransform*>(SelectedGameObject->GetComponent(Component::compType::TRANSFORM));

	float4x4 modelPtr;
	modelPtr = marcburru;

	//tempTransform.Transposed();

	ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), mCurrentGizmoOperation, mCurrentGizmoMode, marcburru.ptr());

	if (ImGuizmo::IsUsing())
	{
		CompTransform* transform;
		transform = dynamic_cast<CompTransform*>(SelectedGameObject->GetComponent(Component::compType::TRANSFORM));
		marcburru.Transpose();

		transform->global_transform = marcburru;

		CompTransform* parent_transform = (CompTransform*)SelectedGameObject->parentGameObject->GetComponent(Component::compType::TRANSFORM);
		if (parent_transform != nullptr)
		{
			transform->local_transform = dynamic_cast<CompTransform*>(transform->gameObject->parentGameObject->GetComponent(Component::compType::TRANSFORM))->global_transform * transform->global_transform;
		}

		transform->local_transform.Decompose(transform->pos, transform->rot, transform->scl);
		transform->UpdateTrans();
	}
}