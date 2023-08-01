set(JVX_TARGET_NAME "jvx-dsp-base-arm_cortex_a8")
message("--> Configuring ${JVX_TARGET_NAME}")
project(${JVX_TARGET_NAME})

set(JVX_PLATFORM_PACKAGE_LIB_NAME jvx-dsp-base)
set(JVX_PLATFORM_NAME "arm_cortex_a8")

# During cmake run phase, create workspace if not available yet
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/ws)
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ws)

	# Import the target project
	execute_process(
		COMMAND ${CCS_ECLIPSE}
		-noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws 
		-application com.ti.ccstudio.apps.projectImport  
		-ccs.location ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME})
endif()

# At build time, run the ccs command script to build release as well as release version of lib
add_custom_target(${JVX_TARGET_NAME} 
	ALL 
	COMMAND ${CCS_ECLIPSE}
	-noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws -application com.ti.ccstudio.apps.projectBuild -ccs.projects 
	${JVX_PLATFORM_PACKAGE_LIB_NAME} -ccs.configuration Debug && 
	${CCS_ECLIPSE} -noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws 
	-application com.ti.ccstudio.apps.projectBuild 
	-ccs.projects ${JVX_PLATFORM_PACKAGE_LIB_NAME} -ccs.configuration Release)
	
# Install both libraries
if(JVX_RELEASE_SDK)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/Debug/${JVX_PLATFORM_PACKAGE_LIB_NAME}.a DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Debug)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/Release/${JVX_PLATFORM_PACKAGE_LIB_NAME}.a DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Release)
	
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../compatibility/ DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_INCLUDE_STATIC}/${JVX_PLATFORM_NAME} FILES_MATCHING PATTERN "*.h")
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_INCLUDE_STATIC}/${JVX_PLATFORM_NAME} FILES_MATCHING PATTERN "*.h")

endif()
