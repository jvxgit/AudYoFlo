if(JVX_USE_DSPFILTERS)
	set(PROJECT_NAME aydigitalfilterlib)
	include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

	# add a target to generate API documentation with Doxygen
	# FIXME: support in common foot for this?
	if(DOXYGEN_FOUND AND LATEX_FOUND AND LATEX_DVIPS_FOUND)
		message("--> Running documentation generation in project <${PROJECT_NAME}>.")

		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
		add_custom_target(${PROJECT_NAME}-doc ALL
			${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
			COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen" VERBATIM
		)
	endif()

	include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/include/os-${JVX_OS}
		${CMAKE_CURRENT_BINARY_DIR}
		)

	set(LOCAL_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxDigitalFilter.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxDigitalFilter.cpp
		)

	set(LOCAL_LIBS jvx-dsp-base_static DSPFilters)

	set(LOCAL_COMPILE_DEFINITIONS "${FFT_COMPILE_DEFINITIONS}")

	set(BUILD_STATIC TRUE)
	set(IS_C_LIB TRUE)
	set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

	include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
	
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/matlab/+dsp_filter DESTINATION ${INSTALL_PATH_MATLAB}/m-files COMPONENT "release" PATTERN "*.m")
endif()

