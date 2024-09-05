set(PROJECT_NAME jvxViNMixer)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

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
  )
  
set(LOCAL_PCG_FILE_OPTIONS 
	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg
	)

set(LOCAL_LIBS 
  jvx-component-templates-base_static
  
  )
  
if(JVX_USE_CBMP)
	include_directories(${JVX_BASE_LIBS_LIB_PATH}/third_party/git/cbmp/cbmp)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_OPEN_BMP_FOR_TEXT")	
	set(LOCAL_LIBS ${LOCAL_LIBS} cbmp_static)
endif()

set(IS_JVX_COMPONENT TRUE)
set(JVX_FORCE_INSTALL_PROJECT_SDK TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
