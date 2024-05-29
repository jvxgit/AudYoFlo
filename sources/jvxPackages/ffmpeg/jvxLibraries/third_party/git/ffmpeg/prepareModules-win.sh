#!/bin/bash  

# Rn this script as follows:
# msys2_shell.cmd -use-full-path -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh"

#postfix="x64"
#if [ $# -ge 1 ]; then 
	postfix=$1
# fi

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
		
		./configure --target-os=win64 --arch=x86_64 --toolchain=msvc --prefix=./ --disable-htmlpages --disable-manpages
		
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

		echo make
		make
	
		echo make install
		make install
	else
		echo "Subfolder lib already exists"
	fi
fi
