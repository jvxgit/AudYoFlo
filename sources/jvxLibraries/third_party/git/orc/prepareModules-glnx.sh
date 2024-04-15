#!/bin/bash

if [ ! -d "orc/$2" ]; then
	git clone https://github.com/GStreamer/orc
	pushd .
	cd orc

	meson setup build$2 -Ddefault_library=static --prefix=$1
	echo meson setup build$2 -Ddefault_library=static --prefix=$1

	cd build$2
	
	meson install
	echo meson install
	
	popd
fi
