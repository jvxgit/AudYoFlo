echo on

echo Running ORC meson build for install folder %~1
cd orc

REM python ..\meson\meson.py setup build -Ddefault_library=static --backend vs --prefix=%~1 
python ..\meson\meson.py setup build -Ddefault_library=shared --prefix=%~1 

cd build
python ..\..\meson\meson.py install
	
