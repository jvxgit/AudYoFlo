## ======================================================================
## The video libs
if(JVX_USE_MFVIDEO)
	include_directories(${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxVideoTechnologies/jvxViTMfWindows)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_MFVIDEO")
	set(LOCAL_LIBS ${LOCAL_LIBS} jvxViTMfWindows_static)
endif()
if(JVX_USE_V4L2VIDEO)
	include_directories(${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxVideoTechnologies/jvxViTV4L2)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_V4L2VIDEO")
	set(LOCAL_LIBS ${LOCAL_LIBS} jvxViTV4L2_static)
endif()
