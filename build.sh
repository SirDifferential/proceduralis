#!/bin/bash

sudo apt-get install git build-essential g++ libxrandr-dev libfreetype6-dev libjpeg62-dev libsndfile1-dev libglew-dev ocl-icd-opencl-dev

wget http://www.sfml-dev.org/download/sfml/2.1/SFML-2.1-sources.zip
unzip SFML-2.1-sources.zip
cd SFML-2.1
mkdir build
cd build
cmake ..
make
sudo make install

cd ..

mkdir build
cd build
cmake ..
make

cp -r ../config.json ../*.cl ../data ./

