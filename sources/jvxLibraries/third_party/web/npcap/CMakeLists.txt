set(PROJECT_NAME npcap-sdk)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/npcap-sdk)
	message("npcap-sdk sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
	message("npcap-sdk installation complete.")
endif()

set(PCAP_LIBRARY_PATH  "${CMAKE_CURRENT_SOURCE_DIR}/npcap-sdk" CACHE INTERNAL "npcap-sdk lib path (windows)")
