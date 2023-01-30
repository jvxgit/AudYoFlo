echo off
REM !!!!! THE FOLLOWING LINE TO BE ADAPTED ACCORDING TO BUILD SYSTEM !!!!!
set JVX_SRC_PATH=../AudYoFlo
REM !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

REM =========================================================
REM The following paths are used by cmake to find the SDKs/header files
REM for different parts of the system. All specifications may also be 
REM realized via environment variables
REM =========================================================
set PATH=%PATH%;%JVX_SRC_PATH%/bin/tools
set QT_PATH_64=R:\Qt\Qt-5.15.2

REM Optional: set option JVX_USE_PYTHON to TRUE
set PYBIND_PATH=C:\Users\jsjav\AppData\Local\Packages\PythonSoftwareFoundation.Python.3.10_qbz5n2kfra8p0\LocalCache\local-packages\Python310\site-packages\pybind11\share\cmake/pybind11

REM Optional: set option JVX_USE_PART_MATLAB to true!
set MATLAB_PATH_64=C:\Program Files\MATLAB\R2018b

REM Optional: set option JVX_USE_PART_OCTAVE to true!
REM set OCTAVE_PATH_64=C:\whereever that is

REM Optional: set option JVX_USE_PCAP to true
REM set PCAP_LIBRARY_PATH=R:\npcap-sdk

REM Optional: set option JVX_USE_BOOST to true
REM set BOOST_LIBRARY_PATH=R:\boost_1_70_0

REM Optional: set option JVX_USE_EIGEN to TRUE
REM set EIGEN_LIBRARY_PATH=R:\eigen-eigen-5a0156e40feb

REM Optional: set option JVX_FLUTTER_UI to TRUE
REM set FLUTTER_SDK_PATH=C:\develop\flutter\3.0.1-stable\flutter

REM ========================================================
REM Specify all feature options here
REM ========================================================
set TOKEN_SDK=-DJVX_RELEASE_SDK=FALSE
set TOKEN_BTOOLS=-DJVX_COMPILE_BUILDTOOLS=FALSE
set ADDITIONAL_FEATURE_TOKENS=^
	-DJVX_USE_CONSOLE_APPS=TRUE ^
	-DJVX_USE_PART_OCTAVE=FALSE ^
	-DJVX_USE_PART_MATLAB=TRUE ^
	-DJVX_USE_PCAP=FALSE ^
	-DJVX_USE_PART_WEBSERVER=TRUE ^
	-DJVX_USE_BOOST=FALSE ^
	-DJVX_USE_EIGEN=FALSE ^
	-DJVX_DEPLOY_EXAMPLES=TRUE ^
	-DJVX_USE_PART_CMINPACK=TRUE ^
	-DJVX_SPLASH_SUPPRESSED=FALSE ^
	-DJVX_MATLAB_PROPERTY_GENERATOR=TRUE ^
	-DJVX_INCLUDE_EBUR128=TRUE ^
	-DJVX_USE_VST=TRUE ^
	-DSMTG_CREATE_PLUGIN_LINK=FALSE ^
	-DJVX_FLUTTER_UI=FALSE ^
	-DJVX_USE_PYTHON=TRUE ^
	-DJVX_LIBHOST_WITH_JSON_SHOW=TRUE ^
	-DJVX_USE_PART_WASAPI=TRUE ^
	-DJVX_USE_LIBMYSOFA=TRUE ^
	-DJVX_USE_EAQUAL=FALSE ^
	-DJVX_COMPILE_GOD_HOST=TRUE
	
REM ####################### Compile options for BT builds
set JVX_GENERATOR_TOKEN_BT="Ninja"
set JVX_GENERATOR_ARCHITECTURE_BT=

REM ####################### Compile options for RT build
REM set JVX_GENERATOR_TOKEN_RT="Ninja"
REM set JVX_GENERATOR_ARCHITECTURE_RT=
set JVX_GENERATOR_TOKEN_RT="Visual Studio 17 2022"
set JVX_GENERATOR_ARCHITECTURE_RT=-A x64

REM ####################### Compile options for SDK build
set JVX_GENERATOR_TOKEN_SDK="Ninja"
set JVX_GENERATOR_ARCHITECTURE_SDK=

set JVX_GENERATOR_BUILDTYPE=
set JVX_RELEASE_TYPE=Debug

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
set JVX_GENERATOR_TOKEN=%JVX_GENERATOR_TOKEN_SDK%
set JVX_GENERATOR_ARCHITECTURE=%JVX_GENERATOR_ARCHITECTURE_SDK%
goto endswitch
:cmake_sdk_bt
set TOKEN_SDK=-DJVX_RELEASE_SDK=TRUE 
set TOKEN_BTOOLS=-DJVX_COMPILE_BUILDTOOLS=TRUE
set JVX_GENERATOR_TOKEN=%JVX_GENERATOR_TOKEN_BT%
set JVX_GENERATOR_ARCHITECTURE=%JVX_GENERATOR_ARCHITECTURE_BT%
goto endswitch
:cmake_rt_bt
set TOKEN_SDK=-DJVX_RELEASE_SDK=FALSE 
set TOKEN_BTOOLS=-DJVX_COMPILE_BUILDTOOLS=TRUE
set JVX_GENERATOR_TOKEN=%JVX_GENERATOR_TOKEN_BT%
set JVX_GENERATOR_ARCHITECTURE=%JVX_GENERATOR_ARCHITECTURE_BT%
goto endswitch
:cmake_rt
set JVX_GENERATOR_TOKEN=%JVX_GENERATOR_TOKEN_RT%
set JVX_GENERATOR_ARCHITECTURE=%JVX_GENERATOR_ARCHITECTURE_RT%

:endswitch


cmake -DCMAKE_BUILD_TYPE=%JVX_RELEASE_TYPE% %JVX_SRC_PATH% ^
  -G %JVX_GENERATOR_TOKEN% %JVX_GENERATOR_ARCHITECTURE% ^
  -Wno-dev -DJVX_DEPLOY_UNSTABLE=TRUE ^
  -DCMAKE_INSTALL_PREFIX=release ^
  -DJVX_PRODUCT=audio ^
  -DJVX_USE_PART_ASIO=TRUE  ^
  -DJVX_USE_PART_PAUDIO=TRUE ^
  -DJVX_USE_PART_RS232=TRUE ^
  -DJVX_USE_QT=TRUE ^
  -DJVX_USE_QWT=TRUE ^
  -DJVX_USE_QWT_POLAR=TRUE ^
  -DJVX_USE_PART_CORECONTROL=TRUE ^
  %TOKEN_SDK% ^
  -DJVX_DEFINE_NUMBER_32BIT_TOKENS_BITFIELD=16 ^
  %TOKEN_BTOOLS% ^
  %ADDITIONAL_FEATURE_TOKENS% ^
  -DJVX_DEPLOY_LIBRARY_DSP_MATLAB=TRUE 

if %JVX_GENERATOR_TOKEN% == "NMake Makefiles"  jom /J 9 install
if %JVX_GENERATOR_TOKEN% == "Ninja"  ninja install

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
del *.vcxproj.user >nul 2>&1
del *.sln >nul 2>&1
del *.VC.db >nul 2>&1
del rules.ninja >nul 2>&1
del build.ninja >nul 2>&1
del .ninja_deps >nul 2>&1
del .ninja_log >nul 2>&1
del CMakeDoxyfile.* >nul 2>&1
del CMakeDoxygenDefaults.* >nul 2>&1
del CPack* >nul 2>&1

rmdir sources /s /q  >nul 2>&1
rmdir sub-projects /s /q  >nul 2>&1
rmdir bin /s /q  >nul 2>&1
rmdir lib /s /q  >nul 2>&1
rmdir cmake /s /q  >nul 2>&1
rmdir CMakeFiles /s /q  >nul 2>&1
rmdir VST3 /s /q  >nul 2>&1
rmdir x64 /s /q  >nul 2>&1
rmdir .vs /s /q  >nul 2>&1
rmdir WIN_PDB64 /s /q  >nul 2>&1

:cc
del CMakeCache.txt >nul 2>&1

goto end

:error
echo Usage: compile ^< rt-bt ^| rt ^| sdk-bt ^| sdk ^| clean ^| distclean ^> 
:end

