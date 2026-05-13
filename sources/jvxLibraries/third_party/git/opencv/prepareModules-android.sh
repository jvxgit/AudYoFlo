#!/bin/bash  

echo "Script prepareModules-android.sh with arguments $1 $2 $3"

# $1: archtoken
# $2 target folder
# $3: build option

if [ ! -d "$2/build-$1" ]; then

    echo "Create build folder <$2/build-$1>"
    mkdir $2/build-$1
	
    cp build/compile-android.sh $2/build-$1
    chmod +x $2/build-$1/compile-android.sh 
fi

if [ ! -d "$2/$2-$3" ]; then
   cd $2/build-$1
   ./compile-android.sh $1 $2 $3
   rm -rf $2/build-$1
fi


