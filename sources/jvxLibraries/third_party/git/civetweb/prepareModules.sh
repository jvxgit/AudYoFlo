#!/bin/bash  

if [ ! -d "civetweb" ]; then
	git clone https://github.com/civetweb/civetweb.git
	cd civetweb
	git checkout 8cc6a8e5bf8bd6cfb7e9c92958a7af555301293f
	cd ..
	patch -p0 < civetweb-src.patch
fi
