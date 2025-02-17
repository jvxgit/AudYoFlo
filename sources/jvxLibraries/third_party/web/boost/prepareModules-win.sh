#!/bin/bash  

if [ ! -d "boost" ]; then

	if [ ! -f "boost-1.70.0.zip" ]; then
		echo "1) Downloading boost binary package - this may really take some time!!"
		# curl -kLSs https://boostorg.jfrog.io/artifactory/main/release/1.87.0/binaries/boost_1_87_0-unsupported-bin-msvc-all-32-64.7z -o boost-1.87.0.7z
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