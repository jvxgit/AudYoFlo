set(PROJECT_NAME myNewRtapAudioNode)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_SUBPRODUCT_MYNEWRTAPPROJECT_LOCAL_LIBS_INCLUDE_DIR}
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_INCLUDE_PATH}/jvx-component-templates-base/generated
  )

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg
  )

  # Add local sources (header files only for project workspace generation!) 
set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNmyNewProject.h
	${JVX_BASE_ROOT}/software/compatibility/CjvxDataProcessor.h
	${JVX_BASE_ROOT}/software/compatibility/IjvxDataProcessor.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNmyNewProject.cpp
	${LOCAL_PCG_FILES}
	)

if(JVX_MATLAB_PROPERTY_GENERATOR) 
	jvx_genMatProperties(${JVX_TARGET_NAME} "JVX_COMPONENT_AUDIO_NODE" "node" "${LOCAL_PCG_FILES}" "${LOCAL_PCG_FILE_OPTIONS}")
endif()

set(LOCAL_LIBS 
	jvx-dsp_static
	jvx-component-templates-base_static
	myNewRtapLibrary_static
	)
	
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_EXTERNAL_CALL_ENABLED")
if(LOCAL_COMPILE_DEFINITIONS MATCHES "JVX_EXTERNAL_CALL_ENABLED")
  include_directories(${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLexternalCall/include)
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

# Group pcg files in subfolders of VS project/solution
source_group("pcg" FILES "${LOCAL_PCG_FILES}")
source_group("cmake" FILES "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")

set(IS_JVX_COMPONENT TRUE)
set(JVX_INSTALL_MATLAB_MINIMAL_FRAMEWORK TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)


