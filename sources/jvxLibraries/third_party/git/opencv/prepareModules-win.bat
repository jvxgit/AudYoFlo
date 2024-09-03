echo running prepareModules-win.bat with arguments %~1, %~2 and %~3
if not exist "%~2\build_%~1" mkdir "%~2\build_%~1"

copy build\compile.bat "%~2\build_%~1"
cd "%~2\build_%~1"
echo compile.bat "%~1" "%~2" "%~3"
compile.bat "%~1" "%~2" "%~3"

cd ..

echo %CD%
dir 

echo rmdir /S /Q build_%~1
rmdir /S /Q build_%~1
