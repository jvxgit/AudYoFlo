set(JVX_SUBPRODUCT_SIGNALS_LIBS_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_SIGNALS_LIBS_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_SIGNALS_LIBS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(JVX_SUBPRODUCT_SIGNALS_LOCAL_LIBS_INCLUDE_DIR "${JVX_SUBPRODUCT_SIGNALS_LIBS_INCLUDE_PATH}/jvx-audiotools/include")
set(JVX_SUBPRODUCT_SIGNALS_LOCAL_QT_LIBS_INCLUDE_DIR "${JVX_SUBPRODUCT_SIGNALS_LIBS_INCLUDE_PATH}/jvx-qt-audiotools/include")

set(JVX_SIGNALS_STATIC_LIBS
  ${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-audiotools-cpp
  )

if(JVX_USE_QT)
	set(JVX_SIGNALS_STATIC_LIBS ${JVX_SIGNALS_STATIC_LIBS}
		${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-qt-audiotools)
endif()

jvx_addSubdirs("Configuring static library for audiotools core functionality" "${JVX_SIGNALS_STATIC_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-audiotools")

if(NOT JVX_C_LIBRARIES_SDK_ONLY) 
	set(JVX_SIGNALS_AUDIONODE
		${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNMeasure
		${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNSynthesizer
		)
  
	set(JVX_SIGNALS_AUDIONODE ${JVX_SIGNALS_AUDIONODE}
		${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxComponents/jvxAudioNodes/jvxAuNAnalyze
		)
	jvx_addSubdirs("Configuring shared library for audiotools audio node" "${JVX_SIGNALS_AUDIONODE}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-audiotools")

	if(NOT JVX_RELEASE_SDK)
		if(JVX_USE_QT)
			set(JVX_SIGNALS_APPLICATION
				${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxApplications/jvxMeasureAudio
				)
		
			set(JVX_SIGNALS_APPLICATION ${JVX_SIGNALS_APPLICATION}
				${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxApplications/jvxAnalyzeAudio
				)
		endif()
	
		jvx_addSubdirs("Configuring applications audiotools " "${JVX_SIGNALS_APPLICATION}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-audiotools")
	endif()
endif()

