set(JVX_SUBPRODUCT_OPENGL_LIBS_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_OPENGL_LIBS_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_OPENGL_LIBS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# set(JVX_DEPLOY_LIBRARY_DSP_OPENGL_MATLAB TRUE)

###
# Installation of Matlab libraries
###
#if(NOT JVX_RELEASE_SDK)
#	if(JVX_DEPLOY_LIBRARY_DSP_OPENGL_MATLAB)
#		set(JVX_OPENGL_MATLAB_LIBS "" )
#		if(JVX_USE_PART_MATLAB)
#			set(JVX_OPENGL_MATLAB_LIBS ${JVX_OPENGL_MATLAB_LIBS}
#				${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-OPENGL-matlab/cmake/matlab
#				)
#		endif(JVX_USE_PART_MATLAB)
#		if(JVX_USE_PART_OCTAVE)
#			set(JVX_OPENGL_MATLAB_LIBS ${JVX_OPENGL_MATLAB_LIBS}
#				${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-opengl-matlab/cmake/octave
#				)
#		endif(JVX_USE_PART_OCTAVE)
#		jvx_addSubdirs("Installation for all Matlab/Octave library files" "${JVX_OPENGL_MATLAB_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-opengl")		
#	endif(JVX_DEPLOY_LIBRARY_DSP_OPENGL_MATLAB)
#endif()
set(JVX_OPENGL_STATIC_LIBS
	${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-opengl
	)
jvx_addSubdirs("Configuring static library for opengl core functionality" "${JVX_OPENGL_STATIC_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-opengl")

set(JVX_OPENGL_AUDIONODE
	${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNOpenGl
	)
jvx_addSubdirs("Configuring shared library for opengl audio node" "${JVX_OPENGL_AUDIONODE}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-opengl")

if(NOT JVX_RELEASE_SDK)
	if(JVX_USE_QT)
		if(JVX_COMPILE_GOD_HOST)
			set(JVX_OPENGL_APPLICATION
				${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxApplications/jvxopengl
				)
			jvx_addSubdirs("Configuring application opengl " "${JVX_OPENGL_APPLICATION}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-opengl")
		endif()
	endif()
endif()
