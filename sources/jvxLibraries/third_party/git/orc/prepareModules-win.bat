echo on

echo Running ORC meson build for install folder %~1 %~2 %~3 %~4
cd orc

python ..\meson\meson.py setup build%~2 -Ddefault_library=static -Db_vscrt=%~3 --reconfigure --prefix=%~1 

cd build%~2
python ..\..\meson\meson.py install

cd ..



	
