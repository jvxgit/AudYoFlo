echo off

REM Setup windows for direct use of Windows SDK - without Visual Studio
REM 1) 1) Download cmake and install locally,
REM https://cmake.org/download/
REM 2) Download ninja and install locally,
REM https://github.com/ninja-build/ninja/releases
REM 3) Get the tool vsget to download the Windows SDK without Visual studio and with not admin rights required.
REM git clone https://github.com/reksar/vsget.git
REM 4) Run 
REM vsget "sdk"
REM 5) In the Windos SDK, add the line
REM set "WindowsSDK_IncludePath=%SDKInclude%winrt\;%WindowsSDK_IncludePath%"
REM in the section to declare all includes, line 103
REM 6) Copy folder vscode to AudYoFlo/.vscode
REM 7) Copy this file to the build folder

REM !!!!! THE FOLLOWING LINE TO BE ADAPTED ACCORDING TO BUILD SYSTEM !!!!!
set JVX_SRC_PATH=%CD%\..\AudYoFlo
REM !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

set FLUTTER_VERSION_STRING=3.29.3
set PATH=%PATH%;%JVX_SRC_PATH%/bin/tools;C:\develop\sdk-and-more\cmake-3.31.1-windows-x86_64\bin;C:\develop\sdk-and-more;C:\develop\sdk-and-more\msys64
set QT_PATH_64=%JVX_SRC_PATH%\sources\jvxLibraries\third_party\web\qt\qt\5.15.2\msvc2019_64
set FLUTTER_SDK_PATH=%JVX_SRC_PATH%\sources\jvxLibraries\third_party\web\flutter\flutter-%FLUTTER_VERSION_STRING%

set ANDROID_NDK=C:/develop/android/ndk/29.0.14206865
set ANDROID_API_VERSION=29

REM set ANDROID_ABI=armeabi-v7a
REM set ANDROID_ABI=arm64-v8a
set ANDROID_ABI=x86_64

REM Add some helping paths to find executables
set PATH=%PATH%;%JVX_SRC_PATH%/bin/tools

set TOKEN_SDK=-DJVX_RELEASE_SDK=false
set TOKEN_BTOOLS=-DJVX_COMPILE_BUILDTOOLS=false

set ADDITIONAL_FEATURE_TOKENS_MIN=^
	-DJVX_USE_CCACHE=false ^
	-DJVX_USE_CONSOLE_APPS=true ^
	-DJVX_USE_PART_OCTAVE=false ^
	-DJVX_USE_PART_MATLAB=false ^
	-DJVX_USE_SIMULINK=false ^
	-DJVX_USE_PCAP=false ^
	-DJVX_USE_PART_WEBSERVER=true ^
	-DJVX_USE_BOOST=false ^
	-DJVX_USE_EIGEN=true ^
	-DJVX_DEPLOY_EXAMPLES=false ^
	-DJVX_USE_PART_CMINPACK=false ^
	-DJVX_SPLASH_SUPPRESSED=false ^
	-DJVX_SYSTEM_USE_DATA_FORMAT_FLOAT=false^
	-DJVX_MATLAB_PROPERTY_GENERATOR=false ^
	-DJVX_INCLUDE_EBUR128=true ^
	-DJVX_USE_VST=false ^
	-DJVX_FLUTTER_UI=true ^
	-DJVX_USE_PYTHON=false ^
	-DJVX_LIBHOST_WITH_JSON_SHOW=true ^
	-DJVX_USE_PART_WASAPI=false ^
	-DJVX_INSTALL_QT_ON_BUILD=false ^
	-DJVX_INSTALL_FLUTTER_ON_BUILD=false ^
	-DJVX_USE_ORC=false ^
	-DJVX_GENERATE_VERSION_TAG_DUMMY=true ^
	-DJVX_USE_OPENCV=true ^
	-DJVX_USE_OPENGL=false ^
	-DJVX_USE_GLEW_GLUT=false ^
	-DJVX_USE_QT=false ^
	-DJVX_USE_MFVIDEO=false ^
	-DJVX_USE_CBMP=false ^
	-DJVX_COMPILE_GOD_HOST=false 
	
set ADDITIONAL_FEATURE_TOKENS_FULL=^
	^
	-DJVX_USE_LIBMYSOFA=false ^
	-DLIBMYSOFA_BUILD_TESTS=false ^
	^
	-DFLUTTER_SDK_PATH=%FLUTTER_SDK_PATH% ^
	-DJVX_USE_EAQUAL=false ^
	-DJVX_USE_FFMPEG=false ^
	-DJVX_USE_KISSFFT=false ^
	-DJVX_USE_HDF5=false ^
	-DAYFBINRENDER_FFMPEG_FILE_IO=false ^
	-DAYFBINRENDER_HEADTRACKER_RS232=false ^
	-DAYFBINRENDER_HEADTRACKER_ART=false ^
	-DJVX_INSTALLER_MBC=false ^
	-DJVX_USE_AWE=false ^
    -DJVX_INSTALLER_MBC=false ^
	-DJVX_USE_RUST=false ^
	-DJVX_FLUTTER_VERSION=%FLUTTER_VERSION_STRING% ^
	-DJVX_FLUTTER_BUILD_SLEEP_TIME=10 ^
    -DJVX_ALLOW_FLUTTER_SIMPLE_INSTALLERS=false ^
	-DAYFSTEREOAEC_SIMPLE_INSTALLER=false ^
	-DAYFSTARTER_SIMPLE_INSTALLER=false ^
	-DAYFCTC_SIMPLE_INSTALLER=false ^
	-DAYFHOA_SIMPLE_INSTALLER=false ^
	-DAYFAWE_SIMPLE_INSTALLER=false ^
	-DAYF_SIMPLEPHONE_INSTALLER=false ^
	-DJVXMBC_SIMPLE_INSTALLER=false ^
	-DAYFSTARTER_USE_RUST_LIBS=false ^
	-DAYFSTEREOAEC_USE_RUST_LIBS=false ^
	-DSMTG_CREATE_PLUGIN_LINK=false

REM Declare the following arguments 
set ADDITIONAL_FEATURE_TOKENS_UNDEFINED=^
	-DJVX_GCC_LINKER_SYMBOLIC=false ^
	-DAYF_BARESIP=false ^
	-DJVX_USE_HOA_CIRC_ARRAY=false ^
	-DAYFBINRENDER_SIMPLE_INSTALLER=false

set ADDITIONAL_FEATURE_TOKENS_FULL_ANDROID=^
	-DJVX_BUILD_ANDROID=true ^
	-DCMAKE_TOOLCHAIN_FILE_AYFANDROID=%ANDROID_NDK%/build/cmake/android.toolchain.cmake ^
	-DANDROID_ABI_AYFANDROID=%ANDROID_ABI% ^
	-DANDROID_NDK_AYFANDROID=%ANDROID_NDK% ^
	-DANDROID_API_VERSION_AYFANDROID=%ANDROID_API_VERSION% ^
	-DANDROID_PLATFORM_AYFANDROID=android-%ANDROID_API_VERSION% ^
	-DCMAKE_ANDROID_ARCH_ABI_AYFANDROID=%ANDROID_ABI% ^
	-DCMAKE_ANDROID_NDK_AYFANDROID=%ANDROID_NDK% ^
	-DCMAKE_SYSTEM_NAME_AYFANDROID=Android ^
	-DCMAKE_SYSTEM_VERSION_AYFANDROID=%ANDROID_API_VERSION% 
	
REM ####################### Compile options for BT builds
set JVX_GENERATOR_TOKEN_BT="Ninja"
set JVX_GENERATOR_ARCHITECTURE_BT=

REM ####################### Compile options for RT build
REM set JVX_GENERATOR_TOKEN_RT="Ninja"
REM set JVX_GENERATOR_ARCHITECTURE_RT=
set JVX_GENERATOR_TOKEN_RT="Ninja"
set JVX_GENERATOR_ARCHITECTURE_RT=

REM ####################### Compile options for SDK build
set JVX_GENERATOR_TOKEN_SDK="Ninja"
set JVX_GENERATOR_ARCHITECTURE_SDK=

set JVX_GENERATOR_BUILDTYPE=
set JVX_RELEASE_TYPE=Debug

IF "%~1"=="sdk-bt" GOTO cmake_sdk_bt
IF "%~1"=="rt-bt" GOTO cmake_rt_bt
IF "%~1"=="clean" GOTO clean
IF "%~1"=="cc" GOTO cc
IF "%~1"=="distclean" GOTO distclean
GOTO error

:cmake_sdk_bt
set TOKEN_SDK=-DJVX_RELEASE_SDK=true 
set TOKEN_BTOOLS=-DJVX_COMPILE_BUILDTOOLS=true
set JVX_GENERATOR_TOKEN=%JVX_GENERATOR_TOKEN_BT%
set JVX_GENERATOR_ARCHITECTURE=%JVX_GENERATOR_ARCHITECTURE_BT%
set ADDITIONAL_FEATURE_TOKENS=^
	%ADDITIONAL_FEATURE_TOKENS_MIN% ^
	%ADDITIONAL_FEATURE_TOKENS_FULL% ^
	%ADDITIONAL_FEATURE_TOKENS_UNDEFINED% ^
	%ADDITIONAL_FEATURE_TOKENS_FULL_ANDROID%
goto endswitch

:cmake_rt_bt
set TOKEN_SDK=-DJVX_RELEASE_SDK=false 
set TOKEN_BTOOLS=-DJVX_COMPILE_BUILDTOOLS=true
set JVX_GENERATOR_TOKEN=%JVX_GENERATOR_TOKEN_BT%
set JVX_GENERATOR_ARCHITECTURE=%JVX_GENERATOR_ARCHITECTURE_BT%
set ADDITIONAL_FEATURE_TOKENS=^
	%ADDITIONAL_FEATURE_TOKENS_MIN% ^
	%ADDITIONAL_FEATURE_TOKENS_FULL% ^
	%ADDITIONAL_FEATURE_TOKENS_UNDEFINED% ^
	%ADDITIONAL_FEATURE_TOKENS_FULL_ANDROID%
goto endswitch

:endswitch

if not exist %JVX_SRC_PATH%\.vscode mkdir %JVX_SRC_PATH%\.vscode
echo copy vscode\settings.json.android.in %JVX_SRC_PATH%\.vscode
echo copy vscode\script.bat.android.in %JVX_SRC_PATH%\.vscode
echo copy vscode\launch.json %JVX_SRC_PATH%\.vscode
echo on
copy vscode\settings.json.android.in %JVX_SRC_PATH%\.vscode
copy vscode\script.bat.android.in %JVX_SRC_PATH%\.vscode
copy vscode\launch.json %JVX_SRC_PATH%\.vscode
echo off
set runninja="yes"


REM Option --debug-output
REM https://stackoverflow.com/questions/38864489/how-to-trace-cmakelists-txt
REM 
 
REM Remove the CMakeLists.txt - everything else needs to be run in VS Code
del CMakeCache.txt >nul 2>&1

echo on
cmake -DCMAKE_BUILD_TYPE=%JVX_RELEASE_TYPE% %JVX_SRC_PATH% ^
  -G %JVX_GENERATOR_TOKEN% %JVX_GENERATOR_ARCHITECTURE% ^
  -Wno-dev -DJVX_DEPLOY_UNSTABLE=true ^
  -DCMAKE_INSTALL_PREFIX=release ^
  -DJVX_PRODUCT=audio ^
  -DJVX_USE_PART_ASIO=true  ^
  -DJVX_USE_PART_PAUDIO=true ^
  -DJVX_USE_PART_RS232=true ^
  -DJVX_USE_QT=true ^
  -DJVX_USE_QWT=true ^
  -DJVX_USE_QWT_POLAR=true ^
  -DJVX_USE_PART_CORECONTROL=true ^
  %TOKEN_SDK% ^
  -DJVX_DEFINE_NUMBER_32BIT_TOKENS_BITFIELD=16 ^
  %TOKEN_BTOOLS% ^
  %ADDITIONAL_FEATURE_TOKENS% ^
  -DJVX_DEPLOY_LIBRARY_DSP_MATLAB=true 

echo off

  REM -DJVX_SYSTEM_USE_DATA_FORMAT_FLOAT=true ^
  REM jom clean
if %JVX_GENERATOR_TOKEN% == "NMake Makefiles"  jom /J 9 install
if %runninja% == "yes"  ninja install

REM Remove the CMakeLists.txt - everything else needs to be run in VS Code
del CMakeCache.txt >nul 2>&1

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
rmdir flutter /s /q  >nul 2>&1
rmdir third_party /s /q  >nul 2>&1
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
