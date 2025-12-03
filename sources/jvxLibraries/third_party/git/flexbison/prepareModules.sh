#!/bin/bash  

# $1: archtoken
# $2: build option
# Commit ID: 2040672b690523e6a5b2e51480d90132a39d4c76 

echo "Running build in folder <flexbison>"

if [ ! -d "winflexbison" ]; then
	echo "Cloning into winflexbison.."
	git clone https://github.com/lexxmark/winflexbison.git
fi

cd winflexbison

echo "Check for <$2/$1/build>"

if [ ! -d "$2/$1/bin" ]; then

	mkdir $2
	mkdir $2/$1
	mkdir $2/$1/build
		
	pushd 
	cd $2/$1/build
	
	# pwd
	
	cmake ../../.. -G "Ninja" -DCMAKE_INSTALL_PREFIX=../bin
	ninja install
	# cmake --build . --config "Release" --target package
	echo popd
fi
