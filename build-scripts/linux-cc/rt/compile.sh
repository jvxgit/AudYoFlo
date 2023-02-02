#!/bin/sh

# Cross-compile AudYoFlo for the yocto linux IMAGEs
# The following must be fulfilled:
# - yocto release complete 
# - yocto release sdk for embedded target

# !!!!! THE FOLLOWING LINES TO BE ADAPTED ACCORDING TO BUILD SYSTEM !!!!!
# Set the links to the cross compilers 
export YOCTO_SDK_PATH=/home/hauke/develop/aries/sdk
export YOCTO_SDK_MACHINE=cortexa9t2hf-neon-poky-linux-gnueabi

# Set the link to the code base
JVX_SRC_PATH=../AudYoFlo

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

TOKEN_SDK=-DJVX_RELEASE_SDK=FALSE
TOKEN_BTOOLS=-DJVX_COMPILE_BUILDTOOLS=FALSE
TOKEN_CCOMPILE="-DCMAKE_TOOLCHAIN_FILE=${JVX_SRC_PATH}/cmake/toolchain/yocto-x-build.cmake\
	       -DJVX_PACKAGE_ROOT_PATH=${YOCTO_SDK_PATH}/sysroots/${YOCTO_SDK_MACHINE}/usr/lib\
	       -DCMAKE_PREFIX_PATH=${YOCTO_SDK_PATH}/sysroots/${YOCTO_SDK_MACHINE}/usr/lib/cmake\
	       -DJVX_PCG=./release/runtime/bin/jvxExPropC\
	       -DJVX_MCG=./release/runtime/bin/jvxExMatC\
               -DOE_QMAKE_PATH_EXTERNAL_HOST_BINS=/usr/bin" 

JVX_GENERATOR_TOKEN_BT="\"Ninja\""
JVX_GENERATOR_TOKEN="\"Ninja\""

if [ "$1" = "rt" ];
then
    # Start cross compilation environment
    . ${YOCTO_SDK_PATH}/environment-setup-${YOCTO_SDK_MACHINE}
    run="cmake"

elif [ "$1" = "sdk" ];
then
    echo "SDK build currently not supported!"
    run="leaveerror"
    #TOKEN_SDK="-DJVX_RELEASE_SDK=TRUE"
    #run="cmake"

elif [ "$1" = "sdk-bt" ];
then
    echo "SDK build currently not supported!"
    run="leaveerror"
    #TOKEN_SDK="-DJVX_RELEASE_SDK=TRUE"
    #TOKEN_BTOOLS="-DJVX_COMPILE_BUILDTOOLS=TRUE"
    #run="cmake"

elif [ "$1" = "rt-bt" ];
then
    TOKEN_BTOOLS="-DJVX_COMPILE_BUILDTOOLS=TRUE"
    TOKEN_CCOMPILE=""
    JVX_GENERATOR_TOKEN=${JVX_GENERATOR_TOKEN_BT}
    run="cmake"

elif [ "$1" = "clean" ];
then
    run="clean"

elif [ "$1" = "distclean" ];
then
    run="distclean"

elif [ "$1" = "cc" ];
then
    run="cc"

else
    run="nothing"
    echo "Usage: compile < rt-bt | rt | sdk-bt | sdk | clean | distclean >"
fi


if [ ${run} = "cmake" ];
then

    # For cmake build...

    OPTIONS="\
 ${TOKEN_CCOMPILE}\
 -DCMAKE_INSTALL_PREFIX=release\
 -G ${JVX_GENERATOR_TOKEN}\
 -DCMAKE_BUILD_TYPE=Debug\
 -DJVX_SYSTEM_USE_DATA_FORMAT_FLOAT=TRUE\
 -DJVX_USE_PART_WEBSERVER=TRUE\
 -DJVX_PRODUCT=industrial\
 -DJVX_USE_PART_OCTAVE=FALSE\
 -DJVX_USE_PART_MATLAB=FALSE\
 -DJVX_USE_PART_ALSA=TRUE\
 -DJVX_USE_PART_PAUDIO=FALSE\
 -DJVX_USE_PART_CMINPACK=FALSE\
 -DJVX_USE_QT=FALSE\
 -DJVX_USE_QWT=FALSE\
 -DJVX_USE_QWT_POLAR=FALSE\
 -DJVX_DEPLOY_LIBRARY_DSP_MATLAB=FALSE\
 -DJVX_USE_PCAP=FALSE\
 -DJVX_USE_PART_RS232=TRUE\
 ${TOKEN_SDK}\
 -DJVX_MATLAB_PROPERTY_GENERATOR=FALSE\
 -DJVX_DEFINE_NUMBER_32BIT_TOKENS_BITFIELD=16 \
 ${TOKEN_BTOOLS}\
 -DJVX_DEPLOY_UNSTABLE=TRUE"

    # # -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake
    # # for compilation @ IND add
    # # -DCMAKE_PREFIX_PATH=/share/packages64/qt-5.5.1/5.5/gcc_64/lib/cmake
    # # -DCMAKE_BUILD_TYPE can be Debug, Release, UnstableDebug, UnstableRelease
    # # -DJVX_PRODUCT can be bdx or audio

    # Run cmake
    echo eval cmake  ${JVX_SRC_PATH} ${OPTIONS}
    eval cmake  ${JVX_SRC_PATH} ${OPTIONS}

    if [ "${JVX_GENERATOR_TOKEN}" = "\"Unix Makefiles\"" ] || [ "${JVX_GENERATOR_TOKEN}" = "\"Eclipse CDT4 - Unix Makefiles\"" ];
    then
	#compile
    	make -j9 install
    fi
    if [ "${JVX_GENERATOR_TOKEN}" = "\"Ninja\"" ] || [ "${JVX_GENERATOR_TOKEN}" = "\"Eclipse CDT4 - Ninja\"" ];
    then
	#compile
   	ninja install
    fi
    #compile
    #make -j9 install

elif [ ${run} = "clean" ] || [ ${run} = "distclean" ] || [ ${run} = "cc" ] ;
then
    # Remove all generated files except for release folder
    rm -rf CMakeCache.txt
    rm -rf CMakeFiles
    if [ ${run} = "clean" ] || [ ${run} = "distclean" ];
    then
	# Remove all generated files except for release folder
	rm -rf cmake/ build.ninja rules.ninja industrial base/  CMakeFiles/  cmake_install.cmake  install_manifest.txt  jvx_dsp_base.h  Makefile
	if [ ${run} = "distclean" ];
	then
            # Remove release dir
            rm -rf release
	fi
    fi
fi


