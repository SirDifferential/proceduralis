# Proceduralis
## Procedural world generator using OpenCL

This is my personal learning experience on my journey in procedural world generation. It is heavily inspired by Dwarf Fortress.

## Core ideas

OpenCL is used as much as possible to speed things up. There are a lot of jams that will need to be removed in future - they mostly exist now as a mean of getting easy debug information. SFML is used for the windowing and graphics, at least for now. In future the redundant usage of sf::Sprites/Textures/Images will be removed and optimized.


## Project setup

### Windows

Download SFML 2 at http://www.sfml-dev.org/download/sfml/2.1/ for your compiler, most likely Visual C++ 2010 or 2012. Note you need at least 2010 for C++11.

Download latest available GPU drivers.

* Nvidia Cuda Toolkit: https://developer.nvidia.com/cuda-toolkit
* AMD App SDK: http://developer.amd.com/tools-and-sdks/heterogeneous-computing/amd-accelerated-parallel-processing-app-sdk/downloads/
* Intel: You are out of luck as of this moment (year 2014) 

Download GLEW: http://glew.sourceforge.net/

Open the .sln file, select the active project(procedural_world), right click it and enter properties, go to VC++ directories:
* Go to include directories and add the SFML 2 include dir. For example c:/SFML/include
* Also add: C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v5.5\include or similar folder depending your GPU
* Also add C:\libs\glew-1.10.0\include or similar based on your GLEW directory.

* Go to library directories and add the SFML 2 library dir. For example c:/SFML/lib
* Also add C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v5.5\lib\x64 or similar depending on your GPU
* Also add: C:\libs\glew-1.10.0\lib\Release\x64 or something similar, based on your GLEW directory

Copy SFML .dlls from the SFML directory in your project folder, build and run. If the game doesn't find data, copy the folder in Debug / Release folder.

### Linux

Download latest GPU drivers. You will have to benchmark a bit to figure out if proprietary or open source drivers offer you better OpenCL performance. With NVidia I currently (year 2014) find the proprietary better.

Download OpenCL development libraries. This varies per distro. On Debian Wheezy, I have:

ocl-icd-libopencl1 - Generic OpenCL ICD Loader

Which are located at:

* libOpenCL.so.1 (libc6,x86-64) => /usr/lib/x86_64-linux-gnu/libOpenCL.so.1
* libOpenCL.so (libc6,x86-64) => /usr/lib/x86_64-linux-gnu/libOpenCL.so

Download SFML 2 source code from http://www.sfml-dev.org/download/sfml/2.1/

Build SFML2:

* unzip  SFML-2.1-sources.zip
* cd SFML-2.1
* mkdir build && cd build
* cmake ..
* make
* sudo make install

Build Proceduralis:

* git clone git@github.com:SirDifferential/proceduralis.git
* cd proceduralis
* mkdir build && cd build
* cmake ..
* make -j 8
* cp -r ../data ../*.cl ../config.json ./
* ./proceduralis 

### OS X

Probably identical to Linux
