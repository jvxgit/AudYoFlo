#!/bin/bash

if [ ! -d "orc" ]; then
	
	meson_version=1.4.0
	if [ ! -d "meson" ]; then
		# Getting meson first!!
		curl -kLSs  https://github.com/mesonbuild/meson/releases/download/$meson_version/meson-$meson_version.tar.gz -o meson.tar.gz	
		tar -xvzf meson.tar.gz
		mv meson-$meson_version meson
		rm meson.tar.gz
	fi
	
	git clone https://github.com/GStreamer/orc

	#pushd .
	#cd orc
	#meson setup build -Ddefault_library=static --prefix=`pwd`/install
	#cd build
	#meson compile
	#popd
fi
