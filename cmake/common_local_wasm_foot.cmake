# build unstable targets only with Unstable build types
if((NOT JVX_DEPLOY_UNSTABLE) AND IS_UNSTABLE)
  message("Excluding unstable target ${JVX_TARGET_NAME} from build.")
  return()
endif()

###
# reset some stuff
###
#set(ADDITIONAL_CXX_FLAGS "")
set(ADDITIONAL_LIBS "")
set(ADDITIONAL_SOURCES "")
set(ADDITIONAL_COMPILE_DEFINITIONS "")
#set(ADDITIONAL_LINKER_FLAGS "")
#set(GENERATED_FILES "")
if(NOT INSTALL_COMPONENT)
	if(IS_UNSTABLE)
		set(INSTALL_COMPONENT "unstable")
	else()
		set(INSTALL_COMPONENT "release")
	endif()
endif()

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/public")
add_executable(${JVX_TARGET_NAME} ${LOCAL_SOURCES} ${ADDITIONAL_SOURCES})
target_link_libraries(${JVX_TARGET_NAME} ${LOCAL_LIBS} ${ADDITIONAL_LIBS} ${JVX_SYSTEM_LIBRARIES} )
target_compile_definitions(${JVX_TARGET_NAME} PRIVATE ${LOCAL_COMPILE_DEFINITIONS} ${ADDITIONAL_COMPILE_DEFINITIONS} ${GLOBAL_COMPILE_DEFINITIONS})

# What these flags all mean:
# - "-s WASM=1" activate web assembly. If WASM=0 js code is generated. That should have identical function but is less efficient
# - "-s BINARYEN_ASYNC_COMPILATION=0 -s SINGLE_FILE=1" should be used in combination with audioWorklets, https://stackoverflow.com/questions/61498612/emscripten-c-functions-not-ready-when-called-asynchronously-in-node-js
#                                                      It seems that an asynchronous load of the c code breqaks function as c code is not ready at first frame
# - "--post-js ../src/em-es6-module.js" additional something required due to https://github.com/kripken/emscripten/issues/6284
# - "-O1" is required in release mode - otherwise an error is thrown which seems to be related to option "--post-js ../src/em-es6-module.js"
# - The option implicetly invokes it with "-s ENVIRONMENT=shell"
# Only -O1 works, all other options do not run!
# set_target_properties(${EMC_TARGET_NAME} PROPERTIES LINK_FLAGS "--bind -s WASM=1 -s BINARYEN_ASYNC_COMPILATION=0 -s SINGLE_FILE=1 --post-js ../src/em-es6-module.js")
#
## The following options allow -O0 - it does not allow any debugging though
# set_target_properties(${EMC_TARGET_NAME} PROPERTIES LINK_FLAGS "--bind -O0 -g -s ASSERTIONS=0 -s WASM=1 -s BINARYEN_ASYNC_COMPILATION=0 -s SINGLE_FILE=1 --post-js ../src/em-es6-module.js")
##  target_compile_definitions(${EMC_TARGET_NAME} PRIVATE "-hauke")
if(JVX_EMSCRIPTEN_WASM_ASSEMBLY)
	set(JVX_WASM_TOKEN "-s WASM=1")
else()
	set(JVX_WASM_TOKEN "-s WASM=0")
endif()

if(JVX_EMSCRIPTEN_SINGLEFILE)
	set(JVX_SINGLEFILE_TOKEN "-s SINGLE_FILE=1")
else()
	set(JVX_SINGLEFILE_TOKEN "-s SINGLE_FILE=0")
endif()

if(JVX_EMSCRIPTEN_SYNC_LOAD)
	set(JVX_SYNC_LOAD_TOKEN "-s BINARYEN_ASYNC_COMPILATION=0")
else()
	set(JVX_SYNC_LOAD_TOKEN "-s BINARYEN_ASYNC_COMPILATION=1")
endif()


if (JVX_EMSCRIPTEN_OUTPUT_MODE_DEBUG)
	message("--> THIS IS THE DEBUG VERSION!")
	set(JVX_LOCAL_LINK_FLAGS "--bind -s ASSERTIONS=0 ${JVX_WASM_TOKEN} ${JVX_SYNC_LOAD_TOKEN} ${JVX_SINGLEFILE_TOKEN} --post-js ${JVX_ESM6_POST_JS}")
else()
	message("--> THIS IS THE RELEASE VERSION!")
	
	# -O3 is not really accepted by emscripten for audioworklets. It seems that the optimizer has a bug with O2 and O3
	#
	# SyntaxError: 'import' and 'export' may appear only with 'sourceType: module' (3504:0)
	#
	string(REPLACE "-O3" "-O1" CMAKE_CXX_FLAGS_LOCAL "${CMAKE_CXX_FLAGS_LOCAL}")
	string(REPLACE "-g" "-O1" CMAKE_CXX_FLAGS_LOCAL "${CMAKE_CXX_FLAGS_LOCAL}")
	
	message("--> Modified CXX_FLAGS: ${CMAKE_CXX_FLAGS_LOCAL}")
	set(JVX_LOCAL_COMILE_FLAGS ${CMAKE_CXX_FLAGS_LOCAL})
	set(JVX_LOCAL_LINK_FLAGS "--bind ${CMAKE_CXX_FLAGS_LOCAL} ${JVX_WASM_TOKEN} ${JVX_SYNC_LOAD_TOKEN} ${JVX_SINGLEFILE_TOKEN} --post-js ${JVX_ESM6_POST_JS}")
	
endif()
set(CMAKE_CXX_FLAGS_DEBUG ${JVX_LOCAL_COMILE_FLAGS} ${ADDITIONAL_COMPILE_DEFINITIONS})
set(CMAKE_CXX_FLAGS_RELEASE ${JVX_LOCAL_COMILE_FLAGS} ${ADDITIONAL_COMPILE_DEFINITIONS})
# set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${JVX_LOCAL_COMILE_FLAGS})
set_target_properties(${JVX_TARGET_NAME} PROPERTIES LINK_FLAGS ${JVX_LOCAL_LINK_FLAGS})

# message(FATAL_ERROR "Hier -- ${JVX_LOCAL_LINK_FLAGS}")

# Bug report for firefox audioworklets:
# https://bugzilla.mozilla.org/show_bug.cgi?id=1572644
#

if(JVX_EMSCRIPTEN_SINGLEFILE)
	install(TARGETS ${JVX_TARGET_NAME} DESTINATION ${INSTALL_PATH_BIN_WASM} COMPONENT ${INSTALL_COMPONENT})
else()
	install(FILES
        "$<TARGET_FILE_DIR:${JVX_TARGET_NAME}>/${JVX_TARGET_NAME}.js"
        "$<TARGET_FILE_DIR:${JVX_TARGET_NAME}>/${JVX_TARGET_NAME}.wasm"
        DESTINATION  ${INSTALL_PATH_BIN_WASM} COMPONENT ${INSTALL_COMPONENT})
	# install(FILES ${JVX_BINARY_DIR}/${JVX_TARGET_NAME}.wasm DESTINATION ${INSTALL_PATH_BIN_WASM} COMPONENT ${INSTALL_COMPONENT})	
endif()
install(FILES ${JVX_WASM_AUDIO_HELPERS} DESTINATION ${INSTALL_PATH_BIN_WASM} COMPONENT ${INSTALL_COMPONENT})