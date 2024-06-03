# The Flutter tooling requires that developers have a version of Visual Studio
# installed that includes CMake 3.14 or later. You should not increase this
# version, as doing so will cause the plugin to fail to compile for some
# customers of the plugin.
cmake_minimum_required(VERSION 3.14)

# Project-level configuration.
set(PROJECT_NAME "ayfcorepack")
project(${PROJECT_NAME} LANGUAGES CXX)

set(AYF_PROJECT ayfbinrender-native-config)
set(AYF_VERBOSE_INFO TRUE)
set(AYF_CORE_PACK_LIB ${AYF_PROJECT}_import)


# This value is used when generating builds using this plugin, so it must
# not be changed
set(PLUGIN_NAME "ayfcorepack_plugin")

# =======================================================
# Link to ayf sdk
# =======================================================
if(AYF_VERBOSE_INFO)
	message("Module <${PROJECT_NAME}>:")
endif()

if(NOT DEFINED ENV{AYF_SDK_PATH})
	message(FATAL_ERROR "Environment variable AYF_SDK_PATH not set.")
endif()
set(AYF_SDK_PATH $ENV{AYF_SDK_PATH})

cmake_path(CONVERT ${AYF_SDK_PATH} TO_CMAKE_PATH_LIST AYF_SDK_PATH)
if(NOT EXISTS ${AYF_SDK_PATH})
	message(FATAL_ERROR "SDK folder ${AYF_SDK_PATH} does not exist!!")
else()
	if(AYF_VERBOSE_INFO)
		message("----> Linking with SDK folder ${AYF_SDK_PATH}!!")
	endif()
endif()


# Any new source files that you add to the plugin should be added here.
list(APPEND PLUGIN_SOURCES
  "${AYF_SDK_PATH}/flutter/project_templates/windows/ayfcorepack/ayfcorepack_plugin.cpp"
  "${AYF_SDK_PATH}/flutter/project_templates/windows/ayfcorepack/ayfcorepack_plugin.h"
  "${AYF_SDK_PATH}/flutter/project_templates/common/ayf-sdk-local-headers.h"
  "${AYF_SDK_PATH}/software/codeFragments/jvxApplications/native-config/exports/windows/exports-flutter-native-config.def"
  "${AYF_SDK_PATH}/include/${AYF_PROJECT}/include/${AYF_PROJECT}.h"

)

configure_file(${AYF_SDK_PATH}/flutter/project_templates/windows/ayfcorepack/include/ayfcorepack/ayfcorepack_plugin_c_api.h.in "${CMAKE_CURRENT_SOURCE_DIR}/include/ayfcorepack/ayfcorepack_plugin_c_api.h" @ONLY)

# Define the plugin library target. Its name must not be changed (see comment
# on PLUGIN_NAME above).
add_library(${PLUGIN_NAME} SHARED
	"${CMAKE_CURRENT_SOURCE_DIR}/include/ayfcorepack/ayfcorepack_plugin_c_api.h"
  	"${AYF_SDK_PATH}/flutter/project_templates/windows/ayfcorepack/ayfcorepack_plugin_c_api.cpp"
  	${PLUGIN_SOURCES}
)

# Apply a standard set of build settings that are configured in the
# application-level CMakeLists.txt. This can be removed for plugins that want
# full control over build settings.
apply_standard_settings(${PLUGIN_NAME})

# Symbols are hidden by default to reduce the chance of accidental conflicts
# between plugins. This should not be removed; any symbols that should be
# exported should be explicitly exported with the FLUTTER_PLUGIN_EXPORT macro.
set_target_properties(${PLUGIN_NAME} PROPERTIES
  CXX_VISIBILITY_PRESET hidden)
# target_compile_definitions(${PLUGIN_NAME} PRIVATE FLUTTER_PLUGIN_IMPL)
target_compile_definitions(${PLUGIN_NAME} PRIVATE "FLUTTER_PLUGIN_IMPL;AYF_PROJECT=${AYF_PROJECT}")

# Source include directories and library dependencies. Add any plugin-specific
# dependencies here.
target_include_directories(${PLUGIN_NAME} INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")

# target_link_libraries(${PLUGIN_NAME} PRIVATE flutter flutter_wrapper_plugin)

include(${AYF_SDK_PATH}/cmake/cmake-sdk/cmake-ayf-minimal-header-includes.cmake)
get_property(AYF_MINIMAL_SDK_HEADER_INCLUDES GLOBAL PROPERTY AYF_MINIMAL_SDK_HEADER_INCLUDES)
if(AYF_VERBOSE_INFO)
	message("----> Found the following headers for linking libraries in the SDK: AYF_MINIMAL_SDK_HEADER_INCLUDES = ${AYF_MINIMAL_SDK_HEADER_INCLUDES}")
endif()

target_include_directories(${PLUGIN_NAME} PRIVATE
	${AYF_MINIMAL_SDK_HEADER_INCLUDES}
	"${AYF_SDK_PATH}/flutter/project_templates/common"
	"${AYF_SDK_PATH}/flutter/project_templates/windows/ayfcorepack"
	"${AYF_SDK_PATH}/include/${AYF_PROJECT}/include"
)

target_link_directories(${PLUGIN_NAME} PRIVATE 
	"${AYF_SDK_PATH}/lib"
	"${AYF_SDK_PATH}/lib/jvxComponents-static"
)
	
target_link_libraries(${PLUGIN_NAME} PRIVATE 
	flutter flutter_wrapper_plugin

	# This is the minimum package libs aggregation: 
	${AYF_CORE_PACK_LIB}
		
	jvx-system-base_static
	# These packages would be necessary otherwise
	# jvx-helpers_static.lib
	# jvx-system-min_static.lib
	# jvx-system-base_static.lib
	# jvx-component-templates-min_static.lib
	# jvx-callprot_static.lib
	# jvx-dsp-base_static.lib
	# jvx-helpers_static.lib
	# jvxLthreads_static.lib
	# jvx-component-templates-min_static.lib
	# jvx-json_static.lib
	# jvx-helpers-product_static.lib
	# ayfAuNBinRenderMixer_static.lib
	# jvxSpNAsyncIo_sm_static.lib
	# jvxAuTFileReader_static.lib
	# jvxAuTFileWriter_static.lib
	# jvxAuCPcm_static.lib
	# jvxAuNForwardBuffer_static.lib
	# jvxAuNBitstreamDecoder_static.lib
	# jvxAuNBitstreamEncoder_static.lib
	# jvxAuNChannelRearrange_static.lib
	# jvxAuNBinauralRender_static.lib
	# ayfATBinRender_static.lib
	# jvxAppNWeb_static.lib
	# jvxTWebServer_static.lib
	# jvxAuTPortAudio_static.lib
	# jvxAuTAsio_static.lib
	# jvxAuTWindows_static.lib
	# ayf-hrtf-dispenser_static.lib
	# jvxLFileIO_static.lib
	# jvx-audio-stack_static.lib
	# jvx-dsp-cpp_static.lib
	# jvxTMswText2Speech_static.lib
	# jvx-component-templates-product_static.lib
	# jvx-audiocodec-helpers_static.lib
	# jvx-dsp_static.lib
	# ayf-node-connections_static.lib
	# jvx-micro-connection_static.lib
	# jvx-automate_static.lib
	# jvxLsockets_st_static.lib
	# portaudio.lib
	# jvx-component-templates-base_static.lib
	# jvx-dsp-base-cpp_static.lib
	## mysofa_static.lib
	## zlib.lib
	## wpcap.lib
	## Packet.lib
	## libfftw3-3.lib
	## libfftw3f-3.lib

	# avrt.lib
)

set(AYF_COPY_DLLS 
	"${AYF_SDK_PATH}/bin/${AYF_CORE_PACK_LIB}.dll"
	"${AYF_SDK_PATH}/bin/zlib.dll"
	"${AYF_SDK_PATH}/bin/libfftw3-3.dll"
	)
if(AYF_VERBOSE_INFO)
	message("----> Copying dlls ${AYF_COPY_DLLS}")
endif()

# List of absolute paths to libraries that should be bundled with the plugin.
# This list could contain prebuilt libraries, or libraries created by an
# external build triggered from this build file.
set(ayfcorepack_bundled_libraries
	${AYF_COPY_DLLS}
	PARENT_SCOPE
	)
