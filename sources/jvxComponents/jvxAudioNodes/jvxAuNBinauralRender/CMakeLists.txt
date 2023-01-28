cmake_minimum_required (VERSION 2.8.11)
if(JVX_USE_FFTW)

	set(PROJECT_NAME jvxAuNBinauralRender)
	include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_CURRENT_SOURCE_DIR}/target
		${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
		${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated

		${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp-cpp/include
		${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-helpers-cpp/include
		)

	set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNBinauralRender.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNBinauralRender_props.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNBinauralRender_config.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNBinauralRender.h
		)

	set(LOCAL_PCG_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg
		)

	set(LOCAL_LIBS 
		jvx-component-templates-base_static
		jvx-dsp-cpp_static
		## jvx-helpers-cpp is a header only library
		)

	# Add the text to speech engine
	if(${JVX_OS} MATCHES "windows")
		set(LOCAL_LIBS ${LOCAL_LIBS}
			jvxTMswText2Speech_static)
	endif()

	set(IS_JVX_COMPONENT TRUE)

	include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
endif()
