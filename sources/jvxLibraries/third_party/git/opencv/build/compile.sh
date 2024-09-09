#!/bin/bash  

arch=$1
opencvdir=$2
buildtype=$3
generatortoken="-G Ninja"

set -x
rm -rf CMakeCache.txt
cmake $generatortoken -DCMAKE_INSTALL_PREFIX=../$opencvdir-$arch -DBUILD_SHARED_LIBS=FALSE -DCMAKE_BUILD_TYPE=$buildtype ..

ninja install

