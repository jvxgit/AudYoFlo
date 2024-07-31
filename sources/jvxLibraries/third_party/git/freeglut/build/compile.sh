#!/bin/bash  

arch=$1
hdfdir=$2
generatortoken="-G Ninja"

rm -rf CMakeCache.txt
echo cmake $generatortoken -DCMAKE_INSTALL_PREFIX="../$hdfdir/$arch" -DCMAKE_BUILD_TYPE=Debug ../$hdfdir
cmake $generatortoken -DCMAKE_INSTALL_PREFIX="../$hdfdir/$arch" -DCMAKE_BUILD_TYPE=Debug ../$hdfdir

ninja install
