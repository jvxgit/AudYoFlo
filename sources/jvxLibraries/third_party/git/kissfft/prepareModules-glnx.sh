#!/bin/bash  

echo "Script prepareModules-glnx.sh with arguments $1 $2 $3 $4 $5 $6 $7"

# $1: archtoken
# $2 target folder
# $3: dataformat
# $4: build option

echo "Checking build folder <build-$1_$3_$4>"
if [ ! -d "build-$1_$3_$4" ]; then

    echo "Create build folder <build-$1_$3_$4>"
    mkdir build-$1_$3_$4

    echo "Copy file build/compile-glnx.sh"
    cp build/compile-glnx.sh build-$1_$3_$4
    chmod +x build-$1_$3_$4/compile-glnx.sh 
fi

echo "Checking compile output folder <$2-$1-$3-$4>"

if [ ! -d "$2-$1-$3-$4" ]; then
   cd build-$1_$3_$4
   ./compile-glnx.sh $1 $2 $3 $4
   
   cd ..

   pwd  
fi


