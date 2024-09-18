set(PROJECT_NAME ayfAuNStarter)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  ${SUBPRODUCT_AYFSTARTER_LIBS_INCLUDE_PATH}/ayfstarterlib/include

  # Step IV
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-mex/include
  )

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAuNStarter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAuNStarter.h
  )

# Add the pcg file build steps
set(LOCAL_PCG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/pcg/exports_node.pcg)
set(LOCAL_PCG_FILE_OPTIONS "${AYFSTARTER_SUB_PROJECT_PCG_FLAGS};-I;${SUBPRODUCT_AYFSTARTER_LIBS_INCLUDE_PATH}//ayfstarterlib/pcg") 
# message(FATAL_ERROR "-->  ${AYFSTARTER_SUB_PROJECT_PCG_FLAGS} -- ${LOCAL_PCG_FILE_OPTIONS}")

set(LOCAL_LIBS_COMMON 
	jvx-dsp_static	
	jvx-component-templates-base_static
	)
	
if(AYFSTARTER_USE_RUST_LIBS)
	set(LOCAL_LIBS  ${LOCAL_LIBS_COMMON} ayfstarterlibrs_static)
else()
	set(LOCAL_LIBS  ${LOCAL_LIBS_COMMON} ayfstarterlib_static)
endif()

set(IS_JVX_COMPONENT TRUE)

#================================= Step IV =================================
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS_COPY};JVX_PROJECT_NAMESPACE=AYFSTARTER")
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};${AYFSTARTER_SUB_PROJECT_COMPILE_DEFINITIONS}")
set(LOCAL_COMPILE_DEFINITIONS_COPY "${LOCAL_COMPILE_DEFINITIONS}")

# message(FATAL_ERROR "-->  ${LOCAL_COMPILE_DEFINITIONS}")

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

#================================= Step IV =================================
if(JVX_USE_PART_MATLAB)
	
	# Add all Matlab specific extensions
	set(LOCAL_SOURCES ${LOCAL_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/CayfAuNStarter_mex.cpp)

	# Create a dedicated property header and add develop options
	set(PCGPOSTFIX "m")
	set(LOCAL_PCG_FILE_OPTIONS ${LOCAL_PCG_FILE_OPTIONS} -D JVX_COMPILE_FOR_DEVELOP) 


	# Setup namespaces
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS_COPY};JVX_PROJECT_NAMESPACE=AYFSTARTERM")

	# Run the Matlab property code generator
	if(JVX_MATLAB_PROPERTY_GENERATOR)
		jvx_genMatProperties(${JVX_TARGET_NAME} "JVX_COMPONENT_AUDIO_NODE" "node" "${LOCAL_PCG_FILES}" "${LOCAL_PCG_FILE_OPTIONS}")
	endif()

	# Install Matlab minimal framework
	set(JVX_INSTALL_MATLAB_MINIMAL_FRAMEWORK TRUE)

	## Generate startscript

	# Add a path to start script
	set(JVX_MATLAB_ORIGIN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries/ayfstarter-matlab-testscripts")
	
	# set the origin of the template
	set(LOCAL_START_SCRIPT_MATLAB_SCRIPT_NAME "ayfStarter-mat")
	set(LOCAL_START_SCRIPT_MATLAB ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${JVX_OS}/start_mat)

	set(JVX_MCG_LOCAL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/mcg/exports_project.mcg)

	# Add the version with testpoints
	set(LOCAL_LIBS ${LOCAL_LIBS_COMMON} ayfstarterlibtp_static)

	# Activate the secondary Matlab project
	JVX_ACTIVATE_VERSION_MATLAB(${PROJECT_NAME} JVX_EXTERNAL_CALL_ENABLED)

	
endif()
# message(FATAL_ERROR "Hier")

