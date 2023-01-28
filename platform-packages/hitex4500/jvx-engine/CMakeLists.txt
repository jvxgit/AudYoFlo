cmake_minimum_required (VERSION 2.8)

set(JVX_TARGET_NAME "jvx-engine-arm_cortex_m4")
message("--> Configuring ${JVX_TARGET_NAME}")
project(${JVX_TARGET_NAME})

set(JVX_PLATFORM_PACKAGE_LIB_NAME jvx-engine)
set(JVX_PLATFORM_NAME "arm_cortex_m4")

if(DEFINED ENV{JVX_LPCOPEN_SDK_ROOT})
	message("LPC Open SDK Root folder specified to be located at <$ENV{JVX_LPCOPEN_SDK_ROOT}>")
else()
	message(FATAL_ERROR "LPC Open SDK Root folder not specified in environment vraiable <JVX_LPCOPEN_SDK_ROOT}>")
endif()

# During cmake run phase, create workspace if not available yet
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/ws)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ws)

  set(JVX_MXU_IMPORT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}")

  if(JVX_OS MATCHES "windows")

    # This must be done - somehow - to work properly in Windows
    STRING(REGEX REPLACE "/" "\\\\" JVX_MXU_IMPORT_PATH ${JVX_MXU_IMPORT_PATH})
  endif()

  message("execute_process(
		COMMAND ${CMAKE_COMMAND} -E env JVX_BASE_ROOT=${JVX_BASE_ROOT} ${MCUXPRESSO}
		-application org.eclipse.cdt.managedbuilder.core.headlessbuild
		-noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws
		--launcher.suppressErrors
		-import ${JVX_MXU_IMPORT_PATH}")

  # Import the target project
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E env JVX_BASE_ROOT=${JVX_BASE_ROOT} ${MCUXPRESSO}
    -application org.eclipse.cdt.managedbuilder.core.headlessbuild
    -noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws
    --launcher.suppressErrors
    -import ${JVX_MXU_IMPORT_PATH}
    )
endif()

# At build time, run the ccs command script to build release as well as release version of lib
add_custom_target(${JVX_TARGET_NAME}
  ALL
  COMMAND ${CMAKE_COMMAND} -E env JVX_BASE_ROOT=${JVX_BASE_ROOT} ${MCUXPRESSO}
  -application org.eclipse.cdt.managedbuilder.core.headlessbuild
  -noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws
  --launcher.suppressErrors
  -build ${JVX_PLATFORM_PACKAGE_LIB_NAME}/Debug &&
  ${CMAKE_COMMAND} -E env JVX_BASE_ROOT=${JVX_BASE_ROOT} ${MCUXPRESSO}
  -application org.eclipse.cdt.managedbuilder.core.headlessbuild
  -noSplash -data ${CMAKE_CURRENT_BINARY_DIR}/ws
  --launcher.suppressErrors
  -build ${JVX_PLATFORM_PACKAGE_LIB_NAME}/Release
  )

# Install both libraries
if(JVX_RELEASE_SDK)
  #install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/Debug/lib${JVX_PLATFORM_PACKAGE_LIB_NAME}.a DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Debug)
  #install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/project/${JVX_PLATFORM_PACKAGE_LIB_NAME}/Release/lib${JVX_PLATFORM_PACKAGE_LIB_NAME}.a DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_LIB_STATIC}/${JVX_PLATFORM_NAME}/Release)

  #install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../compatibility/ DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_INCLUDE_STATIC}/${JVX_PLATFORM_NAME} FILES_MATCHING PATTERN "*.h")
  #install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${INSTALL_PATH_PLATFORM_PACKAGE_INCLUDE_STATIC}/${JVX_PLATFORM_NAME} FILES_MATCHING PATTERN "*.h")

endif()

