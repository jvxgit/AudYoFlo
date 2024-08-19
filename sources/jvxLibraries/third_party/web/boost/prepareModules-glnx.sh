#!/bin/bash  

if [ ! -d "boost" ]; then

	if [ ! -f "boost-1.70.0.7z" ]; then
		echo "1) Downloading boost binary package - this may really take some time!!"
		curl -kLSs https://boostorg.jfrog.io/artifactory/main/release/1.70.0/binaries/boost_1_70_0-unsupported-bin-msvc-all-32-64.7z -o boost-1.70.0.7z
	fi 

	# We need to "sudo apt install p7zip-full"
	echo "2) Extracting boost binary package"
	7z x boost-1.70.0.7z
	
	echo "3) Renaming boost folder"
	mv boost_1_70_0 boost

	echo "4) Removing downloaded packages"
	rm boost-1.70.0.7z
	
fi
