echo off

REM ============================================
REM Run build script for standalone project
REM ============================================

set JVX_GENERATOR_TOKEN="JVX_RPL_GENERATOR_TOKEN"
REM "Visual Studio 14 2015 Win64"
REM "Visual Studio 11 2012 Win64"
REM "Visual Studio 14 2015 Win64"
REM "NMake Makefiles"

set JVX_SDK_PATH="JVX_RPL_SDK_PATH"

echo "=============================="
echo "RTProc SDK path=%JVX_SDK_PATH%"
echo "RTProc cmake generator token=%JVX_GENERATOR_TOKEN%"
echo "=============================="
REM ============================================
REM SDK path is in %ABS_PATH%
REM ============================================
cmake -DCMAKE_BUILD_TYPE=Release .. -G %JVX_GENERATOR_TOKEN% -Wno-dev -DJVX_SDK_RUNTIME_PATH=%JVX_SDK_PATH%/runtime -DJVX_SDK_PATH=%JVX_SDK_PATH%/sdk -DJVX_MATLAB_PROPERTY_GENERATOR=TRUE 
