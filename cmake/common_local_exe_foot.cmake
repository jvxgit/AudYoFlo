# build unstable targets only with Unstable build types
if((NOT JVX_DEPLOY_UNSTABLE) AND IS_UNSTABLE)
  message("Excluding unstable target ${JVX_TARGET_NAME} from build.")
  return()
endif()

###
# reset some stuff
###
#set(ADDITIONAL_CXX_FLAGS "")
set(ADDITIONAL_LIBS "")
set(ADDITIONAL_SOURCES "")
set(ADDITIONAL_UI_SOURCES "")
set(ADDITIONAL_COMPILE_DEFINITIONS "")
#set(ADDITIONAL_LINKER_FLAGS "")
#set(GENERATED_FILES "")
if(NOT INSTALL_COMPONENT)
	if(IS_UNSTABLE)
		set(INSTALL_COMPONENT "unstable")
	else()
		set(INSTALL_COMPONENT "release")
	endif()
endif()

###
# Tracy Profiler
###
if(NOT JVX_COMPILE_BUILDTOOLS)
  # add it even it JVX_USE_TRACY is not set because
  # the tracy target is added as interface library
  # to set the include dirs then
  if(NOT DEFINED JVX_SDK_PATH)
	
	# BUT - this only works in runtime build, not in SDK related build since there, the tracy interface lib does not exist
	set(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} tracy)
  endif()
endif()

###
# QT support
###
set(JVX_ENABLE_QT FALSE)
if(LOCAL_UI_SOURCES OR ADDITIONAL_UI_SOURCES OR USE_QT_LIBRARIES OR LOCAL_QT_RESOURCES OR LOCAL_UI_SOURCE_PATH)
	message("    > Executable (QT)")
  	set(JVX_ENABLE_QT TRUE)
endif()

if(JVX_ENABLE_QT)
	cmake_policy(SET CMP0020 NEW)
  
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_AUTOUIC_SEARCH_PATHS ${LOCAL_UI_SOURCE_PATH})
		
	#else()
	#	QT5_WRAP_UI(UI_HEADERS ${LOCAL_UI_SOURCES} ${ADDITIONAL_UI_SOURCES})
	#	qt5_add_resources(QT_RESOURCES ${LOCAL_QT_RESOURCES})
	#endif()
	
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  set(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} ${JVX_QT_LIBS})
else()
  message("    > Executable")
endif()

# MAC OS bundle
if(JVX_OS MATCHES "macosx" AND JVX_MAC_OS_CREATE_BUNDLE)

	  message("    > Create Mac OS X Bundle")
    set(CMAKE_MACOSX_RPATH TRUE) # Do not use RPATHs if you want to create deployable bundles
    set(MACOSX_BUNDLE_ICON_FILE Icon.icns)
    set(JVX_ICON ${CMAKE_CURRENT_SOURCE_DIR}/xcode/${JVX_TARGET_NAME}/Icons.xcassets/Icon.icns)
    set_source_files_properties(${JVX_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(${JVX_TARGET_NAME} MACOSX_BUNDLE ${LOCAL_SOURCES} ${ADDITIONAL_SOURCES} ${LOCAL_QT_RESOURCES} ${LOCAL_UI_SOURCES} ${ADDITIONAL_UI_SOURCES} ${JVX_ICON})
else()
#message("add_executable(${JVX_TARGET_NAME} ${LOCAL_SOURCES} ${ADDITIONAL_SOURCES} ${QT_RESOURCES} ${UI_HEADERS})")
	add_executable(${JVX_TARGET_NAME} ${JVX_APP_TYPE_TOKEN} ${LOCAL_SOURCES} ${ADDITIONAL_SOURCES} ${LOCAL_QT_RESOURCES} ${LOCAL_UI_SOURCES} ${ADDITIONAL_UI_SOURCES} ${JVX_LOCAL_RC_FILE})
	#else()
	#	add_executable(${JVX_TARGET_NAME} ${JVX_APP_TYPE_TOKEN} ${LOCAL_SOURCES} ${ADDITIONAL_SOURCES} ${QT_RESOURCES} ${UI_HEADERS})
	#endif()
endif()

###
# Some project folder groupings
###
set_property(TARGET ${JVX_TARGET_NAME} PROPERTY FOLDER "${JVX_FOLDER_HIERARCHIE}/executables")

# set flags
target_compile_definitions(${JVX_TARGET_NAME} PRIVATE ${LOCAL_COMPILE_DEFINITIONS} ${ADDITIONAL_COMPILE_DEFINITIONS} ${GLOBAL_COMPILE_DEFINITIONS})
if(IS_C_APP)
  set_target_properties(${JVX_TARGET_NAME} PROPERTIES

    message("##### JVX_CMAKE_C_FLAGS_SHARED=${JVX_CMAKE_C_FLAGS_SHARED}")
    message("##### LOCAL_C_FLAGS=${LOCAL_C_FLAGS}")
    message("##### ADDITIONAL_C_FLAGS=${ADDITIONAL_C_FLAGS}")
    message("##### JVX_LANGUAGE_SPECIFIC_OPTIONS=${JVX_LANGUAGE_SPECIFIC_OPTIONS}")
    
    COMPILE_FLAGS "${JVX_CMAKE_LINKER_FLAGS_EXEC} ${LOCAL_C_FLAGS}"
    LINK_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${JVX_CMAKE_LINKER_FLAGS_EXEC} ${LOCAL_LINKER_FLAGS}")
  target_link_libraries(${JVX_TARGET_NAME} ${LOCAL_LIBS} ${ADDITIONAL_LIBS} ${JVX_SYSTEM_LIBRARIES} )
  
else()
  message("##### JVX_CMAKE_CXX_FLAGS_SHARED=${JVX_CMAKE_CXX_FLAGS_SHARED}")
  message("##### LOCAL_CXX_FLAGS=${LOCAL_CXX_FLAGS}")
  message("##### ADDITIONAL_CXX_FLAGS=${ADDITIONAL_CXX_FLAGS}")
  message("##### JVX_LANGUAGE_SPECIFIC_OPTIONS=${JVX_LANGUAGE_SPECIFIC_OPTIONS}")
  
  set_target_properties(${JVX_TARGET_NAME} PROPERTIES
    COMPILE_FLAGS "${JVX_CMAKE_LINKER_FLAGS_EXEC} ${LOCAL_CXX_FLAGS}"
    LINK_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${JVX_CMAKE_LINKER_FLAGS_EXEC} ${LOCAL_LINKER_FLAGS}")
  target_link_libraries(${JVX_TARGET_NAME} ${LOCAL_LIBS} ${ADDITIONAL_LIBS} ${JVX_SYSTEM_LIBRARIES} )
  
endif()

if(LOCAL_C_STANDARD)
	set_property(TARGET ${JVX_TARGET_NAME} PROPERTY C_STANDARD ${LOCAL_C_STANDARD})
	# message(FATAL_ERROR "->  ${LOCAL_C_STANDARD}")
endif()

if(LOCAL_CXX_STANDARD)
	set_property(TARGET ${JVX_TARGET_NAME} PROPERTY CXX_STANDARD ${LOCAL_CXX_STANDARD})
endif()

if(JVX_LANGUAGE_SPECIFIC_OPTIONS)
		target_compile_options(${JVX_TARGET_NAME} PRIVATE ${JVX_LANGUAGE_SPECIFIC_OPTIONS})
endif()
	
if(JVX_OS MATCHES "macosx")
	  set_target_properties(${JVX_TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_CLANG_GENERATE_DEBUGGING_SYMBOLS "NO")
endif()

jvx_displayFlags(${JVX_TARGET_NAME})

###
# installation
###

# The target itself (*.exe)
if(JVX_OS MATCHES "macosx" AND JVX_MAC_OS_CREATE_BUNDLE)

    message("    > Prepare file for Mac OS X Bundle")
	
	set(JVX_OSX_BUNDLE_APP_PATH "${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app/Contents") 
	## ${INSTALL_PATH_RELEASE_ROOT_MIN_SLASH}${JVX_TARGET_NAME}.app/Contents")
	message("    > Mac OS X Bundle target folder: ${JVX_OSX_BUNDLE_APP_PATH}")

	set(JVX_OSX_BUNDLE_APP_PATH_RUNTIME "${JVX_OSX_BUNDLE_APP_PATH}/runtime")
	message("    > Mac OS X Bundle target runtime folder: ${JVX_OSX_BUNDLE_APP_PATH_RUNTIME}")

    set(CMAKE_MACOSX_RPATH ON)
    SET(CMAKE_SKIP_BUILD_RPATH TRUE)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    
    message("    >> Create MacOSx bundle with QT path set to ${QT_LIB_PATH}/..")
    set_target_properties(${JVX_TARGET_NAME} PROPERTIES INSTALL_RPATH "${QT_LIB_PATH}/..")
    install(TARGETS ${JVX_TARGET_NAME} DESTINATION ${JVX_OSX_BUNDLE_HELPER_PATH_APPS} COMPONENT ${INSTALL_COMPONENT})
    # message("L: ${_locN}")
    SET(allLibs "")
    SET(allBLibs "")
    set(allAPluginsFrom "")
    set(allAPluginsTo "")
    set(allAPluginsFix "")
    set(allLibsFrom "")
    set(allLibsTo "")
    set(allLibsFix "")
    set(allQtLibsFrom "")
    set(allQtLibsTo "")
    set(allQtLibsFix "")
	set(allInstalledQtPlugins "")
	set(allInstalledQtPluginsBundle "")

    # message("Collected items: ${JVX_COLLECT_COMPONENTS_MAC_BUNDLE}")

	## Make the runtime directory within the bundle
	message("    > Creating folder ${JVX_OSX_BUNDLE_APP_PATH_RUNTIME} in bundle")
	file(MAKE_DIRECTORY ${JVX_OSX_BUNDLE_APP_PATH_RUNTIME})
	## message(FATAL_ERROR "Stop DBG")

    ## ========================================================
    # Collect all extra libs
    ## ========================================================
    foreach(alib ${JVX_COLLECT_MISC_LIBS_MAC_BUNDLE})
    	SET(allLibsFrom ${allLibsFrom} ${JVX_OSX_BUNDLE_HELPER_PATH}/${INSTALL_PATH_BIN}/lib${alib})
        SET(allLibsTo ${allLibsTo}
       		${INSTALL_PATH_RELEASE_ROOT_MIN_SLASH}${JVX_TARGET_NAME}.app/Contents/Frameworks/lib${alib})
  	SET(allLibsFix ${allLibsFix}
          	${JVX_OSX_BUNDLE_APP_PATH}/Frameworks/lib${alib})
    endforeach()
    install(FILES ${allLibsFrom} DESTINATION ${JVX_OSX_BUNDLE_APP_PATH}/Frameworks)
	
    ## ========================================================
    # Collect all Qt plugins, seems that all plugins are associated with QtGui
    ## ========================================================
     if(JVX_ENABLE_QT)
		
		## Install the plugins to the bundle
		jvx_install_qt_plugins(${QT_INSTALL_PATH} ${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app/Contents/PlugIns "JVX_QT_PLUGINS_DIR_COPY")

		## Collect the plugin libs to run fixup bundle later
		FOREACH(plugindir ${JVX_QT_PLUGINS_DIR_COPY})
				file(GLOB_RECURSE AAA RELATIVE ${QT_INSTALL_PATH}/plugins ${QT_INSTALL_PATH}/plugins/${plugindir}/*.${JVX_SHARED_EXTENSION})
				foreach(a ${AAA})
					  message("Append: ${a}")
					  list(APPEND allInstalledQtPluginsBundle "${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app/Contents/Plugins/${a}")
				endforeach()
		endforeach()
		## message(FATAL_ERROR "HHH ${allInstalledQtPluginsBundle}") 

        #message("    > Collect QT Plugins for Mac OS X Bundle: ${Qt5Gui_PLUGINS}")
		#foreach(plugin ${Qt5Gui_PLUGINS})
		#  get_target_property(_loc ${plugin} LOCATION)
		# get_filename_component(_locN ${_loc} NAME)
		#  SET(allLibs ${allLibs} ${_loc})
		#  SET(allBLibs ${allBLibs} ${JVX_OSX_BUNDLE_APP_PATH}/PlugIns/${_locN})
		#  # message("SET(allBLibs ${allBLibs} ${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app/Contents/PlugIns/${_locN})")
        #endforeach(plugin ${Qt5Gui_PLUGINS})

		#message("   > Copy to location ${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app/Contents/PlugIns: ${allLibs} ")
		#install(FILES ${allLibs} DESTINATION ${JVX_OSX_BUNDLE_APP_PATH}/PlugIns)
 
        ## ========================================================
        # Collect all extra libs
        ## ========================================================
        foreach(alib ${JVX_COLLECT_MISC_QT_LIBS_MAC_BUNDLE})
    		 SET(allQtLibsFrom ${allQtLibsFrom} ${JVX_OSX_BUNDLE_HELPER_PATH}/${INSTALL_PATH_BIN}/lib${alib})
        	 SET(allQtLibsTo ${allQtLibsTo}
       			${INSTALL_PATH_RELEASE_ROOT_MIN_SLASH}${JVX_TARGET_NAME}.app/Contents/Frameworks/lib${alib})
  		 SET(allQtLibsFix ${allQtLibsFix}
          	 	${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app/Contents/Frameworks/lib${alib})
        endforeach()
        install(FILES ${allQtLibsFrom} DESTINATION ${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app/Contents/Frameworks)
    endif()

    # message(" xxx> All qt libs etc: ${allLibs}")
    # message(" xxx> Target path ${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}.app")


    ## ========================================================
    # Collect all components
    ## ========================================================
    if(JVX_MAC_OS_CREATE_BUNDLE_AUDIO_PLUGINS)
	message("    > Collect jvxrt Plugins for Mac OS X Bundle: ${JVX_COLLECT_COMPONENTS_MAC_BUNDLE}")
	foreach(aplugin ${JVX_COLLECT_COMPONENTS_MAC_BUNDLE})
       		SET(allAPluginsFrom ${allAPluginsFrom} ${JVX_OSX_BUNDLE_HELPER_PATH}/${INSTALL_PATH_COMPONENTS_SHARED}/lib${aplugin})
        	# message("SET(allAPluginsFrom ${allAPluginsFrom} ${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_COMPONENTS_SHARED}/lib${aplugin})")
       		SET(allAPluginsTo ${allAPluginsTo}
       			  ${INSTALL_PATH_RELEASE_ROOT_MIN_SLASH}${JVX_TARGET_NAME}.app/Contents/${INSTALL_PATH_COMPONENTS_SHARED}/lib${aplugin})
        	SET(allAPluginsFix ${allAPluginsFix}
          		   ${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app/Contents/${INSTALL_PATH_COMPONENTS_SHARED}/lib${aplugin})
      	endforeach()

      	install(FILES ${allAPluginsFrom} DESTINATION ${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app/Contents/${INSTALL_PATH_COMPONENTS_SHARED})

		  message("   > Copy to location ${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app/Contents/${INSTALL_PATH_COMPONENTS_SHARED} ")
		  ## message(FATAL_ERROR "Stop DBG")
    
    	if(JVX_COLLECT_MAC_BUNDLES)
	    	set(JVX_COLLECT_MAC_BUNDLES "${JVX_COLLECT_MAC_BUNDLES};${JVX_TARGET_NAME}.app" CACHE INTERNAL "all bundles")
    	else()
	    	set(JVX_COLLECT_MAC_BUNDLES "${JVX_TARGET_NAME}.app" CACHE INTERNAL "all bundles")
    	endif()
    endif()
    
    ## ========================================================
    ## Copy all runtime libraries which were build during project BUILD
    ## ========================================================
    install(DIRECTORY ${JVX_OSX_BUNDLE_HELPER_PATH_LIB}
       		      DESTINATION ${JVX_OSX_BUNDLE_HELPER_PATH_APPS}/${JVX_TARGET_NAME}.app/Contents/bin FILES_MATCHING PATTERN "*.dylib")
    message("---------> ${JVX_OSX_BUNDLE_HELPER_PATH_LIB}")

    #install(DIRECTORY ${JVX_OSX_BUNDLE_HELPER_PATH_LIB}
    #  		      DESTINATION ${JVX_OSX_BUNDLE_HELPER_PATH_APPS}/${JVX_TARGET_NAME}.app/Contents/bin FILES_MATCHING PATTERN "*.dylib")
    #message("---------> ${JVX_OSX_BUNDLE_HELPER_PATH_LIB}")
	
    	INSTALL(CODE "set(BU_CHMOD_BUNDLE_ITEMS ON)
                  include(BundleUtilities)
                  fixup_bundle(\"${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app\"
		  \"${allBLibs};${allAPluginsFix};${allQtLibsFix};${allLibsFix};${allInstalledQtPluginsBundle}\"
                  \"${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app/Contents/bin;${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app/Contents/Plugins;${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app/Contents/${INSTALL_PATH_COMPONENTS_SHARED};${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app/Contents/Frameworks\")" COMPONENT Runtime)
else()
	## only for test ## jvx_install_qt_plugins(${QT_INSTALL_PATH} ${JVX_OSX_BUNDLE_HELPER_PATH_APPS}${JVX_TARGET_NAME}.app/Contents/PlugIns)

	if(NOT JVX_DO_NO_INSTALL)
		install(TARGETS ${JVX_TARGET_NAME} DESTINATION ${INSTALL_PATH_BIN} COMPONENT ${INSTALL_COMPONENT})
	endif()
endif()

# The relevant images (if used)
if(LOCAL_IMAGES_SOURCE_DIR)
	message("Copying png files from ${LOCAL_IMAGES_SOURCE_DIR} to ${INSTALL_COMPONENT}")
  install(DIRECTORY ${LOCAL_IMAGES_SOURCE_DIR}/ DESTINATION ${INSTALL_PATH_IMAGES} COMPONENT ${INSTALL_COMPONENT} FILES_MATCHING PATTERN "*.png")
endif()

if(JVX_PREPARE_QT_WORKSPACE_VS)
	if(
	"${CMAKE_GENERATOR}" MATCHES "Visual Studio 17 2022" OR
	"${CMAKE_GENERATOR}" MATCHES "Visual Studio 16 2019" OR 
	"${CMAKE_GENERATOR}" MATCHES "Visual Studio 14 2015" OR  
	"${CMAKE_GENERATOR}" MATCHES "Visual Studio 14 2015 Win64" OR 
	"${CMAKE_GENERATOR}" MATCHES "Visual Studio 11 2012 Win64" OR 
	"${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017 Win64")
		file(TO_NATIVE_PATH "${JVX_REPLACE_ME}" JVX_REPLACE_ME)
		# message("->JVX_REPLACE_ME_COMMAND_ARGS= ${JVX_REPLACE_ME_COMMAND_ARGS}")
		set(JVX_REPLACE_ME_QT_PATH ${QT_INSTALL_PATH})
		set(JVX_REPLACE_ME_COMMAND_ARGS "-platformpluginpath ${JVX_REPLACE_ME_QT_PATH}/plugins/platforms -platformpluginpath ${JVX_REPLACE_ME_QT_PATH}/plugins --jvxdir jvxComponents --config $(TargetName)${JVX_REPLACE_ME_CONFIG_EXT}.jvx --textlog --verbose_dll --textloglev 10")
		if(JVX_REPLACE_ME_COMMAND_ARGS_VS)
			set(JVX_REPLACE_ME_COMMAND_ARGS ${JVX_REPLACE_ME_COMMAND_ARGS_VS})
		endif()
		if(JVX_HOST_WITH_NO_ARGS)
			configure_file(${JVX_VS_TPL_PATH}/theProject.vcxproj.qt.user.direct.in ${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}.vcxproj.user)
		else()
			configure_file(${JVX_VS_TPL_PATH}/theProject.vcxproj.qt.user.in ${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}.vcxproj.user)
		endif()
	endif()
endif()

if(JVX_PREPARE_WORKSPACE_VS)
	if(
	"${CMAKE_GENERATOR}" MATCHES "Visual Studio 17 2022" OR
	"${CMAKE_GENERATOR}" MATCHES "Visual Studio 16 2019" OR 
	"${CMAKE_GENERATOR}" MATCHES "Visual Studio 14 2015" OR  
	"${CMAKE_GENERATOR}" MATCHES "Visual Studio 14 2015 Win64" OR 
	"${CMAKE_GENERATOR}" MATCHES "Visual Studio 11 2012 Win64" OR 
	"${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017 Win64")
		file(TO_NATIVE_PATH "${JVX_REPLACE_ME}" JVX_REPLACE_ME)
		set(JVX_REPLACE_ME_QT_PATH ${QT_INSTALL_PATH})		
		set(JVX_REPLACE_ME_COMMAND_ARGS "--jvxdir jvxComponents --config $(TargetName)${JVX_REPLACE_ME_CONFIG_EXT}.jvx --textlog --verbose_dll --textloglev 10")
		if(JVX_REPLACE_ME_COMMAND_ARGS_VS)
			set(JVX_REPLACE_ME_COMMAND_ARGS ${JVX_REPLACE_ME_COMMAND_ARGS_VS})
		endif()
		# message("->JVX_REPLACE_ME_COMMAND_ARGS= ${JVX_REPLACE_ME_COMMAND_ARGS}")
		configure_file(${JVX_VS_TPL_PATH}/theProject.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}.vcxproj.user)
	endif()
endif()

# The corresponding start script
if(LOCAL_START_SCRIPT)

	message("Installing local start script ${LOCAL_START_SCRIPT}.")
	# This install is only allowed if we are not within SDK build
	if(JVX_RELEASE_SDK)
		message(FATAL_ERROR "Project ${JVX_TARGET_NAME} run to produce application while compiling for SDK release")
	endif()
	if(JVX_CROSS_COMPILE)
	  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${JVX_OS}-cc)
	    install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${JVX_OS}-cc/${LOCAL_START_SCRIPT}${JVX_SCRIPT_EXTENSION} DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT ${INSTALL_COMPONENT})
	    else()
	    install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${JVX_OS}/${LOCAL_START_SCRIPT}${JVX_SCRIPT_EXTENSION} DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT ${INSTALL_COMPONENT})
	    endif()
	  else()
	  install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${JVX_OS}/${LOCAL_START_SCRIPT}${JVX_SCRIPT_EXTENSION} DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT ${INSTALL_COMPONENT})
	  endif()
else()

	if(CONFIGURE_LOCAL_START_SCRIPT)
		# This install is only allowed if we are not within SDK build
		if(JVX_RELEASE_SDK)
			message(FATAL_ERROR "Project ${JVX_TARGET_NAME} run to produce application while compiling for SDK release")
		endif()
		configure_file(${JVX_BASE_ROOT}/software/templates/scripts/start${JVX_SCRIPT_EXTENSION}.${JVX_OS}.in ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION})
		message("Running script to produce local start script ${INSTALL_PATH_SCRIPTS}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION}.")
		install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION} DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT ${INSTALL_COMPONENT})
	endif()

endif()

# We may install a Matlab script in parallel to the executable installation. In most cases, this is the case
if(JVX_USE_PART_MATLAB)
	if(LOCAL_START_SCRIPT_MATLAB)
		message("Configuring file ${LOCAL_START_SCRIPT_MATLAB}${JVX_SCRIPT_EXTENSION}.in for Matlab start script")
		configure_file(${LOCAL_START_SCRIPT_MATLAB}${JVX_SCRIPT_EXTENSION}.in
			${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}-mat${JVX_SCRIPT_EXTENSION})
		install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}-mat${JVX_SCRIPT_EXTENSION}  DESTINATION ${INSTALL_PATH_SCRIPTS})
	endif()
endif()

if(JVX_USE_PART_OCTAVE)
	if(LOCAL_START_SCRIPT_OCTAVE)
		message("Configuring file ${LOCAL_START_SCRIPT_OCTAVE}${JVX_SCRIPT_EXTENSION}.in for Octave start script")
		configure_file(${LOCAL_START_SCRIPT_OCTAVE}${JVX_SCRIPT_EXTENSION}.in
			${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}-oct${JVX_SCRIPT_EXTENSION})
		install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}-oct${JVX_SCRIPT_EXTENSION} DESTINATION ${INSTALL_PATH_SCRIPTS})
	endif()
endif()

if(NOT EXISTS JVX_COLLECT_INSTALL_DIRECTORIES)
	set(JVX_COLLECT_INSTALL_DIRECTORIES FALSE)
endif()
jvx_install_project_folders(${JVX_COLLECT_INSTALL_DIRECTORIES})
