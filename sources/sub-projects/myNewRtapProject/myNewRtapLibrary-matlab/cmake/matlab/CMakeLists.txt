message("--> Installing myNewProject Matlab DSP libraries")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../+jvx_myNewRtapLibrary DESTINATION ${INSTALL_PATH_MATLAB}/m-files COMPONENT "release" PATTERN "*.m")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../+examples DESTINATION ${INSTALL_PATH_MATLAB}/m-files COMPONENT "release" PATTERN "*.m")
