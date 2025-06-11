set(PROJECT_NAME ayfATTypical)
set(JVX_AUTOMATION_VERBOSE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/templates
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-automate/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-cpp/include  
  )

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CayfATTypical.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CayfATTypical.cpp
  )

if(JVX_AUTOMATION_VERBOSE)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_AUTOMATION_VERBOSE")
endif()

set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};${AUDIO_TECHNOLOGY_COMPILE_DEFINITIONS}")

set(LOCAL_LIBS
  jvx-component-templates-base_static
  jvx-component-templates-product_static
  jvx-automate_static
  )

 set(LOCAL_PCG_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_automation.pcg
	)

set(IS_JVX_COMPONENT TRUE)
set(JVX_INSTALL_MATLAB_MINIMAL_FRAMEWORK FALSE)
set(JVX_COMPONENT_ONLY_STATIC TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
