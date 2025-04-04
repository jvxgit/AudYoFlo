echo "Script prepareModules-glnx.sh with arguments $1 $2 $3"

# $1: target_folder
# $2 buildtype
# $3: arch_simple

cd $1/build
cmake -DCMAKE_BUILD_TYPE=$2 .. -G "Ninja" -DBUILD_TESTS=FALSE -DBUILD_SHARED_LIBS=FALSE -DCMAKE_INSTALL_PREFIX=$3/$2 -Wno-dev
ninja install
