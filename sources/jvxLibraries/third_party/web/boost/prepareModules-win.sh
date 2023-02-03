#!/bin/bash  

if [ ! -d "boost" ]; then
	echo "1) Downloading 7zr.exe"
	curl -kLSs https://7-zip.org/a/7zr.exe -o 7zr.exe
	
	echo "2) Downloading boost binary package - this may really take some time!!"
	curl -kLSs https://boostorg.jfrog.io/artifactory/main/release/1.70.0/binaries/boost_1_70_0-unsupported-bin-msvc-all-32-64.7z -o boost-1.70.0.7z

	echo "3) Extracting boost binary package"
	./7zr.exe x boost-1.70.0.7z -oboost
	
	echo "4) Renaming boost folder"
	mv boost_1_70_0 boost

	echo "5) Removing downloaded packages"
	rm 7zr.exe
	rm boost-1.70.0.7z
	
fi