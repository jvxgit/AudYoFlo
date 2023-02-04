#!/bin/bash  

if [ ! -d "boost" ]; then

	dir=`pwd`
	version=boost_1_70_0
	if [ ! -f "boost_1_70_0.tar.gz" ]; then

		echo "1) Downloading boost library"
		curl -kLSs https://boostorg.jfrog.io/artifactory/main/release/1.70.0/source/$version.tar.gz -o boost_1_70_0.tar.gz
	fi

	echo "2) Extracting boost source package"
	tar -xvzf $version.tar.gz

	
	echo "3) Changing folder"
	cd $version

	echo "4) Bootstrapping"
	./bootstrap.sh --prefix=$dir/boost

	echo "5) Install boost to $dir/boost"
	./b2 install

	cd ..

	echo "6) Removing build artefacts"
	rm $version.tar.gz
	rm -rf $version
	
fi