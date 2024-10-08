#!/bin/bash  

arch=$1
glewdir=$2
buildtype=$3
generatortoken="-G Ninja"

rm -rf CMakeCache.txt

# https://stackoverflow.com/questions/7583172/opencv-as-a-static-library-cmake-options
# https://stackoverflow.com/questions/19772398/build-the-static-opencv-library-with-multi-threaded-debug-dll-mdd-c-runtime-l

echo cmake $generatortoken -DCMAKE_INSTALL_PREFIX="../$glewdir-$arch" -DCMAKE_BUILD_TYPE=$buildtype ../build/cmake
cmake $generatortoken -DCMAKE_INSTALL_PREFIX="../$glewdir-$arch" -DCMAKE_BUILD_TYPE=$buildtype ../build/cmake

ninja install


