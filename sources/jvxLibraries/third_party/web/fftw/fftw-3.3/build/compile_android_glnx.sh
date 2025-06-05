#!/bin/bash

# -DCMAKE_TOOLCHAIN_FILE=/home/hauke/Android/Sdk/ndk/27.0.12077973/build/cmake/android.toolchain.cmake -DANDROID_ABI^=x86_64 -DANDROID_NDK^=/home/hauke/Android/Sdk/ndk/27.0.12077973 -DANDROID_API_VERSION^=28 -DANDROID_PLATFORM^=android-28 -DCMAKE_ANDROID_ARCH_ABI=x86_64 -DCMAKE_ANDROID_NDK^=/home/hauke/Android/Sdk/ndk/27.0.12077973 -DCMAKE_SYSTEM_NAME^=Android -DCMAKE_SYSTEM_VERSION^=1

archtoken=x64-Debug
buildtype=Debug
targetfolder=fftw-3.3.10

 # cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=lib -DCMAKE_TOOLCHAIN_FILE=/home/hauke/Android/Sdk/ndk/27.0.12077973/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86_64 -DANDROID_NDK=/home/hauke/Android/Sdk/ndk/27.0.12077973 -DANDROID_API_VERSION=28 -DANDROID_PLATFORM=android-28 -DCMAKE_ANDROID_ARCH_ABI=x86_64 -DCMAKE_ANDROID_NDK=/home/hauke/Android/Sdk/ndk/27.0.12077973 -DCMAKE_SYSTEM_NAME=Android -DCMAKE_SYSTEM_VERSION=1 -DCMAKE_BUILD_TYPE=Debug -DDISABLE_FORTRAN=TRUE .. -DCMAKE_POLICY_DEFAULT_CMP0057=NEW

 
cd ..

echo Arguments: "$@"

# Get first argument archtoken and then shift 
archtoken=$1
shift

# Get second argument buildtype and then shift
buildtype=$1
shift

# Get the third argument targetfolder and then shift
targetfolder=$1
shift

echo archtoken=$archtoken
echo buildtype=$buildtype
echo targetfolder=$targetfolder

rest="$@"

echo Remaining arguments: "$rest"

cd $targetfolder
if [ ! -d "$archtoken" ]; then
    mkdir "$archtoken"
fi
cd "$archtoken"
	
echo cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=lib $rest -DCMAKE_BUILD_TYPE=$buildtype -DDISABLE_FORTRAN=TRUE .. -DCMAKE_POLICY_DEFAULT_CMP0057=NEW
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=lib $rest -DCMAKE_BUILD_TYPE=$buildtype -DDISABLE_FORTRAN=TRUE .. -DCMAKE_POLICY_DEFAULT_CMP0057=NEW
echo ninja install
ninja install

echo cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=lib $rest -DCMAKE_BUILD_TYPE=$buildtype -DENABLE_FLOAT=TRUE -DDISABLE_FORTRAN=TRUE .. -DCMAKE_POLICY_DEFAULT_CMP0057=NEW
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=lib $rest -DCMAKE_BUILD_TYPE=$buildtype -DENABLE_FLOAT=TRUE -DDISABLE_FORTRAN=TRUE .. -DCMAKE_POLICY_DEFAULT_CMP0057=NEW
echo ninja install
ninja install

