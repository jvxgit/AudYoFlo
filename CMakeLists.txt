cmake_minimum_required (VERSION 2.8.11)

# configure and check subproduct (default audio)
# set(JVX_PRODUCT "audio" CACHE STRING "Select the Javox product to build (audio)")
set(JVX_PRODUCT "audio")

option(JVX_RELEASE_SDK "Produce either a runtime environment or an sdk" OFF)

set(JVX_PREFIX_PROJECT_SPEC "rt")
if(JVX_RELEASE_SDK)
	set(JVX_PREFIX_PROJECT_SPEC "sdk")
endif()

if(JVX_COMPILE_BUILDTOOLS)
	PROJECT(ayf_${JVX_PREFIX_PROJECT_SPEC}_bt)
else()
	PROJECT(ayf_${JVX_PREFIX_PROJECT_SPEC})
endif()

# Check build type
if(NOT ${CMAKE_BUILD_TYPE} MATCHES "^(Debug|Release)$")
  message(FATAL_ERROR "Unknown build type '${CMAKE_BUILD_TYPE}' given")
endif()

if(JVX_DSP_BASE_USE_DATA_FORMAT_FLOAT)
	message(FATAL_ERROR "Using old option <JVX_DSP_BASE_USE_DATA_FORMAT_FLOAT> is deprecated, use <JVX_SYSTEM_USE_DATA_FORMAT_FLOAT> instead")
endif()

# some important directories and settings
set(JVX_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(ENV{JVX_ROOT} ${JVX_ROOT})
set(JVX_BASE_ROOT ${JVX_ROOT})
set(ENV{JVX_BASE_ROOT} ${JVX_BASE_ROOT})
set(JVX_CMAKE_DIR ${JVX_ROOT}/cmake)
set(JVX_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(JVX_GENERATOR_TOKEN ${CMAKE_GENERATOR})
set(JVX_FULL_SYSTEM_BUILD TRUE)

include(${JVX_CMAKE_DIR}/fragments/options.cmake)

if(NOT ${JVX_OS} MATCHES "windows")
	set(JVX_USE_PART_ASIO FALSE)
endif()

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/release CACHE PATH "Set default install path in case we use cmake gui." FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

if(JVX_COMPILE_QT_EGLFS_TAG)
	message("Option for eglfs automatically deactivates SPLASH windows in QT apps")
	set(JVX_SPLASH_SUPPRESSED TRUE)
endif()

###
# basic stuff
###



# Set target subdirectories for install mechanism
if(JVX_RELEASE_SDK)
	set(INSTALL_PATH_RELEASE_SDK "sdk")
	set(INSTALL_PATH_RELEASE_ROOT "sdk")
	set(INSTALL_PATH_RELEASE_ROOT_MIN_SLASH "sdk/")

	set(JVX_MATLAB_PROPERTY_GENERATOR FALSE) # Correct some undefined combinations of settings

	if(JVX_USE_PYTHON)
		set(INSTALL_PATH_PYTHON "${INSTALL_PATH_RELEASE_SDK}/python")		
	endif()
else()
	set(INSTALL_PATH_RELEASE_RUNTIME "runtime")
	set(INSTALL_PATH_RELEASE_ROOT "runtime")
	set(INSTALL_PATH_RELEASE_ROOT_MIN_SLASH "runtime/")

	set(INSTALL_PATH_SCRIPTS "${INSTALL_PATH_RELEASE_RUNTIME}")
	set(INSTALL_PATH_MATLAB "${INSTALL_PATH_RELEASE_RUNTIME}/matlab")
	set(INSTALL_PATH_OCTAVE "${INSTALL_PATH_RELEASE_RUNTIME}/octave")
	

	# We need this helpers to provide locations of foldies for mac os x bundles
	set(JVX_OSX_BUNDLE_HELPER_PATH "${CMAKE_INSTALL_PREFIX}")
	set(JVX_OSX_BUNDLE_HELPER_PATH_APPS "${JVX_OSX_BUNDLE_HELPER_PATH}/${INSTALL_PATH_RELEASE_ROOT_MIN_SLASH}")

	# dylibs end up in bin folder now!
	set(JVX_OSX_BUNDLE_HELPER_PATH_LIB "${JVX_OSX_BUNDLE_HELPER_PATH_APPS}/bin/")

	if(JVX_USE_PYTHON)
		set(INSTALL_PATH_PYTHON "${INSTALL_PATH_RELEASE_RUNTIME}/python")	
	endif()
endif()

if(JVX_USE_PYTHON)
	set(INSTALL_PATH_PYTHON_LIBRARY "${INSTALL_PATH_PYTHON}/library")
	set(INSTALL_PATH_PYTHON_ARCHIVE "${INSTALL_PATH_PYTHON}/archive")
	set(INSTALL_PATH_PYTHON_RUNTIME "${INSTALL_PATH_PYTHON}/runtime")
endif()

# Build tools are NEVER Release builds but the path must be set propperly anyway.
if(JVX_COMPILE_BUILDTOOLS)
	set(JVX_RELEASE_SDK FALSE)
endif()

set(INSTALL_PATH_RUNTIME "${INSTALL_PATH_RELEASE_ROOT}")
set(INSTALL_PATH_BIN "${INSTALL_PATH_RELEASE_ROOT}/bin")
set(INSTALL_PATH_DOC "${INSTALL_PATH_RELEASE_ROOT}/doc")
set(INSTALL_PATH_IMAGES "${INSTALL_PATH_RELEASE_ROOT}/images")
set(INSTALL_PATH_LIB_STATIC "${INSTALL_PATH_RELEASE_ROOT}/lib")
set(INSTALL_PATH_LIB_IMPORT "${INSTALL_PATH_RELEASE_ROOT}/lib")
set(INSTALL_PATH_INCLUDE_STATIC "${INSTALL_PATH_RELEASE_ROOT}/include")
set(INSTALL_PATH_COMPONENTS_STATIC "${INSTALL_PATH_RELEASE_ROOT}/lib/jvxComponents-static")
set(INSTALL_PATH_INCLUDE_COMPONENTS_STATIC "${INSTALL_PATH_RELEASE_ROOT}/include/jvxComponents-static")
set(INSTALL_PATH_LIB_SHARED "${INSTALL_PATH_RELEASE_ROOT}/bin")
set(INSTALL_PATH_INCLUDE_SHARED "${INSTALL_PATH_RELEASE_ROOT}/include")
set(INSTALL_PATH_COMPONENTS_SHARED "${INSTALL_PATH_RELEASE_ROOT}/jvxComponents")
set(INSTALL_PATH_COMPONENTS_IMPORT "${INSTALL_PATH_RELEASE_ROOT}/lib/jvxComponents-import")
set(INSTALL_PATH_INCLUDE_COMPONENTS_SHARED "${INSTALL_PATH_RELEASE_ROOT}/include/jvxComponents-shared")
set(INSTALL_PATH_INCLUDE_COMPONENTS_IMPORT "${INSTALL_PATH_RELEASE_ROOT}/include/jvxComponents-import")
set(INSTALL_PATH_BIN_WASM "${INSTALL_PATH_RELEASE_ROOT}/wasm")

if(AYF_WASM_INSTALL_FOLDER)
	set(INSTALL_PATH_BIN_WASM "${AYF_WASM_INSTALL_FOLDER}")
	#message(FATAL_ERROR "-> ${INSTALL_PATH_BIN_WASM}")
endif()
# message(FATAL_ERROR "-> ${AYF_WASM_INSTALL_FOLDER}")

# Some package specific release folders - if desired
set(INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC ${INSTALL_PATH_LIB_STATIC}/platform-packages)
set(INSTALL_PATH_PLATFORM_PACKAGE_INCLUDE_STATIC ${INSTALL_PATH_INCLUDE_STATIC}/platform-packages)

set(JVX_PRODUCE_FOLDER_HIERARCHIE FALSE)

# Prepare Visual Studio project
if("${CMAKE_GENERATOR}" MATCHES "Visual Studio 17 2022" OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 16 2019" 
	OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017" OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017 Win64" 
	OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 14 2015" OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 14 2015 Win64" 
	OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 11 2012 Win64" OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017 Win64"
	OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017")
	message("Activate VS User template files")

	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	set(JVX_VS_TPL_PATH "${JVX_ROOT}/misc/visualstudio/vs2015")
	set(JVX_REPLACE_ME_CONFIG_EXT "")
	set(JVX_REPLACE_ME "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_RELEASE_ROOT}")
	set(JVX_PRODUCE_FOLDER_HIERARCHIE TRUE)
else()
	message("Not activating VS User template files")
endif()

# include macros used in build system
include(${JVX_CMAKE_DIR}/macros.cmake)

# Just a macro for consistence
set(JVX_BASE_CMAKE_DIR ${JVX_CMAKE_DIR})

# Base folder for cmake build files - BINARY FOLDER
set(JVX_BASE_BINARY_DIR ${JVX_BINARY_DIR})

# Subproduct root folder may be outside the current build tree
set(JVX_SUBPRODUCT_ROOT ${JVX_BASE_ROOT})

# A folder for the subproduct cmake files
set(JVX_SUBPRODUCT_CMAKE_DIR ${JVX_SUBPRODUCT_ROOT}/cmake)

# If so map it here
set(JVX_SUBPRODUCT_BINARY_DIR ${JVX_BINARY_DIR})

# Map all external sub projects for base folder into the binary folder
set(JVXRT_SUBPROJECTS_PATH_MAP ${JVX_BASE_BINARY_DIR}/sub-projects)
set(JVXRT_SUBPROJECTS_FLUTTER_PATH_MAP ${JVX_BASE_BINARY_DIR}/flutter)

# Map all product specific sub-projects here
set(JVXRT_SUBPRODUCT_SUBPROJECTS_BINARY_DIR "${JVX_SUBPRODUCT_BINARY_DIR}/sub-projects")

# Set some shortcut references
set(INSTALL_PATH_MATLAB_SUBPROJECT "${INSTALL_PATH_MATLAB}/m-files/${JVX_PRODUCT}Properties")
set(JVX_BASE_LIBS_INCLUDE_PATH "${JVX_BASE_ROOT}/sources/jvxLibraries")
set(JVX_BASE_LIBS_LIB_PATH "${JVX_BASE_ROOT}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_LIBS_INCLUDE_PATH "${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_LIBS_LIB_PATH "${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries")

# Add include paths for the Matlab/Octave code generation
set(JVX_BASE_LIBS_MATOCT_GEN_LIB "${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-dsp-base-matlab;${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-dsp-matlab")
	 
# Define binary path references for RT and SDK build
set(JVX_BASE_BINARY_INCLUDE_PATH "${JVX_BASE_BINARY_DIR}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_BINARY_INCLUDE_PATH "${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries")

###
# support for different platforms
###
include(${JVX_CMAKE_DIR}/platform.cmake)

# This option is required for qt linkage lateron: other folders for generated output in ninja and VS
get_property(JVX_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

## Here we add the platform specific options
if(JVX_OS MATCHES "macosx" AND JVX_MAC_OS_CREATE_BUNDLE)
	option(JVX_HOST_WITH_NO_ARGS "Compile in command line options" ON)
else()
	option(JVX_HOST_WITH_NO_ARGS "Compile in command line options" OFF)
endif()

include(${JVX_CMAKE_DIR}/fragments/preprocdefines.cmake)

# Activate position independent code for dll linkage
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# common include directories - minimal amount to build at least build tools
include_directories(
	${JVX_BASE_ROOT}/software
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-callprot/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-min/include/platform/${JVX_OS}
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/include/platform/${JVX_OS}
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-dsp-base/include/
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers/include/
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-min/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-dsp-base/include/os-${JVX_OS}
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-json/include/
  ${JVX_SUBPRODUCT_ROOT}/software
  ${CMAKE_CURRENT_BINARY_DIR}/sources/jvxLibraries/jvx-system-min
  )

include(${JVX_CMAKE_DIR}/fragments/gitversion.cmake)

## =============================================================================
## =============================================================================

# Find all external dependencies
include(${JVX_CMAKE_DIR}/fragments/external.cmake)

set(JVX_INTEGRATE_SOURCES_PATH ${JVX_SUBPRODUCT_ROOT})

# Default path and specification of product specific helper lib
set(JVX_SUBPRODUCT_HELPER_PATH "${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/")
set(JVX_SUBPRODUCT_HELPER_PATH_MAP "${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/")
set(JVX_SUBPRODUCT_HELPER_LIB "jvx-helpers-product")

# We may overwrite some of the files here
set(JVX_NO_SYSTEM_EXTENSIONS FALSE)
if(EXISTS ${JVX_SUBPRODUCT_ROOT}/cmake/audio.early.cmake)
	message("--> found ${JVX_SUBPRODUCT_ROOT}/cmake/audio.early.cmake")
	include(${JVX_SUBPRODUCT_ROOT}/cmake/audio.early.cmake)
else()
	message("--> no ${JVX_SUBPRODUCT_ROOT}/cmake/audio.early.cmake found")
endif()

if(JVX_NO_SYSTEM_EXTENSIONS)
	message("Option set to skip all subtype header includes")
	set(GLOBAL_COMPILE_DEFINITIONS "${GLOBAL_COMPILE_DEFINITIONS};JVX_NO_SYSTEM_EXTENSIONS")
endif()

# path to binaries for adding dependencies later
set(JVX_PCG ${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_RELEASE_ROOT}/bin/jvxExPropC${JVX_EXECUTABLE_EXTENSION_OPERATE} CACHE PATH "Set the path to the Javox PCG binary (external path for cross-compilation)")
set(JVX_PCG_MATLAB ${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_RELEASE_ROOT}/bin/jvxExPropMat${JVX_EXECUTABLE_EXTENSION_OPERATE} CACHE PATH "Set the path to the Javox PCG Mat binary (external path for cross-compilation)")
set(JVX_MCG ${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_RELEASE_ROOT}/bin/jvxExMatC${JVX_EXECUTABLE_EXTENSION_OPERATE} CACHE PATH "Set the path to the Javox MCG binary (external path for cross-compilation)")

set(JVX_FOLDER_HIERARCHIE_BASE "AudYoFlo")

message("-> ${JVX_SUBPRODUCT_HELPER_PATH}")
if(JVX_SUBPRODUCT_HELPER_PATH)
	if(EXISTS ${JVX_SUBPRODUCT_HELPER_PATH}${JVX_SUBPRODUCT_HELPER_LIB})
		include_directories(
			${JVX_SUBPRODUCT_HELPER_PATH}${JVX_SUBPRODUCT_HELPER_LIB}/include/)
	endif()
   	include_directories(
   	    ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-product/include/
		)
endif()

# if(${JVX_OS} MATCHES "Emscripten")

#	set(JVX_BASE_SYSTEM_STATIC_LIBS 
#	  base/sources/jvxLibraries/jvx-dsp-base
#	  base/sources/jvxLibraries/jvx-helpers
      #)
#jvx_addSubdirs("Configuring static system libraries" "${JVX_BASE_SYSTEM_STATIC_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/static-system-libs")
	  
#else(${JVX_OS} MATCHES "Emscripten")

set(JVX_BASE_SYSTEM_STATIC_LIBS
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-min
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base
	   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-callprot
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-dsp-base
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-min
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-json
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/git/jsmn
  )

jvx_addSubdirs("Configuring static system libraries" "${JVX_BASE_SYSTEM_STATIC_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/static-libs")

if(JVX_SUBPRODUCT_HELPER_PATH)
	if(EXISTS ${JVX_SUBPRODUCT_HELPER_PATH}${JVX_SUBPRODUCT_HELPER_LIB})		
		jvx_addExtSubdir("Configuring product static system library" 
			"${JVX_SUBPRODUCT_HELPER_PATH}${JVX_SUBPRODUCT_HELPER_LIB}" 
			"${JVX_SUBPRODUCT_HELPER_PATH_MAP}${JVX_SUBPRODUCT_HELPER_LIB}" 
			"${JVX_FOLDER_HIERARCHIE_BASE}/static-libs")
	endif()
endif()

# jvx-json comes along with jsmn in CMAKE dependency
set(JVX_SYSTEM_LIBRARIES
	${JVX_SYSTEM_LIBRARIES}
	jvx-system-min_static
	jvx-system-base_static
	jvx-callprot_static
	jvx-dsp-base_static
	jvx-helpers_static
	jvxLthreads_static
	jvx-component-templates-min_static
	jvx-json_static
	)
	
if(JVX_SUBPRODUCT_HELPER_PATH)
	if(EXISTS ${JVX_SUBPRODUCT_HELPER_PATH}${JVX_SUBPRODUCT_HELPER_LIB})
		set(JVX_SYSTEM_LIBRARIES
			${JVX_SYSTEM_LIBRARIES}
			${JVX_SUBPRODUCT_HELPER_LIB}_static
			)
	endif()
endif()

if(JVX_COMPILE_BUILDTOOLS)

  set(JVX_BASE_EXTOOLS
    ${JVX_BASE_ROOT}/sources/jvxExTools/jvxExMatC
    ${JVX_BASE_ROOT}/sources/jvxExTools/jvxExPropC
    ${JVX_BASE_ROOT}/sources/jvxTools/jvxTconfigProcessor)
  jvx_addSubdirs("Configuring all build helper tools" "${JVX_BASE_EXTOOLS}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvxExTools")


  #install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/base/sources/jvxExTools/jvxExPropC/jvxExPropC DESTINATION ${INSTALL_PATH_BIN})
  #install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/base/sources/jvxExTools/jvxExPropC/jvxExPropMat DESTINATION ${INSTALL_PATH_BIN})
  #install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/base/sources/jvxExTools/jvxExMatC/jvxExMatC DESTINATION ${INSTALL_PATH_BIN})

else(JVX_COMPILE_BUILDTOOLS)

# configure qt # FIXME: to be moved into macros.cmake?
if(JVX_USE_QT)
        find_qt()

	# configure qwt (the lib is built in-tree)
	set(JVX_QCUSTOMPLOT_VERSION "2.0.1") # 2.0.0

else()
	message("--> Qt5 not used")

	# Deactivate the depending QT libraries
	set(JVX_USE_QWT FALSE)
endif()

if(JVX_USE_PART_CUDA)

  enable_language(CUDA)

  message("--> CUDA Include Dir: ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}")
  # message("--> CUDA FFT Libraries: ${CUDA_CUFFT_LIBRARIES}")
  # message("--> CUDA Libraries: ${CUDA_LIBRARIES}")
  # message("--> CUDA Version: ${CUDA_VERSION_STRING}")
endif(JVX_USE_PART_CUDA)


if(JVX_RELEASE_OMAPL137 OR JVX_RELEASE_C674X OR JVX_RELEASE_ARMCORTEXA8)
	find_ccs()
elseif(JVX_RELEASE_ARMCORTEXM4)
	find_mcuxpresso()
endif()

if(JVX_USE_OPENGL_X11)
	find_all_opengl_x11()
elseif(JVX_USE_OPENGL)
	find_gl()	
endif()

if(JVX_USE_OPENCV)
	find_opencv()
endif()

	# BOOST and EIGEN to be handled lateron
	
  # include platform specific stuff
  include(${JVX_SUBPRODUCT_ROOT}/cmake/${JVX_OS}.audio.cmake)

  # configure platform specific stuff (e.g. alsa)
  find_platform_specific()

  if(JVX_FLUTTER_UI)
	set(FLUTTER_SDK_PATH "" CACHE STRING "Flutter sdk find path")
	
	# message("-1-> ${FLUTTER_SDK_PATH}")
	if(NOT EXISTS ${FLUTTER_SDK_PATH} OR ${FLUTTER_SDK_PATH} STREQUAL "" )
		message("FLUTTER_SDK_PATH argument not set, trying to evaluate from environment variable")
		if(DEFINED ENV{FLUTTER_SDK_PATH})
			message("Overriding FLUTTER_SDK_PATH option to ${FLUTTER_SDK_PATH}.")
			set(FLUTTER_SDK_PATH $ENV{FLUTTER_SDK_PATH} CACHE STRING "Flutter sdk find path" FORCE)
		endif()
	endif()
	# message(FATAL_ERROR "-2-> ${FLUTTER_SDK_PATH}")
			
	if(NOT EXISTS ${FLUTTER_SDK_PATH})
		message(FATAL_ERROR "The environment variable <FLUTTER_SDK_PATH> must be set if compiling Flutter support - it is <${FLUTTER_SDK_PATH}>.")
	endif()
  endif()

  if(NOT ${JVX_OS} MATCHES "windows")
		set(JVX_USE_PART_WASAPI FALSE)
  endif()
  if(NOT ${JVX_OS} MATCHES "linux")
		set(JVX_USE_PART_ALSA FALSE)
  endif()
  if(NOT ${JVX_OS} MATCHES "macosx")
		set(JVX_USE_PART_COREAUDIO FALSE)
  endif()

  if(NOT EXISTS "${JVX_PCG}")
	message(FATAL_ERROR "Unable to locate pcg tools <${JVX_PCG}>, run command to build those tools first")
  endif()
  if(NOT EXISTS "${JVX_PCG_MATLAB}")
	message(FATAL_ERROR "Unable to locate pcg tools ${JVX_PCG_MATLAB}, run command to build those tools first")
  endif()
  if(NOT EXISTS "${JVX_MCG}")
	message(FATAL_ERROR "Unable to locate mcg tools ${JVX_MCG}, run command to build those tools first")
  endif()

  if (JVX_USE_PART_RS232)
    set(GLOBAL_COMPILE_DEFINITIONS "${GLOBAL_COMPILE_DEFINITIONS};JVX_RS232_ACTIVE")
  endif()

  include_directories(
			${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base
	)
	
  ###
  # build static libraries
  ###
  set(JVX_BASE_STATIC_LIBS
    ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLsockets_st
    ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-dsp-base-cpp
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-mex
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLexternalCall
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLTextDeviceControl
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-host-json
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLConsoleTools/components-shared
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLConsoleTools/components-static
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLConsoleHost/components-shared
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLConsoleHost/components-static
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-remote-call
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-crc
    # ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-deploy-signals
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-audio-stack
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-automate
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/algorithm_shared
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/algorithm_static
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-opengl-cpp
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-net-helpers
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-sockets
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-audio-sockets-sm
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-buffer-sync
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-micro-connection
	# ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-subqueue
	
	# The audio libs
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-system-product
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-dsp
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-dsp-cpp
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvxLFileIO
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-spectralplot
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-component-templates-product
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-vq
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-signals
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-lib-host-app
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audiocodec-helpers
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/ayf-node-connections
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/ayf-shared-connection-host
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/ayf-component-lib
    )
	
  if(JVX_USE_PART_WEBSERVER)
    set(JVX_BASE_STATIC_LIBS ${JVX_BASE_STATIC_LIBS}
      ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLWebConsoleTools)
    endif()

  if(JVX_USE_QT)
    set(JVX_BASE_STATIC_QT_LIBS 
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-helpers
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-host-widgets
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-central-widgets
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-component-widgets
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-widgets
      ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLQtSaWidgets
      ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLQtSaWidgetWrapper
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLQtMaWidgets
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-http
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-webcontrol
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qt-app-templates
	  )
	  
	if(JVX_COMPILE_GOD_HOST)
		set(JVX_BASE_STATIC_QT_LIBS ${JVX_BASE_STATIC_QT_LIBS}
			${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLDevelopHost)
	endif()
	set(JVX_BASE_STATIC_QT_LIBS ${JVX_BASE_STATIC_QT_LIBS}
	  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-qcp-qt-helpers)
	  
	  set(JVX_BASE_STATIC_QT_LIBS ${JVX_BASE_STATIC_QT_LIBS}
		${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvxLQtAudioWidgets
		${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvxLAudioHost/algorithm_shared
		${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvxLAudioHost/algorithm_static
		${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-qt-measure-ir)

  endif()

	if (JVX_USE_LIBMYSOFA)
		set(JVX_BASE_STATIC_LIBS ${JVX_BASE_STATIC_LIBS}
			${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/ayf-hrtf-dispenser)
	endif()


	include(${JVX_CMAKE_DIR}/fragments/thrdparty.cmake)

  ###
  # build audio technology components
  ###
  set(JVX_BASE_AUDIO_TECHNOLOGIES 
  
		${JVX_BASE_ROOT}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTDefault
		${JVX_BASE_ROOT}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTOfflineAudio
  
		${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTGenericWrapper
		${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioTechnologies/network/jvxAuTNetwork_master
		${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioTechnologies/network/jvxAuTNetwork_slave
		${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTFileReader
		${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTFileWriter
		)
		
  if(JVX_USE_PART_ASIO)
    set(JVX_BASE_AUDIO_TECHNOLOGIES ${JVX_BASE_AUDIO_TECHNOLOGIES} 
		${JVX_BASE_ROOT}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAsio)
  endif(JVX_USE_PART_ASIO)

  if(JVX_USE_PART_WASAPI)
	set(JVX_BASE_AUDIO_TECHNOLOGIES ${JVX_BASE_AUDIO_TECHNOLOGIES} 
		${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTWindows)
  endif()

  if(JVX_USE_PART_PAUDIO)
    set(JVX_BASE_AUDIO_TECHNOLOGIES ${JVX_BASE_AUDIO_TECHNOLOGIES} 
	  ${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTPortAudio)
  endif(JVX_USE_PART_PAUDIO)
  if(JVX_USE_PART_ALSA)
	set(JVX_BASE_AUDIO_TECHNOLOGIES ${JVX_BASE_AUDIO_TECHNOLOGIES} 
		${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTAlsa)
  endif(JVX_USE_PART_ALSA)
  if(JVX_USE_PART_COREAUDIO)
	set(JVX_BASE_AUDIO_TECHNOLOGIES ${JVX_BASE_AUDIO_TECHNOLOGIES} 
		${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTCoreAudio)
  endif(JVX_USE_PART_COREAUDIO)
  if(JVX_USE_PART_IOSAUDIO)
	set(JVX_BASE_AUDIO_TECHNOLOGIES ${JVX_BASE_AUDIO_TECHNOLOGIES} 
		${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioTechnologies/jvxAuTIos)
  endif(JVX_USE_PART_IOSAUDIO)

  ###
  # build video technology components
  ###
  set(JVX_BASE_VIDEO_TECHNOLOGIES
	${JVX_BASE_ROOT}/sources/jvxComponents/jvxVideoTechnologies/jvxViTDefault
	)
  if(JVX_OS MATCHES "windows")
	set(JVX_BASE_VIDEO_TECHNOLOGIES ${JVX_BASE_VIDEO_TECHNOLOGIES}
		${JVX_BASE_ROOT}/sources/jvxComponents/jvxVideoTechnologies/jvxViTMfOpenGL)
  endif()

  set(JVX_BASE_CUSTOM_TECHNOLOGIES
		${JVX_BASE_ROOT}/sources/jvxComponents/jvxCustomTechnologies/jvxCuTDefault
		${JVX_BASE_ROOT}/sources/jvxComponents/jvxCustomTechnologies/jvxCuTNTask
		${JVX_BASE_ROOT}/sources/jvxComponents/jvxCustomTechnologies/jvxCuTGps
		)

  ###
  # build signal processing technology components
  ###
  set(JVX_BASE_SIGNAL_PROCESSING_TECHNOLOGIES 
	${JVX_BASE_ROOT}/sources/jvxComponents/jvxSignalProcessingTechnologies/jvxSpTDefault
	${JVX_BASE_ROOT}/sources/jvxComponents/jvxSignalProcessingTechnologies/jvxSpTDeploySignal
	${JVX_BASE_ROOT}/sources/jvxComponents/jvxSignalProcessingTechnologies/jvxSpTSocketSignal
	)


  ###
  # build external controllers
  ###
  set(JVX_BASE_APPLICATION_CONTROLLER_TECHNOLOGIES 
	${JVX_BASE_ROOT}/sources/jvxComponents/jvxAppCtrTech/jvxAppTDefault)
	
  if(JVX_USE_PART_CORECONTROL)
    set(JVX_BASE_APPLICATION_CONTROLLER_TECHNOLOGIES ${JVX_BASE_APPLICATION_CONTROLLER_TECHNOLOGIES} ${JVX_BASE_ROOT}/sources/jvxComponents/jvxAppCtrTech/jvxAppTCoreController)
  endif(JVX_USE_PART_CORECONTROL)

  ###
  # build external controllers
  ###
  set(JVX_BASE_APPLICATION_CONTROLLER_NODES ${JVX_BASE_ROOT}/sources/jvxComponents/jvxAppCtrNodes/jvxAppNWeb)
  

  ###
  # build audio nodes
  ###
  set(JVX_BASE_AUDIO_NODES
    ${JVX_BASE_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNDefault
    ${JVX_BASE_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNDefaultL
	${JVX_BASE_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNBareNodeExample
	${JVX_BASE_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNBareNNodeExample
    ${JVX_BASE_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNRTViewer
    ${JVX_BASE_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNmexCall
    ${JVX_BASE_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNSimple
	
	${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNPlayChannelId
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNBinauralRender
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNCaptureFile
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNBitstreamDecoder
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNBitstreamEncoder
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNForwardBuffer
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNMeasureIr2Dev
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNChannelRearrange
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioNodes/ayfAuNMatlab
  )
  
  if(JVX_DEPLOY_EXAMPLES)
	set(JVX_BASE_AUDIO_NODES ${JVX_BASE_AUDIO_NODES}
		${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNVocoder
		${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNMyAlgorithm
		${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioNodes/jvxAuNCodec
		)
  endif()

  ###
  # build signal processing nodes
  ###
  set(JVX_BASE_SIGNAL_PROCESSING_NODES
    ${JVX_BASE_ROOT}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNDefault
    ${JVX_BASE_ROOT}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNmexCall
	${JVX_BASE_ROOT}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNBareNNodeExample
	${JVX_BASE_ROOT}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNAsyncIo
	## ${JVX_BASE_ROOT}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSubQueue -> moved to jvx-hoa
	${JVX_BASE_ROOT}/sources/jvxComponents/jvxSignalProcessingNodes/templates/jvxSpNSimpleZeroCopy
	
	${JVX_BASE_ROOT}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNMeasureIr
	${JVX_BASE_ROOT}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeakerEqualizer
	${JVX_BASE_ROOT}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNLevelMeter
	${JVX_BASE_ROOT}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNSpeaker2Binaural
	)

  	
if(JVX_USE_OPENGL_X11)

  ###
  # build audio nodes
  ###
  set(JVX_BASE_VIDEO_NODES
    ${JVX_BASE_ROOT}/sources/jvxComponents/jvxVideoNodes/jvxViNDefault
    ${JVX_BASE_ROOT}/sources/jvxComponents/jvxVideoNodes/jvxViNopenGLViewer
	)
endif()

  ###
  # build hosts
  ###
  set(JVX_BASE_HOST_COMPONENTS
    ${JVX_BASE_ROOT}/sources/jvxHosts/jvxHDefault
    ${JVX_BASE_ROOT}/sources/jvxHosts/jvxHJvx
	${JVX_BASE_ROOT}/sources/jvxHosts/jvxHCon
	${JVX_BASE_ROOT}/sources/jvxFactoryHosts/jvxFHDefault
	${JVX_BASE_ROOT}/sources/jvxFactoryHosts/jvxFHCon)

  ###
  # build event loops
  ###
  set(JVX_BASE_EVENTLOOP_COMPONENTS
    ${JVX_BASE_ROOT}/sources/jvxEventLoop/CjvxEStandalone)

  ##
  # build audio codecs
  ##
  set(JVX_BASE_AUDIOCODEC_COMPONENTS ${JVX_BASE_AUDIOCODEC_COMPONENTS}
		${JVX_SUBPRODUCT_ROOT}/sources/jvxComponents/jvxAudioCodecs/jvxAuCPcm)

  ###
  # build other components
  ###
  set(JVX_BASE_OTHER_COMPONENTS
    ${JVX_BASE_ROOT}/sources/jvxTools/jvxTSimpleDotfuscation
    ${JVX_BASE_ROOT}/sources/jvxTools/jvxTDataLogger
    ${JVX_BASE_ROOT}/sources/jvxTools/jvxTSystemTextLog
	${JVX_BASE_ROOT}/sources/jvxTools/jvxTLocalTextLog
	${JVX_BASE_ROOT}/sources/jvxTools/jvxTRemoteCall
	${JVX_BASE_ROOT}/sources/jvxTools/jvxTSocket
    ${JVX_BASE_ROOT}/sources/jvxTools/jvxTDataLogReader
	${JVX_BASE_ROOT}/sources/jvxTools/jvxPacketFilterRules/jvxPFRDefault
	${JVX_BASE_ROOT}/sources/jvxTools/jvxTThreads
	${JVX_BASE_ROOT}/sources/jvxTools/jvxTSharedMemory
	
	${JVX_SUBPRODUCT_ROOT}/sources/jvxTools/jvxTResampler
	${JVX_SUBPRODUCT_ROOT}/sources/jvxTools/jvxTDataConverter
	${JVX_SUBPRODUCT_ROOT}/sources/jvxTools/jvxTrtAudioFileReader
	${JVX_SUBPRODUCT_ROOT}/sources/jvxTools/jvxTrtAudioFileWriter
	)

  if(${JVX_OS} MATCHES "windows")
		set(JVX_BASE_OTHER_COMPONENTS ${JVX_BASE_OTHER_COMPONENTS} ${JVX_BASE_ROOT}/sources/jvxTools/jvxTMSWText2Speech)
  endif()

  if(JVX_USE_PART_WEBSERVER)
    set(JVX_BASE_OTHER_COMPONENTS ${JVX_BASE_OTHER_COMPONENTS} ${JVX_BASE_ROOT}/sources/jvxTools/jvxTWebServer)
  endif()
  if(JVX_USE_PART_CORECONTROL)
    set(JVX_BASE_OTHER_COMPONENTS ${JVX_BASE_OTHER_COMPONENTS} ${JVX_BASE_ROOT}/sources/jvxTools/jvxTThreadController)
  endif(JVX_USE_PART_CORECONTROL)
  if(JVX_USE_PART_RS232)
	if(${JVX_OS} MATCHES "windows")
		set(JVX_BASE_OTHER_COMPONENTS ${JVX_BASE_OTHER_COMPONENTS} ${JVX_BASE_ROOT}/sources/jvxTools/jvxTRs232Win32)
	endif()
	if(${JVX_OS} MATCHES "linux")
		set(JVX_BASE_OTHER_COMPONENTS ${JVX_BASE_OTHER_COMPONENTS} ${JVX_BASE_ROOT}/sources/jvxTools/jvxTRs232Glnx)
	endif()
  endif(JVX_USE_PART_RS232)
  set(JVX_BASE_OTHER_COMPONENTS ${JVX_BASE_OTHER_COMPONENTS} ${JVX_BASE_ROOT}/sources/jvxTools/jvxTconfigProcessor)
  if(JVX_USE_PART_MATLAB)
    set(JVX_BASE_OTHER_COMPONENTS ${JVX_BASE_OTHER_COMPONENTS} ${JVX_BASE_ROOT}/sources/jvxTools/jvxTExternalCall/cmake/matlab)
  endif(JVX_USE_PART_MATLAB)
  if(JVX_USE_PART_OCTAVE)
    set(JVX_BASE_OTHER_COMPONENTS ${JVX_BASE_OTHER_COMPONENTS} ${JVX_BASE_ROOT}/sources/jvxTools/jvxTExternalCall/cmake/octave)
  endif(JVX_USE_PART_OCTAVE)

	
	## In case of SDK build and if buildng for iOS do not compile any applications
  if(NOT JVX_RELEASE_SDK AND NOT IOS)
	###
	# build applications (tools)
	###
	set(JVX_BASE_APPS
		${JVX_BASE_ROOT}/sources/jvxApplications/jvxShowDefines
		${JVX_BASE_ROOT}/sources/jvxApplications/jvxConsoleApp
		${JVX_BASE_ROOT}/sources/jvxApplications/jvxSocketTestServer
		${JVX_BASE_ROOT}/sources/jvxApplications/jvxSCtrl
		)

	if(JVX_DEPLOY_EXAMPLES)
		
		if(JVX_USE_PART_WEBSERVER)
			set(JVX_BASE_APPS ${JVX_BASE_APPS}
				${JVX_BASE_ROOT}/sources/jvxApplications/jvxWebControlApp
				)
		endif()
		
		if(JVX_USE_QT)
			set(JVX_BASE_APPS ${JVX_BASE_APPS}
				${JVX_BASE_ROOT}/sources/jvxApplications/jvxQtWebControl
				${JVX_BASE_ROOT}/sources/jvxApplications/jvxQmlTest
				${JVX_BASE_ROOT}/sources/jvxApplications/qtexamples/application
				${JVX_BASE_ROOT}/sources/jvxApplications/qtexamples/openglwindow
				
				${JVX_SUBPRODUCT_ROOT}/sources/jvxApplications/jvxRtprocHost
				${JVX_SUBPRODUCT_ROOT}/sources/jvxApplications/jvxRtprocNetHost
				${JVX_SUBPRODUCT_ROOT}/sources/jvxApplications/jvxRtprocHost-1
				${JVX_SUBPRODUCT_ROOT}/sources/jvxApplications/jvxVocoder
				${JVX_SUBPRODUCT_ROOT}/sources/jvxApplications/jvxMyHost
				${JVX_SUBPRODUCT_ROOT}/sources/jvxApplications/jvxMeasureIr
				${JVX_SUBPRODUCT_ROOT}/sources/jvxApplications/jvxMeasureIr2Dev
				${JVX_SUBPRODUCT_ROOT}/sources/jvxApplications/ayfHosts/ayfQtAudioHostStart
				)
				
			# Develop host
			if(JVX_USE_QWT AND JVX_COMPILE_GOD_HOST)
				set(JVX_BASE_APPS ${JVX_BASE_APPS}
					sources/jvxApplications/jvx-qt-host-1
					)
			else()
				message("XX> host jvx-qt-host-1 disabled (QWT and/or QWT-Polar disabled)")
			endif()
		
		endif()

		set(JVX_BASE_APPS ${JVX_BASE_APPS}
			${JVX_BASE_ROOT}/sources/jvxApplications/jvxRemoteCallServer
			${JVX_BASE_ROOT}/sources/jvxApplications/jvxRemoteCallClient
			${JVX_BASE_ROOT}/sources/jvxApplications/jvxUdpDataViewer
			
			${JVX_SUBPRODUCT_ROOT}/sources/jvxApplications/sv56Demo
			${JVX_SUBPRODUCT_ROOT}/sources/jvxApplications/jvxSocketTestClient			
			)
			
		if(JVX_USE_OPENGL_X11)
			if(${JVX_OS} MATCHES "windows")
			  set(JVX_BASE_APPS ${JVX_BASE_APPS}
				${JVX_BASE_ROOT}/sources/jvxApplications/jvxTestOpenGl
				${JVX_BASE_ROOT}/sources/jvxApplications/jvxTestCameraOpenGl
				)
			      endif()
		endif()

		if(JVX_USE_PCAP)
			set(JVX_BASE_APPS ${JVX_BASE_APPS}
				${JVX_BASE_ROOT}/sources/jvxApplications/jvxPacketFilterBridge)
		endif()

		if(JVX_USE_QT)
			if(JVX_USE_PART_WEBSERVER)
				set(JVX_BASE_APPS ${JVX_BASE_APPS}
					${JVX_BASE_ROOT}/sources/jvxProjects/jvxWebExample)
			endif()
		endif()
	endif()

	if(JVX_USE_PART_MATLAB)
		set(JVX_BASE_APPS ${JVX_BASE_APPS}
			${JVX_BASE_ROOT}/sources/jvxApplications/jvxDataLogReader/cmake/matlab
			${JVX_BASE_ROOT}/sources/jvxApplications/jvxConfigReader/cmake/matlab
			${JVX_BASE_ROOT}/sources/jvxApplications/jvxConfigWriter/cmake/matlab	
			
			${JVX_SUBPRODUCT_ROOT}/sources/jvxApplications/jvx-m-host-1/cmake/matlab)
	endif(JVX_USE_PART_MATLAB)
	if(JVX_USE_PART_OCTAVE)
		set(JVX_BASE_APPS ${JVX_BASE_APPS}
			${JVX_BASE_ROOT}/sources/jvxApplications/jvxDataLogReader/cmake/octave
			${JVX_BASE_ROOT}/sources/jvxApplications/jvxConfigReader/cmake/octave
			${JVX_BASE_ROOT}/sources/jvxApplications/jvxConfigWriter/cmake/octave
			
			${JVX_SUBPRODUCT_ROOT}/sources/jvxApplications/jvx-m-host-1/cmake/octave)
	endif(JVX_USE_PART_OCTAVE)

	set(JVX_MEX_FOLDER_HIERARCHIE "${JVX_FOLDER_HIERARCHIE_BASE}/apps")
	add_subdirectory(${JVX_BASE_ROOT}/sources/jvxApplications/mex)

  endif()

 	# ===========================================================================
	# Flutter project code
	# ===========================================================================
	if(JVX_FLUTTER_UI)
		##
		# build flutter native bindings
		##
		set(JVX_BASE_SHARED_LIBS 
			${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-flutter-native-static
			${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-flutter-native-shared)
	endif()

	# ===========================================================================
	# Add plugin extender lib - e.g., for VSTe
	# ===========================================================================
	set(JVX_SDK_EXTENDER_LIBS ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audioplugins-extender/jvx-plugin-connector)
	
	# ==========================================================================
	# WP Modules are compiled to webassembly (operate in web browser) OR python
	# ==========================================================================
	set(JVX_BASE_WPMODULES_PROJECTS 
		${JVX_SUBPRODUCT_ROOT}/sources/wp-modules/simpletest
		# sources/wp-modules/simpletest2 <- this module is no longer in use. It was used for some kind of testing on Firefox
		)	

	# ==========================================================================
	# Python only modules
	# ==========================================================================
	if(JVX_USE_PYTHON)
		##
		# build python functions
		##
		set(JVX_BASE_PYTHON_PROJECTS ${JVX_BASE_PYTHON_PROJECTS}
			sources/python/jvx-dsp-python
			sources/python/jvx-helper-python
			)		
	endif()

	# ==========================================================================
	# ==========================================================================
	# Add the omap l137 packages if desired
    set(JVX_PLATFORM_PACKAGES "")
		if(JVX_RELEASE_OMAPL137)
			set(JVX_BASE_PLATFORM_PACKAGES ${JVX_BASE_PLATFORM_PACKAGES}
            ${JVX_BASE_ROOT}/platform-packages/omapl137/jvx-remote-call-pkg
			${JVX_BASE_ROOT}/platform-packages/omapl137/jvx-dsp-base
        )
    elseif(JVX_RELEASE_C674X)
        set(JVX_BASE_PLATFORM_PACKAGES ${JVX_BASE_PLATFORM_PACKAGES}
            ${JVX_BASE_ROOT}/platform-packages/c674x/jvx-dsp-base
			${JVX_BASE_ROOT}/platform-packages/c674x/jvx-remote-call-pkg
			${JVX_BASE_ROOT}/platform-packages/c674x/jvx-audio-stack
        )
    elseif(JVX_RELEASE_ARMCORTEXA8)
        set(JVX_BASE_PLATFORM_PACKAGES ${JVX_BASE_PLATFORM_PACKAGES}
            ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/platform-packages/arm_cortex_a8/libav_fft
            ${JVX_BASE_ROOT}/platform-packages/arm_cortex_a8/jvx-dsp-base
        )
    elseif(JVX_RELEASE_ARMCORTEXM4)
        set(JVX_BASE_PLATFORM_PACKAGES ${JVX_BASE_PLATFORM_PACKAGES}
            ${JVX_BASE_ROOT}/platform-packages/arm_cortex_m4/jvx-dsp-base
            ${JVX_BASE_ROOT}/platform-packages/arm_cortex_m4/jvx-system-base
            ${JVX_BASE_ROOT}/platform-packages/arm_cortex_m4/jvx-helpers
        )
    endif()

  if(JVX_RELEASE_OMAPL137)
	set(JVX_BASE_PLATFORM_PACKAGES ${JVX_BASE_PLATFORM_PACKAGES}
		${JVX_SUBPRODUCT_ROOT}/platform-packages/omapl137/jvx-engine-pkg
	)
  elseif(JVX_RELEASE_C674X)
	set(JVX_BASE_PLATFORM_PACKAGES ${JVX_BASE_PLATFORM_PACKAGES}
		${JVX_SUBPRODUCT_ROOT}/platform-packages/c674x/cminpack-1
		${JVX_SUBPRODUCT_ROOT}/platform-packages/c674x/jvx-dsp
	)
  elseif(JVX_RELEASE_ARMCORTEXM4)
    set(JVX_BASE_PLATFORM_PACKAGES ${JVX_BASE_PLATFORM_PACKAGES}
         ${JVX_SUBPRODUCT_ROOT}/platform-packages/arm_cortex_m4/jvx-dsp
		 ${JVX_SUBPRODUCT_ROOT}/platform-packages/hitex4500/jvx-engine
		 ${JVX_SUBPRODUCT_ROOT}/platform-packages/hitex4500/jvx-engine-lib
    )
  endif()
  
	if(JVX_USE_VST )
	
		option(JVX_USE_VST_3_7_2 "Vst Version is 3.7.2" OFF)
		
		if(JVX_USE_VST_3_7_2)
			if(NOT EXISTS ${JVXRT_SUBMODULE_PATH}/vst3sdk)	
				message("vst sub project not available. Starting script to download it.") 
				execute_process(COMMAND ${mysh} -c "cd ${JVXRT_SUBMODULE_PATH}; pwd ; ../../patches/vst/prepareModules-3.7.2.sh; exit")
				message("vst installation complete.")		
			endif()
		
			set(SMTG_CREATE_PLUGIN_LINK FALSE)
			set(SMTG_PLUGIN_TARGET_PATH "${JVX_SUBPRODUCT_BINARY_DIR}")
			set(VST_FORCE_PLUGINS_EXAMPLES TRUE)
			set(JVX_VST_SDK_EXAMPLES ${JVXRT_SUBMODULE_PATH}/vst3sdk)
			set(JVX_VST_SDK_EXAMPLES_MAP ${JVXRT_SUBMODULE_PATH}/vst3sdk)
		
			set(JVX_VST_SDK_ROOT "${JVX_VST_SDK_EXAMPLES}")
			set(JVX_VSTGUI_SDK_ROOT "${JVX_VST_SDK_ROOT}/vstgui4")
			jvx_addExtSubdir("Configuring all VST examples" "${JVX_VST_SDK_EXAMPLES}" "${JVX_VST_SDK_EXAMPLES_MAP}" "${JVX_FOLDER_HIERARCHIE_BASE}/vst-sdk3")
	
			set(JVX_VST_SDK_EXTENDER_LIBS 
				${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audioplugins-extender/jvx-vst-plugins)
	
			set(JVX_VST_SDK_WIDGET_LIBS 
				${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audioplugin-widgets/jvx-vst-widgets)
		else()
			# set(VSTSDK_VERSION_COLLECT "unknown" CACHE INTERNAL "VST3 SDK Version")
		
			if(NOT EXISTS ${JVXRT_SUBMODULE_PATH}/vst3sdk)	
				message("vst sub project not available. Starting script to download it.") 
				execute_process(COMMAND ${mysh} -c "cd ${JVXRT_SUBMODULE_PATH}; pwd ; ../../patches/vst/prepareModules-3.7.7.sh; exit")
				message("qwt installation complete.")		
			endif()
		
			set(SMTG_CREATE_PLUGIN_LINK FALSE)
			set(SMTG_PLUGIN_TARGET_PATH "${JVX_SUBPRODUCT_BINARY_DIR}")
			set(VST_FORCE_PLUGINS_EXAMPLES TRUE)
			set(JVX_VST_SDK_EXAMPLES ${JVXRT_SUBMODULE_PATH}/vst3sdk)
			set(JVX_VST_SDK_EXAMPLES_MAP ${JVXRT_SUBMODULE_PATH}/vst3sdk)
			
			set(JVX_VST_SDK_ROOT "${JVX_VST_SDK_EXAMPLES}")
			set(JVX_VSTGUI_SDK_ROOT "${JVX_VST_SDK_ROOT}/vstgui4")
			jvx_addExtSubdir("Configuring all VST examples" "${JVX_VST_SDK_EXAMPLES}" "${JVX_VST_SDK_EXAMPLES_MAP}" "${JVX_FOLDER_HIERARCHIE_BASE}/vst-sdk3")
		
			set(JVX_VST_SDK_EXTENDER_LIBS 
				${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audioplugins-extender/jvx-vst-plugins)
		
			set(JVX_VST_SDK_WIDGET_LIBS 
				${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audioplugin-widgets/jvx-vst-widgets)
				
			get_property(VSTSDK_VERSION_COLLECT GLOBAL PROPERTY VSTSDK_VERSION_COLLECT_PROPERTY)
			# message(FATAL_ERROR "-> ${VSTSDK_VERSION_COLLECT}")
		endif()
	endif()

	# ===========================================================================
	jvx_addSubdirs("Configuring all 3rd party libraries" "${JVX_BASE_3RDPARTY_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/static-libs/3rdparty")
  
	# ===========================================================================
	# Check and configure some of the linked 3rd party libs
	if(JVX_USE_BOOST)
		find_boost()	
	endif()

	if(JVX_USE_EIGEN)

		# Library from here: http://eigen.tuxfamily.org/index.php?title=Main_Page
		find_eigen()
	endif()
  
	# configure pcap
	if(JVX_USE_PCAP)
		find_pcap("FATAL")
		# message(FATAL_ERROR "Error")
	endif()

	# ===========================================================================

  jvx_addSubdirs("Configuring all static libraries" "${JVX_BASE_STATIC_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/static-libs")
  jvx_addSubdirs("Configuring all static QT libraries" "${JVX_BASE_STATIC_QT_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/static-libs/qt")
  
  jvx_addSubdirs("Configuring all shared libraries" "${JVX_BASE_SHARED_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/shared-libs")

  jvx_addSubdirs("Configuring all audio technology components" "${JVX_BASE_AUDIO_TECHNOLOGIES}" "${JVX_FOLDER_HIERARCHIE_BASE}/components/audio-technology-components")
  jvx_addSubdirs("Configuring all video technology components" "${JVX_BASE_VIDEO_TECHNOLOGIES}" "${JVX_FOLDER_HIERARCHIE_BASE}/components/video-technology-components")
  jvx_addSubdirs("Configuring all custom technology components" "${JVX_BASE_CUSTOM_TECHNOLOGIES}" "${JVX_FOLDER_HIERARCHIE_BASE}/components/custom-technology-components")
  jvx_addSubdirs("Configuring all signal processing technology components" "${JVX_BASE_SIGNAL_PROCESSING_TECHNOLOGIES}" "${JVX_FOLDER_HIERARCHIE_BASE}/components/signalprocessing-technology-components")
  jvx_addSubdirs("Configuring all external controller technology components" "${JVX_BASE_APPLICATION_CONTROLLER_TECHNOLOGIES}" "${JVX_FOLDER_HIERARCHIE_BASE}/components/application-controller-components")
  jvx_addSubdirs("Configuring all external controller node components" "${JVX_BASE_APPLICATION_CONTROLLER_NODES}" "${JVX_FOLDER_HIERARCHIE_BASE}/components/application-controller-nodes")
  jvx_addSubdirs("Configuring all audio node components" "${JVX_BASE_AUDIO_NODES}" "${JVX_FOLDER_HIERARCHIE_BASE}/components/audio-node-components")
  jvx_addSubdirs("Configuring all video node components" "${JVX_BASE_VIDEO_NODES}" "${JVX_FOLDER_HIERARCHIE_BASE}/components/video-node-components")  
  jvx_addSubdirs("Configuring all signal processing node components" "${JVX_BASE_SIGNAL_PROCESSING_NODES}" "${JVX_FOLDER_HIERARCHIE_BASE}/components/signalprocessing-node-components")
  jvx_addSubdirs("Configuring all signal processing node templates" "${JVX_BASE_SIGNAL_PROCESSING_NODE_TEMPLATES}" "${JVX_FOLDER_HIERARCHIE_BASE}/components/signalprocessing-node-component-templates")
  jvx_addSubdirs("Configuring all factory components" "${JVX_BASE_FACTORY_COMPONENTS}" "${JVX_FOLDER_HIERARCHIE_BASE}/components/factory-components")
  jvx_addSubdirs("Configuring all host components" "${JVX_BASE_HOST_COMPONENTS}" "${JVX_FOLDER_HIERARCHIE_BASE}/components/host-components")  
  jvx_addSubdirs("Configuring all event loop components" "${JVX_BASE_EVENTLOOP_COMPONENTS}" "${JVX_FOLDER_HIERARCHIE_BASE}/components/eventloop-components")
  jvx_addSubdirs("Configuring all audio codec components" "${JVX_BASE_AUDIOCODEC_COMPONENTS}" "${JVX_FOLDER_HIERARCHIE_BASE}/components/audiocodec-components")
  jvx_addSubdirs("Configuring all other components" "${JVX_BASE_OTHER_COMPONENTS}" "${JVX_FOLDER_HIERARCHIE_BASE}/components/other-components")  
  
  jvx_addSubdirs("Configuring internal development helper applications" "${JVX_BASE_APPS}" "${JVX_FOLDER_HIERARCHIE_BASE}/apps")
  
  jvx_addSubdirs("Configuring all extender libraries" "${JVX_SDK_EXTENDER_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/plugins/plugins-extender")
  jvx_addSubdirs("Configuring all VST extender libraries" "${JVX_VST_SDK_EXTENDER_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/plugins/plugins-extender")
  jvx_addSubdirs("Configuring all VST widget libraries" "${JVX_VST_SDK_WIDGET_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/plugins/plugins-widgets")

  jvx_addSubdirs("Configuring all platform packages" "${JVX_BASE_PLATFORM_PACKAGES}" "${JVX_FOLDER_HIERARCHIE_BASE}/platform-packages")
  
  if(${JVX_OS} MATCHES "Emscripten")
		jvx_addSubdirs("Configuring all audio codec components" "${JVX_BASE_WPMODULES_PROJECTS}" "${JVX_FOLDER_HIERARCHIE_BASE}/webassembly")	
	else()
		jvx_addSubdirs("Configuring all audio codec components" "${JVX_BASE_WPMODULES_PROJECTS}" "${JVX_FOLDER_HIERARCHIE_BASE}/python-projects")	
	endif()
	
  jvx_addSubdirs("Configuring all python projects" "${JVX_BASE_PYTHON_PROJECTS}" "${JVX_FOLDER_HIERARCHIE_BASE}/python-projects")

#set(AYF_FLUTTER_ENVIRONMENT TRUE)
#set(JVX_FLUTTER_PLUGINS ${JVX_FLUTTER_PLUGINS} sources/sub-projects/flutter/ayf-plugins/ayf_host/windows)
#jvx_addSubdirs("Configuring all flutter plugins" "${JVX_FLUTTER_PLUGINS}" "${JVX_FOLDER_HIERARCHIE_BASE}/flutter-plugins")

  ###
  # installation stuff
  ###

  if(JVX_DEPLOY_LIBRARY_DSP_MATLAB)
    set(JVX_MATLAB_LIBS "" )
    if(JVX_USE_PART_MATLAB)
      set(JVX_MATLAB_LIBS 
		${JVX_MATLAB_LIBS}
			${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-dsp-base-matlab/cmake/matlab
			${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-dsp-matlab/cmake/matlab
        )
    endif(JVX_USE_PART_MATLAB)
    if(JVX_USE_PART_OCTAVE)
      set(JVX_MATLAB_LIBS ${JVX_MATLAB_LIBS}
        ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-dsp-base-matlab/cmake/octave
		${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-dsp-matlab/cmake/octave
        )
    endif(JVX_USE_PART_OCTAVE)

	set(JVX_FOLDER_HIERARCHIE "matlab-octave")
    jvx_addSubdirs("Installation for all Matlab/Octave library files" "${JVX_MATLAB_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/matlab-octave")
  endif(JVX_DEPLOY_LIBRARY_DSP_MATLAB)

	include(${JVX_CMAKE_DIR}/fragments/submodules.cmake)
	jvx_submodules("${JVXRT_SUBMODULE_PATH}" "${JVXRT_SUBPROJECTS_PATH_MAP}") # Path is set in the options file
	jvx_submodules("${JVXRT_SUBMODULE_FLUTTER_PATH}" "${JVXRT_SUBPROJECTS_FLUTTER_PATH_MAP}") # Path is set in the options file
	# message(FATAL_ERROR "Stop")
	
  if(JVX_GENERATE_DEPEND_GRAPH)
	add_custom_target(graphviz ALL
		COMMAND ${CMAKE_COMMAND} "--graphviz=${JVX_PRODUCT}.dot" .
		COMMAND dot -Tpng ${JVX_PRODUCT}.dot -o ${JVX_PRODUCT}.png
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
		)
   endif()

  if(NOT JVX_RELEASE_SDK)
    # files which need to be installed independently of any subproject or for more than one project
    if(JVX_CRYPT_MATLAB_CODE)
      if(JVX_USE_PART_MATLAB)
	add_subdirectory(cmake/post_install/matlab_pcode)
      endif(JVX_USE_PART_MATLAB)
    endif(JVX_CRYPT_MATLAB_CODE)

	if(JVX_COLLECT_MAC_BUNDLES)
		message("All handled bundles: ${JVX_COLLECT_MAC_BUNDLES}")
		configure_file(${JVX_BASE_ROOT}/cmake-sdk/standalone-cfg-macosx.in ${CMAKE_CURRENT_BINARY_DIR}/cmake/standalone-cfg-macosx.cmake)
    		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/standalone-cfg-macosx.cmake DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/cmake)
	endif()

	install(DIRECTORY ${JVX_SUBPRODUCT_ROOT}/wavs/ DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/wavs)
	
	if(JVX_VS_START_PROJECT)
		message("--> Activating VS start project ${JVX_VS_START_PROJECT}")
		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${JVX_VS_START_PROJECT})
	endif()

  else(NOT JVX_RELEASE_SDK)
    configure_file(${JVX_BASE_ROOT}/cmake-sdk/standalone-cfg.in ${CMAKE_CURRENT_BINARY_DIR}/cmake/standalone-cfg.cmake)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/standalone-cfg.cmake DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/cmake)
    install(DIRECTORY ${JVX_BASE_ROOT}/cmake-sdk DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/cmake FILES_MATCHING PATTERN "*.cmake")
    install(DIRECTORY ${JVX_SUBPRODUCT_ROOT}/cmake-sdk DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/cmake FILES_MATCHING PATTERN "*.cmake")
    install(DIRECTORY ${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software/codeFragments/jvxHosts) # FILES_MATCHING PATTERN "*.dylib")
    install(DIRECTORY ${JVX_BASE_ROOT}/software/codeFragments/simplify DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software/codeFragments) # FILES_MATCHING PATTERN "*.dylib")
    ## install(DIRECTORY ${JVX_BASE_ROOT}/software/codeFragments/commandline DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software/codeFragments) # FILES_MATCHING PATTERN "*.dylib")
    ### install(DIRECTORY ${JVX_BASE_ROOT}/software/codeFragments/jvxTools DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software/codeFragments) # FILES_MATCHING PATTERN "*.dylib")
    ## install(DIRECTORY ${JVX_BASE_ROOT}/software/interfaces DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software) # FILES_MATCHING PATTERN "*.dylib")
    ## install(DIRECTORY ${JVX_BASE_ROOT}/software/typedefs DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software)
    install(DIRECTORY ${JVX_BASE_ROOT}/software/compatibility DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software)
    ## install(DIRECTORY ${JVX_BASE_ROOT}/software/platform DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software)
    install(DIRECTORY ${JVX_BASE_ROOT}/software/templates DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software)
    install(DIRECTORY ${JVX_BASE_ROOT}/software/exports DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software)
    install(DIRECTORY ${JVX_BASE_ROOT}/cmake DESTINATION ${INSTALL_PATH_RELEASE_ROOT})
	
	# Need to copy this header only library
	install(DIRECTORY ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-cpp/include DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/include/jvx-helpers-cpp)
    if(${JVX_OS} MATCHES "windows")
      install(FILES ${FFT_LIBRARIES} DESTINATION ${INSTALL_PATH_LIB_STATIC}/${FFTW_INCLUDE_PATH_OSGUESS_PREFIX})
      install(DIRECTORY ${FFT_INCLUDEDIR}/ DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/${FFTW_INCLUDE_PATH_OSGUESS_PREFIX} FILES_MATCHING PATTERN "*.h")
	  install(DIRECTORY ${JVX_BASE_ROOT}/misc/visualstudio DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/misc)
      install(DIRECTORY ${JVX_BASE_ROOT}/misc/sdk-copy/windows/tools DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/../)
    endif(${JVX_OS} MATCHES "windows")
    #install(FILES ${JVX_BASE_ROOT}/software/jvx.h DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software)
    #install(FILES ${JVX_BASE_ROOT}/software/jvx_ns.h DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software)
    #install(FILES ${JVX_BASE_ROOT}/software/jvx_config.h DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software)
    configure_file(${JVX_BASE_ROOT}/misc/version/jvx_install_version.txt.in ${CMAKE_CURRENT_BINARY_DIR}/jvx_install_version.txt)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/jvx_install_version.txt DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/../)

	if(NOT JVX_RELEASE_SDK_SUPPRESS_HEADER_INSERTION)
		add_subdirectory(${JVX_CMAKE_DIR}/post_install/header_insert)
	endif(NOT JVX_RELEASE_SDK_SUPPRESS_HEADER_INSERTION)
	
	install(DIRECTORY ${JVX_SUBPRODUCT_ROOT}/cmake/ DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/cmake)
	install(DIRECTORY ${JVX_SUBPRODUCT_ROOT}/software/codeFragments DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software)
	install(DIRECTORY ${JVX_SUBPRODUCT_ROOT}/software/extensions DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software)
  
  endif(NOT JVX_RELEASE_SDK)

  add_subdirectory(${JVX_CMAKE_DIR}/post_install/clear_files)

  # endif(${JVX_OS} MATCHES "Emscripten")
  
endif(JVX_COMPILE_BUILDTOOLS)
