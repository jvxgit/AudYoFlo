cmake_minimum_required (VERSION 2.8.11)
if(JVX_USE_FFTW)

	set(PROJECT_NAME jvxSpNSpeaker2Binaural)
	include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

	include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/target
		${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
		${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
	
		${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp-cpp/include

		${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-cpp/include
		)

	set(LOCAL_SOURCES 
		${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNSpeaker2Binaural.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNSpeaker2Binaural_properties.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNSpeaker2Binaural_hrtfs.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxSpNSpeaker2Binaural.h
		)

	set(LOCAL_PCG_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg
		)

	set(LOCAL_LIBS 
		jvx-component-templates-base_static
		jvx-dsp-cpp_static
		)

	set(IS_JVX_COMPONENT TRUE)
	## set(JVX_PROPERTY_ACCESS_HEADER_COMPONENT ${CMAKE_CURRENT_SOURCE_DIR}/include/jvxSpNMeasureIr_oneMeasurementChannel.h)

	include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
endif()

