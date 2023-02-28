#!/bin/bash  

if [ ! -d "ffmpeg" ]; then

	# Commit id = 05438db02437e241a418e266a354bf4e7be7ac59
	echo git  clone https://git.ffmpeg.org/ffmpeg.git ffmpeg
	git  clone https://git.ffmpeg.org/ffmpeg.git ffmpeg
	
	echo patch -R -p0 --binary < ffmpeg-fft-oc.patch
	patch -R -p0 --binary < ffmpeg-fft-oc.patch

fi

if [ -d "ffmpeg" ]; then
	
	echo cd ffmpeg
	cd ffmpeg
	
	if [ ! -d "lib" ]; then
		echo ./configure --target-os=win64 --arch=x86_64 --toolchain=msvc --prefix=./
		./configure --target-os=win64 --arch=x86_64 --toolchain=msvc --prefix=./
	
		echo make
		make
	
		echo make install
		make install
	fi
fi
