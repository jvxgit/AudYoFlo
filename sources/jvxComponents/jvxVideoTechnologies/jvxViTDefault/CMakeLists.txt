set(PROJECT_NAME jvxViTDefault)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include/jvxVideoTechnologies/CjvxVideoDevice.h
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include/jvxVideoTechnologies/CjvxVideoTechnology.h
  )

set(LOCAL_COMPILE_DEFINITIONS "USE_STANDALONE_SKELETON")

set(LOCAL_LIBS 
  jvx-component-templates-base_static
  )
  
set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
