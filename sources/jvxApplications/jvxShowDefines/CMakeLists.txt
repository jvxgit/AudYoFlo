# Some common include files
set(PROJECT_NAME jvxShowDefines)

include(${JVX_CMAKE_DIR}/common_local_exe_head.cmake)

# Source code files
set(LOCAL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/target/main.cpp
	)


# Some common include files
force_console_app()
include(${JVX_CMAKE_DIR}/common_local_exe_foot.cmake)
