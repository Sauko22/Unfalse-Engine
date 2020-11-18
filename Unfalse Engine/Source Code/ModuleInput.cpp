#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleFBXLoad.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneIntro.h"

#include "Glew/include/glew.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);

	mouse_x = 0;
	mouse_y = 0;
	mouse_z = 0;
	mouse_x_motion = 0;
	mouse_y_motion = 0;
	dropped_filedir = nullptr;
	texture_dropped = false;
	texture_obj_dropped = false;

	name = "";
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	SDL_EventState(SDL_DROPFILE,SDL_ENABLE);
	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, App->renderer3D->frameBuffer);

	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		ImGui_ImplSDL2_ProcessEvent(&e);
		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;

			case SDL_MOUSEMOTION:
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;

			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			break;

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_DROPFILE:
			{
				dropped_filedir = e.drop.file;
				std::string Dir(e.drop.file);
				LOG("FILE DROPPED: %s", Dir.c_str());

				// Get Object name
				std::string fileDir = "";
				std::string extDir = "";
				std::string filetype = "";
				App->filesys->SplitFilePath(Dir.c_str(), &fileDir, &extDir, &filetype);
				LOG("OBJECT NAME: %s", extDir.c_str());
				name = extDir;

				char* buffer = nullptr;
				uint fileSize = 0;

				std::size_t assets = Dir.find("Assets");
				std::string load_directory = Dir.substr(assets);
				std::string norm_load_directory = App->filesys->NormalizePath(load_directory.c_str());
				LOG("FILE DIRECTORY %s", norm_load_directory.c_str());

				fileSize = App->filesys->Load(norm_load_directory.c_str(), &buffer);

				if (norm_load_directory.substr(norm_load_directory.find(".")) == (".fbx") || norm_load_directory.substr(norm_load_directory.find(".")) == (".FBX"))
					App->fbxload->LoadFBX(buffer, fileSize, App->scene_intro->root);
				else
				{
					std::string texturepath = "Assets/Textures/";
					texturepath.append(texname_2).append(".").append(filetype);

					char* tex_path = (char*)texturepath.c_str();
					
					texname = norm_load_directory.find_last_of("/");
					texname_2 = norm_load_directory.substr(texname);
					App->fbxload->LoadTexture(tex_path, App->scene_intro->SelectedGameObject);
						
						
						/*for (int j = 0; j < App->gameobject->emptygameobject_list[i]->gameobject_list.size(); j++)
						{
							if (App->gameobject->emptygameobject_list[i]->gameobject_list[j]->objSelected == true)
							{
								texname = norm_load_directory.find_last_of("/");
								texname_2 = norm_load_directory.substr(texname);
								App->gameobject->emptygameobject_list[i]->gameobject_list[j]->pngname = texname_2;
								texture_obj_dropped = true;
							}
						}
					}*/
				}
					
				SDL_free(dropped_filedir);
			}
			break;

			case SDL_WINDOWEVENT:
			{
				if(e.window.event == SDL_WINDOWEVENT_RESIZED)
					App->renderer3D->OnResize(e.window.data1, e.window.data2);
			}
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}