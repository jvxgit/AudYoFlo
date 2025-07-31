set(JVX_TARGET_NAME "jvx-dsp-base-sc594")
message("--> Configuring ${JVX_TARGET_NAME}")
project(${JVX_TARGET_NAME})

set(JVX_PLATFORM_PACKAGE_LIB_NAME jvx-dsp-base)
set(JVX_PLATFORM_NAME "sc594")

set(JVX_CMAKE_CCSE_BASE_PATH ${JVX_BASE_ROOT})
set(JVX_CMAKE_CCSE_SDK_PATH )
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/macros.ini.in 
	#${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/macros.ini)


# During cmake run phase, create workspace if not available yet
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../ws)
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../ws)

	cmake_path(NATIVE_PATH CMAKE_CURRENT_SOURCE_DIR CMAKE_CURRENT_SOURCE_DIR_NATIVE)
	# message(FATAL_ERROR "-> ${CMAKE_CURRENT_SOURCE_DIR_NATIVE}")
	 
	# Import the target project
	execute_process(
		COMMAND ${CCES_ECLIPSE} -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild 
		-data "${CMAKE_CURRENT_BINARY_DIR}/../ws" -import ${CMAKE_CURRENT_SOURCE_DIR_NATIVE}
		COMMAND_ECHO STDOUT)
		
	execute_process(
		COMMAND ${CCES_ECLIPSE} -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild 
		-data "${CMAKE_CURRENT_SOURCE_DIR}/../ws" -cleanBuild jvx-dsp-base/Debug
		COMMAND_ECHO STDOUT)
endif()

# At build time, run the ccs command script to build release as well as release version of lib
add_custom_target(${JVX_TARGET_NAME} 
	ALL 
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND 	
	${CCES_MAKE} -C Debug clean && ${CCES_MAKE} -C Debug all)
	## 
	
	#${CCES_ECLIPSE} -noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws -application com.ti.ccstudio.apps.projectBuild -ccs.projects 
	#${JVX_PLATFORM_PACKAGE_LIB_NAME} -ccs.configuration Debug && 
	#${CCES_ECLIPSE} -noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws -application com.ti.ccstudio.apps.projectBuild -ccs.projects 
	#${JVX_PLATFORM_PACKAGE_LIB_NAME} -ccs.configuration Release &&
	#${CCES_ECLIPSE} -noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws -application com.ti.ccstudio.apps.projectBuild -ccs.projects 
	#${JVX_PLATFORM_PACKAGE_LIB_NAME} -ccs.configuration Debug-c8 && 
	#${CCES_ECLIPSE} -noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws -application com.ti.ccstudio.apps.projectBuild -ccs.projects 
	#${JVX_PLATFORM_PACKAGE_LIB_NAME} -ccs.configuration Release-c8)
	
# Install both libraries
if(JVX_RELEASE_SDK)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/Debug/${JVX_PLATFORM_PACKAGE_LIB_NAME}.lib DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Debug)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/Release/${JVX_PLATFORM_PACKAGE_LIB_NAME}.lib DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Release)
	
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/Debug-c8/${JVX_PLATFORM_PACKAGE_LIB_NAME}.lib DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Debug-c8)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/Release-c8/${JVX_PLATFORM_PACKAGE_LIB_NAME}.lib DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Release-c8)
	
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../compatibility/ DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_INCLUDE_STATIC}/${JVX_PLATFORM_NAME} FILES_MATCHING PATTERN "*.h")
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_INCLUDE_STATIC}/${JVX_PLATFORM_NAME} FILES_MATCHING PATTERN "*.h")

endif()

## message(FATAL_ERROR "Wait here!")
