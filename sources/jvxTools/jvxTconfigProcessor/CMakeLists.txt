set(PROJECT_NAME jvxTconfigProcessor)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

FLEX_TARGET(rtpL_configParser ${CMAKE_CURRENT_SOURCE_DIR}/flex/grammar.l ${CMAKE_CURRENT_BINARY_DIR}/grammar.c)
BISON_TARGET(${JVX_TARGET_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/yacc/grammar.y ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.c COMPILE_FLAGS "-ydv")

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxConfigProcessor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxConfigProcessor.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSectionOriginList.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSectionOriginList.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/treeList.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/treeList.h
  ${CMAKE_CURRENT_SOURCE_DIR}/flex/grammar.l
  ${CMAKE_CURRENT_SOURCE_DIR}/yacc/grammar.y
  ${CMAKE_CURRENT_BINARY_DIR}/grammar.c # generated
  ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.c # generated
  )
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/grammar.c GENERATED)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/grammar.c PROPERTIES LANGUAGE CXX)
if(JVX_OS MATCHES "windows")
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/grammar.c PROPERTIES COMPILE_DEFINITIONS YY_NO_UNISTD_H)
endif()
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.c GENERATED)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.c PROPERTIES LANGUAGE CXX)

# Added preprocessor macro to set the string length for almost everything in parser
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_CONFIG_EXPRESSION_MAX_LENGTH=256")

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
