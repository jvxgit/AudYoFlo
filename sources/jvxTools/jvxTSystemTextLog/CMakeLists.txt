set(PROJECT_NAME jvxTSystemTextLog)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLthreads/include
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxTextLog.cpp  
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxTextLog.h
  )

if(${JVX_OS} MATCHES "windows")
  set(LOCAL_SOURCES ${LOCAL_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/StackWalker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/StackWalker.h
    )
endif()

set(IS_JVX_COMPONENT TRUE)

set(LOCAL_LIBS
	jvxLthreads_static
)
include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
