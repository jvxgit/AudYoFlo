message("Running code to install third party libs at very early time.")

# Only if we compile the build tools
if(JVX_BUILD_BISON)
	if(CMAKE_HOST_WIN32)
	# if(${JVX_OS} MATCHES "windows")
		set(JVX_BASE_3RDPARTY_LIBS_EARLY_EXTERNAL ${JVX_BASE_3RDPARTY_LIBS_EARLY_EXTERNAL}
			${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/flexbison)
	endif()
endif()
