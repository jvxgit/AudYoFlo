SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
message ("${CMAKE_CURRENT_SOURCE_DIR} --> ${SUBDIRS}")

# LABEL ADD SUB_PROJECTS HERE!
FOREACH(subdir ${SUBDIRS})
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${SUBFOLDER_TOKEN}/${subdir}/.jvxprj)
		message("XX> Building sub project in folder ${JVX_SUBPRODUCT_ROOT}")
		jvx_addSubdirs("Project <${subdir}> in subfolder others" "${CMAKE_CURRENT_SOURCE_DIR}/${SUBFOLDER_TOKEN}/${subdir}" "${JVX_FOLDER_HIERARCHIE}/${subdir}")
		# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${subdir})
	else()
		message("XX> NOT Building project in folder ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
	endif()
ENDFOREACH()

## message(FATAL_ERROR "-> Hier")