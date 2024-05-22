
set(PROJECT_NAME jvxSpNSynchronize)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
   ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated

  # Audio stack
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-audio-stack/include

  )

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNSynchronize.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNSynchronize_sec.h
	# ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMixChainEnterLeave_common.h

	${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNSynchronize.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNSynchronize_sec.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNSynchronize_sec_input.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNSynchronize_sec_output.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNSynchronize_sec_process.cpp


	#${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMixChainEnterLeave_input.cpp
	#${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMixChainEnterLeave_output.cpp

	#${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMixChainEnterLeave_config.cpp
   )

set(LOCAL_LIBS 
  jvx-component-templates-base_static
  jvx-audio-stack_static
  )

  
set(LOCAL_PCG_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg
	)

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
