cmake_minimum_required (VERSION 2.8.11)

# common functions for all subprojects
# called by specialized versions for libraries and executables
# (common_local_lib_head.cmake and common_local_exe_head.cmake)
#
# PROJECT_NAME must be set on entry
#
message("--> Configuring ${PROJECT_NAME}")
set(JVX_TARGET_NAME ${PROJECT_NAME})
set(JVX_MODULE_NAME ${PROJECT_NAME})
project (${JVX_TARGET_NAME})
