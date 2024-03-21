#!/bin/bash

if [ ! -d "orc" ]; then
	git clone https://github.com/GStreamer/orc
	pushd .
	cd orc
	meson setup build -Ddefault_library=static --prefix=`pwd`/install
	cd build
	meson compile
	popd
fi
