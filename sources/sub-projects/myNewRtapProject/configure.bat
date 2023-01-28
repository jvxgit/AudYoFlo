@echo off

REM Check current folder
set ABS_PATH_INSTALL="%CD%"
set RTAP_SDK_PATH=%ABS_PATH_INSTALL%/../release
set PTSED=sed

echo "--> Running standalone project generation in folder %ABS_PATH_INSTALL%"

for %%a in (%ABS_PATH_INSTALL%) do (
    set project_token=%%~nxa
)    
echo "--> Project token: %project_token%"

REM Create build folder
if not exist %ABS_PATH_INSTALL%\build md %ABS_PATH_INSTALL%\build

REM Specify path reference to SDK folder
set sdkpath=%RTAP_SDK_PATH%
set /p sdkpath="--> Specify location of RTProc SDK folder [%sdkpath%]:"
if "%sdkpath%"=="" GOTO cmakegetgenerator

REM Get absolute path to SDK
REM Taken from: https://stackoverflow.com/questions/1645843/resolve-absolute-path-from-relative-path-and-or-file-name
set REL_PATH="%sdkpath%"
set ABS_PATH=
pushd "%REL_PATH%"
set ABS_PATH="%CD%"
popd
set RTAP_SDK_PATH="%ABS_PATH%"

if exist %JVX_SDK_PATH%\tools\sed.exe set PTSED=%JVX_SDK_PATH%\tools\sed

:cmakegetgenerator
echo "--> RTProc SDK folder: %RTAP_SDK_PATH%"
if not exist %RTAP_SDK_PATH%\ GOTO error_no_sdk_path
if not exist %RTAP_SDK_PATH%\sdk\ GOTO error_no_sdk_path
if not exist %RTAP_SDK_PATH%\runtime\ GOTO error_no_sdk_path

REM look for sed and add to path
%PTSED% --help >nul 2>nul || if exist %RTAP_SDK_PATH%\tools\ set PATH=%PATH%;%RTAP_SDK_PATH%\tools
%PTSED% --help >nul 2>nul || echo "Error: sed.exe was not found. Copy sed to %RTAP_SDK_PATH%\tools\ or to your path." && GOTO :leave_script

REM Specify path reference to RUNTIME folder
set RTAP_RUNTIME_PATH=%RTAP_SDK_PATH%\runtime
set rtpath=%RTAP_RUNTIME_PATH%
set /p rtpath="--> Specify location of RTProc RUNTIME folder [%rtpath%]:"
if "%rtpath%"=="" GOTO usedefaultrtpath

REM Get absolute path to RUNTIME
REM Taken from: https://stackoverflow.com/questions/1645843/resolve-absolute-path-from-relative-path-and-or-file-name
set REL_PATH="%rtpath%"
set ABS_PATH=
pushd "%REL_PATH%"
set ABS_PATH="%CD%"
popd
set RTAP_RUNTIME_PATH="%ABS_PATH%"


:usedefaultrtpath
echo "--> RTProc RUNTIME folder: %RTAP_RUNTIME_PATH%"

REM Need to check a file in the runtime folder to find read/write
REM TODO

REM Check if SDK folder is read-only and set options accordingly
set RTAP_RPL_ADD_OPTION="set JVX_ADD_OPTION="
%JVX_SDK_PATH%\tools\accesschk64.exe -w -nobanner %username% %JVX_SDK_PATH%\jvx_install_version.txt > ___j__v__x__tmp.txt
set /p check_RW= <  ___j__v__x__tmp.txt
del /Q ___j__v__x__tmp.txt
REM echo %check_RW%
if "%check_RW%"=="""" set RTAP_RPL_ADD_OPTION="set JVX_ADD_OPTION=-DJVX_NO_WRITE_SDK=TRUE"
echo "--> Additional compile options: %RTAP_RPL_ADD_OPTION%"

REM Specify cmake generator token
set /p cmakegenerator="--> Specify cmake Generator token[1:NMake Makefiles; 2: Visual Studio 2015 Win64; 3: Visual Studio 2012 Win64; 4: Visual Studio 2017 Win64; 5: Visual Studio 2017 win32]:"

if "%cmakegenerator%"=="1" GOTO nmake
if "%cmakegenerator%"=="2" GOTO vs2015
if "%cmakegenerator%"=="3" GOTO vs2012
if "%cmakegenerator%"=="4" GOTO vs2017_64
if "%cmakegenerator%"=="5" GOTO vs2017_32

:nmake
set CMAKE_TOKEN="NMake Makefiles"
GOTO replaceandleave

:vs2012
set CMAKE_TOKEN="Visual Studio 11 2012 Win64"
GOTO replaceandleave

:vs2015
set CMAKE_TOKEN="Visual Studio 14 2015 Win64"
GOTO replaceandleave

:vs2017_64
set CMAKE_TOKEN="Visual Studio 15 2017 Win64"
GOTO replaceandleave

:vs2017_32
set CMAKE_TOKEN="Visual Studio 15 2017"
GOTO replaceandleave

REM Involve the settings to become part of the compile script
:replaceandleave
set "RTAP_SDK_PATH=%RTAP_SDK_PATH:\=/%"
set "RTAP_RUNTIME_PATH=%RTAP_RUNTIME_PATH:\=/%"

echo "--> RTProc SDK path is %RTAP_SDK_PATH%"
echo "--> CMake Generator token choice: %CMAKE_TOKEN%"

%PTSED% s@RTAP_RPL_GENERATOR_TOKEN@%CMAKE_TOKEN%@g standalone\build\compile.bat > ___j__v__x__tmp.bat || ( goto :error_sed_failed )
%PTSED% s@RTAP_RPL_SDK_PATH@%RTAP_SDK_PATH%@g ___j__v__x__tmp.bat > ____j__v__x__tmp.bat || ( goto :error_sed_failed )
%PTSED% s@RTAP_RPL_RUNTIME_PATH@%RTAP_RUNTIME_PATH%@g ____j__v__x__tmp.bat > ___j__v__x__tmp.bat || ( goto :error_sed_failed )
%PTSED% s@RTAP_RPL_ADD_OPTION@%RTAP_RPL_ADD_OPTION%@g ___j__v__x__tmp.bat > build\compile.bat || ( goto :error_sed_failed )

del /Q ___j__v__x__tmp.bat
del /Q ____j__v__x__tmp.bat

echo "--> Overwriting CMakeLists file in project folder"
xcopy /Y /Q /E standalone\cmake\CMakeLists.txt .  || ( goto :error_xcopy_failed )

cd build
echo "--> Type <compile.bat> to run cmake"

GOTO leave_script

:error_no_sdk_path
echo "Error: <%RTAP_SDK_PATH%> is not a valid sdk path!"
GOTO leave_script

:error_no_cmake_token
echo "Error: <%cmakegenerator%> is not a valid cmake generator token!"
GOTO leave_script

:error_sed_failed
echo "Error: sed command failed."
GOTO leave_script

:error_xcopy_failed
echo "Error: xcopy /Y /Q /E standalone\cmake\CMakeLists.txt . failed!"

:leave_script


