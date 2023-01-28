set(PROJECT_NAME jvxSpTSocketSignal)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-sockets/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-audio-sockets-sm/include

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-buffer-sync/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-audio-stack/include

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSignalProcessingDeviceSocket.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSignalProcessingDeviceSocket.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSignalProcessingTechnologySocket.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSignalProcessingTechnologySocket.h

	#${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxBufferSync.cpp
	#${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxBufferSync.h
  )
	
set(LOCAL_LIBS 
  jvx-component-templates-base_static
  jvx-sockets_static
  jvx-audio-sockets-sm_static  
  jvx-buffer-sync_static
  )
  
set(LOCAL_COMPILE_DEFINITIONS "USE_STANDALONE_SKELETON")

set(LOCAL_PCG_FILES
	 ${CMAKE_CURRENT_SOURCE_DIR}/pcg/CjvxSignalProcessingDeviceSocket.pcg)

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

