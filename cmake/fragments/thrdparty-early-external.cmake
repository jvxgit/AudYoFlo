message("Running code to install third party libs at very early time.")

if(JVX_BUILD_BISON)
	set(JVX_BASE_3RDPARTY_LIBS_EARLY_EXTERNAL ${JVX_BASE_3RDPARTY_LIBS_EARLY_EXTERNAL}
		${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/flexbison)	
endif()
