echo running prepareModules-x265-win.bat with arguments %~1, %~2 and %~3
REM if not exist "%~2\build_%~1" mkdir "%~2\build_%~1"
REM cd ffmpeg-%~1

git clone https://bitbucket.org/multicoreware/x265_git.git x265_git_%~1
cd x265_git_%~1

REM Checkout the working revision
git checkout 50087d0b879961afff20f76e2a841eaca301b33e 

REM Apply patch to output a midified .pc file to allow the lib to be involved in ffmpeg
git apply ../libx265-pc-generator.patch

cd build/msys-cl
		
REM The following line can be modified to run directly
REM ./make-Makefiles-64bit.sh
			
REM Here is another hint if the x265 does not build: we MUST use the cmake version from Visual Studio.
REM Otherwise, the option for nmake will not be detected.
REM If you install cmake in the msys2 environment it will shadow VS cake as the local folders precede the inherited options in the 
REM PATH variable
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=%~2 -DCMAKE_CXX_FLAGS="-DWIN32 -D_WINDOWS -W4 -GR -EHsc %~3" -DCMAKE_C_FLAGS="-DWIN32 -D_WINDOWS -W4 %~3"  ../../source -DCMAKE_INSTALL_PREFIX=./install-%~1
ninja  
ninja install
ninja clean
		
REM 			cp install-$release_mode/lib/libx265.lib install-$release_mode/lib/x265.lib
	
