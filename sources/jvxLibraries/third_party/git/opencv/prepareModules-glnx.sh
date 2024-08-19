#!/bin/bash  

if [ ! -d "$2" ]; then

	git clone https://github.com/opencv/opencv.git
	cd opencv
		git checkout 1ca526dcdb9c30600c70537e279f0c672057a1b9
	cd ..
fi

if [ ! -d "build-$1" ]; then

    echo "Create build folder <build/compile.sh>"
    mkdir build-$1
	
    cp build/compile.sh build-$1
    chmod +x build/compile.sh 
fi

if [ ! -d "$2/$1" ]; then
   cd build-$1
   ./compile.sh $1 $2 $3
fi


