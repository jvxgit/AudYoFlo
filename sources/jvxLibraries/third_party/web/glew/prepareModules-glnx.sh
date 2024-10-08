#!/bin/bash  

if [ ! -d "$2" ]; then

	curl -kLSs  https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip -o glew-2.2.0.zip
	unzip glew-2.2.0.zip 
	mv glew-2.2.0 glew
	rm glew-2.2.0.zip
fi

if [ ! -d "$2/build-$1" ]; then

    echo "Create build folder <$2/build-$1>"
    mkdir $2/build-$1
	
    cp build/compile.sh $2/build-$1
    chmod +x $2/build-$1/compile.sh 
fi

if [ ! -d "$2/$2-$1" ]; then
   cd $2/build-$1
   ./compile.sh $1 $2 $3
fi


