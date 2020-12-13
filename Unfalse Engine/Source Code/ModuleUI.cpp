#include "Globals.h"
#include "Application.h"
#include "ModuleUI.h"
#include "Component.h"
#include "GameObject.h"
#include "ModuleFileSystem.h"

#include "Glew\include\glew.h"
#define DROP_ID_HIERARCHY_NODES "hierarchy_node"

ModuleUI::ModuleUI(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	// Toolbar
	showDemo = false;
	showAbout = false;
	showConfig = false;
	showSceneLoad = false;

	// Editor Menu
	 activeInspec = true;
	 activeConsole = true;
	 activeHierach = true;
	
	// Docking
	showDock = false;

	// Wireframe
	wireframe = false;

	// VSync
	vsync = true;

	// Depth
	depth = false;
	
	// Cull face
	cull = true;

	// Lighting
	lighting = true;

	// Color
	color = true;

	// Texture
	texture2d = true;

	// Primitives
	cube = false;
	pyramid = false;
	sphere = false;
	cylinder = false;
	j = 0;
	k = 0;

	// Fps & ms logs
	fps_log = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	ms_log = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	cameradirection = vec3(0.f, 0.f, -1.f);
	playing = false;
	paused = false;
	bounding = false;
	empty = false;
	save = false;
	load = false;
	folder = ("Assets");
}

ModuleUI::~ModuleUI()
{}

bool ModuleUI::Init()
{
	LOG("Loading UI");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if (ret == true)
	{
		//Check for error
		GLenum error = glGetError();
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
	}

	GLenum error = glewInit();
	if (error != GL_NO_ERROR)
	{
		LOG("Error initializing glew library ! %s", SDL_GetError());
		ret = false;
	}
	else
	{
		LOG("using Glew %s", glewGetString(GLEW_VERSION));
	}

	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_Docking;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, context);

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, context);

	ImGui_ImplOpenGL3_Init();

	/*print_commits_info("Sauko22", "Racing-Car");*/

	return ret;
}

bool ModuleUI::CleanUp()
{
	LOG("Unloading UI");

	SDL_GL_DeleteContext(context);

	return true;
}

update_status ModuleUI::Update()
{
	// Start
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	if (showDock) {

		showDockSpace(&showDock);
	}
	showDockSpace(&showDock);

	App->renderer3D->Draw();

	// All windows, toolbars and options of ImGui
	// Toolbar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{

			if (ImGui::MenuItem("New"))
			{

				// New file
			}

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.972, 0.105, 0.105, 1.f));

			if (ImGui::MenuItem("Quit"))
			{
				// Exits the app
				return UPDATE_STOP;
			}

			ImGui::PopStyleColor();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Configuration"))
			{
				showConfig = !showConfig;
			}
			if (ImGui::BeginMenu("Change Theme"))
			{
				if (ImGui::MenuItem("Classic"))
				{
					ImGui::StyleColorsClassic();
				}
				else if(ImGui::MenuItem("Black"))
				{
					ImGui::StyleColorsDark();
				}
				else if (ImGui::MenuItem("Light"))
				{
					ImGui::StyleColorsLight();
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Menus"))
			{
				if (ImGui::MenuItem("Inspector"))
				{
					activeInspec = !activeInspec;
					
				}
				else if (ImGui::MenuItem("Hierachy"))
				{
					activeHierach = !activeHierach;
				}
				else if (ImGui::MenuItem("Console"))
				{
					activeConsole = !activeConsole;
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObject"))
		{
			
			if (ImGui::MenuItem("Cube"))
			{
				std::string file_path = "Assets/Primitives/Cube.fbx";
				/*char* buffer = nullptr;
				uint fileSize = 0;
				cube = true;
				fileSize = App->filesys->Load(file_path.c_str(), &buffer);
				App->fbxload->LoadFBX(buffer, fileSize, App->scene_intro->root);*/
				std::string path;
				std::string texname;
				std::string texname_2;
				std::string texname_3;

				App->filesys->SplitFilePath(file_path.c_str(), &texname, &texname_2, &texname_3);
				path.append(texname).append(texname_2).append(".meta");

				if (App->filesys->Exists(path.c_str()))
				{
					App->input->filedropped = true;
					App->serialization->parentuid = App->resource->GenerateNewUID();
					App->serialization->LoadGameObject(path.c_str());
					App->input->filedropped = false;
				}
				else
				{
					LOG("%s doesn't have a meta file", path.c_str());
				}
			}
			
			if (ImGui::MenuItem("Sphere"))
			{
				std::string file_path = "Assets/Primitives/Sphere.fbx";
				/*char* buffer = nullptr;
				uint fileSize = 0;
				sphere = true;
				fileSize = App->filesys->Load(file_path.c_str(), &buffer);
				App->fbxload->LoadFBX(buffer, fileSize, App->scene_intro->root);*/
				std::string path;
				std::string texname;
				std::string texname_2;
				std::string texname_3;

				App->filesys->SplitFilePath(file_path.c_str(), &texname, &texname_2, &texname_3);
				path.append(texname).append(texname_2).append(".meta");

				if (App->filesys->Exists(path.c_str()))
				{
					App->input->filedropped = true;
					App->serialization->parentuid = App->resource->GenerateNewUID();
					App->serialization->LoadGameObject(path.c_str());
					App->input->filedropped = false;
				}
				else
				{
					LOG("%s doesn't have a meta file", path.c_str());
				}
			}

			if (ImGui::MenuItem("Cylinder"))
			{
				std::string file_path = "Assets/Primitives/Cylinder.fbx";
				/*char* buffer = nullptr;
				uint fileSize = 0;
				cylinder = true;
				fileSize = App->filesys->Load(file_path.c_str(), &buffer);
				App->fbxload->LoadFBX(buffer, fileSize, App->scene_intro->root);*/
				std::string path;
				std::string texname;
				std::string texname_2;
				std::string texname_3;

				App->filesys->SplitFilePath(file_path.c_str(), &texname, &texname_2, &texname_3);
				path.append(texname).append(texname_2).append(".meta");

				if (App->filesys->Exists(path.c_str()))
				{
					App->input->filedropped = true;
					App->serialization->parentuid = App->resource->GenerateNewUID();
					App->serialization->LoadGameObject(path.c_str());
					App->input->filedropped = false;
				}
				else
				{
					LOG("%s doesn't have a meta file", path.c_str());
				}
			}

			if (ImGui::MenuItem("Pyramid"))
			{
				std::string file_path = "Assets/Primitives/Pyramid.fbx";
				/*char* buffer = nullptr;
				uint fileSize = 0;
				pyramid = true;
				fileSize = App->filesys->Load(file_path.c_str(), &buffer);
				App->fbxload->LoadFBX(buffer, fileSize, App->scene_intro->root);*/
				std::string path;
				std::string texname;
				std::string texname_2;
				std::string texname_3;

				App->filesys->SplitFilePath(file_path.c_str(), &texname, &texname_2, &texname_3);
				path.append(texname).append(texname_2).append(".meta");

				if (App->filesys->Exists(path.c_str()))
				{
					App->input->filedropped = true;
					App->serialization->parentuid = App->resource->GenerateNewUID();
					App->serialization->LoadGameObject(path.c_str());
					App->input->filedropped = false;
				}
				else
				{
					LOG("%s doesn't have a meta file", path.c_str());
				}
			}

			if (ImGui::MenuItem("Camera"))
			{
				std::string obj = std::to_string(j);

				GameObject* camera = new GameObject(App->scene_intro->root);
				camera->name.append("Camera_").append(obj);
				(CompTransform*)camera->AddComponent(Component::compType::TRANSFORM);
				(CompCamera*)camera->AddComponent(Component::compType::CAMERA);
				j++;
			}

			if (ImGui::MenuItem("GameObject"))
			{
				std::string obj = std::to_string(k);

				GameObject* emptyObject = new GameObject(App->scene_intro->root);
				emptyObject->name.append("emptyObject_").append(obj);
				(CompTransform*)emptyObject->AddComponent(Component::compType::TRANSFORM);
				k++;
			}

			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Gui Demo"))
			{
				showDemo = !showDemo;
			}

			if (ImGui::MenuItem("Documentation"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/Sauko22/Unfalse-Engine/wiki", NULL, NULL, SW_SHOWNORMAL);
			}

			if (ImGui::MenuItem("Download lastest"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/Sauko22/Unfalse-Engine/releases", NULL, NULL, SW_SHOWNORMAL);
			}

			if (ImGui::MenuItem("Report a bug"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/Sauko22/Unfalse-Engine/issues", NULL, NULL, SW_SHOWNORMAL);
			}

			if (ImGui::MenuItem("About"))
			{
				showAbout = !showAbout;
			}

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.972, 0.105, 0.105, 1.f));

			if (ImGui::MenuItem("Our Github"))
			{

				// Github link:
				ShellExecuteA(NULL, "open", "https://github.com/Sauko22/Unfalse-Engine", NULL, NULL, SW_SHOWNORMAL);
			}
			ImGui::PopStyleColor();
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (activeHierach == true)
	{
		if (ImGui::Begin("Hierarchy"))
		{
			for (int i = 0; i < App->scene_intro->root->children_list.size(); i++)
			{
				Hierarchy(App->scene_intro->root->children_list[i]);
			}
			ImGui::End();
		}
	}

	if (activeInspec == true)
	{
		if (ImGui::Begin("Inspector"))
		{
			showInspectorWin();
			
			ImGui::End();
		}
	}

	if (activeConsole == true) {
		if (ImGui::Begin("Console", NULL, ImGuiWindowFlags_HorizontalScrollbar))
		{
			showConsoleWin();

			ImGui::End();
		}
	}
	TimeWindows();
	
	//Resource Manager windows
	if (ImGui::Begin("Resources", NULL))
	{
		ResourceExplorer();
		
		ImGui::End();
	}
	
	AssetsExplorer();
	// Open windows
	if (showDemo == true) { ImGui::ShowDemoWindow(&showDemo); }
	
	if (showAbout == true) { showAboutWin(&showAbout); }
	
	if (showConfig == true) { showConfigWin(&showConfig); }

	if (showSceneLoad == true) { showSceneLoadWin(&showSceneLoad); }

	return UPDATE_CONTINUE;
}

void ModuleUI::Hierarchy(GameObject* gameobject)
{
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	const char* GameObjname = gameobject->name.c_str();

	if (gameobject->objSelected == true)
	{
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool node_open = ImGui::TreeNodeEx(GameObjname, node_flags);

	if (ImGui::IsItemClicked() || empty == true)
	{
		DeselectGameObjects(App->scene_intro->root);

		App->scene_intro->SelectedGameObject = gameobject;

		
		gameobject->objSelected = true;
		for (int i = 0; i < gameobject->component_list.size(); i++)
		{
			gameobject->component_list[i]->gameobject_selected = true;
		}
	}
	if (ImGui::BeginDragDropTarget()) {
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DROP_ID_HIERARCHY_NODES, ImGuiDragDropFlags_SourceNoDisableHover);

		if (payload != nullptr)
		{
			if (payload->IsDataType(DROP_ID_HIERARCHY_NODES))
			{
				GameObject* obj = *(GameObject**)payload->Data;

				if (obj != nullptr) //The second part of this is bug that needs to be fixed
				{
					ChangeGameObjectParent(obj, gameobject);
				}

			}
			//ImGui::ClearDragDrop();
		}
		ImGui::EndDragDropTarget();
	}
	
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload(DROP_ID_HIERARCHY_NODES, &gameobject, sizeof(GameObject), ImGuiCond_Once);
		ImGui::Text("GameObjname");
		ImGui::EndDragDropSource();
	}

	if (node_open)
	{
		for (int i = 0; i < gameobject->children_list.size(); i++)
		{
			Hierarchy(gameobject->children_list[i]);
		}
		ImGui::TreePop();
	}
}



void ModuleUI::ChangeGameObjectParent(GameObject* obj, GameObject* nextparent)
{
	if (obj != nullptr && nextparent != nullptr) {

		//obj->parent->to_delete = true;

		for (int i = 0; i < obj->parentGameObject->children_list.size(); i++)
		{
			if (obj->parentGameObject->children_list[i] == obj)
			{
				obj->parentGameObject->children_list.erase(obj->parentGameObject->children_list.begin() + i);
				i--;

			}
		}
		//obj->parentGameObject = nextparent;
		//dynamic_cast<CompTransform*>(obj->children_list->GetComponent(Component::compType::TRANSFORM))->UpdateTrans();
		
		obj->parentGameObject = nextparent;
		nextparent->children_list.push_back(obj);
		
		/*for (int i = 0; i < nextparent->children_list.size(); i++)
		{
			obj->parentGameObject = nextparent;
			nextparent->children_list.push_back(obj);
			dynamic_cast<CompTransform*>(obj->GetComponent(Component::compType::TRANSFORM))->local_transform = dynamic_cast<CompTransform*>(nextparent->children_list[i]->GetComponent(Component::compType::TRANSFORM))->local_transform* dynamic_cast<CompTransform*>(obj->GetComponent(Component::compType::TRANSFORM))->local_transform;
			dynamic_cast<CompTransform*>(obj->GetComponent(Component::compType::TRANSFORM))->local_transform.Inverse();
		}*/
		//nextparent->children_list.push_back(obj);
	}
}
void ModuleUI::DeselectGameObjects(GameObject* gameobject)
{
	gameobject->objSelected = false;
	for (int i = 0; i < gameobject->children_list.size(); i++)
	{
		DeselectGameObjects(gameobject->children_list[i]);
	}
	for (int i = 0; i < gameobject->component_list.size(); i++)
	{
		gameobject->component_list[i]->gameobject_selected = false;
	}
}

update_status ModuleUI::PostUpdate()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);

	}

	return UPDATE_CONTINUE;
}

// Windows functions
void ModuleUI::showConsoleWin()
{
	for (uint i = 0; i < items.size(); i++)
	{
		const char* item = items[i].c_str();

		ImGui::Text(item);
	}
}

void ModuleUI::putLog(const char* log)
{
	std::string item;
	item = log;
	items.push_back(item);
}

void ModuleUI::showInspectorWin()
{
	if (App->scene_intro->SelectedGameObject != nullptr)
	{
		App->scene_intro->SelectedGameObject->Inspector();
	}
}

void ModuleUI::showConfigWin(bool* p_open)
{
	if (!ImGui::Begin("Configuration", p_open))
	{
		ImGui::End();
		return;
	}

	if (ImGui::CollapsingHeader("Application"))
	{
		static int fps = 60;
		ImGui::SliderInt("Max fps", &fps, 1, 144);

		if (ImGui::IsItemActive())
		{
			App->scene_intro->FRAMES_PER_SECOND = fps;
		}

		ImGui::Text("Limit framerate: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", fps);

		fps_log.erase(fps_log.begin());
		fps_log.push_back(App->scene_intro->fps_current);
		ms_log.erase(ms_log.begin());
		ms_log.push_back(1000 / App->scene_intro->fps_current);

		char title[25];
		sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(300, 100));
		sprintf_s(title, 25, "Milliseconds %.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 50.0f, ImVec2(310, 100));

		static int a = 0;

		if (vsync == true && a == 0)
		{
			a = 1;
			SDL_GL_SetSwapInterval(1);
			LOG("VSYNC ACTIVATED");
		}
		else if (vsync == false && a == 1)
		{
			a = 0;
			SDL_GL_SetSwapInterval(0);
			LOG("VSYNC DESACTIVATED");
		}
		ImGui::Checkbox("Vsync", &vsync); ImGui::SameLine();

		static int o = 0;
		ImGui::Checkbox("Wireframe view", &wireframe); 

		if (wireframe == true && o == 0)
		{
			o = 1;
			wireframe = true;
			LOG("ISWIREFRAME");
		}
		else if (wireframe == false && o == 1)
		{
			o = 0;
			wireframe = false;
			LOG("ISNOTWIREFRAME");
		}

		static int p = 0;
		ImGui::Checkbox("Depth disable", &depth); ImGui::SameLine();

		if (depth == true && p == 0)
		{
			p = 1;
			depth = true;
			LOG("ISDEPTH");
		}
		else if (depth == false && p == 1)
		{
			p = 0;
			depth = false;
			LOG("ISNOTDEPTH");
		}

		static int q = 0;
		ImGui::Checkbox("Backface cull", &cull);

		if (cull == true && q == 0)
		{
			q = 1;
			cull = true;
			LOG("ISCULL");
		}
		else if (cull == false && q == 1)
		{
			q = 0;
			cull = false;
			LOG("ISNOTCULL");
		}

		static int r = 0;
		ImGui::Checkbox("Lighting", &lighting); ImGui::SameLine();

		if (lighting == true && r == 0)
		{
			r = 1;
			lighting = true;
			LOG("ISLIGHTING");
		}
		else if (lighting == false && r == 1)
		{
			r = 0;
			lighting = false;
			LOG("ISNOTLIGHTING");
		}

		static int s = 0;
		ImGui::Checkbox("Color", &color);

		if (color == true && s == 0)
		{
			s = 1;
			color = true;
			LOG("ISCOLOR");
		}
		else if (color == false && s == 1)
		{
			s = 0;
			color = false;
			LOG("ISNOTCOLOR");
		}

		static int t = 0;
		ImGui::Checkbox("Texture 2D", &texture2d);

		if (texture2d == true && t == 0)
		{
			t = 1;
			texture2d = true;
			LOG("ISTEXTURE2D");
		}
		else if (texture2d == false && t == 1)
		{
			t = 0;
			texture2d = false;
			LOG("ISNOTTEXTURE2D");
		}
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		static float bright = 1;
		ImGui::SliderFloat("Brightness", &bright, 0.0f, 1.0f);
		if (ImGui::IsItemActive()) {	SDL_SetWindowBrightness(App->window->window, bright); }

		static int width = App->window->screen_surface->w;
		static int height = App->window->screen_surface->h;
		ImGui::SliderInt("Width", &width, 640, 1920);
		if (ImGui::IsItemActive())
		{
			SDL_SetWindowSize(App->window->window, width, height);
		}
		ImGui::SliderInt("Height", &height, 480, 1080);
		if (ImGui::IsItemActive()) 
		{ 
			SDL_SetWindowSize(App->window->window, width, height); 
		}
		
		ImGui::Text("Refresh rate: %i", App->scene_intro->fps_current);

		static bool fullscreen = false;
		static int i = 0;
		static bool resizable = false;
		static int j = 0;
		ImGuiWindowFlags window_flags = 0;
		static bool borderless = false;
		static int k = 0;
		static bool desktop = false;
		static int l = 0;
		
		ImGui::Checkbox("Fullscreen", &fullscreen); ImGui::SameLine(150);
		
		
		if (fullscreen == true && i == 0)
		{
			i = 1;
			SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN); 
			LOG("ISFULLSCREEN");
		}
		else if (fullscreen == false && i == 1)
		{
			i = 0;
			SDL_SetWindowFullscreen(App->window->window, 0);
			LOG("ISNOTFULLSCREEN");
		}

		ImGui::Checkbox("Resizable", &resizable);

		if (resizable == true && j == 0)
		{
			j = 1;
			SDL_SetWindowResizable(App->window->window, SDL_TRUE);
			LOG("ISRESIZABLE");
		}
		else if (resizable == false && j == 1)
		{
			j = 0;
			SDL_SetWindowResizable(App->window->window, SDL_FALSE);
			LOG("ISNOTRESIZABLE");
		}

		ImGui::Checkbox("Borderless", &borderless); ImGui::SameLine(150);

		if (borderless == true && k == 0)
		{
			k = 1;
			SDL_SetWindowBordered(App->window->window, SDL_FALSE);
			LOG("ISBORDERLESS");
		}
		else if (borderless == false && k == 1)
		{
			k = 0;
			SDL_SetWindowBordered(App->window->window, SDL_TRUE);
			LOG("ISNOTBORDERLESS");
		}

		ImGui::Checkbox("Full Desktop", &desktop);

		if (desktop == true && l == 0)
		{
			l = 1;
			SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			LOG("ISFULLDESKTOP");
		}
		else if (desktop == false && l == 1)
		{
			l = 0;
			SDL_SetWindowFullscreen(App->window->window, 0);
			LOG("ISNOTFULLDESKTOP");
		}
		
	}

	if (ImGui::CollapsingHeader("Input"))
	{
		ImGui::Text("Mouse pos: %i, %i ", App->input->mouse_x, App->input->mouse_y);
		ImGui::Text("Mouse motion: %i, %i", App->input->mouse_x_motion, App->input->mouse_y_motion);
		ImGui::Text("Mouse wheel: %i", App->input->mouse_z);

		ImGui::Separator();

		ImGui::Text("Mouse down:");     
		ImGuiIO& io = ImGui::GetIO();
		for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
		{
			if (io.MouseDownDuration[i] >= 0.0f)
			{
				ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]);
			}
		}

		ImGui::Text("Keys down:");
		for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
		{
			if (io.KeysDownDuration[i] >= 0.0f)
			{
				ImGui::SameLine(); ImGui::Text("%d (0x%X) (%.02f secs)", i, i, io.KeysDownDuration[i]);
			}
		}
	}

	if (ImGui::CollapsingHeader("Hardware"))
	{
		SDL_version compiled;
		SDL_VERSION(&compiled);
		ImGui::Text("SDL Version: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d.%d.%d", compiled.major, compiled.minor, compiled.patch);
		
		ImGui::Separator();
		
		ImGui::Text("CPUs: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", SDL_GetCPUCount()); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "(Cache: %dkb)", SDL_GetCPUCacheLineSize());
		ImGui::Text("System RAM: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dGb", SDL_GetSystemRAM()/1000);
		ImGui::Text("Caps: "); ImGui::SameLine();
		
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1.f));
		if (SDL_HasRDTSC() == SDL_TRUE) { ImGui::Text("RDTSC,"); } ImGui::SameLine();
		if (SDL_HasMMX() == true) { ImGui::Text("MMX,"); } ImGui::SameLine();
		if (SDL_HasSSE() == true) { ImGui::Text("SSE,"); } ImGui::SameLine();
		if (SDL_HasSSE2() == true) { ImGui::Text("SSE2,"); } ImGui::SameLine();
		if (SDL_HasSSE3() == true) { ImGui::Text("SSE3,"); } ImGui::SameLine();
		if (SDL_HasSSE41() == true) { ImGui::Text("SSE41,"); } ImGui::SameLine();
		if (SDL_HasSSE42() == true) { ImGui::Text("SSE42,"); } ImGui::SameLine();
		if (SDL_HasAVX() == true) { ImGui::Text("AVX"); }
		ImGui::PopStyleColor();

		ImGui::Separator();

		const GLubyte* vendor = glGetString(GL_VENDOR);
		const GLubyte* renderer = glGetString(GL_RENDERER);
		const GLubyte* version = glGetString(GL_VERSION);
		ImGui::Text("GPU: ");
		ImGui::Text("Vendor: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", vendor);
		ImGui::Text("Renderer: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", renderer);
		ImGui::Text("Version: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", version);
		
		ImGui::Separator();

		GLint total_mem_kb = 0;
		glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb);
		GLint cur_avail_mem_kb = 0;
		glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb);
		GLint cur_usage_mem_kb = 0;
		glGetIntegerv(GL_NVX_gpu_memory_info, &cur_usage_mem_kb);

		ImGui::Text("VRAM Budget: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMb", total_mem_kb/1000);
		ImGui::Text("VRAM Usage: "); ImGui::SameLine();
		// Not working
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMb", cur_usage_mem_kb);
		//
		ImGui::Text("VRAM Available: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dMb", cur_avail_mem_kb / 1000);
		ImGui::Text("VRAM Reserved: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "0.0Mb");
	}

	ImGui::End();
}

void ModuleUI::showAboutWin(bool* p_open)
{
	if (!ImGui::Begin("About Unfalse Engine", p_open))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Unfalse Engine v0.1");

	ImGui::Separator();
	ImGui::Text("By Marc Palma Turon and Pol Juan Santos.");
	ImGui::Spacing();
	ImGui::Text("A false engine better than the original.");
	ImGui::Spacing();
	if (ImGui::Button("Marc Palma Github"))
	{
		ShellExecuteA(NULL, "open", "https://github.com/marcpt98", NULL, NULL, SW_SHOWNORMAL);
	}
	if (ImGui::Button("Pol Juan Github"))
	{
		ShellExecuteA(NULL, "open", "https://github.com/Sauko22", NULL, NULL, SW_SHOWNORMAL);
	}
	ImGui::Spacing();
	ImGui::Text("3rd Party Libraries used:");
	
	static ImGuiTextFilter filter;
	const char* lines[] = { "SDL 2.0.0", "SDL Mixer 2.0.0", "Cereal 1.2.2", "Glew 2.0.0",
		"ImGui 1.51", "MathGeaLib 1.5", "OpenGL 3.1", "Assimp 3.1.1", "Devil 1.7.8" };
	for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
	{
		if (filter.PassFilter(lines[i]))
		{
			ImGui::BulletText("%s", lines[i]);
		}
	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Licence:");
	ImGui::Spacing();
	ImGui::Text("MIT License Copyright(c)[2019][Pol Juan Santos and Marc Palma Turon]");
	ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
	ImGui::Text("of this softwareand associated documentation files(the Software), to deal");
	ImGui::Text("in the Software without restriction, including without limitation the rights to use, copy,");
	ImGui::Text("modify, merge, publish, distribute, sublicense, and /or sell copies of the Software,");
	ImGui::Text("and to permit persons to whom the Software is furnished to do so, subject to the following conditions:");
	ImGui::Text("The above copyright noticeand this permission notice shall be included in all");
	ImGui::Text("copies or substantial portions of the Software.");
	ImGui::Text("THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,"); 
	ImGui::Text("INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE"); 
	ImGui::Text("AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,"); 
	ImGui::Text("DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,"); 
	ImGui::Text("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE."); 

	ImGui::End();
}

void ModuleUI::showSceneLoadWin(bool* p_open)
{
	if (!ImGui::Begin("Loading Scene", p_open))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Which scene do you want to load?");

	std::vector<std::string> scenes_list;
	std::vector<std::string> dir;

	// Get path
	std::string _dir = "";
	_dir.append("Library/Scenes/");

	// Get files in path
	App->filesys->DiscoverFiles(_dir.c_str(), scenes_list, dir);

	for (int i = 0; i < scenes_list.size(); i++)
	{
		std::string path;
		path.append("Library/Scenes/").append(scenes_list[i]);

		if (ImGui::Button(scenes_list[i].c_str()))
		{
			App->input->filedropped = true;
			App->serialization->LoadModel(path.c_str());
			App->input->filedropped = false;
		}
	}

	ImGui::End();
}

void ModuleUI::showDockSpace(bool* p_open)
{
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", p_open, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
	else {
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	
	}
	
	ImGui::End();
}

void ModuleUI::TimeWindows() {
	ImVec2 buttonSize = { 50.f, 20.f };
	ImGui::Begin("Time Management", (bool*)false, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::Text("|"); ImGui::SameLine();
	if (playing == true) {
		ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 0, 0, 100));
		if (ImGui::Button("Stop", buttonSize)) {
			playing = false;
		}
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 255, 0, 100));
		if (ImGui::Button("Play", buttonSize)) {
			playing = true;
		}
	}
	ImGui::PopStyleColor();
	ImGui::SameLine();
	if (paused == true) {
		if (ImGui::Button("Resume", buttonSize)) {
			paused = false;
		}
	}
	else {
		if (ImGui::Button("Pause", buttonSize)) {
			paused = true;
		}
	}
	ImGui::SameLine();
	if (bounding == false)
		ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 255, 0, 100));
	else if (bounding == true)
		ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 0, 0, 100));
	if (ImGui::Button("Box", buttonSize))
	{
		bounding =! bounding;
	}
	ImGui::PopStyleColor();
	ImVec2 buttonSizes = { 130.f, 20.f };
	ImGui::SameLine();
	if (ImGui::Button("Empty GameObject", buttonSizes))
	{
		empty = true;
	}
	ImGui::SameLine();
	ImGui::Text("||");
	ImGui::SameLine();
	ImVec2 buttonSizesu = { 100.f, 20.f };
	if (ImGui::Button("Save Scene", buttonSizesu))
	{
		save = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Scene", buttonSizesu))
	{
		load = true;
	}

	ImGui::End();
}

void ModuleUI::ResourceExplorer()
{
	std::vector<ResModel*> resmodel_list;
	std::vector<ResMesh*> resmesh_list;
	std::vector<ResTexture*> restext_list;

	for (std::map<uint, Resource*>::iterator it = App->resource->resources.begin(); it != App->resource->resources.end(); it++)
	{
		if (it->second->Type == Resource::ResType::MODEL)
		{
			resmodel_list.push_back((ResModel*)it->second);
		}
		else if (it->second->Type == Resource::ResType::MESH)
		{
			resmesh_list.push_back((ResMesh*)it->second);
		}
		else if (it->second->Type == Resource::ResType::TEXTURE)
		{
			restext_list.push_back((ResTexture*)it->second);
		}
		else
		{
			LOG("Error reading resource %s", it->second->UID);
		}
	}

	if (ImGui::CollapsingHeader("Models"))
	{
		if (ImGui::TreeNode("Models"))
		{
			for (int i = 0; i < resmodel_list.size(); i++)
			{
				if (resmodel_list[i]->referenceCount != 0)
				{
					ImGui::Text("Name: %s", resmodel_list[i]->name.c_str());
					ImGui::Text("Times loaded: %i", resmodel_list[i]->referenceCount);
				}
			}
			
			ImGui::Separator();
		}
		ImGui::TreePop();
	}
	if (ImGui::CollapsingHeader("Meshes"))
	{
		if (ImGui::TreeNode("Meshes"))
		{
			for (int i = 0; i < resmesh_list.size(); i++)
			{
				if (resmesh_list[i]->referenceCount != 0)
				{
					ImGui::Text("Name: %s", resmesh_list[i]->name.c_str());
					ImGui::Text("Times loaded: %i", resmesh_list[i]->referenceCount);
				}
			}

			ImGui::Separator();
		}
		ImGui::TreePop();
	}
	if (ImGui::CollapsingHeader("Textures"))
	{
		if (ImGui::TreeNode("Textures"))
		{
			for (int i = 0; i < restext_list.size(); i++)
			{
				if (restext_list[i]->referenceCount != 0)
				{
					ImGui::Text("Name: %s", restext_list[i]->texname.c_str());
					ImGui::Text("Times loaded: %i", restext_list[i]->referenceCount);
				}
			}
		}
		ImGui::TreePop();
	}
}
void ModuleUI::AssetsExplorer()
{
	if (ImGui::Begin("Resource Explorer")) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar;
		ImGui::BeginChild("Tree", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.15f, ImGui::GetContentRegionAvail().y), false, window_flags);
		ImGui::Text("Resources");
		//DrawDirectoryRecursive("Assets", nullptr);
		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::BeginChild("Folder", ImVec2(0, ImGui::GetContentRegionAvail().y), true);
		DrawCurrentFolder();
		ImGui::EndChild();
	}
	ImGui::End();
}
void  ModuleUI::DrawCurrentFolder()
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	App->filesys->DiscoverFiles(folder.c_str(), files, dirs);
	ImVec2 buttonsize = { 20.f, 20.f };
	if (ImGui::Button("<-", buttonsize))
	{
		size_t back = folder.find_first_of("/") + 1;
		folder = folder.substr(0, back);
	}

	for (size_t i = 0; i < dirs.size(); i++)
	{
		ImGui::PushID(i);
		if (ImGui::Button(dirs[i].c_str(), ImVec2(100, 100))) {
			folder.append("/");
			folder.append(dirs[i].c_str());
		}

		

		

		ImGui::PopID();
		ImGui::SameLine();
	}

	for (size_t i = 0; i < files.size(); i++)
	{
		if (files[i].find(".meta") != -1)
			continue;

		ImGui::PushID(i);
		if (ImGui::Button(files[i].c_str(), ImVec2(120, 120)))
		{

			LOG("HOLI");


		}

		

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::Button("Delete"))
			{
				/*std::string file_to_delete = current_folder + "/" + files[i];
				App->resource->DeleteAsset(file_to_delete.c_str());
				ImGui::CloseCurrentPopup();*/
			}
			ImGui::EndPopup();
		}

		if (files[i].find(".fbx") != std::string::npos)
		{/*
			ImGui::SameLine();
			if (ImGui::Button("->"))
				ImGui::OpenPopup("Meshes");
			if (ImGui::BeginPopup("Meshes", ImGuiWindowFlags_NoMove))
			{
				std::string model = current_folder + "/" + files[i];
				const char* library_path = App->resource->Find(App->resources->GetUIDFromMeta(model.append(".meta").c_str()));

				std::vector<uint> meshes;
				std::vector<uint> materials;
				ModelImporter::ExtractInternalResources(library_path, meshes, materials);

				for (size_t m = 0; m < meshes.size(); m++)
				{
					ImGui::PushID(meshes[m]);
					ImGui::Text("%d", meshes[m]);
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{
						ImGui::SetDragDropPayload("MESHES", &(meshes[m]), sizeof(int));
						ImGui::EndDragDropSource();
					}
					ImGui::PopID();
				}
				ImGui::EndPopup();*/
			/*}*/
		}

		ImGui::PopID();
		ImGui::SameLine();
	}
}