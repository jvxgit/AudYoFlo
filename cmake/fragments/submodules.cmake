macro(jvx_submodules submodule_path subproject_pathmap pass_expr)
	# Obtain submodule list. If we do not find it in the submodulelist.txt
	# we can also specify as an option.
  set(JVX_SUBMODULE_LIST "" CACHE STRING "Defined submodule list")
  
  set(JVX_SUBMODULELIST_FILE ${JVX_BINARY_DIR}/submodulelist.txt)
  if(NOT JVX_SUBMODULELIST_FILENAME STREQUAL "")
	set(JVX_SUBMODULELIST_FILE ${JVX_SUBMODULELIST_FILENAME})
  endif()
  message(" ### Using submodule list file ${JVX_SUBMODULELIST_FILE} ### ")
	 
  # If provided, load submodule list
  message("------> ${JVX_SUBMODULELIST_FILE}")
  if(EXISTS ${JVX_SUBMODULELIST_FILE})
	include(${JVX_SUBMODULELIST_FILE})
	message("##> Submodule list: ${mysubmodules}")
  else()
	set(mysubmodules ${JVX_SUBMODULE_LIST})
  endif()
  
  message("<< Checking for submodule settings in folder <${submodule_path}> >>")
	if(mysubmodules)
		message("<< Selected submodules: <${mysubmodules}> >>")
	endif()
	if(ignsubmodules)
		message("<< Ignored submodules: <${ignsubmodules}> >>")
	endif()
  	
	# Since this function is a macro we need to reset local variables in case we use this macro a second time
	set(JVX_CONFIGURED_SUBMODULES "")

	set(JVX_CONFIGURED_SUBMODULES_PRE "")
	# SUBDIRLIST(SUBDIRS ${JVXRT_SUBMODULE_PATH})
	
	SUBDIRLIST(SUBDIRS ${submodule_path})

	message ("--> Sub directories to be evaluated: ${SUBDIRS}")
	
	# LABEL ADD SUB_PROJECTS HERE!
	FOREACH(subdir ${SUBDIRS})
		if(mysubmodules)
			message("++> Look for ${subdir} in list of allowed submodules: \"${mysubmodules}\"")
			list(FIND mysubmodules ${subdir} ifoundit)
			if(${ifoundit} GREATER -1)
				if(
					EXISTS ${submodule_path}/${subdir}/.jvxprj.base OR 
					EXISTS ${submodule_path}/${subdir}/.jvxprj.audio OR
					EXISTS ${submodule_path}/${subdir}/.jvxprj.flutter OR
					EXISTS ${submodule_path}/${subdir}/.jvxprj)
					if(EXISTS ${submodule_path}/${subdir}/CMakeLists.txt)
						if(EXISTS ${submodule_path}/${subdir}/.pass.${pass_expr})
							list(APPEND JVX_CONFIGURED_SUBMODULES_PRE ${subdir})
							message("Adding subfolder ${subdir} within pass <${pass_expr}>")
						else()
							message("Subfolder ${submodule_path}/${subdir} not configured for pass <${pass_expr}>.")
						endif()
					else()
						message("There is no CMakeLists.txt in ${submodule_path}/${subdir}, ignoring folder for build process.")
					endif()
				else()
					message("|--> .jvxprj.base|.jvxprj.audio|.jvxprj.flutter|.jvxprj not found in ${submodule_path}/${subdir}, NOT Building folder.")
				endif()
			else()
				message("|--> Project ${submodule_path}/${subdir} is not in list for allowed submodules.")
			endif()
		else()
			if(
				EXISTS ${submodule_path}/${subdir}/.jvxprj.base OR 
				EXISTS ${submodule_path}/${subdir}/.jvxprj.audio OR 
				EXISTS ${submodule_path}/${subdir}/.jvxprj.flutter OR 
				EXISTS ${submodule_path}/${subdir}/.jvxprj)
				if(EXISTS ${submodule_path}/${subdir}/CMakeLists.txt)
					if(EXISTS ${submodule_path}/${subdir}/.pass.${pass_expr})
						list(APPEND JVX_CONFIGURED_SUBMODULES_PRE ${subdir})
						message("Adding subfolder ${subdir} within pass <${pass_expr}>")
					else()						
						message("Subfolder ${submodule_path}/${subdir} not configured for pass <${pass_expr}>.")
					endif()
					# message(FATAL_ERROR "-> ${pass_expr}")
				else()
					message("There is no CMakeLists.txt in ${submodule_path}/${subdir}, ignoring folder for build process.")
				endif()
			else()
				message("|--> .jvxprj.base|.jvxprj.audio|.jvxprj.flutter|.jvxprj not found in ${submodule_path}/${subdir}, NOT Building folder.")
			endif()
		endif()
	ENDFOREACH()
	
	message("##> Step 1: Pre-selected submodules: ${JVX_CONFIGURED_SUBMODULES_PRE}")
	if(ignsubmodules)	
		FOREACH(entry ${JVX_CONFIGURED_SUBMODULES_PRE})
		
			# message("Look for ${entry} in list of ignored submodules: \"${ignsubmodules}\"")
			list(FIND ignsubmodules ${entry} ifoundit)
			if(${ifoundit} GREATER -1)
				# Nothing, ignoring
				message("|--> Project ${submodule_path}/${entry} is in list for ignored submodules - not building it.")
			else()				
				list(APPEND JVX_CONFIGURED_SUBMODULES ${entry})
			endif()
		ENDFOREACH()
	else()
		set(JVX_CONFIGURED_SUBMODULES "${JVX_CONFIGURED_SUBMODULES_PRE}")
	endif()		

	# The second variable may still be in use somewhere!
	set(JVX_CONFIGURED_AUDIO_SUBMODULES ${JVX_CONFIGURED_SUBMODULES})
	message("==> ${JVX_CONFIGURED_SUBMODULES}")
	
	FOREACH(subdir ${JVX_CONFIGURED_SUBMODULES})
		message("XX> Building sub project in folder ${submodule_path}/${subdir}")
		set(JVX_FOLDER_HIERARCHIE_BASE_OLD ${JVX_FOLDER_HIERARCHIE_BASE})
		set(JVX_FOLDER_HIERARCHIE_BASE "${JVX_FOLDER_HIERARCHIE_BASE}/sub-projects/${subdir}")		
		add_subdirectory(${submodule_path}/${subdir} ${subproject_pathmap}/${subdir})
		set(JVX_FOLDER_HIERARCHIE_BASE ${JVX_FOLDER_HIERARCHIE_BASE_OLD})
	ENDFOREACH()
	
endmacro(jvx_submodules)