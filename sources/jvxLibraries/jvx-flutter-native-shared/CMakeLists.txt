if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-flutter-native-shared)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/include

	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-lib-host-app/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include
  
	${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common

	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-flutter-native-static/include
  )


  # Add local sources (header files only for project workspace generation!) 
set(LOCAL_SOURCES 	

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx-flutter-native.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-flutter-forward.cpp
	
  )

# Add the shared object export definition
add_export_definition_linker(EXPORT_SOURCES EXPORT_LINKER_FLAGS ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-flutter-native-static/exports/${JVX_OS}/exports-flutter-native.def)
set(LOCAL_SOURCES ${LOCAL_SOURCES} ${EXPORT_SOURCES})
set(LOCAL_LINKER_FLAGS ${LOCAL_LINKER_FLAGS} ${EXPORT_LINKER_FLAGS})
set(LOCAL_LIBS jvx-flutter-native-static_static)

## ======================================================================

set(BUILD_SHARED TRUE)
set(BUILD_STATIC FALSE)

# this changes the name of the output to be followed by "_import"
# It does NOT produce an import lib in the SDK for all projects which
# are compiled to form a runtime
set(JVX_FORCE_RUNTIME_IMPORTLIB TRUE)

# This extends the previous option and allows to copy import lib (.lib file) to sdk also
set(JVX_SDK_FORCE_DEPLOY_IMPORT_STATIC_LIB TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

# Install the MSVC system libraries
if(JVX_OS MATCHES "windows")
	set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${INSTALL_PATH_BIN}")
	include(InstallRequiredSystemLibraries)
endif()

# message(FATAL_ERROR "XYX -> LOCAL_SOURCES = ${LOCAL_SOURCES} -- LOCAL_LINKER_FLAGS = ${LOCAL_LINKER_FLAGS}")

# 
# Befehl				=	O:\ayfdevelop\AudYoFlo\flutter\ayfbinrender\ayfbinrender\build\windows\runner\Debug\ayfBinRender.exe
# Befehlsargumente		=   --workdir C:\jvxrt\audio-sdk\release\sdk --preload C:\jvxrt\audio-sdk\release\sdk\bin\zlib.dll --preload C:\jvxrt\audio-sdk\release\sdk\bin\libfftw3-3.dll --jvxdir jvxComponents --config ayfbinrender.jvx --textlog --verbose_dll --textloglev 10
# Arbeitsverzeichnis	=   C:\jvxrt\audio-sdk\release\sdk
#

