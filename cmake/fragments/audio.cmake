header_message("Configuring audio submodule")



## ===============================================================================
## We may place the sub-projects in the sub folder or not. The option is:
# if the Subfolder exists we expect the projects there. Otherwise, we will
# look for the sub-projects where the sub midules are!
## ===============================================================================

if(EXISTS ${JVXRT_SUBMODULE_PATH}/sub-projects)
	set(JVXRT_SUBMODULE_PATH ${JVXRT_SUBMODULE_PATH}/sub-projects)
endif()

# set(JVX_SUBPROJECTS_SUBPRODUCT_ROOT_MAP "${JVX_SUBPRODUCT_ROOT_MAP}/sub-projects/")



## ===============================================================================



set(JVX_AUDIOSUBMODULE_LIST "" CACHE STRING "Defined audio submodule list")

# include platform specific stuff
include(${JVX_SUBPRODUCT_ROOT}/cmake/${JVX_OS}.${JVX_PRODUCT}.cmake)
  
# If provided, load submodule list
message("------> ${JVX_BINARY_DIR}/submodulelist.${JVX_PRODUCT}.txt")
if(EXISTS ${JVX_BINARY_DIR}/submodulelist.${JVX_PRODUCT}.txt)
	include(${JVX_BINARY_DIR}/submodulelist.${JVX_PRODUCT}.txt)
	message("##> Submodule list: ${myaudiosubmodules}")
else()
	set(myaudiosubmodules ${JVX_AUDIOSUBMODULE_LIST})
endif()

###
# configure packages specific to rtproc subproduct
###

# configure libsndfile
#find_sndfile()


	













	




# ==========================================================================
# WP Modules are compiled to webassembly (operate in web browser) OR python
# ==========================================================================
set(JVX_RTPROC_WPMODULES_PROJECTS 
		sources/wp-modules/simpletest
		# sources/wp-modules/simpletest2 <- this module is no longer in use. It was used for some kind of testing on Firefox
		)
if(${JVX_OS} MATCHES "Emscripten")
	jvx_addSubdirs("Configuring all audio codec components" "${JVX_RTPROC_WPMODULES_PROJECTS}" "${JVX_FOLDER_HIERARCHIE_BASE}/webassembly")	
else()
	jvx_addSubdirs("Configuring all audio codec components" "${JVX_RTPROC_WPMODULES_PROJECTS}" "${JVX_FOLDER_HIERARCHIE_BASE}/python-projects")	
endif()

# ==========================================================================
# Python only modules
# ==========================================================================
if(JVX_USE_PYTHON)
	##
	# build python functions
	##
	set(JVX_RTPROC_PYTHON_PROJECTS ${JVX_RTPROC_PYTHON_PROJECTS}
		sources/python/jvx-dsp-python
		sources/python/jvx-helper-python
		)
	jvx_addSubdirs("Configuring all python projects" "${JVX_RTPROC_PYTHON_PROJECTS}" "${JVX_FOLDER_HIERARCHIE_BASE}/python-projects")
endif()

	# ==========================================================================
	# ==========================================================================
  # Add the omap l137 packages if desired
  set(JVX_RTPROC_PLATFORM_PACKAGES "")
  if(JVX_RELEASE_OMAPL137)
	set(JVX_RTPROC_PLATFORM_PACKAGES ${JVX_RTPROC_PLATFORM_PACKAGES}
		platform-packages/omapl137/jvx-engine-pkg
	)
  elseif(JVX_RELEASE_C674X)
	set(JVX_RTPROC_PLATFORM_PACKAGES ${JVX_RTPROC_PLATFORM_PACKAGES}
		platform-packages/c674x/cminpack-1
		platform-packages/c674x/jvx-dsp
	)
  elseif(JVX_RELEASE_ARMCORTEXM4)
    set(JVX_RTPROC_PLATFORM_PACKAGES ${JVX_RTPROC_PLATFORM_PACKAGES}
         platform-packages/arm_cortex_m4/jvx-dsp
		 platform-packages/hitex4500/jvx-engine
		 platform-packages/hitex4500/jvx-engine-lib
    )
  endif()
  
  jvx_addSubdirs("Configuring all platform packages" "${JVX_RTPROC_PLATFORM_PACKAGES}" "${JVX_FOLDER_HIERARCHIE_BASE}/platform-packages")

if(JVX_USE_VST)
	if(EXISTS ${JVXRT_SUBMODULE_PATH}/vst3sdk)
		set(SMTG_CREATE_PLUGIN_LINK FALSE)
		set(SMTG_PLUGIN_TARGET_PATH "${JVX_SUBPRODUCT_BINARY_DIR}")
		set(VST_FORCE_PLUGINS_EXAMPLES TRUE)
		set(JVX_VST_SDK_EXAMPLES ${JVXRT_SUBMODULE_PATH}/vst3sdk)
		set(JVX_VST_SDK_EXAMPLES_MAP sub-projects/vst)
		jvx_addExtSubdir("Configuring all VST examples" "${JVX_VST_SDK_EXAMPLES}" "${JVX_VST_SDK_EXAMPLES_MAP}" "${JVX_FOLDER_HIERARCHIE_BASE}/vst-sdk3")

		set(JVX_VST_SDK_ROOT "${JVX_VST_SDK_EXAMPLES}")
		set(JVX_VSTGUI_SDK_ROOT "${JVX_VST_SDK_ROOT}/vstgui4")
	
		set(JVX_VST_SDK_EXTENDER_LIBS 
			sources/jvxLibraries/jvx-audioplugins-extender/jvx-vst-plugins)
		jvx_addSubdirs("Configuring all VST extender libraries" "${JVX_VST_SDK_EXTENDER_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/plugins-extender")
	
		set(JVX_VST_SDK_WIDGET_LIBS 
		sources/jvxLibraries/jvx-audioplugin-widgets/jvx-vst-widgets)
		jvx_addSubdirs("Configuring all VST widget libraries" "${JVX_VST_SDK_WIDGET_LIBS}" "${JVX_FOLDER_HIERARCHIE_BASE}/plugins-widgets")
	else()
		message(FATAL_ERROR "Vst subfolder <${JVXRT_SUBMODULE_PATH}/vst3sdk> does not exist!")
	endif()
endif()

###
# build hosts
###
if(NOT JVX_RELEASE_SDK) # fixme: this should be handled via install components
	
	set(JVX_RTPROC_APPS "")
	
	if(JVX_USE_QT)
		if(JVX_USE_QWT AND JVX_USE_QWT_POLAR AND JVX_COMPILE_GOD_HOST)
			set(JVX_RTPROC_APPS ${JVX_RTPROC_APPS}
				sources/jvxApplications/jvx-qt-host-1
				)
		else(JVX_USE_QWT AND JVX_USE_QWT_POLAR)
			message("XX> host jvx-qt-host-1 disabled (QWT and/or QWT-Polar disabled)")
		endif(JVX_USE_QWT AND JVX_USE_QWT_POLAR)
	else(JVX_USE_QT)

			message("XX> all hosts with qt support disabled")
	endif(JVX_USE_QT)

	# ======================================================
	
	if(JVX_DEPLOY_EXAMPLES)

		set(JVX_RTPROC_APPS ${JVX_RTPROC_APPS}
			sources/jvxApplications/pesqTest
			sources/jvxApplications/sv56Demo
			sources/jvxApplications/jvxSocketTestClient
			sources/jvxLibraries/third_party/EAQUAL/src/EAQUALFrontEnd
			)
	
		if(JVX_USE_QT)
			set(JVX_RTPROC_APPS ${JVX_RTPROC_APPS}
				sources/jvxApplications/jvxRtprocHost
				sources/jvxApplications/jvxRtprocNetHost
				sources/jvxApplications/jvxRtprocHost-1
				sources/jvxApplications/jvxVocoder
				sources/jvxApplications/jvxMyHost
				sources/jvxApplications/jvxAmrWb
				sources/jvxApplications/jvxMeasureIr
				sources/jvxApplications/jvxMeasureIr2Dev
				sources/jvxApplications/ayfHosts/ayfQtAudioHostStart
				)
	

			if(${JVX_OS} MATCHES "windows")
				if(JVX_USE_QWT)
					#set(JVX_RTPROC_APPS ${JVX_RTPROC_APPS} sources/jvxApplications/jvxMerConcept)
				else(JVX_USE_QWT)
					message("XX> host jvxMerConcept disabled (QWT is disabled)")
				endif(JVX_USE_QWT)
			else(${JVX_OS} MATCHES "windows")
				message("XX> host jvxMerConcept disabled (Windows only)")
			endif(${JVX_OS} MATCHES "windows")
		
		else(JVX_USE_QT)

			message("XX> all hosts with qt support disabled")

		endif(JVX_USE_QT)
	endif()

	if(JVX_USE_PART_MATLAB)
		set(JVX_RTPROC_APPS ${JVX_RTPROC_APPS} sources/jvxApplications/jvx-m-host-1/cmake/matlab)
	else(JVX_USE_PART_MATLAB)
		message("XX> matlab host disabled")
	endif(JVX_USE_PART_MATLAB)

	if(JVX_USE_PART_OCTAVE)
		set(JVX_RTPROC_APPS ${JVX_RTPROC_APPS} sources/jvxApplications/jvx-m-host-1/cmake/octave)
	else(JVX_USE_PART_OCTAVE)
		message("XX> octave host disabled")
	endif(JVX_USE_PART_OCTAVE)

	jvx_addSubdirs("Configuring all host components" "${JVX_RTPROC_APPS}" "${JVX_FOLDER_HIERARCHIE_BASE}/apps")
  
	set(JVX_MEX_FOLDER_HIERARCHIE "${JVX_FOLDER_HIERARCHIE_BASE}/apps")
	# add_subdirectory(sources/jvxApplications/mex)

  
else(NOT JVX_RELEASE_SDK)
    message("XX> no audio hosts are built (SDK build) #FIXME: to be handles via install components")
endif(NOT JVX_RELEASE_SDK)

# if(NOT JVX_RELEASE_SDK) # fixme: this should be handled via install components
	
	
# endif()

#if(EXISTS ${JVX_SUBPRODUCT_ROOT}/sub-projects/jvx-myNewProject/.jvxprj)
#  message("XX> Building sub project myNewProject")
#  add_subdirectory(${JVX_SUBPRODUCT_ROOT}/sub-projects/jvx-myNewProject)
#else()
#  message("XX> NOT Building sub project jvx-myNewProject")
#endif()

if(JVX_RELEASE_SDK) # fixme: this should be handled via install components
  install(DIRECTORY ${JVX_SUBPRODUCT_ROOT}/cmake/ DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/cmake)
  install(DIRECTORY ${JVX_SUBPRODUCT_ROOT}/software/codeFragments DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software)
  install(DIRECTORY ${JVX_SUBPRODUCT_ROOT}/software/extensions DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/software)
else()
  install(DIRECTORY ${JVX_SUBPRODUCT_ROOT}/wavs/ DESTINATION ${INSTALL_PATH_RELEASE_ROOT}/wavs)

endif()
