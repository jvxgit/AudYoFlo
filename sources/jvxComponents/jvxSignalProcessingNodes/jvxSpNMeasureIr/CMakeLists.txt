if(JVX_USE_FFTW)
	set(PROJECT_NAME jvxSpNMeasureIr)
	include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

	include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/target
		${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
		${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated

		${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-signals/include
		${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp-cpp/include
		)

	set(LOCAL_SOURCES 
		${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMeasureIr.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMeasureIr_props.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMeasureIr_hooks.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMeasureIr_config.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMeasureIr.h

		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMeasureIr_oneMeasurement.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNMeasureIr_oneMeasurement.cpp
	
		${CMAKE_CURRENT_SOURCE_DIR}/include/jvxSpNMeasureIr_oneMeasurementChannel.h
		)

	set(LOCAL_PCG_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg
		)

	set(LOCAL_LIBS 
		jvx-component-templates-base_static
		jvx-dsp-cpp_static
		jvx-signals_static
		)

	set(IS_JVX_COMPONENT TRUE)
	set(JVX_PROPERTY_ACCESS_HEADER_COMPONENT ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxSpNMeasureIr_oneMeasurementChannel.h)

	include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
endif()

