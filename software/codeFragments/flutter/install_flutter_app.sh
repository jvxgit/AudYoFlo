#!/bin/bash

echo "INSTALL FLUTTER APP $3"

# BASEDIR=$(dirname "$0")
sysfolder=$2
platform=$7

echo "Running with $# arguments"

if [ $# -ge 5 ]
then
	if [ "$5" = "NONE" ]; then
		cd $1/../flutter/$3
		echo "cd $1/../flutter/$3"
	else
		cd $1/../flutter/$3/$5
		echo "cd $1/../flutter/$3/$5"
	fi
else
	cd $1/../flutter/$3
	echo cd "$1/../flutter/$3"
fi 

pwdvar=`pwd`
echo "Making zip release in folder $pwdvar"

# We need to add an entry here for Windows: this script relies on the git bash. 
# Git bash unfortunately has no built-in zip. Therefore the zip binary is part of
# the repo and is linked here.
export PATH=$PATH:$pwdvar/../../../../bin/tools
echo "Running zip release with path=$PATH"

echo "flutter pub get"
flutter pub get

echo "flutter build $2 $6"
flutter build $2 $6

if [[ "$6" == "--debug" ]]
then
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
		subfolder=debug/bundle
    elif [[ "$OSTYPE" == "msys"* ]]; then
		subfolder=runner/Debug
	else
		echo "UNKNOWN BUILD SYSTEM"
		subfolder=runner/Debug
    fi
else
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
		subfolder=release/bundle    
    elif [[ "$OSTYPE" == "msys"* ]]; then
		subfolder=runner/Release
	else
		echo "UNKNOWN BUILD SYSTEM"
		subfolder=runner/Release
    fi
fi

if [[ "$2" == "web" ]]
then
	# What to do here??
	echo "Compiling for target web"
else
	if [[ -d build/$sysfolder/$platform/$subfolder/data/flutter_assets ]]; then
		echo cp $1/.$3.env.$OSTYPE build/$sysfolder/$platform/$subfolder/data/flutter_assets/.$3.env
		cp $1/.$3.env.$OSTYPE build/$sysfolder/$platform/$subfolder/data/flutter_assets/.$3.env
	fi 
	#if [[ -d build/$sysfolder/$platform/$subfolder/data/flutter_assets/packages/ayfcorepack/local_assets ]]; then
	#	echo ls build/$sysfolder/$platform/$subfolder/data/flutter_assets/packages/ayfcorepack/local_assets
	#	ls build/$sysfolder/$platform/$subfolder/data/flutter_assets/packages/ayfcorepack/local_assets
	
	#	echo cp build/$sysfolder/$platform/$subfolder/data/flutter_assets/packages/ayfcorepack/local_assets/* build/$sysfolder/$platform/$subfolder
	#	cp build/$sysfolder/$platform/$subfolder/data/flutter_assets/packages/ayfcorepack/local_assets/* build/$sysfolder/$platform/$subfolder
	#fi
	
	if [ -d "$4/$3" ]; then
		echo "rm -rf $4/$3"
		rm -rf $4/$3
	fi

	if [ -d "build/$sysfolder/$platform/$subfolder" ]; then
		echo "mv build/$sysfolder/$platform/$subfolder $4/$3"
		mv build/$sysfolder/$platform/$subfolder $4/$3
	fi
fi

echo "Flutter app preparation completed!"

