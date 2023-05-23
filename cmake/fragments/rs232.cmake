if (JVX_USE_PART_RS232)
    if(${JVX_OS} MATCHES "windows")
		include_directories(${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTRs232Win32)
        set(LOCAL_LIBS ${LOCAL_LIBS} jvxTRs232Win32_static)
    endif()
    if(${JVX_OS} MATCHES "linux")
		include_directories(${JVX_BASE_BINARY_DIR}/sources/jvxTools/jvxTRs232Glnx)
        set(LOCAL_LIBS ${LOCAL_LIBS} jvxTRs232Glnx_static)
    endif()
endif()


