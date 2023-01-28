echo off
set NEWNAME=myNewProject
set INSTALLORIGINFOLDER="%JVX_BASE_ROOT%\..\audio\standalone-projects\fullProjectTemplate"
if "%~1"=="" GOTO no_data_copy
set NEWNAME="%~1"

if "%~2"=="" GOTO no_data_copy
set INSTALLORIGINFOLDER="%~2"

: no_data_copy
REM Check current folder
set ABS_PATH_INSTALL=%CD%

echo "--> Running project generation in folder %ABS_PATH_INSTALL%"
echo "--> Project generation token: %NEWNAME%"
echo "--> Installation origin folder: %INSTALLORIGINFOLDER%"

REM dir %ABS_PATH_INSTALL%
if not exist %ABS_PATH_INSTALL%\project_VC110 md %ABS_PATH_INSTALL%\project_VC110
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\project_VC110\jvx-tpl.sln > %ABS_PATH_INSTALL%\project_VC110\jvx-%NEWNAME%.sln
if not exist %ABS_PATH_INSTALL%\project_VC140 md %ABS_PATH_INSTALL%\project_VC140
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\project_VC140\jvx-tpl.sln > %ABS_PATH_INSTALL%\project_VC140\jvx-%NEWNAME%.sln

REM Generate all files for the APPLICATION
if not exist %ABS_PATH_INSTALL%\sources md %ABS_PATH_INSTALL%\sources
if not exist %ABS_PATH_INSTALL%\sources\jvxApplications md %ABS_PATH_INSTALL%\sources\jvxApplications
if not exist %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME% md %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%
if not exist %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC110 md %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC110
if not exist %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC110\jvx%NEWNAME% md %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC110\jvx%NEWNAME%
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC110\jvxNEWTPL\build-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC110\jvx%NEWNAME%\build-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC110\jvxNEWTPL\build-win64.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC110\jvx%NEWNAME%\build-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC110\jvxNEWTPL\cleanbuild-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC110\jvx%NEWNAME%\cleanbuild-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC110\jvxNEWTPL\cleanbuild-win64.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC110\jvx%NEWNAME%\cleanbuild-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC110\jvxNEWTPL\rebuild-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC110\jvx%NEWNAME%\rebuild-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC110\jvxNEWTPL\rebuild-win64.bat ^
 	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC110\jvx%NEWNAME%\rebuild-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC110\jvxNEWTPL\jvxNEWTPL.vcxproj ^
 	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC110\jvx%NEWNAME%\jvx%NEWNAME%.vcxproj
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC110\jvxNEWTPL\jvxNEWTPL.vcxproj.filters ^
 	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC110\jvx%NEWNAME%\jvx%NEWNAME%.vcxproj.filters
if not exist %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC140 md %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC140
if not exist %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC140\jvx%NEWNAME% md %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC140\jvx%NEWNAME%
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC140\jvxNEWTPL\build-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC140\jvx%NEWNAME%\build-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC140\jvxNEWTPL\build-win64.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC140\jvx%NEWNAME%\build-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC140\jvxNEWTPL\cleanbuild-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC140\jvx%NEWNAME%\cleanbuild-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC140\jvxNEWTPL\cleanbuild-win64.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC140\jvx%NEWNAME%\cleanbuild-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC140\jvxNEWTPL\rebuild-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC140\jvx%NEWNAME%\rebuild-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC140\jvxNEWTPL\rebuild-win64.bat ^
 	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC140\jvx%NEWNAME%\rebuild-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC140\jvxNEWTPL\jvxNEWTPL.vcxproj ^
 	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC140\jvx%NEWNAME%\jvx%NEWNAME%.vcxproj
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\project_VC140\jvxNEWTPL\jvxNEWTPL.vcxproj.filters ^
 	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\project_VC140\jvx%NEWNAME%\jvx%NEWNAME%.vcxproj.filters
if not exist %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\src md %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\src
xcopy /Y /Q  %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\src\myCentralWidget.h %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\src\myCentralWidget.h*
xcopy /Y /Q  %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\src\myCentralWidget.cpp %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\src\myCentralWidget.cpp*
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\src\jvxAudioHost_hooks.cpp ^
 	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\src\jvxAudioHost_hooks.cpp
if not exist %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\uisrc md %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\uisrc
xcopy /Y /Q  %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\uisrc\images ^
	%ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\uisrc\images\
xcopy /Y /Q  %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\uisrc\jvxNEWTPL.qrc ^
	%ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\uisrc\jvx%NEWNAME%.qrc*
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\uisrc\myMainWidget.ui ^
 	> %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\uisrc\myMainWidget.ui
	
if not exist %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\xcode md %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\xcode

sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxApplications\jvxNEWTPL\CMakeLists.txt > %ABS_PATH_INSTALL%\sources\jvxApplications\jvx%NEWNAME%\CMakeLists.txt

REM Generating all files for jvxAudioNode
if not exist %ABS_PATH_INSTALL%\sources\jvxComponents md %ABS_PATH_INSTALL%\sources\jvxComponents
if not exist %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes md %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes
if not exist %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME% md %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%
xcopy /Y /Q  %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\codeGen\exports_node.pcg ^
	%ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\codeGen\exports_node.pcg*
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\codeGen\export_project.mcg ^
 	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\codeGen\export_project.mcg
xcopy /Y /Q  %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\matlab-in ^
	%ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\matlab-in\
xcopy /Y /Q  %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\matlab-local ^
	%ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\matlab-local\
if not exist %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC110 md %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC110
if not exist %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC110\jvxAuN%NEWNAME% md %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC110\jvxAuN%NEWNAME%
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC110\jvxAuNNEWTPL\build-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC110\jvxAuN%NEWNAME%\build-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC110\jvxAuNNEWTPL\build-win64.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC110\jvxAuN%NEWNAME%\build-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC110\jvxAuNNEWTPL\cleanbuild-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC110\jvxAuN%NEWNAME%\cleanbuild-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC110\jvxAuNNEWTPL\cleanbuild-win64.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC110\jvxAuN%NEWNAME%\cleanbuild-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC110\jvxAuNNEWTPL\rebuild-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC110\jvxAuN%NEWNAME%\rebuild-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC110\jvxAuNNEWTPL\rebuild-win64.bat ^
 	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC110\jvxAuN%NEWNAME%\rebuild-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC110\jvxAuNNEWTPL\jvxAuNNEWTPL.vcxproj ^
 	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC110\jvxAuN%NEWNAME%\jvxAuN%NEWNAME%.vcxproj
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC110\jvxAuNNEWTPL\jvxAuNNEWTPL.vcxproj.filters ^
 	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC110\jvxAuN%NEWNAME%\jvxAuN%NEWNAME%.vcxproj.filters
if not exist %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC140 md %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC140
if not exist %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC140\jvxAuN%NEWNAME% md %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC140\jvxAuN%NEWNAME%
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC140\jvxAuNNEWTPL\build-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC140\jvxAuN%NEWNAME%\build-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC140\jvxAuNNEWTPL\build-win64.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC140\jvxAuN%NEWNAME%\build-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC140\jvxAuNNEWTPL\cleanbuild-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC140\jvxAuN%NEWNAME%\cleanbuild-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC140\jvxAuNNEWTPL\cleanbuild-win64.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC140\jvxAuN%NEWNAME%\cleanbuild-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC140\jvxAuNNEWTPL\rebuild-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC140\jvxAuN%NEWNAME%\rebuild-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC140\jvxAuNNEWTPL\rebuild-win64.bat ^
 	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC140\jvxAuN%NEWNAME%\rebuild-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC140\jvxAuNNEWTPL\jvxAuNNEWTPL.vcxproj ^
 	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC140\jvxAuN%NEWNAME%\jvxAuN%NEWNAME%.vcxproj
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\project_VC140\jvxAuNNEWTPL\jvxAuNNEWTPL.vcxproj.filters ^
 	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\project_VC140\jvxAuN%NEWNAME%\jvxAuN%NEWNAME%.vcxproj.filters
if not exist %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\src md %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\src
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\src\CjvxAuNNEWTPL.cpp ^
 	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\src\CjvxAuN%NEWNAME%.cpp
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\src\CjvxAuNNEWTPL.h ^
 	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\src\CjvxAuN%NEWNAME%.h
if not exist %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\target md %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\target
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\target\componentEntry.cpp ^
 	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\target\componentEntry.cpp
if not exist %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\install md %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\install
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\install\config-NEWTPL.jvx ^
 	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\install\config-%NEWNAME%.jvx
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxComponents\jvxAudioNodes\jvxAuNNEWTPL\CMakeLists.txt ^
	> %ABS_PATH_INSTALL%\sources\jvxComponents\jvxAudioNodes\jvxAuN%NEWNAME%\CMakeLists.txt

REM Generating all files for jvxAudioNode
if not exist %ABS_PATH_INSTALL%\sources\jvxLibraries md %ABS_PATH_INSTALL%\sources\jvxLibraries
if not exist %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME% md %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%
xcopy /Y /Q  %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\doc %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\doc\
if not exist %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\include md %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\include
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\include\jvx_NEWTPL.h ^
 	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\include\jvx_%NEWNAME%.h
if not exist %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\pcg md %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\pcg
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\pcg\jvx_NEWTPL.pcgf ^
 	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\pcg\jvx_%NEWNAME%.pcgf
if not exist %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\src md %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\src
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\src\jvx_NEWTPL.c ^
 	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\src\jvx_%NEWNAME%.c
if not exist %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC110 md %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC110
if not exist %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC110\jvx-%NEWNAME% md %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC110\jvx-%NEWNAME%
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC110\jvx-NEWTPL\build-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC110\jvx-%NEWNAME%\build-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC110\jvx-NEWTPL\build-win64.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC110\jvx-%NEWNAME%\build-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC110\jvx-NEWTPL\cleanbuild-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC110\jvx-%NEWNAME%\cleanbuild-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC110\jvx-NEWTPL\cleanbuild-win64.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC110\jvx-%NEWNAME%\cleanbuild-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC110\jvx-NEWTPL\rebuild-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC110\jvx-%NEWNAME%\rebuild-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC110\jvx-NEWTPL\rebuild-win64.bat ^
 	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC110\jvx-%NEWNAME%\rebuild-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC110\jvx-NEWTPL\jvx-NEWTPL.vcxproj ^
 	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC110\jvx-%NEWNAME%\jvx-%NEWNAME%.vcxproj
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC110\jvx-NEWTPL\jvx-NEWTPL.vcxproj.filters ^
 	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC110\jvx-%NEWNAME%\jvx-%NEWNAME%.vcxproj.filters
xcopy /Y /Q %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\doc %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\doc\
if not exist %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC140 md %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC140
if not exist %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC140\jvx-%NEWNAME% md %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC140\jvx-%NEWNAME%
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC140\jvx-NEWTPL\build-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC140\jvx-%NEWNAME%\build-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC140\jvx-NEWTPL\build-win64.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC140\jvx-%NEWNAME%\build-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC140\jvx-NEWTPL\cleanbuild-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC140\jvx-%NEWNAME%\cleanbuild-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC140\jvx-NEWTPL\cleanbuild-win64.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC140\jvx-%NEWNAME%\cleanbuild-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC140\jvx-NEWTPL\rebuild-win32.bat ^
	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC140\jvx-%NEWNAME%\rebuild-win32.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC140\jvx-NEWTPL\rebuild-win64.bat ^
 	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC140\jvx-%NEWNAME%\rebuild-win64.bat
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC140\jvx-NEWTPL\jvx-NEWTPL.vcxproj ^
 	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC140\jvx-%NEWNAME%\jvx-%NEWNAME%.vcxproj
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\project_VC140\jvx-NEWTPL\jvx-NEWTPL.vcxproj.filters ^
 	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\project_VC140\jvx-%NEWNAME%\jvx-%NEWNAME%.vcxproj.filters
xcopy /Y /Q  %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\doc %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\doc\
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL\CMakeLists.txt ^
	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%\CMakeLists.txt

if not exist %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab md %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab
if not exist %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab\+examples md %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab\+examples
xcopy /Y /Q  %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL-matlab\+examples\jvx_demo_NEWTPL.m %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab\+examples\jvx_demo_%NEWNAME%.m*
if not exist %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab\+jvx_%NEWNAME% md %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab\+jvx_%NEWNAME%
xcopy /Y /Q  %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL-matlab\+jvx_NEWTPL\+core %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab\+jvx_%NEWNAME%\+core\

if not exist %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab\cmake md %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab\cmake
if not exist %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab\cmake\matlab md %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab\cmake\matlab
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL-matlab\cmake\matlab\CMakeLists.txt ^
	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab\cmake\matlab\CMakeLists.txt
if not exist %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab\cmake\octave md %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab\cmake\octave
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL-matlab\cmake\octave\CMakeLists.txt ^
	> %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab\cmake\octave\CMakeLists.txt
xcopy /Y /Q  %INSTALLORIGINFOLDER%\sources\jvxLibraries\jvx-NEWTPL-matlab\doc %ABS_PATH_INSTALL%\sources\jvxLibraries\jvx-%NEWNAME%-matlab\doc\

if not exist %ABS_PATH_INSTALL%\standalone md %ABS_PATH_INSTALL%\standalone
if not exist %ABS_PATH_INSTALL%\standalone\cmake md %ABS_PATH_INSTALL%\standalone\cmake
sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\standalone\cmake\CMakeLists.txt ^
 	> %ABS_PATH_INSTALL%\standalone\cmake\CMakelists.txt
if not exist %ABS_PATH_INSTALL%\standalone\build md %ABS_PATH_INSTALL%\standalone\build
if not exist %ABS_PATH_INSTALL%\standalone\build\windows\ md %ABS_PATH_INSTALL%\standalone\build\windows
xcopy /Y /Q  %INSTALLORIGINFOLDER%\standalone\build\windows %ABS_PATH_INSTALL%\standalone\build\windows\

sed s/JVXTPL/%NEWNAME%/g %INSTALLORIGINFOLDER%\CMakeLists.txt ^
	> %ABS_PATH_INSTALL%\CMakeLists.txt
xcopy /Y /Q  %INSTALLORIGINFOLDER%\.jvxprj %ABS_PATH_INSTALL%\

