if not exist "build%~1" mkdir "build%~1"

copy build\compile.bat "build%~1"
cd "build%~1"
echo compile.bat "%~1" "%~2"
compile.bat "%~1" "%~2"

