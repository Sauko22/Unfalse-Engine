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
	mCurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	mCurrentGizmoMode = ImGuizmo::MODE::WORLD;
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
	App->resource->ImportFile(file_path.c_str());

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
	float4x4 objectTransform = dynamic_cast<CompTransform*>(SelectedGameObject->GetComponent(Component::compType::TRANSFORM))->local_transform;
	objectTransform.Transposed();
	ImGuizmo::SetDrawlist();
	cornerPos = Vec2(App->renderer3D->img_corner.x, App->window->windowSize.y - App->renderer3D->img_corner.y - App->renderer3D->img_size.y);
	ImGuizmo::SetRect(App->renderer3D->img_corner.x, cornerPos.y, App->renderer3D->img_size.x, App->renderer3D->img_size.y);

	float tempTransform[16];
	memcpy(tempTransform, objectTransform.ptr(), 16 * sizeof(float));
	ImGuizmo::MODE finalMode = (mCurrentGizmoOperation == ImGuizmo::OPERATION::SCALE ? ImGuizmo::MODE::LOCAL : mCurrentGizmoMode);
	ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), mCurrentGizmoOperation,  finalMode, tempTransform);

	if (ImGuizmo::IsUsing())
	{
		float4x4 newTransform;
		newTransform.Set(tempTransform);
		objectTransform = newTransform.Transposed();
		objectTransform.Transposed();
		dynamic_cast<CompTransform*>(SelectedGameObject->GetComponent(Component::compType::TRANSFORM))->local_transform = objectTransform;

		//dynamic_cast<CompTransform*>(SelectedGameObject->GetComponent(Component::compType::TRANSFORM))->UpdateTrans();
	}
}
