set(PROJECT_NAME cbmp)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cbmp)
	message("cbmp sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
	message("cbmt installation complete.")
endif()

# message(FATAL_ERROR "Hier")
#if(JVX_RELEASE_SDK)
#	install(TARGETS ${JVX_TARGET_NAME} ARCHIVE DESTINATION ${INSTALL_PATH_LIB_STATIC} RUNTIME DESTINATION ${INSTALL_PATH_LIB_SHARED} COMPONENT "release")
#	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/jsmn/jsmn.h DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/third_party/git/jsmn/jsmn)
#endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/cbmp
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/cbmp/cbmp.c
  ${CMAKE_CURRENT_SOURCE_DIR}/cbmp/cbmp.h
  )

set(BUILD_STATIC TRUE)
set(IS_C_LIB TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(INSTALL_DEDICATED_GENERATED_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
