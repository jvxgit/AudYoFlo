#!/bin/bash  

arch=$1
kissfftdir=$2
dataformat=$3
buildtype=$4

androidndk=$5 # C:/develop/android/ndk/29.0.14206865
androidplatform=$6 # android-29 
androidabi=$7 # x86_64 

generatortoken="-G Ninja"

androidexts="-DKISSFFT_DATATYPE=$dataformat -DKISSFFT_OPENMP=FALSE -DKISSFFT_PKGCONFIG=FALSE -DKISSFFT_STATIC=TRUE -DKISSFFT_TEST=FALSE -DKISSFFT_TOOLS=FALSE -DKISSFFT_USE_ALLOCA=FALSE -DCMAKE_TOOLCHAIN_FILE=$androidndk/build/cmake/android.toolchain.cmake -DANDROID_ABI=$androidabi -DANDROID_PLATFORM=$androidplatform"

set -x
rm -rf CMakeCache.txt

echo cmake $generatortoken -DCMAKE_INSTALL_PREFIX=../$kissfftdir-$arch-$dataformat-$buildtype -DBUILD_SHARED_LIBS=FALSE $androidexts -DCMAKE_BUILD_TYPE=$buildtype ../$2
cmake $generatortoken -DCMAKE_INSTALL_PREFIX=../$kissfftdir-$arch-$dataformat-$buildtype -DBUILD_SHARED_LIBS=FALSE $androidexts -DCMAKE_BUILD_TYPE=$buildtype ../$2

ninja install