echo off
REM !!!!! THE FOLLOWING LINE TO BE ADAPTED ACCORDING TO BUILD SYSTEM !!!!!
set JVX_SRC_PATH=../AudYoFlo
REM !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

set PATH=%PATH%;%JVX_SRC_PATH%/bin/tools
set QT_PATH_64=R:\Qt\Qt-5.15.2
REM R:\Qt\Qt5.13.2\5.13.2\msvc2017_64
set MATLAB_PATH_64=C:\Program Files\MATLAB\R2018b
set PCAP_LIBRARY_PATH=R:\npcap-sdk
set BOOST_LIBRARY_PATH=R:\boost_1_70_0
set EIGEN_LIBRARY_PATH=R:\eigen-eigen-5a0156e40feb
REM set PYBIND_PATH=%JVX_SRC_PATH%/../jvxrt-submodules/audio/sources/jvxApplications/python/jvx-dsp-python/python/venv/Lib/site-packages/pybind11/share/cmake/pybind11
set PYBIND_PATH=C:\Users\jsjav\AppData\Local\Packages\PythonSoftwareFoundation.Python.3.10_qbz5n2kfra8p0\LocalCache\local-packages\Python310\site-packages\pybind11\share\cmake/pybind11

set TOKEN_SDK=-DJVX_RELEASE_SDK=FALSE
set TOKEN_BTOOLS=-DJVX_COMPILE_BUILDTOOLS=FALSE
set ADDITIONAL_FEATURE_TOKENS=^
	-DJVX_USE_CONSOLE_APPS=TRUE ^
	-DJVX_USE_PART_OCTAVE=TRUE ^
	-DJVX_USE_PART_MATLAB=TRUE ^
	-DJVX_USE_PCAP=TRUE ^
	-DJVX_USE_PART_WEBSERVER=TRUE ^
	-DJVX_USE_BOOST=TRUE ^
	-DJVX_USE_EIGEN=TRUE ^
	-DJVX_DEPLOY_EXAMPLES=TRUE ^
	-DJVX_USE_PART_CMINPACK=TRUE ^
	-DJVX_SPLASH_SUPPRESSED=FALSE ^
	-DJVX_MATLAB_PROPERTY_GENERATOR=TRUE ^
	-DJVX_INCLUDE_EBUR128=TRUE ^
	-DJVX_USE_VST=TRUE ^
	-DSMTG_CREATE_PLUGIN_LINK=FALSE ^
	-DJVX_FLUTTER_UI=TRUE ^
	-DJVX_USE_PYTHON=TRUE ^
	-DJVX_LIBHOST_WITH_JSON_SHOW=TRUE ^
	-DJVX_INSTALLER_MBC=TRUE ^
	-DJVX_USE_PART_WASAPI=TRUE ^
	-DJVX_USE_LIBMYSOFA=TRUE ^
	-DLIBMYSOFA_BUILD_TESTS=FALSE ^
	-DAYFBINRENDER_SIMPLE_INSTALLER=FALSE ^
	-DJVX_USE_EAQUAL=TRUE ^
	-DJVX_COMPILE_GOD_HOST=TRUE
	
REM -DFERNLIVE_SIMPLE_INSTALLER=TRUE ^
	
REM -DBUILD_SHARED_LIBS=FALSE ^
REM -DJVX_USE_CONSOLE_APPS=FALSE 
REM -DJVX_DE0_CUSTOMLEDS_DRIVER_FOLDER=U:\jvx-driver\glnx86\jvx-de0-customleds ^

REM -DGLOBAL_COMPILE_DEFINITIONS=VERBOSE_BUFFERING_REPORT
REM -DQT_INSTALL_PATH=P:\Qt\Qt5.12.2\5.12.2\msvc2017_64

REM ===================================================================
REM -DJVX_HOST_WITH_NO_ARGS=TRUE ^
REM	-DJVX_DEPLOY_EXAMPLES=TRUE ^
REM	-DJVX_USE_PART_WEBSERVER=TRUE ^
REM	-DJVX_USE_BOOST=TRUE ^
REM	-DJVX_USE_OPENGL_X11=TRUE ^
REM	-DJVX_USE_EIGEN=TRUE ^
REM	-DJVX_USE_PCAP=TRUE ^
REM	-DJVX_USE_PART_CMINPACK=TRUE ^
REM	-DJVX_COMPILE_GOD_HOST=TRUE  ^
REM	-DJVX_DE0_CUSTOMLEDS_DRIVER_FOLDER=O:\jvx-driver\glnx86\jvx-de0-customleds ^
REM	-DJVX_MATLAB_PROPERTY_GENERATOR=TRUE
REM ===================================================================


REM Options: 
REM -DJVX_USE_CONSOLE_APPS=TRUE -> Attach console to executables to output runtime prints
REM	-DJVX_DEPLOY_EXAMPLES=TRUE -> Compile and release example applications
REM -DJVX_USE_PART_WEBSERVER=TRUE -> Compile and attach web server component
REM	-DJVX_USE_BOOST=TRUE -> Use boost library
REM -DJVX_USE_VIDEO=TRUE -> Use video functionality (capture & opengl rendering)
REM -DJVX_USE_EIGEN=TRUE -> Use Eigen library for some math functions
REM -DJVX_USE_PART_OCTAVE=TRUE -> Compile and release octave entries
REM -DJVX_USE_PART_MATLAB=TRUE -> Compile and release Matlab entries
REM -DJVX_USE_PCAP=TRUE -> Compile and use pcap low level ethernet protocol
REM -DJVX_USE_PART_CMINPACK=TRUE -> Attach cminpack library
REM -DJVX_COMPILE_GOD_HOST=TRUE  -> Compile and release GODs HOST
REM -DJVX_MATLAB_PROPERTY_GENERATOR=TRUE -> Run the mtlab property access functions
REM -DGLOBAL_COMPILE_DEFINITIONS=VERBOSE_BUFFERING_REPORT
REM -DQT_INSTALL_PATH=P:\Qt\Qt5.12.2\5.12.2\msvc2017_64 ^

	
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

REM set JVX_GENERATOR_TOKEN="Ninja"
REM set JVX_GENERATOR_ARCHITECTURE=
REM set JVX_GENERATOR_TOKEN="NMake Makefiles"
REM set JVX_GENERATOR_ARCHITECTURE=
set JVX_GENERATOR_BUILDTYPE=
set JVX_RELEASE_TYPE=Debug
REM set JVX_GENERATOR_TOKEN="Ninja"
REM JVX_GENERATOR_TOKEN="\"Unix Makefiles\""

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

REM Run the cmake command
REM	-G "NMake Makefiles" ^
REM	-G "Visual Studio 14 2015 Win64" ^
 
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

  REM -DJVX_SYSTEM_USE_DATA_FORMAT_FLOAT=TRUE ^
  REM jom clean
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

REM 
REM OPTIONS="-DCMAKE_INSTALL_PREFIX=release\
REM  -G ${JVX_GENERATOR_TOKEN} -DCMAKE_BUILD_TYPE=Debug\
REM  -DJVX_PRODUCT=industrial\
REM  -DJVX_DSP_BASE_USE_DATA_FORMAT_FLOAT=TRUE\
REM  -DJVX_USE_PART_OCTAVE=FALSE\
REM  -DJVX_USE_PART_MATLAB=FALSE\
REM  -DJVX_USE_PART_ALSA=TRUE\
REM  -DJVX_USE_PART_PAUDIO=TRUE\
REM  -DJVX_USE_PART_CMINPACK=FALSE\
REM  -DJVX_USE_QT=TRUE\
REM  -DJVX_USE_QWT=TRUE\
REM  -DJVX_USE_QWT_POLAR=TRUE\
REM  -DJVX_DEPLOY_LIBRARY_DSP_MATLAB=FALSE\
REM  -DJVX_USE_PCAP=FALSE\
REM  ${TOKEN_SDK}\
REM  -DJVX_MATLAB_PROPERTY_GENERATOR=TRUE\
REM  -DJVX_DEFINE_NUMBER_32BIT_TOKENS_BITFIELD=16 \
REM  ${TOKEN_BTOOLS}\
REM  -DJVX_DEPLOY_UNSTABLE=TRUE\
REM  ${ADDITIONAL_FEATURE_TOKENS}\
REM  -Wno-dev \
REM  -DJVX_AUDIO_WITH_GST=FALSE"

    