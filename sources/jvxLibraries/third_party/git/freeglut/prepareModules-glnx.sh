#!/bin/bash  

if [ ! -d "$2" ]; then

	git clone https://github.com/freeglut/freeglut.git
	cd freeglut
		git checkout b789505b4eed2ef5a5a7069e1539c6f6472a7dd3
	cd ..
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


