# The Flutter tooling requires that developers have a version of Visual Studio
# installed that includes CMake 3.14 or later. You should not increase this
# version, as doing so will cause the plugin to fail to compile for some
# customers of the plugin.
cmake_minimum_required(VERSION 3.14)

# Project-level configuration.
set(PROJECT_NAME "pixel_buffer_texture")
set(AYF_VERBOSE_INFO TRUE)
project(${PROJECT_NAME} LANGUAGES CXX)

# =======================================================
# Link to ayf sdk
# =======================================================
if(AYF_VERBOSE_INFO)
	message("Module <${PROJECT_NAME}>: in folder ${CMAKE_CURRENT_SOURCE_DIR}")
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


# This value is used when generating builds using this plugin, so it must
# not be changed
set(PLUGIN_NAME "pixel_buffer_texture_plugin")

# Any new source files that you add to the plugin should be added here.
list(APPEND PLUGIN_SOURCES
  "my_pixel_buffer_texture.cpp"
  "pixel_buffer_texture_plugin.cpp"
  "pixel_buffer_texture_plugin.h"
  "${AYF_SDK_PATH}/flutter/project_templates/common/ayf-sdk-local-headers.h"
)

# Define the plugin library target. Its name must not be changed (see comment
# on PLUGIN_NAME above).
add_library(${PLUGIN_NAME} SHARED
  "include/pixel_buffer_texture/pixel_buffer_texture_plugin_c_api.h"
  "pixel_buffer_texture_plugin_c_api.cpp"
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

include(${AYF_SDK_PATH}/cmake/cmake-sdk/cmake-ayf-minimal-header-includes.cmake)

get_property(AYF_MINIMAL_SDK_HEADER_INCLUDES GLOBAL PROPERTY AYF_MINIMAL_SDK_HEADER_INCLUDES)
if(AYF_VERBOSE_INFO)
	message("----> Found the following headers for linking libraries in the SDK: AYF_MINIMAL_SDK_HEADER_INCLUDES = ${AYF_MINIMAL_SDK_HEADER_INCLUDES}")
endif()

target_include_directories(${PLUGIN_NAME} PRIVATE
	${AYF_MINIMAL_SDK_HEADER_INCLUDES}
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_directories(${PLUGIN_NAME} PRIVATE 
	"${AYF_SDK_PATH}/lib"
	"${AYF_SDK_PATH}/lib/jvxComponents-static"
)
target_link_libraries(${PLUGIN_NAME} PRIVATE 
	flutter flutter_wrapper_plugin
	jvx-system-base_static)

# List of absolute paths to libraries that should be bundled with the plugin.
# This list could contain prebuilt libraries, or libraries created by an
# external build triggered from this build file.
set(pixel_buffer_texture_bundled_libraries
  ""
  PARENT_SCOPE
)
