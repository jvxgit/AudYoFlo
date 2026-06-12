#/bin/sh

# Check current folder
ABS_PATH_INSTALL=`pwd`
RTAP_SDK_PATH="$(cd ${ABS_PATH_INSTALL}/../sdk "$0" && pwd -P)"

echo "--> Running standalone project generation in folder ${ABS_PATH_INSTALL}"

project_token=`basename ${ABS_PATH_INSTALL}`

echo "--> Project token: ${project_token}"

# Create build folder
if [ ! -d ${ABS_PATH_INSTALL}/build ]; then
	mkdir ${ABS_PATH_INSTALL}/build
fi

# Specify path reference to SDK folder
read -p "--> Specify location of SDK folder [${RTAP_SDK_PATH}]:" sdkpath

if [ ! -d ${sdkpath} ]; then
 echo "--> Specified SDK folder does not exist"
 exit
fi

if [ -n "${sdkpath}" ]; then
	RTAP_SDK_PATH="${sdkpath}"
fi 

echo "--> RTProc SDK folder: ${RTAP_SDK_PATH}"

mysystem=`uname`

# Specify cmake generator token
if [ "${mysystem}" == "Darwin" ]; then
	read -p "--> Specify cmake Generator token[1:Unix Makefiles; 2: Eclipse/CDT; 3: XCode (only on Mac OS)]:" cmakegenerator
	if [ "${cmakegenerator}" == "1" ]; then 
 		CMAKE_TOKEN="Unix Makefiles"
	elif [ "${cmakegenerator}" == "2" ]; then 
    	CMAKE_TOKEN="Eclipse CDT4 - Unix Makefiles"
	elif [ "${cmakegenerator}" == "3" ]; then 
    	CMAKE_TOKEN="Xcode"
	else
   		echo "--> Undefined selection of cmake generator type, please specify 1, 2 or 3"
   		exit 
	fi

else
	read -p "--> Specify cmake Generator token[1:Unix Makefiles; 2: Eclipse/CDT]:" cmakegenerator
	if [ "${cmakegenerator}" == "1" ]; then 
 		CMAKE_TOKEN="Unix Makefiles"
	elif [ "${cmakegenerator}" == "2" ]; then 
    	CMAKE_TOKEN="Eclipse CDT4 - Unix Makefiles"
	else
   		echo "--> Undefined selection of cmake generator type, please specify 1 or 2"
   		exit 
	fi
fi


# Involve the settings to become part of the compile script
echo "--> RTProc SDK path is ${RTAP_SDK_PATH}"
echo "--> CMake Generator token choice: ${CMAKE_TOKEN}"

sed "s@RTAP_RPL_GENERATOR_TOKEN@${CMAKE_TOKEN}@g" standalone/build/compile.sh > ___j__v__x__tmp.sh
sed "s@RTAP_RPL_SDK_PATH@${RTAP_SDK_PATH}@g" ___j__v__x__tmp.sh > build/compile.sh 
rm ___j__v__x__tmp.sh
chmod +x build/compile.sh

echo "--> Overwriting CMakeLists file in project folder"
cp standalone/cmake/CMakeLists.txt . 

cd build
echo "--> Type <./compile.sh> to run cmake"
