#pragma once
#include "Module.h"

#include "Globals.h"
#include "Timer.h"
#include "ModuleGameObject.h"
#include "PathNode.h"
#include <vector>

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


	Uint32 fps_lasttime; //the last recorded time.
	Uint32 fps_current; //the current FPS.
	Uint32 fps_frames; //frames passed since the last recorded fps
	
	//The frames per second
	int FRAMES_PER_SECOND;
	
	//Keep track of the current frame
	int frame;

	//The frame rate regulator
	Timer fps;
};
