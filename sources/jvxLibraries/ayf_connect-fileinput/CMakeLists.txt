cmake_minimum_required (VERSION 2.8.11)

set(PROJECT_NAME ayf_connect-fileinput)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_SOURCES
  
	${CMAKE_CURRENT_SOURCE_DIR}/include/CayfConnectFileInput.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CayfConnectFileInput.cpp

	${JVX_BASE_LIBS_INCLUDE_PATH}/ayf-component-common/include/ayf-component-connect.h
	${JVX_BASE_LIBS_INCLUDE_PATH}/ayf-component-common/include/ayf-component-entries-tpl.h
    )

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/include
	# ${JVX_BASE_COMPONENTS_BINARY_DIR}/jvxAudioNodes/ayfAuNMixer/
	
	${JVX_BASE_COMPONENTS_BINARY_DIR}/jvxSignalProcessingNodes/jvxSpNMixChainEnterLeave/  	


	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-component-templates-base/include
	${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/ayf-shared-connection-host/include
	${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/ayf-component-lib/include
	${JVX_SUBPRODUCT_BINARY_INCLUDE_PATH}/ayf-component-lib/generated
	
	${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/ayf-embedding-proxy/include
	${JVX_BASE_LIBS_INCLUDE_PATH}/ayf-component-common/include	
  )

set(LOCAL_LIBS
	# ayfAuNMixer_static
	jvxSpNMixChainEnterLeave_static
	ayf-component-lib_static
	)

include (${JVX_INTEGRATE_SOURCES_PATH}/cmake/specific.cmake)
set(LOCAL_LIBS_ALL ${LOCAL_LIBS})


if(JVX_USES_SDK)
	set(LOCAL_LIBS ${LOCAL_LIBS} 
		# libayfstarter_static 
		ayf-shared-connection-host_import)
endif(JVX_USES_SDK)

set(BUILD_STATIC TRUE)

set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};AYF_PROJECT_POSTFIX=FileInput")

# Excluding the import lib variant
# set(BUILD_SHARED TRUE)
# set(JVX_FORCE_RUNTIME_IMPORTLIB TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

