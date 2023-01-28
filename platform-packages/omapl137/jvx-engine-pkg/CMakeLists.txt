cmake_minimum_required (VERSION 2.8)

set(JVX_TARGET_NAME "jvx-engine-omapl137")
message("--> Configuring ${JVX_TARGET_NAME}")
project(${JVX_TARGET_NAME})

set(JVX_PLATFORM_PACKAGE_LIB_NAME jvx-engine)
set(JVX_PLATFORM_NAME "omapl137")

set(JVX_CMAKE_CCS_BASE_PATH ${JVX_BASE_ROOT})
set(JVX_CMAKE_CCS_SDK_PATH )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/macros.ini.in 
	${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/macros.ini)

# During cmake run phase, create workspace if not availableyet
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/ws)
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ws)

	# Import the target project
	execute_process(
		COMMAND ${CCS_ECLIPSE}  
		-noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws 
		-application com.ti.ccstudio.apps.projectImport  
		-ccs.location ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME})
else()
	message("Folder ${CMAKE_CURRENT_BINARY_DIR}/ws already exists, skipping receation to safe user modifications")
endif()

	# At build time, run the ccs command script to build release as well as release version of lib
	add_custom_target(${JVX_TARGET_NAME} 
		ALL 
		COMMAND 
		${CCS_ECLIPSE} -noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws 
		-application com.ti.ccstudio.apps.projectBuild -ccs.projects 
		${JVX_PLATFORM_PACKAGE_LIB_NAME} -ccs.configuration Debug-dev &&
		${CCS_ECLIPSE} -noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws 
		-application com.ti.ccstudio.apps.projectBuild 
		-ccs.projects ${JVX_PLATFORM_PACKAGE_LIB_NAME} -ccs.configuration Release-dev && 
		${CCS_ECLIPSE} -noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws 
		-application com.ti.ccstudio.apps.projectBuild 
		-ccs.projects ${JVX_PLATFORM_PACKAGE_LIB_NAME} -ccs.configuration Release-rt &&
		${CCS_ECLIPSE} -noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws 
		-application com.ti.ccstudio.apps.projectBuild -ccs.projects 
		${JVX_PLATFORM_PACKAGE_LIB_NAME} -ccs.configuration Debug-c8-dev && 
		${CCS_ECLIPSE} -noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws 
		-application com.ti.ccstudio.apps.projectBuild 
		-ccs.projects ${JVX_PLATFORM_PACKAGE_LIB_NAME} -ccs.configuration Release-c8-dev &&
		${CCS_ECLIPSE} -noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws 
		-application com.ti.ccstudio.apps.projectBuild 
		-ccs.projects ${JVX_PLATFORM_PACKAGE_LIB_NAME} -ccs.configuration Release-c8-rt)

	
# Install both libraries
if(JVX_RELEASE_SDK)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/Debug-dev/${JVX_PLATFORM_PACKAGE_LIB_NAME}.lib DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Debug-dev)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/Release-dev/${JVX_PLATFORM_PACKAGE_LIB_NAME}.lib DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Release-dev)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/Release-rt/${JVX_PLATFORM_PACKAGE_LIB_NAME}.lib DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Release-rt)
	
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/Debug-c8-dev/${JVX_PLATFORM_PACKAGE_LIB_NAME}.lib DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Debug-c8-dev)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/Release-c8-dev/${JVX_PLATFORM_PACKAGE_LIB_NAME}.lib DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Release-c8-dev)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/Release-c8-rt/${JVX_PLATFORM_PACKAGE_LIB_NAME}.lib DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Release-c8-rt)

	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_INCLUDE_STATIC}/${JVX_PLATFORM_NAME} FILES_MATCHING PATTERN "*.h")
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/user/ DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_INCLUDE_STATIC}/${JVX_PLATFORM_NAME} FILES_MATCHING PATTERN "*.h")
endif()

