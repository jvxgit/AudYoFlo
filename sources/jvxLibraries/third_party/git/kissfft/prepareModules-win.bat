if not exist "build_%~1_%~3_%~4" mkdir "build_%~1_%~3_%~4"

REM "prepareModules-win.bat ${archtoken} ${targetfolder} ${dataformattoken}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}

copy build\compile.bat "build_%~1_%~3_%~4"
cd "build_%~1_%~3_%~4"
echo compile.bat "%~1" "%~2" "%~3" "%~4"
compile.bat "%~1" "%~2" "%~3" "%~4"

REM  "x64" "kissfft" "double"
REM compile.bat "%~1" "%~2" "%~3"