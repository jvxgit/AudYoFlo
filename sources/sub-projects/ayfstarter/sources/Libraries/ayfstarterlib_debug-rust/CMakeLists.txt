set(PROJECT_NAME ayfstarterlibdbgrs)

include(${JVX_CMAKE_DIR}/common_local_lib_head_rust.cmake)

set(LOCAL_CARGO_FEATURES "jvx-allow-profiler-data-macros")

include(${JVX_CMAKE_DIR}/common_local_lib_foot_rust.cmake)
