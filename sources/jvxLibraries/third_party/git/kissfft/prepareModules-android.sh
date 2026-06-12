#!/bin/bash  

echo "Script prepareModules-android.sh with arguments $1 $2 $3 $4 $5 $6 $7"

# $1: archtoken
# $2 target folder
# $3: dataformat
# $4: build option

# $5: androidndk
# $6: androidplatform
# $7: androidabi

# echo "Checking build folder <build-$1_$3_$4>"
if [ ! -d "build-$1_$3_$4" ]; then

    echo "Create build folder <build-$1_$3_$4>"
    mkdir build-$1_$3_$4
	
    cp build/compile-android.sh build-$1_$3_$4
    chmod +x build-$1_$3_$4/compile-android.sh 
fi

# echo "Checking compile output folder <$2-$1-$3-$4>"

if [ ! -d "$2-$1-$3-$4" ]; then
   cd build-$1_$3_$4
   ./compile-android.sh $1 $2 $3 $4 $5 $6 $7
   
   cd ..

   pwd  
fi


