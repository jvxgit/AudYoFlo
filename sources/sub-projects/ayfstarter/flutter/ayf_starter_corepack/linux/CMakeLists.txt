# The Flutter tooling requires that developers have CMake 3.10 or later
# installed. You should not increase this version, as doing so will cause
# the plugin to fail to compile for some customers of the plugin.
cmake_minimum_required(VERSION 3.10)

# Project-level configuration.
set(PROJECT_NAME "ayfcorepack")
project(${PROJECT_NAME} LANGUAGES CXX)

set(AYF_PROJECT ayfstarter-native-config)
set(AYF_VERBOSE_INFO TRUE)
set(AYF_CORE_PACK_LIB lib${AYF_PROJECT}_import.so)
set(AYF_CORE_PACK_INCLUDE ${AYF_PROJECT})

# This value is used when generating builds using this plugin, so it must
# not be changed.
set(PLUGIN_NAME "ayfcorepack_plugin")

# =======================================================
# Link to ayf sdk
# =======================================================
if(AYF_VERBOSE_INFO)
	message("Module <${PROJECT_NAME}> in folder ${CMAKE_CURRENT_SOURCE_DIR}")
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

list(APPEND PLUGIN_SOURCES
  "ayfcorepack_plugin.cc"
  "include/ayfcorepack/ayfcorepack_plugin.h"
  "../common/ayf-sdk-local-headers.h"
  "${AYF_SDK_PATH}/software/codeFragments/jvxApplications/native-config/exports/windows/exports-flutter-native-config.def"
  "${AYF_SDK_PATH}/include/${AYF_CORE_PACK_INCLUDE}/include/ayfstarter-native-config.h"
)

# Define the plugin library target. Its name must not be changed (see comment
# on PLUGIN_NAME above).
#
# Any new source files that you add to the plugin should be added here.
add_library(${PLUGIN_NAME} SHARED
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
target_compile_definitions(${PLUGIN_NAME} PRIVATE FLUTTER_PLUGIN_IMPL)

# Source include directories and library dependencies. Add any plugin-specific
# dependencies here.
target_include_directories(${PLUGIN_NAME} INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${PLUGIN_NAME} PRIVATE flutter)
target_link_libraries(${PLUGIN_NAME} PRIVATE PkgConfig::GTK)

include(${AYF_SDK_PATH}/cmake/cmake-sdk/cmake-ayf-minimal-header-includes.cmake)
get_property(AYF_MINIMAL_SDK_HEADER_INCLUDES GLOBAL PROPERTY AYF_MINIMAL_SDK_HEADER_INCLUDES)
if(AYF_VERBOSE_INFO)
	message("----> Found the following headers for linking libraries in the SDK: AYF_MINIMAL_SDK_HEADER_INCLUDES = ${AYF_MINIMAL_SDK_HEADER_INCLUDES}")
endif()

target_include_directories(${PLUGIN_NAME} PRIVATE
	${AYF_MINIMAL_SDK_HEADER_INCLUDES}
	${AYF_SDK_PATH}/include/${AYF_CORE_PACK_INCLUDE}/include
	../common
)

target_link_directories(${PLUGIN_NAME} PRIVATE 
	"${AYF_SDK_PATH}/lib"
	"${AYF_SDK_PATH}/bin"
	"${AYF_SDK_PATH}/lib/jvxComponents-static"
)

target_link_libraries(${PLUGIN_NAME} PRIVATE 

	# This is the minimum package libs aggregation: 
	${AYF_CORE_PACK_LIB}
	jvx-system-base_static
	)

target_compile_features(${PLUGIN_NAME} PRIVATE cxx_std_17)

target_compile_options(${PLUGIN_NAME} PRIVATE "-Wno-error")
target_compile_options(${PLUGIN_NAME} PRIVATE "-Wno-unused-variable")
target_compile_options(${PLUGIN_NAME} PRIVATE "-Wno-unused-function")

set(AYF_COPY_DLLS 
	"${AYF_SDK_PATH}/bin/${AYF_CORE_PACK_LIB}"
	)
if(AYF_VERBOSE_INFO)
	message("----> Copying dlls ${AYF_COPY_DLLS}")
endif()


# List of absolute paths to libraries that should be bundled with the plugin.
# This list could contain prebuilt libraries, or libraries created by an
# external build triggered from this build file.
set(ayfcorepack_bundled_libraries
  ""
  ${AYF_COPY_DLLS}
  PARENT_SCOPE
)
