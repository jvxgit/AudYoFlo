@echo off

for %%i in (*) do (
	if not "%%i" == "clean.bat" if not "%%i" == "README.md" if not "%%i" == "compile.bat" (
		del "%%i"
	)
)
for /d %%i in (*) do (
	rmdir /s /q %%i
)