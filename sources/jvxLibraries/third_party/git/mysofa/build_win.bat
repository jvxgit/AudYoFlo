echo off
REM set arch=x64
set target_folder=%~1
set buildtype=%~2
set arch_simple=%~3

cd %target_folder%\build
cmake -DCMAKE_BUILD_TYPE=%buildtype% .. -G "Ninja" -DBUILD_TESTS=FALSE -DBUILD_SHARED_LIBS=FALSE -DCMAKE_INSTALL_PREFIX=%arch_simple%/%buildtype% -Wno-dev
ninja install

