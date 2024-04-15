#!/bin/bash

if [ ! -d "orc" ]; then
	git clone https://github.com/GStreamer/orc
	pushd .
	cd orc

	meson setup build -Ddefault_library=static --prefix=$1
	echo meson setup build -Ddefault_library=static --prefix=$1

	cd build
	
	meson install
	echo meson install
	
	popd
fi
