#!/bin/bash  

arch=$1
opencvdir=$2
buildtype=$3

androidndk=$4 # C:/develop/android/ndk/29.0.14206865
androidplatform=$5 # android-29 
androidabi=$6 # x86_64 

generatortoken="-G Ninja"

androidexts="-DCMAKE_TOOLCHAIN_FILE=$androidndk/build/cmake/android.toolchain.cmake -DANDROID_ABI=$androidabi -DANDROID_PLATFORM=$androidplatform -DANDROID_SDK_ROOT=$androidndk/../.. \
	-DBUILD_opencv_java=OFF -DBUILD_opencv_java_bindings_generator=OFF -DBUILD_JAVA=OFF -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_ANDROID_EXAMPLES=OFF \
	-DWITH_ANDROID_MEDIANDK=OFF -DWITH_IPP=OFF -DBUILD_IPP_IW=OFF -DWITH_ITT=OFF -DBUILD_ITT=OFF -DWITH_KLEIDICV=OFF -DWITH_CAROTENE=OFF"

echo => OpenCV for Android => $androidexts

set -x
rm -rf CMakeCache.txt

echo cmake $generatortoken -DCMAKE_INSTALL_PREFIX=../$opencvdir-$arch -DBUILD_SHARED_LIBS=FALSE -DBUILD_TESTS=FALSE -DBUILD_PERF_TESTS=FALSE -DBUILD_OPENJPEG=ON $androidexts -DCMAKE_BUILD_TYPE=$buildtype ..
cmake $generatortoken -DCMAKE_INSTALL_PREFIX=../$opencvdir-$arch -DBUILD_SHARED_LIBS=FALSE -DBUILD_TESTS=FALSE -DBUILD_PERF_TESTS=FALSE -DBUILD_OPENJPEG=ON $androidexts -DCMAKE_BUILD_TYPE=$buildtype ..

ninja install