if(NOT JVX_USES_SDK)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/paths.local.cmake)
	include(${JVX_BASE_CMAKE_DIR}/paths.cmake)
	include(${JVX_SUBPRODUCT_CMAKE_DIR}/paths.${JVX_PRODUCT}.cmake)
else()
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/paths.sdk.local.cmake)
	include(${JVX_BASE_ROOT}/cmake/cmake-sdk/paths.sdk.cmake)
	include(${JVX_SUBPRODUCT_ROOT}/cmake/cmake-sdk/paths.sdk.${JVX_PRODUCT}.cmake)
endif()

set(AYF_STARTER_AUDIONODE
  ${CMAKE_CURRENT_SOURCE_DIR}/sources/Components/AudioNodes/ayfAuNStarter
  )
jvx_addSubdirs("Configuring shared library for ayfstarter audio node" "${AYF_STARTER_AUDIONODE}" "${JVX_FOLDER_HIERARCHIE_BASE}/AudioNode")

if(NOT JVX_RELEASE_SDK)
	set(AYF_STARTER_APPLICATION "")
	if(JVX_USE_PART_WEBSERVER)
	
		set(AYF_STARTER_APPLICATION ${AYF_STARTER_APPLICATION}
			${CMAKE_CURRENT_SOURCE_DIR}/sources/Applications/ayfstarter-web)
		
	endif()
	
	if(JVX_USE_QT)
		
		set(AYF_STARTER_APPLICATION ${AYF_STARTER_APPLICATION} 
			${CMAKE_CURRENT_SOURCE_DIR}/sources/Applications/ayfstarter-qt)
				
	endif()	
	jvx_addSubdirs("Configuring application ayfstarter " "${AYF_STARTER_APPLICATION}" "${JVX_FOLDER_HIERARCHIE_BASE}/Application")
endif()