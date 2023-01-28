# common functions for all subprojects that create a library
# PROJECT_NAME must be set on entry
include(${JVX_CMAKE_DIR}/common_local_head.cmake)
set(JVX_TARGET_NAME_STATIC ${PROJECT_NAME}_static)
set(JVX_TARGET_NAME_IMPORT ${PROJECT_NAME}_import)
