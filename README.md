# DawnStar

## About
'DawnStar' is a in-development no graphical game engine written based on opengl and c++.

## Setup 
- install opengl for linux
```
sudo apt-get update
sudo apt-get install mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev xorg-dev
```

- Clone DawnStar Game Engine using git. Make sure you do a --recursive clone!
```
git clone --recursive https://github.com/Ladun/DawnStar.git
```

## Current feature

Right now, DawnStar Game Engine is very bare-bones. Its most remarkable features are:

| Feature               | Description                                                                           | Status   |
| -------               | -----------                                                                           | ------   |
| **ECS support**       | Entity Component System approach through *entt*.                                      | Done ✔️ |
| **2D Renderer**       | 2D batch renderer supporting OpenGL.                                                  | Done ✔️ |
| **Simple UI System**         | Buttons, UI images, image scaling according to screen ratio, etc., overall in-game UI system  | Done 💻 |

## Planned Features
| Feature               | Description                                                                           | Status   |
| -------               | -----------                                                                           | ------   |
| **Text drawing**     | Drawing  font using *msdf-atlas-gen*.       | WIP 💻  |
| **ImGui support**     | In-game debug UI using *dear               | WIP 💻  |
| **2D Physics**        | 2D Physics using *Box2D* with Rigidbody, Box and Circle Colliders and joints.         | TODO 📋 |
| **3D Renderer**       | Basic 3D renderer with lighting, IBL and PBR workflow supporting OpenGL.              | TODO 📋 |
| **Shadow Support**    | Soft and Hard shadow support in OpenGL 3D Renderer.                                   | TODO 📋 |
| **3D Physics**        | Basic 3D Physics using *Jolt Physics* with Rigidbody, Box and Sphere Colliders.   	| TODO 📋 |
| **Audio**             | Basic Sound API with spatialization through *miniaudio*.                              | TODO 📋 |
