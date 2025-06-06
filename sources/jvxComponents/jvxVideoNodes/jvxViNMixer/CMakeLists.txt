set(PROJECT_NAME jvxViNMixer)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(JVX_IMAGE_FROM_TEST_BMP FALSE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxViNMixer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxViNMixer.h
  )

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/exports_node.pcg
  ${CMAKE_CURRENT_SOURCE_DIR}/src/exports_node_selected.pcg
  )
  
set(LOCAL_PCG_FILE_OPTIONS 
	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg
	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg/jvxVisualDisplay	
	-D JVX_DECLARE_PROP_REF_INSTALL_CALLBACK
	)

set(LOCAL_LIBS 
  jvx-component-templates-base_static
  
  )
  
if(JVX_USE_CBMP)
	if(JVX_IMAGE_FROM_TEST_BMP)
		include_directories(${JVX_BASE_LIBS_LIB_PATH}/third_party/git/cbmp/cbmp)
		set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_OPEN_BMP_FOR_TEST")	
		set(LOCAL_LIBS ${LOCAL_LIBS} cbmp_static)
	endif()
endif()

set(IS_JVX_COMPONENT TRUE)
set(JVX_FORCE_INSTALL_PROJECT_SDK TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
