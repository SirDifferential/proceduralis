#!/bin/bash

for library in `ldd ./proceduralis | grep \=\> | awk '{print $1}'`
do
    if [ -f /usr/lib/x86_64-linux-gnu/$library ]
    then 
    cp /usr/lib/x86_64-linux-gnu/$library ./lib/
    elif [ -f /usr/lib/mesa/$library ]
    then
    cp /usr/lib/mesa/$library ./lib/
    elif [ -f /lib64/$library ]
    then
    cp /lib64/$library ./lib/
    elif [ -f /usr/lib/$library ]
    then
    cp /usr/lib/$library ./lib/
    fi
done
rm ./lib/libdl.so.*
rm ./lib/libc.so.*
rm ./lib/libm.so.*

