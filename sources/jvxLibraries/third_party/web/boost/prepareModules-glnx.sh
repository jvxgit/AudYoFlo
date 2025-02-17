#!/bin/bash  

if [ ! -d "boost" ]; then

	if [ ! -f "boost-1.70.0.7z" ]; then
		echo "1) Downloading boost binary package - this may really take some time!!"
		wget https://archives.boost.io/release/1.70.0/source/boost_1_70_0.zip
	fi 

	echo "2) Extracting boost binary package"
	unzip -q boost_1_70_0.zip
	
	pwd 
	ls
	
	echo "3) Renaming boost folder"
	mv boost_1_70_0 boost

	echo "4) Removing downloaded packages"
	rm boost_1_70_0.zip
	
fi
