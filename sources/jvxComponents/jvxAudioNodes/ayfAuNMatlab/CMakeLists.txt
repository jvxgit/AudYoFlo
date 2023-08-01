if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME ayfAuNMatlab)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_SUBPRODUCT_HEADREST_LOCAL_LIBS_INCLUDE_DIR}/
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated

  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base-cpp/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-cpp/include

  )

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg
  )

#set(LOCAL_PCG_FILE_OPTIONS
#	-I ${JVX_SUBPRODUCT_HEADREST_LIBS_INCLUDE_PATH}/xyz/pcg)

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAuNMatlab.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAuNMatlab_process.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAuNMatlab_config.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAuNMatlab.h
  ${LOCAL_PCG_FILES}
  )


if(JVX_MATLAB_PROPERTY_GENERATOR) 
	jvx_genMatProperties(${JVX_TARGET_NAME} "JVX_COMPONENT_AUDIO_NODE" "node" "${LOCAL_PCG_FILES}")
endif()

set(LOCAL_LIBS 
	jvx-dsp_static
	jvx-component-templates-base_static
	)	

set(LOCAL_COMPILE_DEFINITIONS_COPY "${LOCAL_COMPILE_DEFINITIONS}")
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS_COPY};JVX_PROJECT_NAMESPACE=AYFHEADREST")
set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

if(JVX_USE_PART_MATLAB)
	set(LOCAL_START_SCRIPT_MATLAB ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${JVX_OS}/start_mat)
	set(JVX_INSTALL_MATLAB_MINIMAL_FRAMEWORK TRUE)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS_COPY};JVX_PROJECT_NAMESPACE=AYFMATLABM")
	set(LOCAL_SOURCES ${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAuNMatlab_matlab.cpp)
	JVX_ACTIVATE_VERSION_MATLAB(${PROJECT_NAME} JVX_EXTERNAL_CALL_ENABLED)
endif()
