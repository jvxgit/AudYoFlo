git clone https://git.ffmpeg.org/ffmpeg.git ffmpeg

1) Install MSYS2
2) Install yasm, make, base-devel

	pacman -S yasm
	pacman -S make
	pacman -S base-devel
	pacman -S unzip
3) Add path to msys2 to PATH environment variable.

4) In msys2.ini, uncomment line 
MSYS2_PATH_TYPE=inherit

5) In x64 native command shell, in this folder, run 

msys2_shell.cmd -use-full-path -where "%cd%" -c "./prepareModules.sh"

This will start msys2 in the current folder, inherit all enviroment variables and run the build procedure
Refer to prepareModules.sh for further details!