#!/bin/sh

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

TOKEN_SDK=-DJVX_RELEASE_SDK=FALSE
TOKEN_BTOOLS=-DJVX_COMPILE_BUILDTOOLS=FALSE

JVX_GENERATOR_TOKEN_BT="\"Ninja\""
JVX_GENERATOR_TOKEN="\"Eclipse CDT4 - Ninja\""

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

elif [ "$1" = "show" ];
then
    run="show"

elif [ "$1" = "distclean" ];
then
    run="distclean"

else
    run="nothing"
    echo "Usage: compile < rt-bt | rt | sdk-bt | sdk | clean | distclean >"
fi

# Setup the typical cmake config options
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

if [ ${run} = "show" ];
then
    echo eval cmake  ${JVX_SOURCE_ROOT} ${OPTIONS}

elif [ ${run} = "cmake" ];
then


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
    rm -rf Matlab build.ninja cmake rules.ninja CMakeCache.txt  CMakeFiles/  cmake_install.cmake  install_manifest.txt  jvx_dsp_base.h  Makefile sources sub-projects VST3 bin lib compile_commands.json .cproject .project .ninja_deps .ninja_log
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

