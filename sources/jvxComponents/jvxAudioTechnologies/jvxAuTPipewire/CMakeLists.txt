set(PROJECT_NAME jvxAuTPipewire)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PIPEWIRE REQUIRED libpipewire-0.3)

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_technology.pcg
  )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base-cpp/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  ${PIPEWIRE_INCLUDE_DIRS}
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp  
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioPWireTechnology_api.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioPWireDevice.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioPWireDevice_api.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioPWireDevice_input.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioPWireDevice_output.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioPWireDevice_duplex.cpp

	# JVX_CALLINGCONVENTION CjvxMixedMasterTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	# CjvxTemplateTechnologyVD<CjvxAudioPWireDevice>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	 #{
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioPWireTechnology.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioPWireTechnology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioPWireDevice.h

  )

set(LOCAL_LIBS
	jvx-component-templates-base_static
  ${PIPEWIRE_LIBRARIES}
  )

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
