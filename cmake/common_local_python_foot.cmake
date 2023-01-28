# build unstable targets only with Unstable build types
if((NOT JVX_DEPLOY_UNSTABLE) AND IS_UNSTABLE)
  message("Excluding unstable target ${JVX_TARGET_NAME} from build.")
  return()
endif()

# do not start mex files with lib...
set(CMAKE_SHARED_LIBRARY_PREFIX "")

if(IS_UNSTABLE)
  set(INSTALL_COMPONENT "unstable")
else()
  set(INSTALL_COMPONENT "release")
endif()

message("    > Python Module")

  # include_directories(${OCTAVE_INCLUDE_PATH})
  # link_directories(${OCTAVE_PATH_LIB})
  ## message(FATAL_ERROR "-${JVX_TARGET_NAME}-> LOCAL_SOURCES = ${LOCAL_SOURCES}")
  # Details on the PRIVATE keyword here: 
  # https://github.com/pybind/pybind11/issues/387
  pybind11_add_module(${JVX_TARGET_NAME} ${LOCAL_SOURCES})					   
  target_compile_definitions(${JVX_TARGET_NAME} PRIVATE ${LOCAL_COMPILE_DEFINITIONS} ${GLOBAL_COMPILE_DEFINITIONS})
  target_link_libraries(${JVX_TARGET_NAME} PRIVATE ${LOCAL_LIBS} ${ADDITIONAL_LIBS} ${JVX_SYSTEM_LIBRARIES})
  jvx_displayFlags(${JVX_TARGET_NAME})

  # EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
  # define (VERSION_INFO) here.
  # target_compile_definitions(cmake_example
  #                           PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
  target_compile_definitions(${JVX_TARGET_NAME}
                           PRIVATE VERSION_INFO="MYVERSION")
						   
  install(TARGETS ${JVX_TARGET_NAME}
	COMPONENT python
	RUNTIME DESTINATION "${INSTALL_PATH_PYTHON_RUNTIME}"
	LIBRARY DESTINATION "${INSTALL_PATH_PYTHON_LIBRARY}"
	ARCHIVE DESTINATION "${INSTALL_PATH_PYTHON_ARCHIVE}")
  	
 

  #FOREACH(LOCAL_DIR_ENTRY ${LOCAL_M_FILES_SOURCE_DIR})
	#install(DIRECTORY ${LOCAL_DIR_ENTRY}/ DESTINATION ${INSTALL_PATH_OCTAVE}/m-files COMPONENT ${INSTALL_COMPONENT})
  #ENDFOREACH()

  #if(LOCAL_IMAGES_SOURCE_DIR)
   # install(DIRECTORY ${LOCAL_IMAGES_SOURCE_DIR}/ DESTINATION ${INSTALL_PATH_IMAGES} COMPONENT ${INSTALL_COMPONENT})
  #endif(LOCAL_IMAGES_SOURCE_DIR)
  if(NOT JVX_RELEASE_SDK)
	  install(FILES ${JVX_BASE_ROOT}/software/codeFragments/python/requirements.txt DESTINATION ${INSTALL_PATH_PYTHON} COMPONENT ${INSTALL_COMPONENT})  
		install(FILES ${JVX_BASE_ROOT}/software/codeFragments/python/local.py DESTINATION ${INSTALL_PATH_PYTHON_RUNTIME} COMPONENT ${INSTALL_COMPONENT}) 
  
  # Generate local python target path import
  if(JVX_LOCAL_PYTHON_CONFIGURATION_TEMPLATE)
	set(JVX_PYTHON_PATH_RELEASE_LIB "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_PYTHON_LIBRARY}") # C:\\jvxrt\\build.audio.allp\\release\\runtime\\python\\library
	set(JVX_PYTHON_PATH_RELEASE_DLL "${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_BIN}") # C:\\jvxrt\\build.audio.allp\\release\\runtime\\bin
	configure_file(${JVX_BASE_ROOT}/software/codeFragments/python/local.py.in ${JVX_LOCAL_PYTHON_CONFIGURATION_TEMPLATE})		
  endif()

		if(LOCAL_PYTHON_SCRIPT)
			install(FILES ${LOCAL_PYTHON_SCRIPT} DESTINATION ${INSTALL_PATH_PYTHON_RUNTIME} COMPONENT ${INSTALL_COMPONENT})    
		endif(LOCAL_PYTHON_SCRIPT)
  endif()

  	###
	# Some project folder groupings
	###
	set_property(TARGET ${JVX_TARGET_NAME} PROPERTY FOLDER "${JVX_FOLDER_HIERARCHIE}/python")
	
	#if(LOCAL_IMAGES_OCTAVE_SOURCE_DIR)
	#	install(DIRECTORY ${LOCAL_IMAGES_OCTAVE_SOURCE_DIR}/ DESTINATION ${INSTALL_PATH_IMAGES} COMPONENT ${INSTALL_COMPONENT} FILES_MATCHING PATTERN "*.png")
	#endif(LOCAL_IMAGES_SOURCE_DIR)
	if(NOT EXISTS JVX_COLLECT_INSTALL_DIRECTORIES)
		set(JVX_COLLECT_INSTALL_DIRECTORIES FALSE)
	endif()
	jvx_install_project_folders(${JVX_COLLECT_INSTALL_DIRECTORIES})



