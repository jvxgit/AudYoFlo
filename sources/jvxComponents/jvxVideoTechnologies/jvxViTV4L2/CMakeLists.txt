set(PROJECT_NAME jvxViTV4L2)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
  )

set(LOCAL_PCG_FILE_OPTIONS
	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg
	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg/jvxVisualDisplay
	-D JVX_USE_AS_TRIGGERED_MULTIBUFFER
	)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvxLthreads/include
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
	${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
	${OPENCV_LIBRARY_PATH}
  )

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoV4L2Technology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoV4L2Device.cpp

  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include/jvxVideoTechnologies/CjvxVideoDevice.h
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include/jvxVideoTechnologies/CjvxVideoTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoV4L2Technology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoV4L2Device.h

  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoV4L2Device_properties.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxVideoV4L2Device_read.cpp
  )

set(IS_JVX_COMPONENT TRUE)

set(LOCAL_LIBS
	jvx-component-templates-base_static
	${OPENCV_LIBRARIES}
	)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
