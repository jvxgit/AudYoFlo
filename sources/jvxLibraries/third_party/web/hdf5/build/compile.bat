echo off
REM set arch=x64
set arch=%~1
set hdfdir=%~2
set zlibdir=%~3
set buildtype=%~4
set arch_simple=%~5

REM set zlib_root_lib="%CD%\..\..\..\platform-packages\msvc\zlib\msvc2017_%arch_simple%\lib\zlib\zlib.lib"
set zlib_root_lib="%zlibdir%\lib\zlib\zlib.lib"
set zlib_root_incdir="%CD%\..\..\..\platform-packages\msvc\zlib\msvc2017_%arch_simple%\include\zlib"
REM set generatortoken=-G "Visual Studio 16 2019" -A "win32"
set generatortoken=-G "Ninja"

del CMakeCache.txt
echo cmake %generatortoken% -DZLIB_LIBRARY=%zlib_root_lib% -DZLIB_INCLUDE_DIR=%zlib_root_incdir% -DCMAKE_INSTALL_PREFIX="../%hdfdir%/%arch%" -DCMAKE_BUILD_TYPE=%buildtype% ../%hdfdir%
cmake %generatortoken% -DZLIB_LIBRARY=%zlib_root_lib% -DZLIB_INCLUDE_DIR=%zlib_root_incdir% -DCMAKE_INSTALL_PREFIX="../%hdfdir%/%arch%" -DCMAKE_BUILD_TYPE=%buildtype% ../%hdfdir%
ninja install
