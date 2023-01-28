set(JVX_SUBPRODUCT_EXAMPLE_LIBS_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_EXAMPLE_LIBS_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries")
set(JVX_SUBPRODUCT_EXAMPLE_LIBS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

set(JVX_INSPIRE_STATIC_LIBS
  ${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-example
#  ${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxLibraries/jvx-inspire-evaluate
  )
jvx_addSubdirs("Configuring static library for inspire functionality" "${JVX_INSPIRE_STATIC_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-example")

#set(JVX_INSPIRE_NODE
#  ${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxComponents/jvxCustomNodes/jvxCuNinspire
#  )
#jvx_addSubdirs("Configuring shared library for nr audio node" "${JVX_nr_AUDIONODE}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-nr")

set(JVX_INSPIRE_TECHNOLOGIES
  ${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxComponents/jvxCustomTechnologies/jvxCuTExample
  )
jvx_addSubdirs("Configuring shared library for inspire specific technology" "${JVX_INSPIRE_TECHNOLOGIES}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-example")

if(NOT JVX_RELEASE_SDK)
	set(JVX_INSPIRE_APPLICATION ${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxApplications/jvxWebHost)
  	if(JVX_USE_QT)
	  set(JVX_INSPIRE_APPLICATION ${JVX_INSPIRE_APPLICATION}
		${CMAKE_CURRENT_SOURCE_DIR}/sources/jvxApplications/jvxWebControl
		)
	endif()
	jvx_addSubdirs("Configuring application inspire " "${JVX_INSPIRE_APPLICATION}" "${JVX_FOLDER_HIERARCHIE_BASE}/jvx-example")
endif()
