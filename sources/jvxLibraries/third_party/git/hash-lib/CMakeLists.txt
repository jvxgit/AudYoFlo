set(JVX_TARGET_NAME "hash")

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/jsmn)
	message("hash-lib sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "./prepareModules.sh; exit")
	message("hash-lib installation complete.")
endif()

message("--> Configuring ${JVX_TARGET_NAME}")
project(${JVX_TARGET_NAME})

set(LOCAL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/hash-library/crc32.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/hash-library/digest.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/hash-library/keccak.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/hash-library/md5.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/hash-library/sha1.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/hash-library/sha3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/hash-library/sha256.cpp
	)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/hash-library)
  
add_library(${JVX_TARGET_NAME} STATIC ${LOCAL_SOURCES})
  
###
# Some project folder groupings
###
set_property(TARGET ${JVX_TARGET_NAME} PROPERTY FOLDER "${JVX_FOLDER_HIERARCHIE}/libraries-3rdparty")

if(JVX_RELEASE_SDK)
	get_filename_component(suffix ${CMAKE_CURRENT_SOURCE_DIR}  NAME )
	message("SUFFIX: ${suffix}")
	install(TARGETS ${JVX_TARGET_NAME} ARCHIVE DESTINATION ${INSTALL_PATH_LIB_STATIC} RUNTIME DESTINATION ${INSTALL_PATH_LIB_SHARED} COMPONENT "release")
	# install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/jsmn.h DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/third_party/${suffix})
endif()
