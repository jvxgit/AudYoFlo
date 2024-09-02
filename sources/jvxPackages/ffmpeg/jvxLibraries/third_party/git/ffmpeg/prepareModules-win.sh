#!/bin/bash  

# Rn this script as follows:
# msys2_shell.cmd -use-full-path -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh"

postfix=$1
release_mode=$2
compile_flags=$3
folder="ffmpeg-$postfix"

# Activate the verbose mode!!
set -x

echo "Checking for existence of folder $folder"
if [ ! -d $folder ]; then
	
	# Commit id = 05438db02437e241a418e266a354bf4e7be7ac59
	git  clone https://github.com/FFmpeg/FFmpeg.git ffmpeg
	cd ffmpeg
	git checkout 05438db02437e241a418e266a354bf4e7be7ac59
	cd ..
	
	patch -R -p0 --binary < ffmpeg-fft-oc.patch
	patch -R -p0 --binary <  ffmpeg-dbg-ffprobe.patch
	patch -R -p0 --binary <  ffmpeg-dbg-fdctdsp.patch
	patch -R -p0 --binary <  ffmpeg-dbg-hevcdsp.patch
	patch -R -p0 --binary <  ffmpeg-dbg-v210i.patch
	patch -R -p0 --binary < ffmpeg-dbg-v210enci.patch
	patch -R -p0 --binary <  ffmpeg-dbg-vs1dsp.patch
	patch -R -p0 --binary < ffmpeg-dbg-vf-convi.patch
	patch -R -p0 --binary <  ffmpeg-dbg-vf-gbluri.patch
	patch -R -p0 --binary <  ffmpeg-dbg-rtmp.patch

	mv ffmpeg $folder
else
	echo "Folder $folder already exists"
fi

if [ -d $folder ]; then
	
	cd $folder
	
	echo "Checking for existence of subfolder lib"
	if [ ! -d "lib" ]; then
		
		if [ ! -d "x264/install-$release_mode" ]; then
			# Extensions in ffmpeg to install libx264
			# -> https://www.roxlu.com/2016/057/compiling-x264-on-windows-with-msvc		
			git clone http://git.videolan.org/git/x264.git
			pushd .
			cd x264
			CC=cl ./configure --enable-static --extra-cflags=$compile_flags --prefix=${PWD}/install-$release_mode
			make 
			make install
			popd
		fi
		
		export PKG_CONFIG_PATH=${PWD}/x264/install-$release_mode/lib/pkgconfig:$PKG_CONFIG_PATH
		 
		if [ ! -d "x265_git/build/msys-cl/install-$release_mode" ]; then
			git clone https://bitbucket.org/multicoreware/x265_git.git
			pushd .
			cd x265_git/build/msys-cl
		
			# The following line can be modified to run directly
			# ./make-Makefiles-64bit.sh
			cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=$release_mode -DCMAKE_CXX_FLAGS="-DWIN32 -D_WINDOWS -W4 -GR -EHsc" -DCMAKE_C_FLAGS="-DWIN32 -D_WINDOWS -W4 $compile_flags"  ../../source -DCMAKE_INSTALL_PREFIX=./install-$release_mode
			nmake 
			nmake install
		
			cp install-$release_mode/lib/libx265.lib install-$release_mode/lib/x265.lib
			popd
		fi

		export PKG_CONFIG_PATH=${PWD}/x265_git/build/msys-cl/install-$release_mode/lib/pkgconfig:$PKG_CONFIG_PATH
		echo PKG_CONFIG_PATH = $PKG_CONFIG_PATH
	    
		## Test pkf-config of a problem is reported
		## pkg-config --debug x264

		# H264 -> 
		# https://stackoverflow.com/questions/50693934/different-h264-encoders-in-ffmpeg
		# https://superuser.com/questions/512368/unknown-encoder-libx264-on-windows
		./configure --target-os=win64 --arch=x86_64 --toolchain=msvc --enable-gpl --enable-libx264 --enable-libx265 --extra-cflags=-MDd --prefix=./ --disable-htmlpages --disable-manpages
		
		#echo ./configure --target-os=win64 --arch=x86_64 --enable-debug=3 --toolchain=msvc --prefix=./
		#./configure --target-os=win64 --arch=x86_64 --enable-debug=3 --toolchain=msvc --prefix=./
	
		#
		# Options other than the standard (as delivered with ffmpeg) are not really supported in Windows. If you 
		# want to build the lib in another way, you can modify ffbuild/config.mak between configure and make.
		# For example, if you want to generate debug information and disable optimization, you can set the entry
		#
		# CFLAGS= -nologo -Zi -MDd -W3 -wd4018 -wd4146 -wd4244 -wd4305 -wd4554 -utf-8
		#
		# and remove the -02 flag!!
		#
		# Also, I need to introduce some dirty hacks to compile in Windows to prevent undefined references which are 
		# not reported in Release mode
		#
		# Patches:
		#
		#
		# Warning: O:\ayfdevelop\AudYoFlo\sources\jvxPackages\ffmpeg\jvxLibraries\third_party\git\ffmpeg\ffmpeg-x64\config.h(1): warning C4828: Die Datei enthält ein Zeichen, das bei Offset 0x20e beginnt. Dieses Zeichen ist im aktuellen Quellzeichensatz (Codepage 65001) unzulässig
		# We can remove this warning by removing the "für" in config.h, assignment CC_IDENT
		#
		
		echo make
		make -j8
	
		echo make install
		make install
	else
		echo "Subfolder lib already exists"
	fi
fi
