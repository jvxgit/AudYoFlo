echo off
REM set arch=x64
set arch=%~1
set opencvdir=%~2

set generatortoken=-G "Ninja"
del CMakeCache.txt

echo cmake %generatortoken% -DCMAKE_INSTALL_PREFIX=./install -DCMAKE_BUILD_TYPE=Debug ../%opencvdir%
cmake %generatortoken% -DCMAKE_INSTALL_PREFIX=./install -DCMAKE_BUILD_TYPE=Debug -DBUILD_OPENJPEG=ON ../opencv

ninja install
