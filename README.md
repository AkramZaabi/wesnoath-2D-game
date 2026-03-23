# Wesnoath 2D Game

## Overview
This is a 2D game built in C using the **SDL library** (Simple DirectMedia Layer). It includes support for rendering graphics with SDL_image and drawing text with SDL_ttf. 

## Prerequisites
To compile and run this project, you need the following installed on your system:

- **C Compiler**: such as GCC (MinGW for Windows).
- **Development Environment**: [Code::Blocks](https://www.codeblocks.org/) is recommended as the project includes a .cbp file.
- **SDL Libraries** (specifically version 1.2, based on the SDL and SDLmain flags):
  - SDL
  - SDL_image
  - SDL_ttf

## Setup Instructions

### 1. Install Dependencies
* Download and install **SDL**, **SDL_image**, and **SDL_ttf** development libraries for MinGW/Code::Blocks.
* By default, the project expects these libraries to be located at C:/SDL/include and C:/SDL/lib. You should extract the SDL?? libraries to C:/SDL/ so the folders match.

### 2. Open the Project
* Open Code::Blocks.
* Go to **File > Open** and select the 	est.cbp file located in the project root.

### 3. Build & Run
* All text, images, and fonts must be located in the ssets/ directory.
* Click **Build and Run** (or press F9) in Code::Blocks.
* The game should compile and launch automatically. If you encounter DLL missing errors, make sure you copy SDL.dll, SDL_image.dll, and SDL_ttf.dll into the project root folder or your in/Debug directory.

