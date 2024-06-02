# DawnStar

## About
'DawnStar' is a in-development no graphical game engine written based on opengl and c++.

## Setup 
- Clone DawnStar Game Engine using git. Make sure you do a --recursive clone!
```
git clone --recursive https://github.com/Ladun/DawnStar.git
```

- Copy the premake files from `DawnStar/third-party/premake_files` to an appropriate location.
	- The location is indicated in the prefix of each file.
## Current feature

Right now, DawnStar Game Engine is very bare-bones. Its most remarkable features are:

| Feature               | Description                                                                           | Status   |
| -------               | -----------                                                                           | ------   |
| **ECS support**       | Entity Component System approach through *entt*.                                      | Done ✔️ |
| **2D Renderer**       | 2D batch renderer supporting OpenGL.                                                  | Done ✔️ |
| **Simple UI System**         | Buttons, UI images, image scaling according to screen ratio, etc., overall in-game UI system  | Done ✔️ |
| **Text drawing**     | Drawing  font using *msdf-atlas-gen*.       | Done ✔️  |
| **ImGui support**     | In-game debug UI using *dear               | Done ✔️   |

## Planned Features
| Feature               | Description                                                                           | Status   |
| -------               | -----------                                                                           | ------   |
| **3D Renderer**       | Basic 3D renderer with lighting, IBL and PBR workflow supporting OpenGL.              | WIP 💻 |
| **2D Physics**        | 2D Physics using *Box2D* with Rigidbody, Box and Circle Colliders and joints.         | TODO 📋 |
| **Shadow Support**    | Soft and Hard shadow support in OpenGL 3D Renderer.                                   | TODO 📋 |
| **3D Physics**        | Basic 3D Physics using *Jolt Physics* with Rigidbody, Box and Sphere Colliders.   	| TODO 📋 |
| **Audio**             | Basic Sound API with spatialization through *miniaudio*.                              | TODO 📋 |