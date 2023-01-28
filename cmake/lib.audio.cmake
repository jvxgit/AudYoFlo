if(JVX_INSTALL_MATLAB_MINIMAL_FRAMEWORK)
	if(NOT JVX_RELEASE_SDK)

		message("    > Installing Matlab minimal framework")
		
		# Copy the project specific start and stop hooks
		if(JVX_USE_PART_MATLAB)
		
			install(DIRECTORY ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/matlab/m-files/host-hooks/ DESTINATION ${INSTALL_PATH_MATLAB}/m-files/subprojects-audionode/+${PROJECT_NAME} COMPONENT ${INSTALL_COMPONENT})
			
			if(DEFINED JVX_COPY_DEDICATED_MATLAB_FOLDER)
				install(DIRECTORY JVX_COPY_DEDICATED_MATLAB_FOLDER/ DESTINATION ${INSTALL_PATH_MATLAB}/m-files/subprojects-audionode/+${PROJECT_NAME} COMPONENT ${INSTALL_COMPONENT})
			endif()

			# Copy the default processing callbacks which link to the local project Matlab working directory
			install(DIRECTORY ${JVX_SUBPRODUCT_ROOT}/software/codeFragments/matlab/m-files/local-calls/ DESTINATION ${INSTALL_PATH_MATLAB}/m-files/subprojects-audionode/+${PROJECT_NAME} COMPONENT ${INSTALL_COMPONENT})

			# Install an init callback for the Matlab host which branches into the local directory
			set(JVX_LOC_MAT_PATH_PROJECT ${CMAKE_CURRENT_SOURCE_DIR}/matlab-local)
			configure_file(${CMAKE_CURRENT_SOURCE_DIR}/matlab-in/jvx_init_callback.m.in ${CMAKE_CURRENT_BINARY_DIR}/jvx_init_callback.m)
			install(FILES ${CMAKE_CURRENT_BINARY_DIR}/jvx_init_callback.m DESTINATION ${INSTALL_PATH_MATLAB}/m-files/subprojects-audionode/+${PROJECT_NAME} COMPONENT ${INSTALL_COMPONENT})
		endif()
	endif()
endif()
