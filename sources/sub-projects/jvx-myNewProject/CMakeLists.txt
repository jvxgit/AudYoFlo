set(JVX_SUBPRODUCT_myNewProject_LIBS_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_myNewProject_LIBS_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_myNewProject_LIBS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

set(JVX_DEPLOY_LIBRARY_DSP_myNewProject_MATLAB TRUE)

###
# Installation of Matlab libraries
###
if(NOT JVX_RELEASE_SDK)
	if(JVX_DEPLOY_LIBRARY_DSP_myNewProject_MATLAB)
		set(JVX_myNewProject_MATLAB_LIBS "" )
		if(JVX_USE_PART_MATLAB)
			set(JVX_myNewProject_MATLAB_LIBS ${JVX_myNewProject_MATLAB_LIBS}
				${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-myNewProject-matlab/cmake/matlab
				)
		endif(JVX_USE_PART_MATLAB)
		if(JVX_USE_PART_OCTAVE)
			set(JVX_myNewProject_MATLAB_LIBS ${JVX_myNewProject_MATLAB_LIBS}
				${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-myNewProject-matlab/cmake/octave
				)
		endif(JVX_USE_PART_OCTAVE)
		jvx_addSubdirs("Installation for all Matlab/Octave library files" "${JVX_myNewProject_MATLAB_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-myNewProject")		
	endif(JVX_DEPLOY_LIBRARY_DSP_myNewProject_MATLAB)
endif()

set(JVX_myNewProject_STATIC_LIBS
  ${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-myNewProject
  )
jvx_addSubdirs("Configuring static library for myNewProject core functionality" "${JVX_myNewProject_STATIC_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-myNewProject")

set(JVX_myNewProject_AUDIONODE
  ${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNmyNewProject
  )
jvx_addSubdirs("Configuring shared library for myNewProject audio node" "${JVX_myNewProject_AUDIONODE}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-myNewProject")

if(NOT JVX_RELEASE_SDK)
	if(JVX_USE_QT)
		set(JVX_myNewProject_APPLICATION
			${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxApplications/jvxmyNewProject
			)
		jvx_addSubdirs("Configuring application myNewProject " "${JVX_myNewProject_APPLICATION}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-myNewProject")
	endif()
endif()