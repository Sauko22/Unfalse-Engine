#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "Glew\include\glew.h"

#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	X = 0.1;
	Y = 0.1;
	Z = 1;

	Position = float3(60.0f, 60.0f, 60.0f);
	Reference = float3(0.0f, 0.0f, 0.0f);

	camera_speed = 0;
	lalt = false;
	orbit = false;

	scene_camera = nullptr;
	_scene_camera = nullptr;

	origin = origin.zero;
	dest = dest.zero;
	offsetx = 0.091;
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
		camera_speed = 2;
	}
	else
	{
		camera_speed = 0.5;
	}

	// Camera zoom
	if (App->input->mouse_z > 0) 
	{
		_scene_transform->pos += _scene_camera->frustum.front * camera_speed;
	}
	if (App->input->mouse_z < 0)
	{
		_scene_transform->pos -= _scene_camera->frustum.front * camera_speed;
	}

	// Camera 2D movement
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) 
	{
		_scene_transform->pos -= _scene_camera->frustum.WorldRight() * camera_speed;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
	{
		_scene_transform->pos += _scene_camera->frustum.WorldRight() * camera_speed;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		_scene_transform->pos += _scene_camera->frustum.front * camera_speed;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		_scene_transform->pos -= _scene_camera->frustum.front * camera_speed;
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

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		float normalized_x = (App->renderer3D->win_size.x - App->input->GetMouseX()) / App->renderer3D->img_size.x;
		float normalized_y = (App->renderer3D->win_size.y - App->input->GetMouseY()) / App->renderer3D->img_size.y;

		normalized_x = -(normalized_x - 0.5) / 0.5;
		normalized_y = (normalized_y - 0.5) / 0.5;

		normalized_x -=  App->renderer3D->img_size.x / App->renderer3D->win_size.x;
		normalized_x += offsetx;

		LineSegment picking = _scene_camera->frustum.UnProjectLineSegment(normalized_x, normalized_y);

		origin = picking.a;
		dest = picking.b;
	}

	// Draw pickup line
	glBegin(GL_LINES);
	glVertex3fv(origin.ptr());
	glVertex3fv(dest.ptr());
	glEnd();

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
