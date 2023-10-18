SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

message ("--> ${SUBDIRS}")
# LABEL ADD SUB_PROJECTS HERE!
FOREACH(subdir ${SUBDIRS})
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/.jvxprj)
		message("XX> Building sub project in folder ${JVX_SUBPRODUCT_ROOT}/mex-projects/${subdir}")
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/cmake)
	else()
		message("XX> NOT Building project in folder ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
	endif()
ENDFOREACH()
