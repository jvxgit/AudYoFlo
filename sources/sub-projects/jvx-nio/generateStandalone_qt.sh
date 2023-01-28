#/bin/sh

# Check current folder
ABS_PATH_INSTALL=`pwd`
RTAP_SDK_PATH="/opt/develop/myRtapSdk"
RTAP_QT_PATH="/opt/develop/qt"

echo "--> Running standalone project generation in folder ${ABS_PATH_INSTALL}"

project_token=`basename ${ABS_PATH_INSTALL}`

echo "--> Project token: ${project_token}"

# Create build folder
if [ ! -d ${ABS_PATH_INSTALL}/../build ]; then
	mkdir ${ABS_PATH_INSTALL}/../build
fi

# Specify path reference to SDK folder
read -p "--> Specify location of SDK folder [${RTAP_SDK_PATH}]:" sdkpath

if [ ! -d ${sdkpath} ]; then
 echo "--> Specified SDK folder does not exist"
 exit
fi

RTAP_SDK_PATH="${sdkpath}"
echo "--> RTProc SDK folder: ${RTAP_SDK_PATH}"



# Specify path reference to qt
read -p "--> Specify location of QT folder [${RTAP_QT_PATH}]:" qtpath

if [ ! -d ${qtpath} ]; then
 echo "--> Specified QT folder does not exist"
 exit
fi

RTAP_QT_PATH="${qtpath}"

echo "--> QT folder: ${RTAP_QT_PATH}"



# Specify cmake generator token
read -p "--> Specify cmake Generator token[1:Unix Makefiles; 2: Eclipse/CDT; 3: XCode]:" cmakegenerator

if [ "${cmakegenerator}" == "1" ]; then 
 CMAKE_TOKEN="Unix Makefiles"
elif [ "${cmakegenerator}" == "2" ]; then 
    CMAKE_TOKEN="Eclipse CDT4 - Unix Makefiles"
elif [ "${cmakegenerator}" == "3" ]; then 
    CMAKE_TOKEN="Xcode"
else
   echo "--> Undefined selection of cmake generator type, please specify 1, 2, 3"
   exit 
fi

# Involve the settings to become part of the compile script
echo "--> RTProc SDK path is ${RTAP_SDK_PATH}"
echo "--> QT path is ${RTAP_QT_PATH}"
echo "--> CMake Generator token choice: ${CMAKE_TOKEN}"

sed "s@RTAP_RPL_GENERATOR_TOKEN@${CMAKE_TOKEN}@g" standalone/build/compile_qt.sh > ___j__v__x__tmp.sh
sed "s@RTAP_RPL_SDK_PATH@${RTAP_SDK_PATH}@g" ___j__v__x__tmp.sh > ____j__v__x__tmp.sh
sed "s@RTAP_RPL_QT_PATH@${RTAP_QT_PATH}@g" ____j__v__x__tmp.sh > ../build/compile.sh 
#rm ___j__v__x__tmp.sh
chmod +x ../build/compile.sh

echo "--> Copy CMakeLists file to project folder"
if [ ! -e ../CMakeLists.txt ]; then
    cp standalone/cmake/CMakeLists.txt ../ 
fi

cd ../build
echo "--> Type <./compile.sh> to run cmake"
