#!/bin/bash  

# Rn this script as follows:
# msys2_shell.cmd -use-full-path -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh"

if [ ! -d "ffmpeg" ]; then

	# Commit id = 05438db02437e241a418e266a354bf4e7be7ac59
	echo git  clone https://git.ffmpeg.org/ffmpeg.git ffmpeg
	git  clone https://git.ffmpeg.org/ffmpeg.git ffmpeg
	
	echo patch -R -p0 --binary < ffmpeg-fft-oc.patch
	patch -R -p0 --binary < ffmpeg-fft-oc.patch

	echo patch -R -p0 --binary <  ffmpeg-dbg-ffprobe.patch
	patch -R -p0 --binary <  ffmpeg-dbg-ffprobe.patch

	echo patch -R -p0 --binary <  ffmpeg-dbg-fdctdsp.patch
	patch -R -p0 --binary <  ffmpeg-dbg-fdctdsp.patch

	echo patch -R -p0 --binary <  ffmpeg-dbg-hevcdsp.patch
	patch -R -p0 --binary <  ffmpeg-dbg-hevcdsp.patch

	echo patch -R -p0 --binary  <  ffmpeg-dbg-v210i.patch
	patch -R -p0 --binary <  ffmpeg-dbg-v210i.patch

	echo patch -R -p0 --binary < ffmpeg-dbg-v210enci.patch
	patch -R -p0 --binary < ffmpeg-dbg-v210enci.patch

	echo patch -R -p0 --binary <  ffmpeg-dbg-vs1dsp.patch
	patch -R -p0 --binary <  ffmpeg-dbg-vs1dsp.patch

	echo patch -R -p0 --binary < ffmpeg-dbg-vf-convi.patch
	patch -R -p0 --binary < ffmpeg-dbg-vf-convi.patch

	echo patch -R -p0 --binary <  ffmpeg-dbg-vf-gbluri.patch
	patch -R -p0 --binary <  ffmpeg-dbg-vf-gbluri.patch

	echo patch -R -p0 --binary <  ffmpeg-dbg-rtmp.patch
	patch -R -p0 --binary <  ffmpeg-dbg-rtmp.patch

fi

if [ -d "ffmpeg" ]; then
	
	echo cd ffmpeg
	cd ffmpeg
	
	if [ ! -d "lib" ]; then
		
		echo ./configure --target-os=win64 --arch=x86_64 --toolchain=msvc --prefix=./
		./configure --target-os=win64 --arch=x86_64 --toolchain=msvc --prefix=./
		
		#echo ./configure --target-os=win64 --arch=x86_64 --enable-debug=3 --toolchain=msvc --prefix=./
		#./configure --target-os=win64 --arch=x86_64 --enable-debug=3 --toolchain=msvc --prefix=./
	
		#
		# Options other than the standard (as delivered with ffmpeg) are not really supported in Windows. If you 
		# want to build the lib in another way, you can modify ffbuild/config.mk between configure and make.
		# For example, if you want to generate debug information and disable optimization, you can set the entry
		# CFLAGS= -nologo -Zi -MDd -W3 -wd4018 -wd4146 -wd4244 -wd4305 -wd4554 -utf-8
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
	fi
fi
