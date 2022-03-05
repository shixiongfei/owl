#!/usr/bin/env sh

cd ./3rd/sdl2/

if [ -d "build" ]; then
    rm -rf build
fi

mkdir build
cd build

../configure
make

cd ../../..

if [ ! -d "libs" ]; then
    mkdir libs
fi

cp ./3rd/sdl2/build/build/.libs/libSDL2.a ./libs
cp ./3rd/sdl2/build/build/.libs/libSDL2main.a ./libs
cp ./3rd/sdl2/build/build/.libs/libSDL2_test.a ./libs
