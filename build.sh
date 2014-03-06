#!/bin/bash

SFML_URL="http://www.sfml-dev.org/download/sfml"
SFML_VERSION="2.1"
SFML_FILENAME="SFML-$SFML_VERSION-sources.zip"
INSTALL_PATH="/opt"
PROJECT_NAME="proceduralis"
INSTALL_SFML=true
INSTALL_PROCEDURALIS=true
INSTALL_LIBS=true
NUMBER_OF_THREADS=4

while :
do
    case $1 in
        -h | --help |-\?)
            echo "Parameters:"
            echo "--nolibs: Don't install the libraries required by SFML or $PROJECT_NAME"
            echo "--nosfml: Don't build / install SFML"
            echo "--noproceduralis: Don't build / install the project itself"
            echo "--installpath /home/cooldir: Install to specified directory instead of the default one at $INSTALL_PATH/$PROJECT_NAME"
            echo "--threads 4: Set the number of threads used by make."
            exit 0
            ;;
        --nolibs)
            export INSTALL_LIBS=false
            echo "Not installing libraries required by SFML / $PROJECT_NAME"
            shift
            ;;
        --nosfml)
            export INSTALL_SFML=false
            echo "Not building / installing SFML"
            shift
            ;;
        --noproceduralis)
            export INSTALL_PROCEDURALIS=false
            echo "Not building / installing $PROJECT_NAME"
            shift
            ;;
        --installpath)
            export INSTALL_PATH="$2"
            echo "Install path set to $INSTALL_PATH"
            shift 2
            ;;
        --threads)
            export NUMBER_OF_THREADS="$2"
            echo "Number of builder threads set to $NUMBER_OF_THREADS"
            shift 2
            ;;
        --) # End of parameters
            shift
            break
            ;;
        *)  # No more options
            break
            ;;
    esac
done

if [ ! -d $INSTALL_PATH ]; then
    echo "!! No install path directory exists. Attempting to create it."
    mkdir $INSTALL_PATH
    if [ $? -ne 0 ]; then
        echo "!! Unable to create $INSTALL_PATH. Permission problem?"
        exit 1
    fi
    echo "== Created $INSTALL_PATH successfully"
else
    echo "== Install path exists at $INSTALL_PATH"
fi


echo "The script is ready to begin installing the specified libraries (if not otherwise specified)"
echo "If you are sure you want to continue, type yes"

while true; do
    read -p "Continue?" yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done

if [ $(/usr/bin/id -u) -ne 0 ]; then
    echo "!! This script requires sudo to install the required libraries."
    echo "!! Run the script using the command:"
    echo "sudo sh $0 $*"
    exit 1
fi

install_libs()
{
    sudo apt-get install git cmake unzip build-essential g++ libxrandr-dev libfreetype6-dev libjpeg8-dev libsndfile1-dev libglew-dev ocl-icd-opencl-dev libopenal-dev
    
    if [ $? -ne 0 ]; then
        echo "!! Error installing requires libs. Some package name probably has changed."
        exit 1
    fi
}
    
install_sfml()
{
    
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
    
    make -j $NUMBER_OF_THREADS
    
    if [ $? -ne 0 ]; then
        echo "!! Error building SFML"
        exit 1
    fi
    
    echo "== Installing SFML"
    
    sudo make install
    
    if [ $? -ne 0 ]; then
        echo "!! Error installing SFML"
        exit 1
    fi

}

install_proceduralis()
{

    if [ -d ./build ]; then
        rm -rf ./build
    fi
    
    mkdir build
    cd build
    
    echo "== Running Proceduralis cmake"
    
    cmake -DCMAKE_INSTALL_PREFIX:PATH=$INSTALL_PATH  ..
    
    if [ $? -ne 0 ]; then
        echo "!! Error running proceduralis cmake"
        exit 1
    fi
    
    echo "== Building Proceduralis"
    
    make -j $NUMBER_OF_THREADS
    
    if [ $? -ne 0 ]; then
        echo "!! Error running proceduralis make"
        exit 1
    fi
    
    cp -r ../config.json ../*.cl ../data ./
    
    if [ ! -f ./proceduralis ]; then
        echo "!! Main binary is missing. Something went wrong."
        exit 1
    fi
    
    echo "== Installing"
    
    sudo make install
    
    if [ ! -d $INSTALL_PATH/$PROJECT_NAME ]; then
        echo "!! Error installing files. The program probably still works in the source directory."
        exit 1
    fi
    
    if [ ! -f $INSTALL_PATH/$PROJECT_NAME/proceduralis ]; then
        echo "!! Main binary is not located under $INSTALL_PATH/$PROJECT_NAME"
    fi
    
    echo "== Build finished!"
    echo "== The files have been installed to: "
    echo $INSTALL_PATH/$PROJECT_NAME

    echo "The binary is of type: "
    file $INSTALL_PATH/$PROJECT_NAME/$PROJECT_NAME
}

if [ $INSTALL_LIBS = "true" ]; then
    install_libs
fi

if [ $INSTALL_SFML = "true" ]; then
    install_sfml
fi

if [ $INSTALL_PROCEDURALIS = "true" ]; then
    install_proceduralis
fi


