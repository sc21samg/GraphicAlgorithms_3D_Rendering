GraphicAlgorithms_3D_Rendering
=====================

## Overview
This repository contains the implementation of a 3D rendering application using modern shader-based OpenGL, with additional functionality for handling matrices, vectors, texturing, lighting, and animation. The project is divided into several tasks, each addressing different components of the rendering system, such as matrix/vector operations, basic 3D rendering, texturing, and lighting.

## Project Structure
- assets/: Contains runtime assets like textures, models, and shaders. All runtime data should be stored here, and avoid loading data from outside this directory.
- main/: The main project source code, where most of your work will be.
- support/: Pre-provided support functions from exercises. Do not modify this code.
- vmlib/: Math library with some incomplete functions. You will need to implement missing functions and can add additional operators/functions.
- third_party/: Holds third-party code. Any added third-party libraries must be documented and require prior permission via email.

## Team Members
This project was developed collaboratively by Ayesha Rahman, Sandra Guran, and Geeyoon Lim. Together, we implemented various components of the 3D rendering application as part of our coursework. In the attached report, a table outlining the distribution of individual and collaborative tasks is presented.

## Tasks Breakdown

1. Matrix/Vector Functions
- Implemented matrix and vector operations for 3D computations, tested using Catch2 (matrix multiplication, rotations, etc.). Verified results using Catch2 matchers.
2. 3D Renderer Basics
    ![Alt text](https://github.com/sc21samg/GraphicAlgorithms_3D_Rendering/blob/main/1.2%20git.png)
- Developed a 3D renderer using OpenGL to load and display a terrain OBJ file. Implemented first-person camera controls and a basic lighting model.
3. Texturing
  ![Alt text](https://github.com/sc21samg/GraphicAlgorithms_3D_Rendering/blob/main/1.3%20git.png)
- Applied orthophoto textures to the terrain mesh using custom shaders for texture mapping and sampling. Debugged shader and texture coordinate issues.
4. Simple Instancing
- Added two launch pads to the scene at different positions, applying textures and rendering them correctly.
5. Custom Model
  ![Alt text](https://github.com/sc21samg/GraphicAlgorithms_3D_Rendering/blob/main/1.5%20git.png)
- Built a 3D rocket model from basic shapes (cubes, cylinders, cone) and placed it on a launch pad.
6. Local Light Sources
  ![Alt text](https://github.com/sc21samg/GraphicAlgorithms_3D_Rendering/blob/main/1.6%20git.png)
- Implemented Blinn-Phong shading with ambient, diffuse, and specular reflections. Lighting calculations added to shaders, but some issues remain with lights following the camera.
7. Animation
- Created a smooth flight animation for the rocket, with vertical lift-off and horizontal flight, using a state machine for flight phases.
8. Measuring Performance
- Measured rendering performance using OpenGL queries and CPU timing. Identified performance bottlenecks in texture loading and complex animations.- 
