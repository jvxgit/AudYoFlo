set(PROJECT_NAME jvxLexternalCall)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/include
  )

set(LOCAL_SOURCES
	 ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxMexCalls.h
	 ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxMexCallsTpl.h

	 ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxMexCalls.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxMexCalls_prv.cpp
  )

set(LOCAL_PCG_FILES
   ${CMAKE_CURRENT_SOURCE_DIR}/src/exports_mexcall.pcg
  )

set(LOCAL_MCG_FILES
   ${CMAKE_CURRENT_SOURCE_DIR}/src/exports_matlab.mcg
  )

set(LOCAL_SOURCES
	 ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxMexCalls.cpp
	 ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxMexCalls_prv.cpp
	 ${LOCAL_MCG_FILES}
	 ${LOCAL_PCG_FILES}
  )

set(LOCAL_COMPILE_DEFINITIONS JVX_EXTERNAL_CALL_ENABLED)
set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
