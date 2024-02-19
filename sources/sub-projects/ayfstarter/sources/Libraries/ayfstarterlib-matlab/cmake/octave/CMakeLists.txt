if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

message("--> Installing myNewProject Octave DSP libraries") 
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../+jvx_myNewProject	DESTINATION ${INSTALL_PATH_OCTAVE}/m-files COMPONENT "release" PATTERN "*.m")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../+examples DESTINATION ${INSTALL_PATH_OCTAVE}/m-files COMPONENT "release" PATTERN "*.m")
