echo off
REM set arch=x64
set arch=%~1

set zlib_root_lib="%CD%\..\..\..\platform-packages\msvc\zlib\msvc2017_%arch%\lib\zlib\zlib.lib"
set zlib_root_incdir="%CD%\..\..\..\platform-packages\msvc\zlib\msvc2017_%arch%\include\zlib"
REM set generatortoken=-G "Visual Studio 16 2019" -A "win32"
set generatortoken=-G "Ninja"

del CMakeCache.txt
echo cmake %generatortoken% -DZLIB_LIBRARY=%zlib_root_lib% -DZLIB_INCLUDE_DIR=%zlib_root_incdir% -DCMAKE_INSTALL_PREFIX="../%~2/%arch%" -DCMAKE_BUILD_TYPE=Debug ../%~2
cmake %generatortoken% -DZLIB_LIBRARY=%zlib_root_lib% -DZLIB_INCLUDE_DIR=%zlib_root_incdir% -DCMAKE_INSTALL_PREFIX="../%~2/%arch%" -DCMAKE_BUILD_TYPE=Debug ../%~2
ninja install
