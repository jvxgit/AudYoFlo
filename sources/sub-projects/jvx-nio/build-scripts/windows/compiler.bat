echo off
set TOKEN_SDK=-DJVX_RELEASE_SDK=FALSE 
set TOKEN_BTOOLS=-DJVX_COMPILE_BUILDTOOLS=FALSE
set ADDITIONAL_FEATURE_TOKENS=-DJVX_MEASURE_AUDIO_WITH_MASTERNETWORK=TRUE -DJVX_USE_VIDEO=TRUE
set JVX_GENERATOR_TOKEN="Visual Studio 14 2015 Win64"
REM set JVX_GENERATOR_TOKEN="Visual Studio 11 2012"
REM set JVX_GENERATOR_TOKEN="NMake Makefiles"

IF "%~1"=="rt" GOTO cmake_rt
IF "%~1"=="sdk" GOTO cmake_sdk
IF "%~1"=="sdk-bt" GOTO cmake_sdk_bt
IF "%~1"=="rt-bt" GOTO cmake_rt_bt
IF "%~1"=="clean" GOTO clean
IF "%~1"=="cc" GOTO cc
IF "%~1"=="distclean" GOTO distclean
GOTO error

:cmake_sdk
set TOKEN_SDK=-DJVX_RELEASE_SDK=TRUE 
goto endswitch
:cmake_sdk_bt
set TOKEN_SDK=-DJVX_RELEASE_SDK=TRUE 
set TOKEN_BTOOLS=-DJVX_COMPILE_BUILDTOOLS=TRUE
set JVX_GENERATOR_TOKEN="NMake Makefiles"
goto endswitch
:cmake_rt_bt
set TOKEN_SDK=-DJVX_RELEASE_SDK=FALSE 
set TOKEN_BTOOLS=-DJVX_COMPILE_BUILDTOOLS=TRUE
set JVX_GENERATOR_TOKEN="NMake Makefiles"
goto endswitch
:cmake_rt

:endswitch

REM Run the cmake command
REM	-G "NMake Makefiles" ^
REM	-G "Visual Studio 14 2015 Win64" ^
  
 cmake -DCMAKE_BUILD_TYPE=Release ../.. ^
  -G %JVX_GENERATOR_TOKEN% ^
  -Wno-dev -DJVX_DEPLOY_UNSTABLE=TRUE ^
  -DCMAKE_INSTALL_PREFIX=release ^
  -DJVX_PRODUCT=audio ^
  -DJVX_USE_PART_OCTAVE=TRUE ^
  -DJVX_USE_PART_MATLAB=TRUE ^
  -DJVX_USE_PART_ASIO=TRUE  ^
  -DJVX_USE_PART_PAUDIO=TRUE ^
  -DJVX_USE_PART_RS232=TRUE ^
  -DJVX_USE_PART_CORECONTROL=TRUE ^
  -DJVX_USE_QT=TRUE ^
  -DJVX_USE_QWT=TRUE ^
  -DJVX_USE_QWT_POLAR=TRUE ^
  -DJVX_USE_PCAP=TRUE ^
  -DJVX_USE_PART_CMINPACK=TRUE ^
  %TOKEN_SDK% ^
  -DJVX_MATLAB_PROPERTY_GENERATOR=TRUE ^
  -DJVX_DEFINE_NUMBER_32BIT_TOKENS_BITFIELD=16 ^
  %TOKEN_BTOOLS% ^
  %ADDITIONAL_FEATURE_TOKENS% ^
  -DJVX_DEPLOY_LIBRARY_DSP_MATLAB=TRUE 

  REM jom clean
if %JVX_GENERATOR_TOKEN% == "NMake Makefiles"  jom /J 9 install

goto end

:distclean
rmdir release /s /q  >nul 2>&1

:clean
del cmake_install.cmake >nul 2>&1
del Makefile >nul 2>&1
del install_manifest_release.txt >nul 2>&1
del install_manifest*.txt >nul 2>&1
del jvx_dsp_base.h >nul 2>&1
del octave.lib >nul 2>&1
del octinterp.lib >nul 2>&1
del *.vcxproj >nul 2>&1
del *.vcxproj.filters >nul 2>&1
del *.sln >nul 2>&1
del *.VC.db >nul 2>&1

rmdir base /s /q  >nul 2>&1
rmdir cmake /s /q  >nul 2>&1
rmdir audio /s /q  >nul 2>&1
rmdir bdx /s /q  >nul 2>&1
rmdir CMakeFiles /s /q  >nul 2>&1
rmdir x64 /s /q  >nul 2>&1
rmdir .vs /s /q  >nul 2>&1

:cc
del CMakeCache.txt >nul 2>&1

goto end

:error
echo Usage: compile ^< rt-bt ^| rt ^| sdk-bt ^| sdk ^| clean ^| distclean ^> 
:end