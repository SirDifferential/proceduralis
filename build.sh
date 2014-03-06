#!/bin/bash

SFML_URL="http://www.sfml-dev.org/download/sfml"
SFML_VERSION="2.1"
SFML_FILENAME="SFML-$SFML_VERSION-sources.zip"

echo "== This script will install the required libraries for building Proceduralis on Debian and similar systems"
echo "== If you don't want this script doing this stuff, then abort now."
while true; do
    read -p "Continue?" yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done

sudo apt-get install git cmake unzip build-essential g++ libxrandr-dev libfreetype6-dev libjpeg8-dev libsndfile1-dev libglew-dev ocl-icd-opencl-dev libopenal-dev
if [ $? -ne 0 ]; then
    echo "!! Error installing requires libs. Some package name probably has changed."
    exit 1
fi

if [ -f ./$SFML_FILENAME ]; then
    rm ./$SFML_FILENAME
fi

if [ -d ./SFML-$SFML_VERSION ]; then
    rm -rf ./SFML-$SFML_VERSION
fi

echo "== Downloading SFML sources"

wget $SFML_URL/$SFML_VERSION/$SFML_FILENAME
unzip $SFML_FILENAME
cd SFML-$SFML_VERSION

mkdir build
cd build

echo "== Running SFML cmake"

cmake ..

if [ $? -ne 0 ]; then
    echo "!! Error running SFML cmake"
    exit 1
fi

echo "== Building SFML"

make

if [ $? -ne 0 ]; then
    echo "!! Error building SFML"
    exit 1
fi

echo "== Installing SFML"

#sudo make install
#
#if [ $? -ne 0 ]; then
#    echo "!! Error installing SFML"
#    exit 1
#fi

cd ..
cd ..

if [ -d ./build ]; then
    rm -rf ./build
fi

mkdir build
cd build

echo "== Running Proceduralis cmake"

cmake ..

if [ $? -ne 0 ]; then
    echo "!! Error running proceduralis cmake"
    exit 1
fi

echo "== Building Proceduralis"

make

if [ $? -ne 0 ]; then
    echo "!! Error running proceduralis make"
    exit 1
fi

cp -r ../config.json ../*.cl ../data ./

if [ ! -f ./proceduralis ]; then
    echo "!! Main binary is missing. Something went wrong."
    exit 1
fi

file ./proceduralis

echo "== Build finished!"

