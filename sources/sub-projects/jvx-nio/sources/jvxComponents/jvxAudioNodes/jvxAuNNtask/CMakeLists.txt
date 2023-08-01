if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuNNtask)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  #${JVX_SUBPRODUCT_MIXMATRIX_LIBS_INCLUDE_PATH}/jvx-mixmatrix/include
  #${JVX_SUBPRODUCT_MIXMATRIX_LIBS_INCLUDE_PATH}/jvx-mixmatrix-cpp/include
  #${BOOST_INCLUDEDIR}
  #${EIGEN_INCLUDEDIR}
  #${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-audio-stack/include
  #${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp-cpp/include

  #${CMAKE_CURRENT_BINARY_DIR}/../../../jvxLibraries/jvx-nio/generated
  #${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/jvx-dsp/generated
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base-cpp/include
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
)

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNNtask.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNNtask.h
  )


# set(LOCAL_PCG_FILES
#  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg
#  )

#set(LOCAL_PCG_FILE_OPTIONS 
#	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg)

set(LOCAL_LIBS
  # jvx-hoa-cpp_static
  # jvx-hoa_static
  jvx-component-templates-base_static
  jvx-audio-stack_static
  jvx-dsp-cpp_static
  jvx-dsp_static
  )

set(IS_JVX_COMPONENT TRUE)

if(JVX_COMPILE_NTASK_WITH_LOGFILEWRITER)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_NTASK_ENABLE_LOGFILE")
endif()

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

set(JVX_TARGET_NAME "${PROJECT_NAME}m")
set(JVX_TARGET_NAME_STATIC "${PROJECT_NAME}m_static")
set(JVX_TARGET_NAME_IMPORT "${PROJECT_NAME}m_import")
set(JVX_COMPONENT_ONLY_SHARED FALSE) # required for sub-references

if(JVX_MATLAB_PROPERTY_GENERATOR)
	jvx_genMatProperties(${JVX_TARGET_NAME} "JVX_COMPONENT_AUDIO_NODE" "node" "${LOCAL_PCG_FILES}")
endif()

set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_EXTERNAL_CALL_ENABLED")
if(LOCAL_COMPILE_DEFINITIONS MATCHES "JVX_EXTERNAL_CALL_ENABLED")
  include_directories(
	${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLexternalCall/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-audio-stack/include)

  set(LOCAL_MCG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/export_project.mcg
    )
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvxLexternalCall_static
	)
	set(LOCAL_SOURCES ${LOCAL_SOURCES}
		${LOCAL_MCG_FILES})
		
	source_group("mcg" FILES "${LOCAL_MCG_FILES}")
endif()

if(${JVX_OS} MATCHES "windows")

	# There is a somewhat ununderstood warning in Windows
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};_SCL_SECURE_NO_WARNINGS")
endif()

set(JVX_INSTALL_MATLAB_MINIMAL_FRAMEWORK TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
