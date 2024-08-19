#!/bin/bash  

if [ ! -d "$2" ]; then

	curl -kLSs  https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip -o glew-2.2.0.zip
	unzip glew-2.2.0.zip 
	mv glew-2.2.0 glew
	rm glew-2.2.0.zip
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


