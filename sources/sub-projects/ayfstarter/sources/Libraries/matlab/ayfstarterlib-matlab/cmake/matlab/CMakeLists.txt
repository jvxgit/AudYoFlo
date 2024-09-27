
if(JVX_USE_PART_MATLAB)
	message("--> Installing ayfstarterlib Matlab DSP libraries")
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../+ayfstarterlib DESTINATION ${INSTALL_PATH_MATLAB}/m-files COMPONENT "release" PATTERN "*.m")
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../+examples DESTINATION ${INSTALL_PATH_MATLAB}/m-files COMPONENT "release" PATTERN "*.m")
endif()

if(JVX_USE_PART_OCTAVE)
	message("--> Installing ayfstarterlib Octave DSP libraries") 
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../+jvx_myNewProject	DESTINATION ${INSTALL_PATH_OCTAVE}/m-files COMPONENT "release" PATTERN "*.m")
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../+examples DESTINATION ${INSTALL_PATH_OCTAVE}/m-files COMPONENT "release" PATTERN "*.m")
endif()
