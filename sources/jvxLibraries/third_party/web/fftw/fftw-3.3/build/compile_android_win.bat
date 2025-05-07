@echo off
cd ..

echo Arguments: "%* %~1 %~2"

REM %~1: archtoken
set archtoken=%~1
shift

REM %~2: Build type
set buildtype=%~1
shift

REM %~2: Build type
set targetfolder=%~1
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

REM %~3: All other arguments: Android cmake options, to be forwarded using %*
cd %targetfolder%
if not exist "%archtoken%" mkdir "%archtoken%"
cd "%archtoken%"
	
echo cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=lib %rest% -DCMAKE_BUILD_TYPE=%buildtype% -DDISABLE_FORTRAN=TRUE ..
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=lib %rest% -DCMAKE_BUILD_TYPE=%buildtype% -DDISABLE_FORTRAN=TRUE ..
echo ninja install
ninja install

echo cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=lib %rest% -DCMAKE_BUILD_TYPE=%buildtype% -DENABLE_FLOAT=TRUE -DDISABLE_FORTRAN=TRUE ..
cmake -G "Ninja" -DCMAKE_INSTALL_PREFIX=lib %rest% -DCMAKE_BUILD_TYPE=%buildtype% -DENABLE_FLOAT=TRUE -DDISABLE_FORTRAN=TRUE ..
echo ninja install
ninja install

REM Source for loop code to collect rest:
REM x.AI: Ich schreibe ein Windows Batch Skript mit Argumenten. Die ersten zwei Argumente sind fest, alle folgenden beliebig. Das Skript soll nun die ersten zwei Argumente in Variablen speichern, dann jedoch das Kommando command mit allen folgenden Argumenten aufrufen. %* zur Adressierung aller Argumente liefert nur alle Argumente, ich bräuchte etwas für die Adressierung aller Argumente bis auf die ersten beiden.
REM x.AI: Bei einem Windows Batch Skript werden = Zeichen entfernt, wenn man die Übergabeargumente einzeln adressiert. Kann man das verhindern?
REM Conclusion: I can not really prevent the batch to remove the "=" sign. However, we assume that all entries here are pairs for assignments