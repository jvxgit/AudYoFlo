#!/bin/bash

echo "INSTALL FLUTTER APP $3"

# BASEDIR=$(dirname "$0")
sysfolder=$2

echo "Running with $# arguments"

if [ $# -ge 5 ]
then
	cd $1/../flutter/$3/$5
	echo "cd $1/../flutter/$3/$5"
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

echo "flutter build $2 $6"
flutter build $2 $6

if [[ "$6" == "--debug" ]]
then
	subfolder=Debug
else
	subfolder=Release
fi

if [[ "$2" == "web" ]]
then
	# What to do here??
	echo "Compiling for target web"
else
	echo cp $1/.$3.env build/$sysfolder/runner/$subfolder/data/flutter_assets
	cp $1/.$3.env build/$sysfolder/runner/$subfolder/data/flutter_assets

	if [ -d "$4/$3" ]; then
		echo "rm -rf $4/$3"
		rm -rf $4/$3
	fi

	echo "mv build/$sysfolder/runner/$subfolder $4/$3"
	mv build/$sysfolder/runner/$subfolder $4/$3
fi

echo "Flutter app preparation completed!"

