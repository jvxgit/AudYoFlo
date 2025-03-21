set(PROJECT_NAME jvxViTMfWindows)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
  )

set(LOCAL_PCG_FILE_OPTIONS 
		-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
	${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoMfWindowsTechnology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoMfWindowsDevice.cpp

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include/jvxVideoTechnologies/CjvxVideoDevice.h
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include/jvxVideoTechnologies/CjvxVideoTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoMfWindowsTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoMfWindowsDevice.h

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoMfWindowsDevice_properties.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoMfWindowsDevice_com.cpp
  )

set(IS_JVX_COMPONENT TRUE)

set(LOCAL_LIBS 
	jvx-component-templates-base_static
	runtimeobject
	mfplat
	mf
	mfreadwrite
	mfuuid
	shlwapi
	)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
