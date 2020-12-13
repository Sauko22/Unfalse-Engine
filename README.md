# Unfalse-Engine
Unfalse Engine is a game engine made with C & C++ by two students from the 3rd grade of Design and Development of Videogames, from the CITM UPC (Terrassa).
Marc Palma & Pol Juan were the responsibles to make this project come true.

## Repository
https://github.com/Sauko22/Unfalse-Engine

## Team
- Pol Juan Santos: https://github.com/Sauko22
- Marc Palma Turon: https://github.com/marcpt98

## Assignment 1 - Geometry Viewer

## Introduction to Unfalse Engine
### Menu
The menu is divided by four options: File, View, GameObject and Help.
#### File
Is formed by two buttons: New and Quit. 
- New: Still doesn't has a functionality.
- Quit: Let you exit from the engine.

#### View
Is formed by three buttons: Configuration, Change Theme and Menus.
- Configuration: Opens a window with four headers collapsed: Application, Window, Input and Hardware.
	- Application
		- Control engine fps
		- Framerate and millisecons graph
		- Enable/Disable vsync
		- Enable/Disable wireframe view
		- Enable/Disable depth
		- Enable/Disable blackface cull
		- Enable/Disable lighting
		- Enable/Disable color
		- Enable/Disable texture 2D
	- Window
		- Control Brightness
		- Resize window (width and height)
		- Enable/Disable fullscreen
		- Enable/Disable if window can be resized
		- Enable/Disable borderless window
		- Enable/Disable full desktop
	- Input
		- Mouse position
		- Mouse motion
		- Mouse wheel motion
		- Which mouse button is pressed and how long
		- Which keyboard key is pressed and how long
	- Hardware
		- SDL version
		- CPU cores and cache
		- System RAM
		- Caps
		- GPU vendor, renderer and version
		- VRAM budget, usage, available and reserved
- Change Theme: Engine theme can be changed to classic, black or light (last one not recommended)
- Menus: Toggle the visualization of Hierarchy, Inspector and Console window

#### GameObject
In this window you can create four types of primitives: cube, sphere, cylinder and pyramid.

#### Help
- Gui Demo: All the information about ImGui
- Documentation: Link to the documentation
- Download lastest: Link to the last release
- Report a bug: Link to the issues page
- About: Window with authors of the engine, link to their repositories, all 3rd party libraries used and licence
- Our Github: Link to the github engine page

### Hierarchy
This window shows all the empty gameobjects and gameobjects in the scene and they can be selected from here to change his properties in the Inspector window.
If a FBX is dragged to the scene, it will appear and empty gameobject with all the gameobjects inside that form this FBX.
If a texture is dragged to the scene, if the emptyobject is selected the texture will be applied to all gameobjects inside. If a gameobject is selected, the texture will only be applied to this gameobject.

### Inspector
Shows all the information about the selected gameobject.
Empty gameobjects only have a transform property and active/deactive render bool.
Gameobjects have:
- Activate/deactivate render
- Transform properties
- Normals visualizer
- Mesh properties (index, normals, vertex, faces and texture coordinates)
- FBX name
- Activate/deactivate texture
- Texture name with his width and height pixels
- Texture previsualizer 
- Activate/deactivate default texture
- Default texture previsualizer

### Console
Shows all the logs from the code.

### Camera controls
- WASD: Move camera like fps
- Shift + WASD: Move camera faster
- Right button: move camera freely
- Left alt + left button: orbit around selected object
- Mouse wheel: Move camera forward/backwards
- F: Focus gameobject selected

### Additional information
- FBX that has a texture linked, will be loaded with his respective texture
- Models must be in the models folder and textures must be in textures folder

## Assignment 2 - Engine Editor

### File Updated
- New: Delete all the gameObjects leaving room for an empty and new scene.

### Scene Serialitzacion
Saves and Load all the objects of the main window as a Scene. For that, we use the buttons found in the Time Management bar.

### Time Management bar
- Play/Stop: Allows to change the scene and return to the previous state
- Box : Toggle box colliders visibility
- Empty GameObject: Create a new gameobject in the hierachy
- Save Scene: Save the current scene
- Load Scene: Load a scene of your preference through the load menu
- RayCasting: Toggle raycast visibility

### Transformations
GameObjects position/rotation/scale can be modificated by the inspector and using Gizmos.
- I: Gizmo Translate
- O: Gizmo Rotation
- P: Gizmo Scale

### GameObject Inspector
- Delete Object: Delete all the childs and parent gameobject
- Empty Child: Create an empty child to the selected GameObject

### Camera Component
- A camera can be created in the menu of GameObjects
- It has a Culling Option functional with the scene
- FOV, planes and transformation can be changed in the inspector

### Inspector Updated
Gameobject can be reparent with other gameobjects by dragging them

### Assets Explorer
- The User can navigate through folders and load files in the same window by clicking the file
- The tree shows the order of the folders, but can't load files or delete them
- Each type of file has a diferent icon
- The "<-" Button allow to return the previous folder
- Left Click: Delete the file

### Resource Explorer
A windows the shows the loaded resources in the scene and how many times are loaded

### Mouse Picking
By using raycasting, gameobjects can be selected in the scene.

## Comments and difficulties
- Delete GameObjects doesn't work as expected with the gameobject created at the start of the engine. The rest of the gameobjects created afterwards are deleted properly
- Scene load return all gameobjects loaded to the initial position instead of keeping the state before the save.
- If a GameObject_X transformation is modified, the childs transforms of the gameobject using guizmos doesn't work properly. 
If the parent transform is not modified, guizmos of his childs work correctly. 
All transforms work correctly with the inspector.
- Mousepicking is used even if the mouse is not on the scene (you can select gameobjects clicking on the console).
- Cameras can't be moved with guizmos.
- If you reparent an object and you move the parent without moving the child first, the child will return to his original transformation in the world.

## License & Copyright 

[MIT License]

Copyright (c) 2020 by Pol Juan Santos & Marc Palma Turon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.




