#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	X = 0.1;
	Y = 0.1;
	Z = 1;

	Position = float3(60.0f, 60.0f, 60.0f);
	Reference = float3(0.0f, 0.0f, 0.0f);

	mousepos = mousepos.zero;

	camera_speed = 0;
	lalt = false;
	orbit = false;

	scene_camera = nullptr;
	_scene_camera = nullptr;
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;
	
	scene_camera = new CompCamera(nullptr);

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::PreUpdate()
{
	_scene_camera = (CompCamera*)App->scene_intro->camera->GetComponent(Component::compType::CAMERA);
	_scene_transform = (CompTransform*)App->scene_intro->camera->GetComponent(Component::compType::TRANSFORM);

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
	{
		camera_speed = 4;
	}
	else
	{
		camera_speed = 2;
	}

	// Camera zoom
	if (App->input->mouse_z > 0) 
	{
		_scene_transform->pos.z += Z * camera_speed;
		_scene_camera->frustum.pos.z = _scene_transform->pos.z;
		Reference.z += Z * 4;
	}
	if (App->input->mouse_z < 0)
	{
		_scene_transform->pos.z -= Z * camera_speed;
		_scene_camera->frustum.pos.z = _scene_transform->pos.z;
		Reference.z -= Z * 4;
	}

	// Camera 2D movement
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		_scene_transform->pos.x += X * camera_speed;
		_scene_camera->frustum.pos.x = _scene_transform->pos.x;
		Reference.x += X * camera_speed;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		_scene_transform->pos.x -= X * camera_speed;
		_scene_camera->frustum.pos.x = _scene_transform->pos.x;
		Reference.x -= X * camera_speed;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		_scene_transform->pos.y += Y * camera_speed;
		_scene_camera->frustum.pos.y = _scene_transform->pos.y;
		Reference.y += Y * camera_speed;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		_scene_transform->pos.y -= Y * camera_speed;
		_scene_camera->frustum.pos.y = _scene_transform->pos.y;
		Reference.y -= Y * camera_speed;
	}
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		LookAt(App->UI->cameradirection);
	}

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN) lalt = true;
	else if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_UP)
	{
		lalt = false;
		orbit = false;
	}
	if (lalt== true && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		if (orbit == false) 
		{
			LookAt(App->UI->cameradirection);
			orbit = true;
		}
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		Position -= Reference;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			/*X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));*/
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			/*Y = rotate(Y, DeltaY, X);
			Z = rotate(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
			}*/
		}

		//Position = Reference + Z * length(Position);
	}


	// Mouse motion ----------------
	if(lalt == false && App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		// TODO (Homework): Rotate the camera with the mouse
		vec3 Forward = -Z;

		Forward = rotate(Forward, dx, Y);
		Forward = rotate(Forward, dy, X);

		//LookAt(Forward + Position);
	}

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		mousepos.x = App->input->mouse_x;
		mousepos.y = App->input->mouse_y;
		
		mousepos.Normalize();
		//LOG("Mouse pos: %f, %f ", mousepos.x, mousepos.y);

		//LineSegment picking = dummy->frustum.UnProjectLineSegment(normalized_x, normalized_y);
	}

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	/*this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();*/
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{

	/*float3 difference = spot - _frustum.pos;

	float3x3 matrix = float3x3::LookAt(_frustum.front, difference.Normalized(), _frustum.up, float3::unitY);

	_frustum.front = matrix.MulDir(_frustum.front).Normalized();
	_frustum.up = matrix.MulDir(_frustum.up).Normalized();*/
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	/*Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();*/
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrixCamera()
{
	scene_camera = _scene_camera;
	scene_transform = _scene_transform;

	float4x4 viewMatrix;

	viewMatrix = scene_camera->frustum.ViewMatrix();
	viewMatrix.Transpose();

	return (float*)viewMatrix.v;
}
