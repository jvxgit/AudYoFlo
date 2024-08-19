#!/bin/bash  

if [ ! -d "$2" ]; then

	git clone https://github.com/freeglut/freeglut.git
	cd freeglut
		git checkout b789505b4eed2ef5a5a7069e1539c6f6472a7dd3
	cd ..
fi

if [ ! -d "build-$1" ]; then

    echo "Create build folder <build-$1/compile.sh>"
    mkdir build-$1
	
    cp build/compile.sh build-$1
    chmod +x build-$1/compile.sh 
fi

if [ ! -d "$2/$1" ]; then
   cd build-$1
   ./compile.sh $1 $2 $3
fi


