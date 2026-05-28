#!/bin/bash  

arch=$1
kissfftdir=$2
dataformat=$3
buildtype=$4

generatortoken="-G Ninja"

glnxexts="-DKISSFFT_DATATYPE=$dataformat -DKISSFFT_OPENMP=FALSE -DKISSFFT_PKGCONFIG=FALSE -DKISSFFT_STATIC=FALSE -DKISSFFT_TEST=FALSE -DKISSFFT_TOOLS=FALSE -DKISSFFT_USE_ALLOCA=FALSE"

set -x
rm -rf CMakeCache.txt

echo cmake $generatortoken -DCMAKE_INSTALL_PREFIX=../$kissfftdir-$arch-$dataformat-$buildtype -DBUILD_SHARED_LIBS=FALSE $glnxexts -DCMAKE_BUILD_TYPE=$buildtype ../$2
cmake $generatortoken -DCMAKE_INSTALL_PREFIX=../$kissfftdir-$arch-$dataformat-$buildtype -DBUILD_SHARED_LIBS=FALSE $glnxexts -DCMAKE_BUILD_TYPE=$buildtype ../$2

ninja install
