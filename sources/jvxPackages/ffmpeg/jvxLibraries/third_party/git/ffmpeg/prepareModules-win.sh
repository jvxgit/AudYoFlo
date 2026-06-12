#!/bin/bash  

# Rn this script as follows:
# msys2_shell.cmd -use-full-path -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules-win.sh"

postfix=$1
release_mode=$2
compile_flags=$3
h265_path=$4
folder="ffmpeg-$postfix"
compile_flags_c=(
		"$compile_flags"
		)
compile_flags_cxx=(
		"$compile_flags"
		)
end_flags_ffmpeg=""
end_flags_h264=""


# compile_flags='-MDd -Z7 -D_DEBUG -D_ITERATOR_DEBUG_LEVEL=2'
if [ "$compile_flags" = "-MDd" ]; then
	compile_flags_c=(
		"-MDd"
		"-Od"
		"-Z7"
		"-D_DEBUG"
	)
	compile_flags_cxx=(
		"-MDd"
		"-Od"
		"-Z7"
		"-D_DEBUG"
		"-D_ITERATOR_DEBUG_LEVEL=2"
	)
	end_flags_ffmpeg="--disable-optimizations"	
	end_flags_h264="--enable-debug"
fi
	
echo "--> ${compile_flags[*]}"

# Activate the verbose mode!!
# set -x

echo "Checking for existence of folder $folder"
if [ ! -d $folder ]; then
	
	# FFmpeg 8.1 release
	git  clone https://github.com/FFmpeg/FFmpeg.git ffmpeg
	cd ffmpeg
	
	# Previous version: git checkout 507a51fbe9732f0f6f12f43ce12431e8faa834b7
	git checkout 17734f696752e996a37f80077c2bf116444ad340 
	
	# This patch is required to build ffmpeg with H.264 and H.265 - otherwise a 
	# warning breaks the build!
	git apply ../0001-Removed-warning-to-be-taken-as-error-in-configure-sc.patch
	cd ..
	
	# Old patches - still there during next weeks for testing
	#patch -R -p0 --binary < ffmpeg-fft-oc.patch
	#patch -R -p0 --binary <  ffmpeg-dbg-ffprobe.patch
	#patch -R -p0 --binary <  ffmpeg-dbg-fdctdsp.patch
	#patch -R -p0 --binary <  ffmpeg-dbg-hevcdsp.patch
	#patch -R -p0 --binary <  ffmpeg-dbg-v210i.patch
	#patch -R -p0 --binary < ffmpeg-dbg-v210enci.patch
	#patch -R -p0 --binary <  ffmpeg-dbg-vs1dsp.patch
	#patch -R -p0 --binary < ffmpeg-dbg-vf-convi.patch
	#patch -R -p0 --binary <  ffmpeg-dbg-vf-gbluri.patch
	#patch -R -p0 --binary <  ffmpeg-dbg-rtmp.patch

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
			echo ./configure --enable-static --extra-cflags="${compile_flags_c[*]}" --prefix=${PWD}/install-$release_mode ${end_flags_h264}
			CC=cl ./configure --enable-static --extra-cflags="${compile_flags_c[*]}" --prefix=${PWD}/install-$release_mode ${end_flags_h264}
			make 
			make install
			make clean
			popd
		fi
		
		export PKG_CONFIG_PATH=${PWD}/x264/install-$release_mode/lib/pkgconfig:$PKG_CONFIG_PATH
		 
		# New logic for x265: The x265 librry must be built in Windows in advance as it requires Windows cmake version
		# Then, we pass the relevant path to find x265 also in ffmpeg. Note: The windows version does not install x665.lib, therefore,
		# the pc file temüplate must be patched in x265!!
		
		echo "H265-Path: $h265_path"
		export PKG_CONFIG_PATH=${PWD}/$h265_path/pkgconfig:$PKG_CONFIG_PATH
		echo PKG_CONFIG_PATH = $PKG_CONFIG_PATH
		
		# Output all pkg-config findings for special features
		pkg-config --libs x264
		pkg-config --libs x265

		
		# H264 -> 
		# https://stackoverflow.com/questions/50693934/different-h264-encoders-in-ffmpeg
		# https://superuser.com/questions/512368/unknown-encoder-libx264-on-windows
		echo ./configure --target-os=win64 --arch=x86_64 --toolchain=msvc --enable-shared --disable-static --enable-gpl --enable-libx264 --enable-libx265 --extra-cflags="${compile_flags_c[*]}" --extra-cxxflags="${compile_flags_cxx[*]}" --prefix=./ --disable-htmlpages --disable-manpages ${end_flags_ffmpeg}
		./configure --target-os=win64 --arch=x86_64 --toolchain=msvc --enable-shared --disable-static --enable-gpl --enable-libx264 --enable-libx265 --extra-cflags="${compile_flags_c[*]}" --extra-cxxflags="${compile_flags_cxx[*]}" --prefix=./ --disable-htmlpages --disable-manpages ${end_flags_ffmpeg}
		
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
		sleep 2
		echo make -j8
		make -j8 V=1 2>&1 | tee  ffmpeg-build.log
	
		echo make install
		make install
		
		echo make clean
		make clean
		
		# We need this to generate some code, seems to be a bug otherwise
		make V=1 fftools/resources/graph.html.o
		make V=1 fftools/resources/graph.css.o
		
	else
		echo "Subfolder lib already exists"
	fi
fi
