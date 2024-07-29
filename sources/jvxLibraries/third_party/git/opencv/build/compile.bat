echo off
REM set arch=x64
set arch=%~1
set opencvdir=%~2
set buildtype=%~3

set generatortoken=-G "Ninja"
del CMakeCache.txt

REM https://stackoverflow.com/questions/7583172/opencv-as-a-static-library-cmake-options
REM https://stackoverflow.com/questions/19772398/build-the-static-opencv-library-with-multi-threaded-debug-dll-mdd-c-runtime-l

echo cmake %generatortoken% -DCMAKE_INSTALL_PREFIX=./opencv -DBUILD_SHARED_LIBS=FALSE -DBUILD_WITH_STATIC_CRT=FALSE -DCMAKE_BUILD_TYPE=%buildtype% ..
cmake %generatortoken% -DCMAKE_INSTALL_PREFIX=./opencv -DBUILD_SHARED_LIBS=FALSE -DBUILD_WITH_STATIC_CRT=FALSE -DCMAKE_BUILD_TYPE=%buildtype% ..

ninja install
