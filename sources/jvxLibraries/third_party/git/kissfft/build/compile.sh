#!/bin/bash  

arch=$1
kissfftdir=$2
buildtype=$3
generatortoken="-G Ninja"

set -x
rm -rf CMakeCache.txt
cmake $generatortoken -DCMAKE_INSTALL_PREFIX=../$kissfftdir-$arch -DBUILD_SHARED_LIBS=FALSE -DBUILD_TESTS=FALSE -DBUILD_PERF_TESTS=FALSE -DBUILD_OPENJPEG=ON -DCMAKE_BUILD_TYPE=$buildtype ..

ninja install

