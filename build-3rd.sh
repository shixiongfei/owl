#!/usr/bin/env sh

# Build SDL
cd ./3rd/sdl2/

if [ -d "build" ]; then
    rm -rf build
fi

mkdir build
cd build

../configure
make

cd ../../..

# Copy Libs
if [ ! -d "libs" ]; then
    mkdir libs
fi

cp ./3rd/sdl2/build/build/.libs/libSDL2.dylib ./libs
cp ./3rd/sdl2/build/build/.libs/libSDL2main.a ./libs

cd ./libs

install_name_tool -id "@rpath/libSDL2.dylib" libSDL2.dylib
install_name_tool -add_rpath "@executable_path" libSDL2.dylib
Install_name_tool -add_rpath "@loader_path" libSDL2.dylib

cd ..
