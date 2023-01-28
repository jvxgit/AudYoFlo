set(JVX_SUBPRODUCT_MIXMATRIX_LIBS_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_MIXMATRIX_LIBS_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_MIXMATRIX_LIBS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

set(JVX_MIXMATRIX_STATIC_LIBS
  ${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-mixmatrix
  ${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-mixmatrix-cpp
  )

if(JVX_USE_QT)
	set(JVX_MIXMATRIX_STATIC_LIBS ${JVX_MIXMATRIX_STATIC_LIBS}
		${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-qt-mixmatrix)
endif()

jvx_addSubdirs("Configuring static library for mixmatrix core functionality" "${JVX_MIXMATRIX_STATIC_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-mixmatrix")

set(JVX_MIXMATRIX_AUDIONODE
  ${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNMixMatrix
  )
jvx_addSubdirs("Configuring shared library for mixmatrix audio node" "${JVX_MIXMATRIX_AUDIONODE}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-mixmatrix")

if(NOT JVX_RELEASE_SDK)
	if(JVX_USE_QT)
		set(JVX_MIXMATRIX_APPLICATION
			${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxApplications/jvxMixMatrix
			)
		jvx_addSubdirs("Configuring application jvxMixMatrix " "${JVX_MIXMATRIX_APPLICATION}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-mixmatrix")
	endif()
endif()