#!/bin/sh

# In case we want to compile GST projects, let us define which location
# to link with
# export JVX_GST_INSTALL_PREFIX="/opt/install-local2"

JVX_RELEASE_TYPE=Debug
JVX_SOURCE_ROOT=../AudYoFlo
ADDITIONAL_FEATURE_TOKENS="-DJVX_USE_CONSOLE_APPS=TRUE\
	-DJVX_USE_PART_MATLAB=TRUE\
	-DJVX_COMPILE_GOD_HOST=TRUE\
	-DJVX_MATLAB_PROPERTY_GENERATOR=TRUE\
        -DJVX_USE_PART_WEBSERVER=TRUE\
        -DJVX_DEPLOY_EXAMPLES=TRUE\
	-DJVX_USE_BOOST=TRUE\
	-DJVX_USE_EIGEN=TRUE\
        -DJVX_DEPLOY_LIBRARY_DSP_MATLAB=TRUE\
	-DJVX_INCLUDE_EBUR128=TRUE \
	-DJVX_USE_VST=TRUE \
	-DSMTG_CREATE_PLUGIN_LINK=FALSE \
	-DJVX_USE_PART_MPG123=FALSE \
	-DJVX_USE_LIBMYSOFA=TRUE \
	-DLIBMYSOFA_BUILD_TESTS=FALSE \
	-DJVX_USE_HOA_CIRC_ARRAY=FALSE \
 	-DQT_INSTALL_PATH=/home/hauke/develop/install/Qt5.13.2/5.13.2/gcc_64"

# VST in Linux seems to be not yet ready..
#	-DJVX_USE_VST=TRUE \
#	-DSMTG_CREATE_PLUGIN_LINK=FALSE \

# -DQT_INSTALL_PATH=P:\Qt\Qt5.12.2\5.12.2\msvc2017_64
# -DCMAKE_PREFIX_PATH=/home/hauke/install/Qt5.11.2/5.11.2/gcc_64

# Options: 
# -DJVX_USE_PART_CMINPACK=TRUE
# -DJVX_DEPLOY_EXAMPLES=TRUE 
# -DJVX_USE_PCAP=TRUE
# -DJVX_USE_BOOST=TRUE 
# -DJVX_USE_OPENGL_X11=TRUE 
# -DJVX_USE_EIGEN=TRUE 
# -DJVX_USE_PART_OCTAVE=TRUE 
# -DJVX_USE_PART_WEBSERVER=TRUE -> Attach web server function
# -DJVX_USE_CONSOLE_APPS=TRUE -> Attach console to executables to output runtime prints
# -DJVX_DEPLOY_EXAMPLES=TRUE -> Compile and release example applications
# -DJVX_USE_PART_WEBSERVER=TRUE -> Compile and attach web server component
# -DJVX_USE_BOOST=TRUE -> Use boost library
# -DJVX_USE_VIDEO=TRUE -> Use video functionality (capture & opengl rendering)
# -DJVX_USE_EIGEN=TRUE -> Use Eigen library for some math functions
# -DJVX_USE_PART_OCTAVE=TRUE -> Compile and release octave entries
# -DJVX_USE_PART_MATLAB=TRUE -> Compile and release Matlab entries
# -DJVX_USE_PCAP=TRUE -> Compile and use pcap low level ethernet protocol
# -DJVX_USE_PART_CMINPACK=TRUE -> Attach cminpack library
# -DJVX_COMPILE_GOD_HOST=TRUE  -> Compile and release GODs HOST
# -DJVX_MATLAB_PROPERTY_GENERATOR=TRUE -> Run the mtlab property access functions
# -DGLOBAL_COMPILE_DEFINITIONS=VERBOSE_BUFFERING_REPORT
# -DQT_INSTALL_PATH=P:\Qt\Qt5.12.2\5.12.2\msvc2017_64 
# -DGLOBAL_COMPILE_DEFINITIONS=VERBOSE_BUFFERING_REPORT
# -DJVX_DEPLOY_LIBRARY_DSP_MATLAB=FALSE
# -DJVX_AUDIO_WITH_GST=FALSE
# -DJVX_TARGET_GCC_ARM_YOCTO=FALSE

TOKEN_SDK=-DJVX_RELEASE_SDK=FALSE
TOKEN_BTOOLS=-DJVX_COMPILE_BUILDTOOLS=FALSE

JVX_GENERATOR_TOKEN_BT="\"Ninja\""
JVX_GENERATOR_TOKEN="\"Eclipse CDT4 - Ninja\""
# JVX_GENERATOR_TOKEN="\"Eclipse CDT4 - Unix Makefiles\""



if [ "$1" = "rt" ];
then
    run="cmake"

elif [ "$1" = "sdk" ];
then
    TOKEN_SDK="-DJVX_RELEASE_SDK=TRUE"
    run="cmake"

elif [ "$1" = "sdk-bt" ];
then
    TOKEN_SDK="-DJVX_RELEASE_SDK=TRUE"
    TOKEN_BTOOLS="-DJVX_COMPILE_BUILDTOOLS=TRUE"
	JVX_GENERATOR_TOKEN=${JVX_GENERATOR_TOKEN_BT}
    run="cmake"

elif [ "$1" = "rt-bt" ];
then
    TOKEN_BTOOLS="-DJVX_COMPILE_BUILDTOOLS=TRUE"
    JVX_GENERATOR_TOKEN=${JVX_GENERATOR_TOKEN_BT}
    run="cmake"

elif [ "$1" = "clean" ];
then
    run="clean"

elif [ "$1" = "cc" ];
then
    run="cc"

elif [ "$1" = "distclean" ];
then
    run="distclean"

else
    run="nothing"
    echo "Usage: compile < rt-bt | rt | sdk-bt | sdk | clean | distclean >"
fi


if [ ${run} = "cmake" ];
then

    # For cmake build...

 OPTIONS="-DCMAKE_INSTALL_PREFIX=release\
 -G ${JVX_GENERATOR_TOKEN} -DCMAKE_BUILD_TYPE=${JVX_RELEASE_TYPE}\
 -DJVX_PRODUCT=audio\
 ${TOKEN_SDK}\
 -DJVX_DEFINE_NUMBER_32BIT_TOKENS_BITFIELD=16 \
 ${TOKEN_BTOOLS}\
 -DJVX_DEPLOY_UNSTABLE=TRUE\
 -DJVX_USE_PART_ALSA=TRUE\
 -DJVX_USE_PART_PAUDIO=TRUE\
 -DJVX_USE_PART_RS232=TRUE \
 -DJVX_USE_QT=TRUE \
 -DJVX_USE_QWT=TRUE \
 -DJVX_USE_QWT_POLAR=TRUE \
 -DJVX_USE_PART_CORECONTROL=TRUE \
 ${ADDITIONAL_FEATURE_TOKENS}\
 -Wno-dev"

    # # -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake
    # # for compilation @ IND add
    # # -DCMAKE_PREFIX_PATH=/share/packages64/qt-5.5.1/5.5/gcc_64/lib/cmake
    # # -DCMAKE_BUILD_TYPE can be Debug, Release, UnstableDebug, UnstableRelease
    # # -DJVX_PRODUCT can be bdx or audio

	echo eval cmake  ${JVX_SOURCE_ROOT} ${OPTIONS}
	eval cmake  ${JVX_SOURCE_ROOT} ${OPTIONS}

	if [ "${JVX_GENERATOR_TOKEN}" = "\"Unix Makefiles\"" ] || [ "${JVX_GENERATOR_TOKEN}" = "\"Eclipse CDT4 - Unix Makefiles\"" ] ;
	then
		#compile
    	make -j9 install
	fi
	if [ "${JVX_GENERATOR_TOKEN}" = "\"Ninja\"" ] || [ "${JVX_GENERATOR_TOKEN}" = "\"Eclipse CDT4 - Ninja\"" ];
	then
		#compile
    	ninja install
	fi

elif [ ${run} = "clean" ] || [ ${run} = "distclean" ];
then
    # Remove all generated files except for release folder
    rm -rf Matlab build.ninja cmake rules.ninja audio/  base/  CMakeCache.txt  CMakeFiles/  cmake_install.cmake  install_manifest.txt  jvx_dsp_base.h  Makefile
    if [ ${run} = "distclean" ];
    then
        # Remove release dir
        rm -rf release
    fi
elif [ ${run} = "cc" ];
then
    rm CMakeCache.txt
    rm -r CMakeFiles
fi

