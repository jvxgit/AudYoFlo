set(PROJECT_NAME ayfAuNStarter)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  ${SUBPRODUCT_AYFSTARTER_LIBS_INCLUDE_PATH}/ayfstarterlib/include
  )

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAuNStarter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAuNStarter.h
  )

# Add the pcg file build steps
set(LOCAL_PCG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/pcg/exports_node.pcg)
set(LOCAL_PCG_FILE_OPTIONS -I ${SUBPRODUCT_AYFSTARTER_LIBS_INCLUDE_PATH}//ayfstarterlib/pcg) 

set(LOCAL_LIBS 
	jvx-dsp_static
	ayfstarterlib_static
	jvx-component-templates-base_static
	)
	
set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)


