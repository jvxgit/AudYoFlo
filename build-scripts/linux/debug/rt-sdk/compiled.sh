#!/bin/sh

## Before start, on Ubuntu 22.04, install as follows:
# sudo apt-get install valgrind cmake build-essential curl
# sudo apt install -y libgl1-mesa-dev curl libglew-dev libsdl2-dev libsdl2-image-dev libglm-dev libfreetype6-dev freeglut3-dev libxmu-dev flex bison portaudio19-dev libsndfile1-dev libfftw3-3 libfftw3-dev alsa-utils libasound2-dev octave liboctave-dev libboost-dev libeigen3-dev libopencv-dev python3-opencv libpcap-dev ninja-build python3-pip

## This for VST:
# sudo apt install -y libxcb-util-dev libxcb-cursor-dev libxcb-keysyms1-dev libxcb-xkb-de libxkbcommon-x11-dev libcairo-dev libpango1.0-dev libgtkmm-3.0-dev libsqlite3-dev

## This for FFMPEG:
#sudo apt-get install -y yasm
#sudo apt-get install -y libsdl2-dev libsqlite3-dev libgtkmm-3.0-dev libpango1.0-dev libgtkmm-3.0-dev libpango1.0-dev pangocairo pangoft2 libcairo-dev libxkbcommon-x11-dev libxcb-keysyms1-dev libxcb-cursor-dev libxcb-util-dev

# Manual modifications:
# 1)
# File ayfdevelop/AudYoFlo/sources/sub-projects/ayfbaresip/sources/Libraries/third_party/git/baresip/baresip/CMakeLists.txt
# Line 335:
# if(NOT STATIC AND UNIX) --> if(NOT STATIC AND UNIX AND BARESIP_ORIG_BUILD)
#
# 2)
# File ayfdevelop/AudYoFlo/sources/sub-projects/vst3sdk/public.sdk/source/vst/moduleinfo/moduleinfoparser.cpp
# Add #include <limits> in line 42
#
# 3)
# In file ayfdevelop/AudYoFlo/sources/sub-projects/ayfbaresip/sources/Libraries/third_party/git/re/re/cmake/re-config.cmake
# Make sure code looks as follows:
#check_function_exists(thrd_create HAVE_THREADS_FUN)
#check_include_file(threads.h HAVE_THREADS_H)
# if(HAVE_THREADS_FUN AND HAVE_THREADS_H)
#   set(HAVE_THREADS TRUE)
#   list(APPEND RE_DEFINITIONS -DHAVE_THREADS)
# endif()
#
# In file ayfdevelop2/AudYoFlo/sources/sub-projects/ayfbaresip/sources/Libraries/third_party/git/baresip/baresip/cmake/modules.cmake, comment out portaudio
# # if(PORTAUDIO_FOUND)
# #   list(APPEND MODULES portaudio)
# # endif()
#
# 4) Activate option JVX_GCC_LINKER_SYMBOLIC in case of gcc compiler to link with ffmpeg



JVX_RELEASE_TYPE=Debug
JVX_SRC_PATH=$PWD/..
export FLUTTER_SDK_PATH=${JVX_SRC_PATH}/sources/jvxLibraries/third_party/web/flutter-3.7.4
## QT_PATH_64=${JVX_SRC_PATH}/sources/jvxLibraries/third_party/web/qt/qt/5.15.2/gcc_64
## AQT install path

QT_PATH_64=/usr/lib/qt5

ADDITIONAL_FEATURE_TOKENS_MIN="-DJVX_USE_CONSOLE_APPS=TRUE\
	-DJVX_USE_PART_OCTAVE=TRUE \
	-DJVX_USE_PART_MATLAB=FALSE\
	-DJVX_USE_PCAP=FALSE \
	-DJVX_COMPILE_GOD_HOST=TRUE\
	-DJVX_MATLAB_PROPERTY_GENERATOR=TRUE\
        -DJVX_USE_PART_WEBSERVER=TRUE\
	-DJVX_SPLASH_SUPPRESSED=FALSE \
        -DJVX_DEPLOY_EXAMPLES=TRUE\
	-DJVX_USE_BOOST=TRUE\
	-DJVX_USE_EIGEN=TRUE\
	-DJVX_USE_PART_CMINPACK=FALSE \
        -DJVX_DEPLOY_LIBRARY_DSP_MATLAB=TRUE\
	-DJVX_INCLUDE_EBUR128=TRUE \
	-DJVX_USE_VST=TRUE \
	-DJVX_FLUTTER_UI=FALSE \
	-DJVX_USE_PYTHON=FALSE \
	-DJVX_LIBHOST_WITH_JSON_SHOW=TRUE \
	-DJVX_USE_PART_ALSA=TRUE \
 	-DJVX_INSTALL_QT_ON_BUILD=TRUE \
 	-DJVX_USE_ORC=TRUE \
	-DJVX_INSTALL_FLUTTER_ON_BUILD=TRUE \
	-DCMAKE_PREFIX_PATH=${QT_PATH_64}" 

## For Octave - on Octave prompt
# 1) pkg install -forge control
# 2) pkg install -forge signal
# 3) pkg load control

## For JVX_INSTALL_QT_ON_BUILD
# pip install aqtinstall

ADDITIONAL_FEATURE_TOKENS_FULL="\
	-DLIBMYSOFA_BUILD_TESTS=FALSE \
	-DJVX_USE_FFMPEG=TRUE \
	-DJVX_USE_LIBMYSOFA=TRUE \
	-DJVX_USE_HDF5=TRUE \
	-DLIBMYSOFA_BUILD_TESTS=FALSE \
	-DQT_INSTALL_PATH=${QT_PATH_64} \
	-DJVX_GCC_LINKER_SYMBOLIC=TRUE \
	-DSMTG_CREATE_PLUGIN_LINK=FALSE"

	
TOKEN_SDK=-DJVX_RELEASE_SDK=FALSE
TOKEN_BTOOLS=-DJVX_COMPILE_BUILDTOOLS=FALSE

JVX_GENERATOR_TOKEN_BT="\"Ninja\""
JVX_GENERATOR_ARCHITECTURE_BT=""
JVX_GENERATOR_TOKEN_RT="\"Ninja\""
JVX_GENERATOR_ARCHITECTURE_RT=""
JVX_GENERATOR_TOKEN_SDK="\"Ninja\""
JVX_GENERATOR_ARCHITECTURE_SDK=""
	
if [ "$1" = "rt" ];
then
    run="cmake"
    JVX_GENERATOR_TOKEN=${JVX_GENERATOR_TOKEN_RT}
    JVX_GENERATOR_ARCHITECTURE=${JVX_GENERATOR_ARCHITECTURE_RT}
    ADDITIONAL_FEATURE_TOKENS="${ADDITIONAL_FEATURE_TOKENS_MIN} ${ADDITIONAL_FEATURE_TOKENS_FULL}"

elif [ "$1" = "sdk" ];
then
    TOKEN_SDK="-DJVX_RELEASE_SDK=TRUE"
    JVX_GENERATOR_TOKEN=${JVX_GENERATOR_TOKEN_SDK}
    JVX_GENERATOR_ARCHITECTURE=${JVX_GENERATOR_ARCHITECTURE_SDK}
    ADDITIONAL_FEATURE_TOKENS="${ADDITIONAL_FEATURE_TOKENS_MIN} ${ADDITIONAL_FEATURE_TOKENS_FULL}"
    run="cmake"

elif [ "$1" = "sdk-bt" ];
then
    TOKEN_SDK="-DJVX_RELEASE_SDK=TRUE"
    TOKEN_BTOOLS="-DJVX_COMPILE_BUILDTOOLS=TRUE"
    JVX_GENERATOR_TOKEN=${JVX_GENERATOR_TOKEN_BT}
    JVX_GENERATOR_ARCHITECTURE=${JVX_GENERATOR_ARCHITECTURE_BT}
    ADDITIONAL_FEATURE_TOKENS=${ADDITIONAL_FEATURE_TOKENS_MIN}
    run="cmake"

elif [ "$1" = "rt-bt" ];
then
    TOKEN_BTOOLS="-DJVX_COMPILE_BUILDTOOLS=TRUE"
    JVX_GENERATOR_TOKEN=${JVX_GENERATOR_TOKEN_BT}
    JVX_GENERATOR_ARCHITECTURE=${JVX_GENERATOR_ARCHITECTURE_BT}
    ADDITIONAL_FEATURE_TOKENS=${ADDITIONAL_FEATURE_TOKENS_MIN}
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
OPTIONS="-G ${JVX_GENERATOR_TOKEN} ${JVX_GENERATOR_ARCHITECTURE} \
    -DCMAKE_BUILD_TYPE=${JVX_RELEASE_TYPE}\
    -Wno-dev -DJVX_DEPLOY_UNSTABLE=TRUE \
    -DCMAKE_INSTALL_PREFIX=release \
    -DJVX_PRODUCT=audio \
    -DJVX_USE_PART_ALSA=TRUE  \
    -DJVX_USE_PART_PAUDIO=TRUE \
    -DJVX_USE_PART_RS232=TRUE \
    -DJVX_USE_QT=TRUE \
    -DJVX_USE_QWT=TRUE \
    -DJVX_USE_QWT_POLAR=TRUE \
    -DJVX_USE_PART_CORECONTROL=TRUE \
    ${TOKEN_SDK} \
    -DJVX_DEFINE_NUMBER_32BIT_TOKENS_BITFIELD=16 \
    ${TOKEN_BTOOLS} \
    ${ADDITIONAL_FEATURE_TOKENS} \
    -DJVX_DEPLOY_LIBRARY_DSP_MATLAB=TRUE"


if [ ${run} = "show" ];
then
    echo eval cmake  ${JVX_SRC_PATH} ${OPTIONS}

elif [ ${run} = "cmake" ];
then


	echo eval cmake  ${JVX_SRC_PATH} ${OPTIONS}
	eval cmake  ${JVX_SRC_PATH} ${OPTIONS}

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

