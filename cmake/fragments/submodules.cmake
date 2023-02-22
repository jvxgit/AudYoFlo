macro(jvx_submodules submodule_path subproject_pathmap)
	# Obtain submodule list
  set(JVX_SUBMODULE_LIST "" CACHE STRING "Defined submodule list")
  # If provided, load submodule list
  message("------> ${JVX_BINARY_DIR}/submodulelist.txt")
  if(EXISTS ${JVX_BINARY_DIR}/submodulelist.txt)
	include(${JVX_BINARY_DIR}/submodulelist.txt)
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
  	
	# message(FATAL_ERROR "Hier")
		
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
						set(JVX_CONFIGURED_SUBMODULES_PRE "${JVX_CONFIGURED_SUBMODULES_PRE};${subdir}")
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
					set(JVX_CONFIGURED_SUBMODULES_PRE "${JVX_CONFIGURED_SUBMODULES_PRE};${subdir}")
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
				set(JVX_CONFIGURED_SUBMODULES "${JVX_CONFIGURED_SUBMODULES};${entry}")
			endif()
		ENDFOREACH()
	else()
		set(JVX_CONFIGURED_SUBMODULES "${JVX_CONFIGURED_SUBMODULES_PRE}")
	endif()		

	# The second variable may still be in use somewhere!
	set(JVX_CONFIGURED_AUDIO_SUBMODULES ${JVX_CONFIGURED_SUBMODULES})

	FOREACH(subdir ${JVX_CONFIGURED_SUBMODULES})
		message("XX> Building sub project in folder ${submodule_path}/${subdir}")
		set(JVX_FOLDER_HIERARCHIE_BASE_OLD ${JVX_FOLDER_HIERARCHIE_BASE})
		set(JVX_FOLDER_HIERARCHIE_BASE "${JVX_FOLDER_HIERARCHIE_BASE}/sub-projects/${subdir}")		
		add_subdirectory(${submodule_path}/${subdir} ${subproject_pathmap}/${subdir})
		set(JVX_FOLDER_HIERARCHIE_BASE ${JVX_FOLDER_HIERARCHIE_BASE_OLD})
	ENDFOREACH()
	
endmacro(jvx_submodules)