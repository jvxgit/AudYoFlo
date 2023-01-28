echo off

REM ============================================
REM Run build script for standalone project
REM ============================================

REM ============================================
REM Convert relative path into absolute path,
REM recipe found in 
REM http://stackoverflow.com/questions/1645843/resolve-absolute-path-from-relative-path-and-or-file-name
REM ============================================

set REL_PATH=..\..\env-JVXTPL\
set ABS_PATH=

rem // Save current directory and change to target directory
pushd %REL_PATH%

rem // Save value of CD variable (current directory)
set ABS_PATH=%CD%

rem // Restore original directory
popd

REM ============================================
REM SDK path is in %ABS_PATH%
REM ============================================


REM cmake -DCMAKE_BUILD_TYPE=JVXTPL ../project -G "Visual Studio 11" ^
REM cmake -DCMAKE_BUILD_TYPE=JVXTPL ../project -G "Visual Studio 11 2012 Win64" ^
cmake -DCMAKE_BUILD_TYPE=JVXTPL ../../project -G "NMake Makefiles" ^
   -Wno-dev ^
   -DJVX_SDK_RUNTIME_PATH=%ABS_PATH%/runtime ^
   -DJVX_SDK_PATH=%ABS_PATH%/sdk ^
   -DJVX_COMPILE_LIB_MATLAB=TRUE

REM nmake
