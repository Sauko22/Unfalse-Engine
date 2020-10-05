#pragma once
#include "Module.h"

#include "Globals.h"

#define BOUNCER_TIME 200
#define FPS_INTERVAL 1.0 //seconds.

struct PhysBody3D;
class Cube;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();

public:


	Uint32 fps_lasttime = SDL_GetTicks(); //the last recorded time.
	Uint32 fps_current; //the current FPS.
	Uint32 fps_frames = 0; //frames passed since the last recorded fps
};
