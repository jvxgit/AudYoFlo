#!/bin/bash  

if [ ! -d "hdf5-1.14.2" ]; then

	curl -kLSs  https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.14/hdf5-1.14.2/src/hdf5-1.14.2.zip -o hdf5.zip	
	unzip hdf5.zip
	rm hdf5.zip
fi

if [ ! -d "build-glnx" ]; then

    echo "Create build folder <build/compile.sh>"
    mkdir build-glnx
	
    cp build/compile.sh build-glnx
    chmod +x build/compile.sh 
fi

if [ ! -d "hdf5-1.14.2/glnx" ]; then
   cd build-glnx
   ./compile.sh glnx hdf5-1.14.2
fi


