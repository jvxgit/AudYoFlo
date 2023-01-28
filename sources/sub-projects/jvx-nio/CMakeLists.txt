set(JVX_SUBPRODUCT_NTASK_LIBS_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_NTASK_LIBS_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_NTASK_LIBS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

set(JVX_DEPLOY_LIBRARY_DSP_NTASK_MATLAB TRUE)
set(JVX_COMPILE_NTASK_WITH_LOGFILEWRITER FALSE)

if(NOT JVX_DEPLOY_LIBRARY_DSP_MATLAB)
	set(JVX_DEPLOY_LIBRARY_DSP_NTASK_MATLAB FALSE)
endif()

if(NOT JVX_USES_SDK)
		#include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/paths.local.cmake)
		include(${JVX_BASE_CMAKE_DIR}/paths.cmake)
		include(${JVX_SUBPRODUCT_CMAKE_DIR}/paths.${JVX_PRODUCT}.cmake)
	else()
		#include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/paths.sdk.local.cmake)
		include(${JVX_BASE_ROOT}/cmake/cmake-sdk/paths.sdk.cmake)
		include(${JVX_SUBPRODUCT_ROOT}/cmake/cmake-sdk/paths.sdk.${JVX_PRODUCT}.cmake)
	endif()
	
if(NOT JVX_RELEASE_SDK)
	#if(JVX_DEPLOY_LIBRARY_DSP_NTASK_MATLAB)
	#	set(JVX_NTASK_MATLAB_LIBS "" )
	#	if(JVX_USE_PART_MATLAB)
	#		set(JVX_NTASK_MATLAB_LIBS ${JVX_NTASK_MATLAB_LIBS}
	#			${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-NTASK-matlab/cmake/matlab
	#		)
	#	endif(JVX_USE_PART_MATLAB)
	#	if(JVX_USE_PART_OCTAVE)
	#		set(JVX_NTASK_MATLAB_LIBS ${JVX_NTASK_MATLAB_LIBS}
	#			${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-NTASK-matlab/cmake/octave
	#		)
	#	endif(JVX_USE_PART_OCTAVE)
	#	jvx_addSubdirs("Installation for all Matlab/Octave library files" "${JVX_NTASK_MATLAB_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-Ntask")		
	#endif(JVX_DEPLOY_LIBRARY_DSP_NTASK_MATLAB)
endif()

# if(NOT JVX_USE_BOOST)
#	message(FATAL_ERROR "Activated sub-project <jvx-Ntask> but boost not activated.")
# endif()

# if(NOT JVX_USE_EIGEN)
# 	message(FATAL_ERROR "Activated sub-project <jvx-Ntask> but Eigen not activated.")
# endif()

set(JVX_NTASK_STATIC_LIBS
	${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-ntask
	#${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-Ntask-cpp
	)

if(JVX_USE_QT)
	#set(JVX_NTASK_STATIC_LIBS ${JVX_NTASK_STATIC_LIBS}
	#	${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-qt-Ntask)
endif()

jvx_addSubdirs("Configuring static library for NTASK core functionality" "${JVX_NTASK_STATIC_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-Ntask")

set(JVX_NTASK_AUDIONODE
	${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNNtask
	)
jvx_addSubdirs("Configuring shared library for NTASK audio node" "${JVX_NTASK_AUDIONODE}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-Ntask")

set(JVX_NTASK_SPNODE
	${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxComponents/jvxSignalProcessingNodes/jvxSpNVtask
	)
jvx_addSubdirs("Configuring shared library for NTASK signal processing node" "${JVX_NTASK_SPNODE}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-Ntask")

if(NOT JVX_RELEASE_SDK)
	if(JVX_USE_QT)
		set(JVX_NTASK_APPLICATION
			${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxApplications/jvxNtask
			)
		jvx_addSubdirs("Configuring application jvxNtask " "${JVX_NTASK_APPLICATION}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-Ntask")
	endif()
endif()

