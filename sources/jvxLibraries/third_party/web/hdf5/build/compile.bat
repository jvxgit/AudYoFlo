echo off
REM set arch=x64
set arch=%~1
REM set rel=MDd
set rel=%~2

REM set zlib_root="%CD%\..\zlib-win64\lib\zlibstat.lib"
set zlib_root="%CD%\..\zlib-windows\binaries\%arch%\zlibstat%rel%.lib"

REM echo cmake -G "Visual Studio 17 2022" -A "x64" -DZLIB_LIBRARY=%zlib_root% -DCMAKE_INSTALL_PREFIX=".\x64" -DCMAKE_BUILD_TYPE=Debug ..\hdf5-1.14.2
REM cmake -G "Visual Studio 17 2022" -A "x64" -DZLIB_LIBRARY=%zlib_root% -DCMAKE_INSTALL_PREFIX=".\x64" -DCMAKE_BUILD_TYPE=Debug ..\hdf5-1.14.2

echo cmake -G "Ninja" -DZLIB_LIBRARY=%zlib_root% -DCMAKE_INSTALL_PREFIX="../%~3/%arch%_%rel%" -DCMAKE_BUILD_TYPE=Debug ../%~3
cmake -G "Ninja" -DZLIB_LIBRARY=%zlib_root% -DCMAKE_INSTALL_PREFIX="../%~3/%arch%_%rel%" -DCMAKE_BUILD_TYPE=Debug ../%~3
ninja install