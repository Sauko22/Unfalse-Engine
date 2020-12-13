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
