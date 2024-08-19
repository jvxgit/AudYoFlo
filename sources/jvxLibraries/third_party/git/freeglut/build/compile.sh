#!/bin/bash  

arch=$1
glutdir=$2
buildtype=$3
generatortoken="-G Ninja"

rm -rf CMakeCache.txt
echo cmake $generatortoken -DCMAKE_INSTALL_PREFIX="../$glutdir/$arch" -DBUILD_SHARED_LIBS=FALSE -DCMAKE_BUILD_TYPE=$buildtype ../$glutdir
cmake $generatortoken -DCMAKE_INSTALL_PREFIX="../$glutdir/$arch" -DBUILD_SHARED_LIBS=FALSE -DCMAKE_BUILD_TYPE=$buildtype ../$glutdir

ninja install
