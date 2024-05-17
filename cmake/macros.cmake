	# print a clearly visible header message
macro(header_message text)
  message("")
  message("---------------------------------------------------------")
  message("${text}")
  message("---------------------------------------------------------")
  message("")
endmacro(header_message)

# List directories
MACRO(SUBDIRLIST result curdir)

  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

# execute sed on an input file
macro(jvx_one_sed_entry_define target input output commandline)
  add_custom_command(
    TARGET ${target}
    COMMAND ${SED}
    ARGS \"${commandline}\" ${input} > ${output}
    DEPENDS ${input}
    COMMENT "${target} :: ${input} -> ${output}")
endmacro(jvx_one_sed_entry_define)

# add object handles to a library
macro(jvx_addObjectAccess target suffix)
  message(WARNING "CMAKE MACRO jvx_addObjectAccess IS DEPRECATED")
  jvx_one_sed_entry_define(${target}${suffix} ${JVX_BASE_ROOT}/software/templates/sed_seeds/objectAccess.h.sed
    ${CMAKE_CURRENT_BINARY_DIR}/${target}${suffix}.h
    "s/JVX_PREPROCESSOR_TOKEN/__${target}${suffix}__H__/g\;s/JVX_OBJECT_INIT_FUNCTION/${target}_init/g\;s/JVX_OBJECT_TERMINATE_FUNCTION/${target}_terminate/g")
endmacro(jvx_addObjectAccess)

# add a collection of subdirs to build process
macro(jvx_addSubdirs message subdirlist  hierarch_token)
  header_message(${message})
  set(JVX_FOLDER_HIERARCHIE "${hierarch_token}")
  foreach(oneProject ${subdirlist})
    add_subdirectory(${oneProject})
    message("")
  endforeach(oneProject ${subdirlist})
endmacro(jvx_addSubdirs)

# add a collection of subdirs to build process
macro(jvx_addExtSubdir message subdir subdir_map hierarch_token)
  header_message(${message})
  set(JVX_FOLDER_HIERARCHIE "${hierarch_token}")
  add_subdirectory(${subdir} ${subdir_map})
  message("")
endmacro(jvx_addExtSubdir)

# display flags
macro(jvx_displayFlags target)
  GET_DIRECTORY_PROPERTY(output INCLUDE_DIRECTORIES)
  MESSAGE("      Include Directories: ${output}")
  get_target_property(USED_COMPILE_DEFINITIONS ${target} COMPILE_DEFINITIONS)
  message("      Compile definitions: ${USED_COMPILE_DEFINITIONS}")
  get_target_property(USED_COMPILE_FLAGS ${target} COMPILE_FLAGS)
  message("      Compiler flags:      ${USED_COMPILE_FLAGS}")
  get_target_property(USED_LINK_FLAGS ${target} LINK_FLAGS)
  message("      Linker flags:        ${USED_LINK_FLAGS}")
  get_target_property(USED_LINK_LIBRARIES ${target} LINK_LIBRARIES)
  if("${USED_LINK_LIBRARIES}" MATCHES "USED_LINK_LIBRARIES-NOTFOUND")
    message("      Linked libraries:")
  else()
    message("      Linked libraries:    ${USED_LINK_LIBRARIES}")
  endif()
  message("       Application configuration: ${JVX_APP_TYPE_TOKEN}")
  MESSAGE("******Project hierarchie token: ${JVX_FOLDER_HIERARCHIE}")
endmacro(jvx_displayFlags)

macro(jvx_genMatProperties targetname componenttype componentprefix localfilelist localoptions)
	string(REPLACE "-" "_" targetname_tweaked ${targetname})
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/m-files/+${targetname_tweaked})
  foreach(PCGFILE ${localfilelist})
	string(REPLACE ";" " " localoptionsstr "${localoptions}")
	if(JVX_MAX_PATH_PROP_MAT)
		set(localoptionsstr "${localoptionsstr} -maxl ${JVX_MAX_PATH_PROP_MAT}")
	endif()
	
	message("    > PCG MAT Generator: ${PCGFILE} for component of type ${componenttype}, generator prefix ${componentprefix}, local options = ${localoptionsstr}")
	# message("exec_program(\"${JVX_PCG_MATLAB} \\\"${PCGFILE}\\\" -o \\\"${CMAKE_CURRENT_BINARY_DIR}/m-files/+${targetname_tweaked}\\\" -ctp ${componenttype} -cpf ${componentprefix} ${localoptionsstr}\")")
	# pcg_mat_opts "${localoptions}")
	# message("--> 1 ${localoptions} 2")	
	# Added some quotes for systems with spaces in path
	install(CODE "exec_program(\"${JVX_PCG_MATLAB} \\\"${PCGFILE}\\\" -o \\\"${CMAKE_CURRENT_BINARY_DIR}/m-files/+${targetname_tweaked}\\\" -ctp ${componenttype} -cpf ${componentprefix} ${localoptionsstr}\")")
  endforeach()
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/m-files/+${targetname_tweaked} DESTINATION ${INSTALL_PATH_MATLAB_SUBPROJECT})
endmacro(jvx_genMatProperties)


macro(find_qt)
	# qt, if desired test the system for what is required
	message("--> Looking for Qt5")

	# If we do a cross compile, the moc, uic and rcc must be called from another place
	# than that at which the libraries and headers exist
	if(JVX_CROSS_COMPILE)
		add_executable(Qt5::moc IMPORTED)
		set(imported_location "moc")
		set_target_properties(Qt5::moc PROPERTIES
			IMPORTED_LOCATION ${imported_location}
		)
		# For CMake automoc feature
		get_target_property(QT_MOC_EXECUTABLE Qt5::moc LOCATION)

		add_executable(Qt5::uic IMPORTED)
		set(imported_location "uic")
		set_target_properties(Qt5::uic PROPERTIES
			IMPORTED_LOCATION ${imported_location}
		)
		# For CMake automoc feature
		get_target_property(QT_UIC_EXECUTABLE Qt5::uic LOCATION)

		add_executable(Qt5::rcc IMPORTED)
		set(imported_location "rcc")
		set_target_properties(Qt5::rcc PROPERTIES
			IMPORTED_LOCATION ${imported_location}
		)
		# For CMake automoc feature
		get_target_property(QT_RCC_EXECUTABLE Qt5::rcc LOCATION)

                add_executable(Qt5::qmake IMPORTED)
		set(imported_location "qmake")
		set_target_properties(Qt5::qmake PROPERTIES
			IMPORTED_LOCATION ${imported_location}
		)
		# For CMake automoc feature
		get_target_property(QT_QMAKE_EXECUTABLE Qt5::qmake LOCATION)

	endif()
	
	if(QT_INSTALL_PATH)
		message("Using QT_INSTALL_PATH=${QT_INSTALL_PATH}")
		set(QT_INSTALL_CMAKE_PATH ${QT_INSTALL_PATH}/lib/cmake)
		# FIXME: needed under win? set(JVX_QT_RPATH -Wl,-rpath-link,${QT_PATH}/lib)
		set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${QT_INSTALL_CMAKE_PATH}")
	else()
		message("Warning, no QT installation specified")
	endif()

	if(JVX_QT_VERSION_6)
		find_package(Qt6Core)

		if(NOT ${Qt6Core_FOUND})
			set(JVX_USE_QT FALSE)
			message("    lib qt6 not available.")
		else()
			find_package(Qt6Gui REQUIRED)
			find_package(Qt6Core REQUIRED)
			find_package(Qt6Widgets REQUIRED)
			find_package(Qt6Concurrent REQUIRED)
			find_package(Qt6PrintSupport REQUIRED)

			set(JVX_QT_LIBS Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Concurrent Qt6::PrintSupport)
			get_target_property(QT_LIB_PATH Qt6::Core LOCATION)

			find_package(Qt6Network REQUIRED)
			# find_package(Qt6WebSockets REQUIRED)
			# set(JVX_QT_NET_LIB Qt6::Network Qt6::WebSockets)
			set(JVX_QT_NET_LIB Qt6::Network)

			find_package(Qt6Qml REQUIRED)
			set(JVX_QT_QML_LIB Qt6::Qml)
			
			#find_package(Qt63DAnimation REQUIRED)
			#find_package(Qt63DCore REQUIRED)
			#find_package(Qt63DExtras REQUIRED)
			#find_package(Qt63DInput REQUIRED)
			#find_package(Qt63DLogic REQUIRED)
			#find_package(Qt63DQuick REQUIRED)
			#find_package(Qt63DQuickAnimation REQUIRED)
			#find_package(Qt63DQuickExtras REQUIRED)
			#find_package(Qt63DQuickInput REQUIRED)
			#find_package(Qt63DQuickRender REQUIRED)
			#find_package(Qt63DQuickScene2D REQUIRED)
			#find_package(Qt63DRender REQUIRED)
			#set(JVX_QT_3D_LIB Qt6::3DAnimation 
			#	Qt6::3DCore Qt6::3DExtras Qt6::3DInput 
			#	Qt6::3DLogic Qt6::3DQuick Qt6::3DQuick 
			#	Qt6::3DQuickAnimation Qt6::3DQuickExtras 
			#	Qt6::3DQuickInput Qt6::3DQuickRender 
			#	Qt6::3DQuickScene2D Qt6::3DRender)

			find_package(Qt6OpenGL REQUIRED)
			set(JVX_QT_OPENGL_LIB Qt6::OpenGL)
		
			find_package(Qt6Svg REQUIRED)
			set(JVX_QT_SVG_LIB Qt6::Svg)
		endif()
		
	else()
	
		find_package(Qt5Core)

		if(NOT ${Qt5Core_FOUND})
			set(JVX_USE_QT FALSE)
			message("    lib qt5 not available.")
			
		else()
			find_package(Qt5Gui REQUIRED)
			find_package(Qt5Core REQUIRED)
			find_package(Qt5Widgets REQUIRED)
			find_package(Qt5Concurrent REQUIRED)
			find_package(Qt5PrintSupport REQUIRED)
			find_package(Qt5Svg REQUIRED)

			set(JVX_QT_LIBS Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent Qt5::PrintSupport Qt5::Svg)
			get_target_property(QT_LIB_PATH Qt5::Core LOCATION)

			find_package(Qt5Network REQUIRED)
			find_package(Qt5WebSockets REQUIRED)
			set(JVX_QT_NET_LIB Qt5::Network Qt5::WebSockets)

			find_package(Qt5Qml REQUIRED)
			set(JVX_QT_QML_LIB Qt5::Qml)
		
			find_package(Qt53DAnimation REQUIRED)
			find_package(Qt53DCore REQUIRED)
			find_package(Qt53DExtras REQUIRED)
			find_package(Qt53DInput REQUIRED)
			find_package(Qt53DLogic REQUIRED)
			find_package(Qt53DQuick REQUIRED)
			find_package(Qt53DQuickAnimation REQUIRED)
			find_package(Qt53DQuickExtras REQUIRED)
			find_package(Qt53DQuickInput REQUIRED)
			find_package(Qt53DQuickRender REQUIRED)
			find_package(Qt53DQuickScene2D REQUIRED)
			find_package(Qt53DRender REQUIRED)
			set(JVX_QT_3D_LIB Qt5::3DAnimation 
				Qt5::3DCore Qt5::3DExtras Qt5::3DInput 
				Qt5::3DLogic Qt5::3DQuick Qt5::3DQuick 
				Qt5::3DQuickAnimation Qt5::3DQuickExtras 
				Qt5::3DQuickInput Qt5::3DQuickRender 
				Qt5::3DQuickScene2D Qt5::3DRender)

			find_package(Qt5OpenGL REQUIRED)
			set(JVX_QT_OPENGL_LIB Qt5::OpenGL)
		
			find_package(Qt5Svg REQUIRED)
			set(JVX_QT_SVG_LIB Qt5::Svg)
		endif()
	endif()

	if(JVX_USE_QT)
		get_filename_component(QT_LIB_PATH ${QT_LIB_PATH} DIRECTORY)
		message("    lib: ${QT_LIB_PATH}")

		if(JVX_CROSS_COMPILE)

		  if(JVX_SDK_FIX_DEPENDENCIES_GL)

		    ## Somehow the QT5 GL package has dependencies to these libraries but
		    ## they are not specified in cmake build process
			#find_library(GLES2_LIBRARY NAMES GLESv2 HINTS "${QT_INSTALL_PATH}/lib")
			#set(JVX_QT_LIBS "${JVX_QT_LIBS};${GLES2_LIBRARY}")

			#find_library(PNG16_LIBRARY NAMES png16 HINTS "${QT_INSTALL_PATH}/lib")
			#set(JVX_QT_LIBS "${JVX_QT_LIBS};${PNG16_LIBRARY}")

			#find_library(DRM_LIBRARY NAMES drm HINTS "${QT_INSTALL_PATH}/lib")
			#set(JVX_QT_LIBS "${JVX_QT_LIBS};${DRM_LIBRARY}")

			#find_library(GLAPI_LIBRARY NAMES glapi HINTS "${QT_INSTALL_PATH}/lib")
			#set(JVX_QT_LIBS "${JVX_QT_LIBS};${GLAPI_LIBRARY}")
			## message(FATAL_ERROR "-----> ${GLAPI_LIBRARY} seached for in ${QT_INSTALL_PATH}/lib")

		      endif()
		endif()
	endif()
endmacro()

FUNCTION(JVX_PREPEND var prefix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar "${prefix}/${f}")
   ENDFOREACH(f)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(JVX_PREPEND)

macro(JVX_PREPEND_OUTFILE var prefix)
   SET(listVar "")
   FOREACH(f ${var})
      LIST(APPEND listVar "${prefix}/${f}")
   ENDFOREACH(f)
   message("###> ${listVar}")
   SET(OUTFILE "${listVar}")
endmacro(JVX_PREPEND_OUTFILE)

function(JVX_CHECK_DEPENDENCIES JVX_DEPENDENCY_MODULES JVX_CONFIGURED_SUBMODULES PROJECT_NAME)
	message("--> Checking module dependencies for project ${PROJECT_NAME}: Look for ${JVX_DEPENDENCY_MODULES} in ${JVX_CONFIGURED_SUBMODULES}")
	FOREACH(lookfor ${JVX_DEPENDENCY_MODULES})
		list(FIND JVX_CONFIGURED_SUBMODULES ${lookfor} ifoundit)
		if(${ifoundit} GREATER -1)
			message("    > Found ${lookfor} in current configuration")
		else()
			message(FATAL_ERROR "!!! For module ${PROJECT_NAME}, you need to activate module ${lookfor} !!!")
		endif()
	ENDFOREACH()
endfunction(JVX_CHECK_DEPENDENCIES)

macro(JVX_CHECK_MODULE JVX_DEPENDENCY_MODULES JVX_CONFIGURED_SUBMODULES PROJECT_NAME JVX_CHECK_MODULE_RESULT)
	set(${JVX_CHECK_MODULE_RESULT} TRUE)
	
	# FOREACH(inlist ${JVX_CONFIGURED_SUBMODULES})
	#	message("-LL-> ${inlist}")
	# endforeach()
	
	# message("--> Checking module dependencies for project ${PROJECT_NAME}: Look for ${JVX_DEPENDENCY_MODULES} in ${JVX_CONFIGURED_SUBMODULES}")
	FOREACH(lookfor ${JVX_DEPENDENCY_MODULES})
		
		set(LOCAL_FIND FALSE)
		FOREACH(inlist ${JVX_CONFIGURED_SUBMODULES})
			# message("-LL-> ${inlist} -- ${lookfor}")
			if(${lookfor} STREQUAL ${inlist})
				message("if(${lookfor} STREQUAL ${inlist}) -> TRUE")
				set(LOCAL_FIND TRUE)
			endif()
			# message("-LL-> ${LOCAL_FIND}")
		endforeach()
		
		if(NOT ${LOCAL_FIND})
			set(${JVX_CHECK_MODULE_RESULT} FALSE)
		endif()
		#list(FIND JVX_CONFIGURED_SUBMODULES ${lookfor} ifoundit)
		#message("Result: ${ifoundit}")
		#if(${ifoundit} GREATER -1)
		#	message("    > Found ${lookfor} in current configuration")
		#	set( TRUE)			
		#else()
		#	message("!!! For module ${PROJECT_NAME}, you need to activate module ${lookfor} !!!")
		#endif()
	ENDFOREACH()
endmacro(JVX_CHECK_MODULE)

macro(JVX_ACTIVATE_VERSION_MATLAB project_name local_project_options)
        if(NOT JVX_USE_PART_MATLAB)
               message(FATAL_ERROR "Matlab support requested, but it is disabled")
        endif()
	set(JVX_TARGET_NAME ${project_name}m)
	set(JVX_TARGET_NAME_STATIC ${project_name}m)
	set(JVX_COMPONENT_ONLY_SHARED TRUE)
	set(JVX_MATLAB_PROJECT_CLONE TRUE)

	# Activate/deactivate external Matlab interfacing
	if(${local_project_options} MATCHES "JVX_EXTERNAL_CALL_ENABLED")
		set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};${local_project_options}")
		include_directories(${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLexternalCall/include)
		set(JVX_MCG_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/export_project.mcg)
		if(JVX_MCG_LOCAL_FILE)
			set(JVX_MCG_FILE_NAME ${JVX_MCG_LOCAL_FILE})
		endif()
		if(EXISTS ${JVX_MCG_FILE_NAME})
			message("--> MCG code generation running on file(s) ${JVX_MCG_FILE_NAME}")
			set(LOCAL_MCG_FILES ${JVX_MCG_FILE_NAME})				
			set(LOCAL_SOURCES ${LOCAL_SOURCES} ${LOCAL_MCG_FILES})
		endif()	
		set(LOCAL_LIBS ${LOCAL_LIBS} jvxLexternalCall_static)		
	endif()

	include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
endmacro(JVX_ACTIVATE_VERSION_MATLAB)

macro(JVX_ACTIVATE_ANOTHER modified_project_name)

	set(JVX_TARGET_NAME ${modified_project_name})
	set(JVX_MODULE_NAME ${JVX_TARGET_NAME})
	set(JVX_TARGET_NAME_STATIC ${modified_project_name}_static)
	set(JVX_TARGET_NAME_IMPORT ${modified_project_name}_import)
	
	include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
endmacro(JVX_ACTIVATE_ANOTHER)

macro(force_console_app)
	set(JVX_APP_TYPE_TOKEN "")
endmacro(force_console_app)

macro(jvx_install_project_folders collect_install_subproject)
	if(NOT JVX_RELEASE_SDK OR JVX_FORCE_INSTALL_PROJECT_SDK)
		
		## message("collect_subproject = ${collect_subproject}")
		set(LOCAL_COLLECT_LIST ${JVX_SUB_PROJECT_INSTALLS})
		if(${collect_install_subproject})
			message("** Collect install folders **")
		endif()
		
		message("Looking for additional install files..")
		message("-1-> Looking for folder ${CMAKE_CURRENT_SOURCE_DIR}/install")
		if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/install)
			message("-1.A->Looking for folder ${CMAKE_CURRENT_SOURCE_DIR}/install/${JVX_OS}")
			if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/install/${JVX_OS})
				message("Install from folder ${CMAKE_CURRENT_SOURCE_DIR}/install/${JVX_OS} -> ${INSTALL_PATH_RUNTIME}")
				install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/install/${JVX_OS}/ DESTINATION ${INSTALL_PATH_RUNTIME} USE_SOURCE_PERMISSIONS)

				if(${collect_install_subproject})
					LIST(APPEND LOCAL_COLLECT_LIST "${CMAKE_CURRENT_SOURCE_DIR}/install/${JVX_OS}/")
				endif()
				
				if(JVX_COLLECT_LOCAL_INSTALL_BUNDLES)
					set(JVX_COLLECT_LOCAL_INSTALL_BUNDLES "${JVX_COLLECT_LOCAL_INSTALL_BUNDLES};${CMAKE_CURRENT_SOURCE_DIR}/install/${JVX_OS}/" CACHE INTERNAL "project install folders")
    				else()
					set(JVX_COLLECT_LOCAL_INSTALL_BUNDLES "${CMAKE_CURRENT_SOURCE_DIR}/install/${JVX_OS}/" CACHE INTERNAL "project install folders")
    				endif()
			endif()
			message("-1.B->Looking for folder ${CMAKE_CURRENT_SOURCE_DIR}/install/common")
			if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/install/common)
				message("Install from folder ${CMAKE_CURRENT_SOURCE_DIR}/install/common -> ${INSTALL_PATH_RUNTIME}")
				install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/install/common/ DESTINATION ${INSTALL_PATH_RUNTIME} USE_SOURCE_PERMISSIONS)
			
				if(${collect_install_subproject})
					LIST(APPEND LOCAL_COLLECT_LIST "${CMAKE_CURRENT_SOURCE_DIR}/install/common/")
				endif()

				if(JVX_COLLECT_LOCAL_INSTALL_BUNDLES)
					set(JVX_COLLECT_LOCAL_INSTALL_BUNDLES "${JVX_COLLECT_LOCAL_INSTALL_BUNDLES};${CMAKE_CURRENT_SOURCE_DIR}/install/common/" CACHE INTERNAL "project install folders")
    				else()
					set(JVX_COLLECT_LOCAL_INSTALL_BUNDLES "${CMAKE_CURRENT_SOURCE_DIR}/install/common/" CACHE INTERNAL "project install folders")
    				endif()
			endif()
			if(JVX_CROSS_COMPILE)
				message("-1.C->Looking for folder ${CMAKE_CURRENT_SOURCE_DIR}/install/${JVX_OS}-cc")
				if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/install/${JVX_OS}-cc)
					message("Install from folder ${CMAKE_CURRENT_SOURCE_DIR}/install/${JVX_OS}-cc -> ${INSTALL_PATH_RUNTIME}")
					install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/install/${JVX_OS}-cc/ DESTINATION ${INSTALL_PATH_RUNTIME} USE_SOURCE_PERMISSIONS)
					
					if(${collect_install_subproject})
						LIST(APPEND LOCAL_COLLECT_LIST "${CMAKE_CURRENT_SOURCE_DIR}/install/${JVX_OS}-cc/")
					endif()
				
					if(JVX_COLLECT_LOCAL_INSTALL_BUNDLES)
						set(JVX_COLLECT_LOCAL_INSTALL_BUNDLES "${JVX_COLLECT_LOCAL_INSTALL_BUNDLES};${CMAKE_CURRENT_SOURCE_DIR}/install/${JVX_OS}-cc/" CACHE INTERNAL "project install folders")
    					else()
						set(JVX_COLLECT_LOCAL_INSTALL_BUNDLES "${CMAKE_CURRENT_SOURCE_DIR}/install/${JVX_OS}-cc/" CACHE INTERNAL "project install folders")
    					endif()
				endif()
			endif()			
		endif()
		message("-2-> Looking for folder ${CMAKE_CURRENT_SOURCE_DIR}/wavs")
		if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/wavs)
			message("Install from folder ${CMAKE_CURRENT_SOURCE_DIR}/wavs/ -> ${INSTALL_PATH_RUNTIME}/wavs")
			install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/wavs/ DESTINATION ${INSTALL_PATH_RUNTIME}/wavs USE_SOURCE_PERMISSIONS)
					
			if(JVX_COLLECT_LOCAL_INSTALL_BUNDLES)
				set(JVX_COLLECT_LOCAL_INSTALL_BUNDLES "${JVX_COLLECT_LOCAL_INSTALL_BUNDLES};${CMAKE_CURRENT_SOURCE_DIR}/wavs/" CACHE INTERNAL "project install folders")
    			else()
				set(JVX_COLLECT_LOCAL_INSTALL_BUNDLES "${CMAKE_CURRENT_SOURCE_DIR}/wavs/" CACHE INTERNAL "project install folders")
    			endif()
		endif()
		#if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/python)
		#	message("Install from folder ${CMAKE_CURRENT_SOURCE_DIR}/python/ -> ${INSTALL_PATH_RUNTIME}/python")
		#	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python/ DESTINATION ${INSTALL_PATH_RUNTIME}/python USE_SOURCE_PERMISSIONS)
					
		#	if(JVX_COLLECT_LOCAL_INSTALL_BUNDLES)
		#		set(JVX_COLLECT_LOCAL_INSTALL_BUNDLES "${JVX_COLLECT_LOCAL_INSTALL_BUNDLES};${CMAKE_CURRENT_SOURCE_DIR}/python/" CACHE INTERNAL "project install folders")
    	#		else()
		#		set(JVX_COLLECT_LOCAL_INSTALL_BUNDLES "${CMAKE_CURRENT_SOURCE_DIR}/python/" CACHE INTERNAL "project install folders")
    	#		endif()
		#endif()
	endif()
	
	if(${collect_install_subproject})
		message("** ${JVX_SUB_PROJECT_INSTALLS} **")
		set(JVX_SUB_PROJECT_INSTALLS ${LOCAL_COLLECT_LIST} CACHE INTERNAL "sub project install folders")
	endif()
		
endmacro(jvx_install_project_folders)

macro(jvx_install_dedicated_file jvx_install_file_from  jvx_install_file_to)
	if(EXISTS ${jvx_install_file_from})
		install(FILES ${jvx_install_file_from} DESTINATION ${jvx_install_file_to} COMPONENT ${INSTALL_COMPONENT})
	else()
		message(FATAL_ERROR "Could not find file ${jvx_install_file_from}")
	endif()
endmacro(jvx_install_dedicated_file)

macro(jvx_install_qt_plugins src_folder dest_folder copy_dir_list)

	if("${copy_dir_list}")
	
		#FOREACH(pluginprefix ${${copy_file_list}})
			# MESSAGE("Copy plugin dll ${src_folder}/plugins/${pluginprefix}.${JVX_SHARED_EXTENSION}")
			
		#	install(FILES "${src_folder}/plugins/${pluginprefix}.${JVX_SHARED_EXTENSION}" DESTINATION ${dest_folder}				
		#		CONFIGURATIONS Debug Release UnstableDebug UnstableRelease COMPONENT qtplugins)
		#ENDFOREACH()
		
		FOREACH(plugindir ${${copy_dir_list}})
			MESSAGE("Copy plugin folder ${src_folder}/plugins/${plugindir}")
			
			install(DIRECTORY "${src_folder}/plugins/${plugindir}" DESTINATION ${dest_folder}				
				CONFIGURATIONS Debug Release UnstableDebug UnstableRelease COMPONENT qtplugins)
		ENDFOREACH()

	else()
		install(DIRECTORY
			${src_folder}/plugins/ DESTINATION ${dest_folder}				
			CONFIGURATIONS Debug Release UnstableDebug UnstableRelease
			COMPONENT qtplugins
			FILES_MATCHING PATTERN "*.${JVX_SHARED_EXTENSION}")
	endif()
		
endmacro(jvx_install_qt_plugins)

macro(jvx_subproject_copy_system_files main_app_name sub_project_install_dirs install_target_path)
	
		## message("${sub_project_install_dirs} -- ${main_app_name}")
		if(NOT ${sub_project_install_dirs} EQUAL "")
			
			FOREACH(f ${sub_project_install_dirs})

				message("--> install(DIRECTORY ${f} DESTINATION ${install_target_path}${main_app_name}.app/Contents/runtime USE_SOURCE_PERMISSIONS)")
				install(DIRECTORY ${f} DESTINATION ${install_target_path}${main_app_name}.app/Contents/runtime USE_SOURCE_PERMISSIONS)
			ENDFOREACH(f)
		endif()

	## message(FATAL_ERROR "Stop here")
		
endmacro(jvx_subproject_copy_system_files)

macro(jvx_pcg_files_convert LOCAL_PCG_FILES LOCAL_PCG_FILE_OPTIONS PROJECT_TARGET PROJECT_TARGET_PCG ADDITIONAL_PCG_FILE_DEPENDS )

	message("    > PCG (${JVX_PCG})")
	message("      Build Options: ${LOCAL_PCG_FILE_OPTIONS}")

	#
	# https://stackoverflow.com/questions/2937128/cmake-add-custom-command-not-being-run
	#
	
	## message("Macro <jvx_pcg_files_convert>: ${LOCAL_PCG_FILES}")
	## message("-Arg 1: ${LOCAL_PCG_FILES}")
	## message("-Arg 2: ${LOCAL_PCG_FILE_OPTIONS}")
	## message("-Arg 3: ${PROJECT_TARGET}")
	## message("-Arg 4: ${PROJECT_TARGET_PCG}")
	## message("-Arg 5: ${ADDITIONAL_PCG_FILE_DEPENDS}")
	
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)
	include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated)
	set(ADDITIONAL_PCG_FILE_DEPENDS_LST "")
	
	foreach(PCGFILE ${LOCAL_PCG_FILES})
		get_filename_component(PCGDIR ${PCGFILE} DIRECTORY)
		get_filename_component(PCGTOKEN ${PCGFILE} NAME_WE)
		set(command "${JVX_PCG} -M ${PCGFILE} ${LOCAL_PCG_FILE_OPTIONS}")
		execute_process(COMMAND ${command} OUTPUT_VARIABLE ADDITIONAL_PCG_FILE_DEPENDS)

		if(ADDITIONAL_PCG_FILE_DEPENDS)

			foreach(PCGDEPFILE ${ADDITIONAL_PCG_FILE_DEPENDS})

				# On windows, remove backslashes
				string(REGEX REPLACE "\\\\" "/" PCGDEPFILE_ADD ${PCGDEPFILE})
				set(ADDITIONAL_PCG_FILE_DEPENDS_LST ${ADDITIONAL_PCG_FILE_DEPENDS_LST} ${PCGDEPFILE_ADD})
			endforeach()
			message("      depends on: ${ADDITIONAL_PCG_FILE_DEPENDS} ${LOCAL_PCG_FILE_DEPENDS}")
		endif()
		
		## message("add_custom_command(
		##	COMMAND ${JVX_PCG}
		##	ARGS ${PCGDIR}/${PCGTOKEN}.pcg -o ${CMAKE_CURRENT_BINARY_DIR}/generated/pcg_${PCGTOKEN}.h ${LOCAL_PCG_FILE_OPTIONS}
		##	DEPENDS ${PCGDIR}/${PCGTOKEN}.pcg ${JVX_PCG} ${LOCAL_PCG_FILE_DEPENDS} ${ADDITIONAL_PCG_FILE_DEPENDS_LST}
		##	# BYPRODUCTS byproduct_${PCGTOKEN}.txt # explicit byproduct specification
		##	OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/generated/pcg_${PCGTOKEN}.h)")
		add_custom_command(
			COMMAND ${JVX_PCG}
			ARGS ${PCGDIR}/${PCGTOKEN}.pcg -o ${CMAKE_CURRENT_BINARY_DIR}/generated/pcg_${PCGTOKEN}.h ${LOCAL_PCG_FILE_OPTIONS}
			DEPENDS ${PCGDIR}/${PCGTOKEN}.pcg ${JVX_PCG} ${LOCAL_PCG_FILE_DEPENDS} ${ADDITIONAL_PCG_FILE_DEPENDS_LST}
			# BYPRODUCTS byproduct_${PCGTOKEN}.txt # explicit byproduct specification
			OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/generated/pcg_${PCGTOKEN}.h
			)
		#message("-PCG-> Producing byproduct file byproduct_${PCGTOKEN}.txt")

		list(APPEND JVX_PCG_OUTPUT_FILES ${CMAKE_CURRENT_BINARY_DIR}/generated/pcg_${PCGTOKEN}.h)
	endforeach()
	
	add_custom_target(
		${PROJECT_TARGET_PCG}
		DEPENDS ${JVX_PCG_OUTPUT_FILES}
		)
	add_dependencies(${PROJECT_TARGET} ${PROJECT_TARGET_PCG})
	
endmacro(jvx_pcg_files_convert)

macro(jvx_exclude_platform jvx_platform)

	message("Check exclude platform ${jvx_platform} vs ${JVX_OS}")
	if(${jvx_platform} MATCHES ${JVX_OS})
		return()
	endif()
endmacro(jvx_exclude_platform)

macro(jvx_single_platform jvx_platform)

	message("Check single platform: ${jvx_platform} vs ${JVX_OS}")
	if(NOT ${jvx_platform} MATCHES ${JVX_OS})
		return()
	endif()
endmacro(jvx_single_platform)

macro(windows_to_shell_path WIN_PATH OUTDIR)
cmake_path(CONVERT ${WIN_PATH} TO_CMAKE_PATH_LIST PATH)
string(REGEX REPLACE "^([a-zA-Z]):" "/\\1" ${OUTDIR} "${PATH}")
endmacro()



