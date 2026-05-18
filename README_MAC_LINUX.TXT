Building on Mac or Linux - using CMake
======================================

The project uses CMake as its build system. You will need to install CMake, a C++ compiler,
and the SDL3 development libraries before building.


1) Install prerequisites
-------------------------

Mac:
   Ensure Xcode Command Line Tools are installed (required for the compiler):
   xcode-select --install

   Install Homebrew (https://brew.sh) if not already installed, then:
   brew install cmake sdl3 sdl3_image sdl3_ttf

Linux (e.g., Ubuntu 25.04+):
   sudo apt install build-essential cmake libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev

   (build-essential provides gcc/g++ and make, which are required to compile the project.)


2) Build the project - Recommended: use an IDE (e.g., VS Code or CLion)
-----------------------------------------------

Both VS Code and CLion support CMake projects directly and are the recommended way to
work on this project, as they handle building and running for you.

  VS Code:
    - Install the "CMake Tools" extension (ms-vscode.cmake-tools)
    - Open the project root folder (the one containing CMakeLists.txt)
    - CMake Tools will detect the project and prompt you to configure it
    - Use the Build / Run buttons in the status bar

  CLion:
    - File > Open, select the project root folder
    - CLion detects CMakeLists.txt and configures the project automatically
    - Use the Build / Run buttons as normal


3) Alternatively: build from the terminal
------------------------------------------

If you prefer the command line, open a terminal in the project root directory and run:

   mkdir build
   cd build
   cmake ..
   make

The output executable will be placed in the build directory.
Resources (fonts, images) are copied there automatically as part of the build.


Notes
-----

- If you add new source files to the project, remember to add them to CMakeLists.txt.
