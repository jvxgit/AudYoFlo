#!/bin/bash  

if [ ! -d "boost" ]; then

	dir=`pwd`
	version=boost_1_70_0
	if [ ! -f "$version.tar.gz" ]; then

		echo "1) Downloading boost library"
		curl -kLSs https://boostorg.jfrog.io/artifactory/main/release/1.70.0/source/$version.tar.gz -o $version.tar.gz
	fi

	echo "2) Extracting boost source package"
	tar -xvzf $version.tar.gz

	
	echo "3) Changing folder"
	cd $version

	echo "4) Bootstrapping"
	./bootstrap.sh

	echo "5) Install boost headers"
	./b2 headers

	cd ..

	echo "6) Renaming folder to boost"
	mv $version boost
	
	echo "7) Removing build artefacts"
	rm $version.tar.gz

	
fi
