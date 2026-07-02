@echo off
cd ..

echo Arguments: "%* %~1 %~2 %~3"

REM %~1: archtoken
set archtoken=%~1
shift

REM %~2: Build type
set buildtype=%~1
shift

REM %~3: Build type
set targetfolder=%~1
shift

REM %~3: Build type
set dspacecc=%~1
shift

echo archtoken=%archtoken%
echo buildtype=%buildtype%
echo targetfolder=%targetfolder%

:: Sammle die verbleibenden Argumente
set "rest="
:loop
if "%1"=="" goto endloop
set arg1=%1
shift
set arg2=%1
shift
set rest=%rest% %arg1%=%arg2%
goto loop
:endloop

echo Remaining arguments: "%rest%"

echo "->%CD%"

REM %~3: All other arguments: Android cmake options, to be forwarded using %*

cd %targetfolder%
if not exist "%archtoken%" mkdir "%archtoken%"
cd "%archtoken%"

REM We run the cmake and the build command twice: first run for datatype float, 
cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE="%dspacecc%" -DCMAKE_BUILD_TYPE=%buildtype% -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=OFF -DENABLE_FLOAT=ON -DENABLE_THREADS=OFF -DENABLE_OPENMP=OFF -DCMAKE_INSTALL_PREFIX=lib ..
ninja install 
del CMakeCache.txt

cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE="%dspacecc%" -DCMAKE_BUILD_TYPE=%buildtype% -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=OFF -DENABLE_FLOAT=OFF -DENABLE_THREADS=OFF -DENABLE_OPENMP=OFF -DCMAKE_INSTALL_PREFIX=lib ..
ninja install 
del CMakeCache.txt
cd ..

REM second run for datatype double
REM cmake -S %targetfolder% -B %archtoken%-%buildtype% -G "Ninja" -DCMAKE_TOOLCHAIN_FILE="O:\ayfdevelopbmw\AudYoFlo\sources\sub-projects\ayfanc\Application\DSpace\common\cmake\dspace-toolchain.cmake" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=OFF -DENABLE_FLOAT=OFF -DENABLE_THREADS=OFF -DENABLE_OPENMP=OFF -DCMAKE_INSTALL_PREFIX=%archtoken%-%buildtype%/libs
REM cd %archtoken%-%buildtype%
REM ninja install 
REM del CMakeCache.txt
REM cd ..


REM Source for loop code to collect rest:
REM x.AI: Ich schreibe ein Windows Batch Skript mit Argumenten. Die ersten zwei Argumente sind fest, alle folgenden beliebig. Das Skript soll nun die ersten zwei Argumente in Variablen speichern, dann jedoch das Kommando command mit allen folgenden Argumenten aufrufen. %* zur Adressierung aller Argumente liefert nur alle Argumente, ich bräuchte etwas für die Adressierung aller Argumente bis auf die ersten beiden.
REM x.AI: Bei einem Windows Batch Skript werden = Zeichen entfernt, wenn man die Übergabeargumente einzeln adressiert. Kann man das verhindern?
REM Conclusion: I can not really prevent the batch to remove the "=" sign. However, we assume that all entries here are pairs for assignments