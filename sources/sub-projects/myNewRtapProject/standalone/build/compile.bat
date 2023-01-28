echo off

REM ============================================
REM Run build script for standalone project
REM ============================================

REM ============================================
REM Convert relative path into absolute path,
REM recipe found in 
REM http://stackoverflow.com/questions/1645843/resolve-absolute-path-from-relative-path-and-or-file-name
REM ============================================

set JVX_GENERATOR_TOKEN="RTAP_RPL_GENERATOR_TOKEN"
REM "Visual Studio 14 2015 Win64"
REM "Visual Studio 11 2012 Win64"
REM "Visual Studio 14 2015 Win64"
REM "NMake Makefiles"

set JVX_SDK_PATH="RTAP_RPL_SDK_PATH"
set JVX_RUNTIME_PATH="RTAP_RPL_RUNTIME_PATH"
RTAP_RPL_ADD_OPTION

echo "=============================="
echo "RTProc SDK path=%JVX_SDK_PATH%"
echo "RTProc cmake generator token=%JVX_GENERATOR_TOKEN%"
REM ============================================
REM SDK path is in %ABS_PATH%
REM ============================================
echo cmake -DCMAKE_BUILD_TYPE=Debug .. -G %JVX_GENERATOR_TOKEN% -Wno-dev -DJVX_SDK_RUNTIME_PATH=%JVX_RUNTIME_PATH% -DJVX_SDK_PATH=%JVX_SDK_PATH%/sdk -DJVX_MATLAB_PROPERTY_GENERATOR=TRUE %JVX_ADD_OPTION%
echo "=============================="

cmake -DCMAKE_BUILD_TYPE=Debug .. -G %JVX_GENERATOR_TOKEN% -Wno-dev -DJVX_SDK_RUNTIME_PATH=%JVX_RUNTIME_PATH% -DJVX_SDK_PATH=%JVX_SDK_PATH%/sdk -DJVX_MATLAB_PROPERTY_GENERATOR=TRUE %JVX_ADD_OPTION%



