set(PROJECT_NAME vst)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vst3sdk)
	message("vst3sdk sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
	message("vst3sdk installation complete.")
endif()

set(VST3_SDK_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/vst3sdk CACHE INTERNAL "vst3sdk source code path")

# Some configurations
set(SMTG_CREATE_PLUGIN_LINK FALSE)
set(SMTG_PLUGIN_TARGET_PATH "${JVX_SUBPRODUCT_BINARY_DIR}")
# set(VST_FORCE_PLUGINS_EXAMPLES TRUE)

# SDK nur einmal einbinden
add_subdirectory(${VST3_SDK_ROOT} ${CMAKE_BINARY_DIR}/vst3sdk)

#get_property(VSTSDK_VERSION_COLLECT GLOBAL PROPERTY VSTSDK_VERSION_COLLECT_PROPERTY)
#message(FATAL_ERROR "-> ${VSTSDK_VERSION_COLLECT}")

# VST cmake-Helfer explizit includen, damit sie überall bekannt sind
#include(${VST3_SDK_ROOT}/cmake/modules/SMTG_AddVST3Library.cmake)
#include(${VST3_SDK_ROOT}/cmake/modules/SMTG_AddVST3AuV2.cmake)  # optional

# message(FATAL_ERROR "Hier")
#if(JVX_RELEASE_SDK)
#	install(TARGETS ${JVX_TARGET_NAME} ARCHIVE DESTINATION ${INSTALL_PATH_LIB_STATIC} RUNTIME DESTINATION ${INSTALL_PATH_LIB_SHARED} COMPONENT "release")
#	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/jsmn/jsmn.h DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/third_party/git/jsmn/jsmn)
#endif()
