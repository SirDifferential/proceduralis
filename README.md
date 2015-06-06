# Proceduralis
## Procedural world generator using OpenCL

This is my personal learning experience on my journey in procedural world generation. It is heavily inspired by Dwarf Fortress.

## Development

I'm actively working on this, on and off. There are some very hacky parts in the code that will eventually be cleaned. I'm not even sure if I will stick with SFML when the actual world generator works well enough.

## Core ideas

Detailed blog entry: http://jessekaukonen.blogspot.fi/2014/02/generation-of-procedural-worlds-using.html

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

Now, you can either use the build script "build.sh" located at the root of this git repo, or do the steps manually. The script is run by:
sudo sh build.sh

The parameters for this script are:
* --nolibs: Does not install any of the libraries that are used by SFML / Proceduralis
* --nosfml: Does not build / install SFML
* --noproceduralis: Does not build / install the project at all
* --installpath /insert/path/here: Installs the project to the specified directory instead of /opt
* --threads numberhere: Uses the number of threads when executing make. Default is 4.

If you prefer to do all this manually:

Download OpenCL development libraries. This varies per distro. On Debian Jessie:

NVidia:

* nvidia-libopencl1 NVIDIA OpenCL ICD Loader library
* nvidia-opencl-icd NVIDIA OpenCL installable client driver

The library is located at:

* /usr/lib/x86_64-linux-gnu/libnvidia-opencl.so.1: symbolic link to /etc/alternatives/nvidia--libnvidia-opencl.so.1-x86_64-linux-gnu

AMD:

* amd-opencl-icd
* amd-libopencl1

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


## License

Every file, unless otherwise stated, is licensed under the MIT License.

Some parts of the project were copied and inspired from other projects and are marked with their separate licenses.

Copyright (c) 2014 Jesse Kaukonen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
