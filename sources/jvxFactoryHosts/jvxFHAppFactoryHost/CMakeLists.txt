if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxFHAppFactoryHost)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates/include

	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
	${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAppFactoryHost.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAppFactoryHost_config.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxAppFactoryHost.h
  )

set(LOCAL_COMPILE_DEFINITIONS_SHARED "COMPONENT_DESCRIPTOR_NAME=CJVXHOST-DLL")
set(LOCAL_COMPILE_DEFINITIONS_STATIC "COMPONENT_DESCRIPTOR_NAME=CJVXHOST-LIB")

set(LOCAL_LIBS jvx-component-templates-base_static)

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

# once again for no-dll versions (with _nd suffix)
set(JVX_TARGET_NAME ${JVX_TARGET_NAME}_nd)
set(JVX_TARGET_NAME_STATIC ${JVX_TARGET_NAME_STATIC}_nd)
set(JVX_TARGET_NAME_IMPORT ${JVX_TARGET_NAME_IMPORT}_nd)
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_HOST_USE_ONLY_STATIC_OBJECTS")
set(LOCAL_COMPILE_DEFINITIONS_SHARED "COMPONENT_DESCRIPTOR_NAME=CJVXHOST-DLL-NODLL")
set(LOCAL_COMPILE_DEFINITIONS_STATIC "COMPONENT_DESCRIPTOR_NAME=CJVXHOST-LIB-NODLL")
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

