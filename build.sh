#!/bin/bash

SFML_URL="http://www.sfml-dev.org/download/sfml"
SFML_VERSION="2.1"
SFML_FILENAME="SFML-$SFML_VERSION-sources.zip"

echo "This script will install the required libraries for building Proceduralis on Debian and similar systems"
echo "If you don't want this script doing this stuff, then abort now."
while true; do
    read -p "Continue?" yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done


sudo apt-get install git cmake build-essential g++ libxrandr-dev libfreetype6-dev libjpeg8-dev libsndfile1-dev libglew-dev ocl-icd-opencl-dev
if [ $? -ne 0 ]; then
    echo "Error installing requires libs. Some package name probably has changed."
    exit 1
fi

if [ -f ./$SFML_FILENAME ]; then
    rm ./$SFML_FILENAME
fi

if [ -d ./SFML-$SFML_VERSION ]; then
    rm -rf ./SFML-$SFML_VERSION
fi

wget $SFML_URL/$SFML_VERSION/$SFML_FILENAME
unzip $SFML_FILENAME
cd SFML-$SFML_VERSION

mkdir build
cd build
cmake ..

if [ $? -ne 0 ]; then
    echo "Error running SFML cmake"
    exit 1
fi

make

if [ $? -ne 0 ]; then
    echo "Error building SFML"
    exit 1
fi

sudo make install

if [ $? -ne 0 ]; then
    echo "Error installing SFML"
    exit 1
fi

cd ..
cd ..

mkdir build
cd build
cmake ..

if [ $? -ne 0 ]; then
    echo "Error running proceduralis cmake"
    exit 1
fi

make

if [ $? -ne 0 ]; then
    echo "Error running proceduralis make"
    exit 1
fi

cp -r ../config.json ../*.cl ../data ./
echo "Build finished!"

