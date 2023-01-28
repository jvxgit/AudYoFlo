set(JVX_SUBPRODUCT_MCAS_LIBS_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_MCAS_LIBS_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_MCAS_LIBS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
# set(JVX_SUBPRODUCT_MCAS_LOCAL_LIBS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-mcas/include")

set(JVX_MCAS_AUDIONODE
  ${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNMcas
  )
jvx_addSubdirs("Configuring shared library for mcas audio node" "${JVX_MCAS_AUDIONODE}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-mcas")

if(NOT JVX_RELEASE_SDK)
	if(JVX_USE_QT)
		set(JVX_MCAS_APPLICATION
			${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxApplications/jvxmcas
			)
		jvx_addSubdirs("Configuring application mcas " "${JVX_MCAS_APPLICATION}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-mcas")
	endif()
endif()
