cmake_minimum_required (VERSION 2.8.11)

set(PROJECT_NAME ayf_connect-ctrl)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_SOURCES
  
	${CMAKE_CURRENT_SOURCE_DIR}/include/CayfConnectCtrl.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CayfConnectCtrl.cpp

	${JVX_BASE_LIBS_INCLUDE_PATH}/ayf-component-common/include/ayf-component-connect.h
	${JVX_BASE_LIBS_INCLUDE_PATH}/ayf-component-common/include/ayf-component-entries-tpl.h

  )

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_EXEC} ${JVX_PLATFORM_FLAGS}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${JVX_SUBPRODUCT_CTC_LIBS_BINARY_DIR}/sources/Components/AudioNodes/ayfAuNCtc/

	${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-component-templates-base/include
	${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/ayf-shared-connection-host/include
	${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/ayf-component-lib/include
	${JVX_SUBPRODUCT_BINARY_INCLUDE_PATH}/ayf-component-lib/generated
	
	${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/ayf-embedding-proxy/include
	${JVX_BASE_LIBS_INCLUDE_PATH}/ayf-component-common/include	
  )

#if(JVX_OS MATCHES "windows")#
#	if(NOT JVX_USE_CONSOLE_APPS)
#		set(LOCAL_COMPILE_DEFINITIONS "")
#	endif()
#endif()

set(LOCAL_LIBS
	ayfAuNCtc_static
	ayf-component-lib_static
	
	# We open this lib as a dll - without any compile time linkage!
	# ayf-embedding-proxy_import

	#jvxTconfigProcessor_static
    #jvxTSystemTextLog_static
	#jvxTDataLogger_static
	#jvxTThreads_static
	#
	#jvx-app-host-static_static
	)

include (${JVX_INTEGRATE_SOURCES_PATH}/cmake/specific.cmake)
set(LOCAL_LIBS_ALL ${LOCAL_LIBS})

# set(LOCAL_PCG_FILE_OPTIONS
#		-P
#		-L
#		-D JVX_WITH_AUDIO_PLUGIN_IF
#		-I ${JVX_SUBPRODUCT_mbc_LIBS_INCLUDE_PATH}/jvx-mbc/pcg)
#set(LOCAL_PCG_FILES
#  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_lib.pcg
#  )

if(JVX_USES_SDK)
	set(LOCAL_LIBS ${LOCAL_LIBS} 
		libayfctc_static 
		ayf-shared-connection-host_import)
endif(JVX_USES_SDK)

set(BUILD_STATIC TRUE)

set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};AYF_PROJECT_POSTFIX=Ctrl")

# Excluding the import lib variant
# set(BUILD_SHARED TRUE)
# set(JVX_FORCE_RUNTIME_IMPORTLIB TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

