set(PROJECT_NAME jvxCuTGps)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

# add a target to generate API documentation with Doxygen
# FIXME: support in common foot for this?
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
  add_custom_target(${PROJECT_NAME}-doc ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
    COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen" VERBATIM
    )
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc DESTINATION ${INSTALL_PATH_DOC}/${PROJECT_NAME})
endif(DOXYGEN_FOUND)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLTextDeviceControl/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvxLTextDeviceControl/generated
  )

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxCuTGpsTechnology.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxCuTGpsDevice.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxCuTGpsDevice_nmea.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxCuTGpsTechnology.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxCuTGpsDevice.h

	${CMAKE_CURRENT_SOURCE_DIR}/install/common/instructions_gps_nmea.jif
  )

  set(LOCAL_PCG_FILE_OPTIONS
	-I ${CMAKE_CURRENT_SOURCE_DIR}/codeGen
	)
	
set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
  )
	  
set(IS_JVX_COMPONENT TRUE)

set(LOCAL_LIBS jvxLTextDeviceControl_static jvx-component-templates-base_static)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)



