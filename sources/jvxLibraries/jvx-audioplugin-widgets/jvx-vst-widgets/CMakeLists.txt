if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-vst-widgets)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
#	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
	${JVX_VSTGUI_SDK_ROOT}

	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audioplugins-extender/jvx-plugin-connector/include
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-audioplugin-widgets/common/include
)

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/../common/include/IjvxPluginWidgets.h

	${CMAKE_CURRENT_SOURCE_DIR}/../common/include/jvxPluginWidget.h
	${CMAKE_CURRENT_SOURCE_DIR}/../common/src/jvxPluginWidget.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxVstGridWidget.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVstGridWidget.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxVstCurveGridWidget.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVstCurveGridWidget.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvxVstUpdatedCurveGridWidget.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvxVstUpdatedCurveGridWidget.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx-vst-widgets.h
	)

	
set(LOCAL_LIBS
  )

set(BUILD_STATIC TRUE)
set(IS_C_LIB FALSE)

set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
