#!/bin/bash  

if [ ! -d "libsdl" ]; then
	curl -kLSs  https://github.com/libsdl-org/SDL/releases/download/release-2.26.3/SDL2-devel-2.26.3-VC.zip -o libsdl.zip	
	mkdir libsdl
	cd libsdl
	unzip ../libsdl.zip
	cd ..	
	rm libsdl.zip
fi
