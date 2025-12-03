#!/bin/bash  

if [ ! -d "winflexbison" ]; then
	echo "Cloning into winflexbison.."
	git clone https://github.com/lexxmark/winflexbison.git
	
	cd winflexbison
	mkdir build
	cd build
	cmake .. -G "Ninja"
	cmake --build . --config "Release" --target package
	cd ..

fi
