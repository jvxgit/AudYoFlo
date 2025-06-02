cmake_minimum_required (VERSION 2.8.11)

set(PROJECT_NAME ayfstarter-native-config)
set(JVX_AUTOMATION_VERBOSE FALSE)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-flutter-native-static/include
 )

# Involving the template files for the native config lib
configure_file(${JVX_BASE_ROOT}/software/codeFragments/jvxApplications/native-config/include/generic-native-config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/ayfstarter-native-config.h)
configure_file(${JVX_BASE_ROOT}/software/codeFragments/jvxApplications/native-config/src/generic-config-hook.cpp.in ${CMAKE_CURRENT_SOURCE_DIR}/src/ayfstarter-config-hook.cpp)

  # Add local sources (header files only for project workspace generation!) 
set(LOCAL_SOURCES 	

	${JVX_BASE_ROOT}/software/codeFragments/jvxApplications/native-config/exports/${JVX_OS}/exports-flutter-native-config.def

	${CMAKE_CURRENT_SOURCE_DIR}/src/ayfstarter-config-hook.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/ayfstarter-native-config.h
	
  )

if(JVX_AUTOMATION_VERBOSE)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_AUTOMATION_VERBOSE")
endif()

# Specify the max number of devices to be mixed during processing per technology. Here we can use inf
#set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_NUMBER_AUDIO_DEVICES_MIX=JVX_SIZE_UNSELECTED")
## 

if(${JVX_OS} MATCHES "Android")
	find_library(jniloglib log REQUIRED)
	# target_link_libraries(<dein-target> ${log-lib})
	set(LOCAL_LIBS "${LOCAL_LIBS};${jniloglib}")
endif()

## ======================================================================
# Shared and static libraries here!
set(BUILD_SHARED TRUE)
set(BUILD_STATIC TRUE)

# We need to install the generated header to the SDK - to add it in the flutter build file
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

# this changes the name of the output to be followed by "_import"
# It does NOT produce an import lib in the SDK for all projects which
# are compiled to form a runtime
set(JVX_FORCE_RUNTIME_IMPORTLIB TRUE)

# This allows to copy import lib (.lib file) to sdk also
set(JVX_SDK_FORCE_DEPLOY_IMPORT_STATIC_LIB TRUE)

## ======================================================================


include(../common/local_config.cmake)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

# message(FATAL_ERROR "End here!")
# Install the MSVC system libraries
## if(JVX_OS MATCHES "windows")
## 	set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${INSTALL_PATH_BIN}")
## 	include(InstallRequiredSystemLibraries)
## endif()

# 
# Befehl				=	O:\ayf-templates\windows\jvxrt-submodules\ayfbinrender\sources\flutter\ayfbinrender\build\windows\runner\Debug\ayfBinRender.exe
# Befehlsargumente		=   --workdir C:\jvxrt\audio\release\runtime --preload C:\jvxrt\audio\release\runtime\bin\zlib.dll --preload C:\jvxrt\audio\release\runtime\bin\libfftw3-3.dll --jvxdir jvxComponents --config ayfbinrender.jvx --textlog --verbose_dll --textloglev 10
# Arbeitsverzeichnis	=   C:\jvxrt\build.audio.all\release\runtime
#
