# Dynamic Shape Animation with SFML and ImGui

<img width="1277" height="721" alt="Screenshot 2025-11-21 011027" src="https://github.com/user-attachments/assets/6447111f-5c31-413b-85d2-ee6049761346" />

## Overview
This C++ application demonstates real-time 2D animation and interactive editing using
SFML (Simple and Fast Multimedia Library) for rendering and Dear ImGui for an immediate-mode
graphical user interface.

The program reads a configuration file that describes a collection of geometric shapes
(rectangles and circles). Upon launch, each shape is instantiated with its specified position,
velocity, color, size, and name. The shapes move continously within the window and "bounce" off
the boundaries using collision detection based on their global bounding boxes.

Every shape displays its name centered inside it, renderered with a user-defined font, size, and color.

A floating ImGui control panel provides full runtime manipulation of the scene without restarting
the application.

## Features
- **Configuration-driven scene**: All window parameters, fonts, and shapes are loaded from a plain text file
- **Physics simulation**: Constant-velocity movement with perfect reflection on window edges.
- **Dynamic shape editing via ImGui**: 
	- Select any shape from a list
	- Toggle visibility
	- Adjust scale (0.1x - 5.0x)
	- Modify velocity components (+-8 units/frame independently for X and Y)
	- Change fill color (RGB)
	- Edit the shape's displayed name

## Build Requirements
- C++17 or later
- SFML 2.6+ (graphics, window, system modules)
- Dear ImGui (with SFML backend)
- A compatible C++ compiler (g++, clang++, MSVC)

## Configuration File Format
The configuration file consists of space-seperated tokens. Lines may appear in any order.

```
Window W H # Window resolution (integers)

Font filename.ttf size R G B # Font file (no spaces), size, and default text color

Rectangle Name xPos yPos velocityX velocityY R G B Width Height
Circle Name xPos yPos velocityX velocityY R G B Radius
```

## Acknowledgements
This program is inspired by an assignment given in COMP 4300 at Memorial University Newfoundland and 
provides practical experience with:
- File I/O and parsing in modern C++
- Polymorphism and container management
- Real-time animation and collision response
- Integrating immediate-mode GUI toolkits

Thanks.
