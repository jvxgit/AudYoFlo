set(JVX_TARGET_NAME "sc594")
message("--> Configuring ${JVX_TARGET_NAME}")
project(${JVX_TARGET_NAME})

set(JVX_PLATFORM_PACKAGE_LIB_NAME jvx-dsp-base)
set(JVX_PLATFORM_NAME "sc594")

set(JVX_LOCAL_BUILD_COMMAND "cleanBuild")
# set(JVX_LOCAL_BUILD_COMMAND "build")

set(JVX_CMAKE_CCSE_BASE_PATH ${JVX_BASE_ROOT})
set(JVX_CMAKE_CCSE_SDK_PATH )
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/macros.ini.in 
	#${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/macros.ini)

set(JVX_OPTION_MAKEFILE_BUILD FALSE)
set(JVX_OPTION_BUILD_DEBUG TRUE)

# During cmake run phase, create workspace if not available yet
message("Checking folder <${CMAKE_CURRENT_BINARY_DIR}/ws>.")
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/ws)
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ws)

	set(CMAKE_CURRENT_SOURCE_DIR_PROJECT "${CMAKE_CURRENT_SOURCE_DIR}/jvx-dsp-base")	
	cmake_path(NATIVE_PATH CMAKE_CURRENT_SOURCE_DIR_PROJECT CMAKE_CURRENT_SOURCE_DIR_NATIVE)
	# message(FATAL_ERROR "-> ${CMAKE_CURRENT_SOURCE_DIR_NATIVE}")
	 
	# Import the target project <jvx-dsp-base>
	execute_process(
		COMMAND ${CCES_ECLIPSE} -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild 
		-data "${CMAKE_CURRENT_BINARY_DIR}/ws" -import ${CMAKE_CURRENT_SOURCE_DIR_NATIVE}
		COMMAND_ECHO STDOUT)
		
	set(CMAKE_CURRENT_SOURCE_DIR_PROJECT "${CMAKE_CURRENT_SOURCE_DIR}/jvx-component-templates-min")	
	cmake_path(NATIVE_PATH CMAKE_CURRENT_SOURCE_DIR_PROJECT CMAKE_CURRENT_SOURCE_DIR_NATIVE)
	# message(FATAL_ERROR "-> ${CMAKE_CURRENT_SOURCE_DIR_NATIVE}")
	 
	# Import the target project <jvx-component-templates-min>
	execute_process(
		COMMAND ${CCES_ECLIPSE} -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild 
		-data "${CMAKE_CURRENT_BINARY_DIR}/ws" -import ${CMAKE_CURRENT_SOURCE_DIR_NATIVE}
		COMMAND_ECHO STDOUT)		
	
	set(CMAKE_CURRENT_SOURCE_DIR_PROJECT "${CMAKE_CURRENT_SOURCE_DIR}/jvx-system-min")	
	cmake_path(NATIVE_PATH CMAKE_CURRENT_SOURCE_DIR_PROJECT CMAKE_CURRENT_SOURCE_DIR_NATIVE)
	# message(FATAL_ERROR "-> ${CMAKE_CURRENT_SOURCE_DIR_NATIVE}")
	 
	# Import the target project <jvx-component-templates-min>
	execute_process(
		COMMAND ${CCES_ECLIPSE} -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild 
		-data "${CMAKE_CURRENT_BINARY_DIR}/ws" -import ${CMAKE_CURRENT_SOURCE_DIR_NATIVE}
		COMMAND_ECHO STDOUT)		

	 # message(FATAL_ERROR "Creating cces workspace complete.")
endif()

add_custom_target(${JVX_TARGET_NAME}-jvx-dsp-base
	ALL 
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND 
	echo "Running sc 594 build step" &&
	${CCES_ECLIPSE} -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild 
	-data "${CMAKE_CURRENT_BINARY_DIR}/ws" -${JVX_LOCAL_BUILD_COMMAND} jvx-dsp-base/Debug && 
	${CCES_ECLIPSE} -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild 
	-data "${CMAKE_CURRENT_BINARY_DIR}/ws" -${JVX_LOCAL_BUILD_COMMAND} jvx-dsp-base/Release
	JOB_POOL console)

add_custom_target(${JVX_TARGET_NAME}-jvx-system-min
	ALL 
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS ${JVX_TARGET_NAME}-jvx-dsp-base
	COMMAND 
	echo "Running sc 594 build step" &&
	${CCES_ECLIPSE} -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild 
	-data "${CMAKE_CURRENT_BINARY_DIR}/ws" -${JVX_LOCAL_BUILD_COMMAND} jvx-system-min/Debug && 
	${CCES_ECLIPSE} -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild 
	-data "${CMAKE_CURRENT_BINARY_DIR}/ws" -${JVX_LOCAL_BUILD_COMMAND} jvx-system-min/Release
	JOB_POOL console)

add_custom_target(${JVX_TARGET_NAME}-jvx-component-templates-min
	ALL 
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS ${JVX_TARGET_NAME}-jvx-system-min
	COMMAND 
	echo "Running sc 594 build step" &&
	${CCES_ECLIPSE} -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild 
	-data "${CMAKE_CURRENT_BINARY_DIR}/ws" -${JVX_LOCAL_BUILD_COMMAND} jvx-component-templates-min/Debug && 
	${CCES_ECLIPSE} -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild 
	-data "${CMAKE_CURRENT_BINARY_DIR}/ws" -${JVX_LOCAL_BUILD_COMMAND} jvx-component-templates-min/Release
	JOB_POOL console)
	
	# message(FATAL_ERROR "${CCES_ECLIPSE} -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild 
	# -data "${CMAKE_CURRENT_BINARY_DIR}/ws" -${JVX_LOCAL_BUILD_COMMAND} jvx-dsp-base/Debug && 
	# ${CCES_ECLIPSE} -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild 
	# -data "${CMAKE_CURRENT_BINARY_DIR}/ws" -${JVX_LOCAL_BUILD_COMMAND} jvx-dsp-base/Release &&
	# ${CCES_ECLIPSE} -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild 
	# -data "${CMAKE_CURRENT_BINARY_DIR}/ws" -${JVX_LOCAL_BUILD_COMMAND} jvx-component-templates-min/Debug && 
	# ${CCES_ECLIPSE} -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild 
	# -data "${CMAKE_CURRENT_BINARY_DIR}/ws" -${JVX_LOCAL_BUILD_COMMAND} jvx-component-templates-min/Release")
	
# Install both libraries
if(JVX_RELEASE_SDK)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/jvx-component-templates-min/Debug/jvx-component-templates-min.dlb DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Debug)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/jvx-component-templates-min/Release/jvx-component-templates-min.dlb DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Release)

	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/jvx-dsp-base/Debug/jvx-dsp-base.dlb DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Debug)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/jvx-dsp-base/Release/jvx-dsp-base.dlb DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Release)
	
	# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../compatibility/ DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_INCLUDE_STATIC}/${JVX_PLATFORM_NAME} FILES_MATCHING PATTERN "*.h")
	# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_INCLUDE_STATIC}/${JVX_PLATFORM_NAME} FILES_MATCHING PATTERN "*.h")

endif()

# message(FATAL_ERROR "Wait here!")
