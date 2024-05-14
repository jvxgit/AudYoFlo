set(JVX_CMAKE_DIR ${JVX_SDK_PATH}/cmake)
	set(JVX_BASE_ROOT ${JVX_SDK_PATH})
	set(JVX_SUBPRODUCT_ROOT ${JVX_SDK_PATH})

	set(JVX_USES_SDK TRUE)
	include(${JVX_CMAKE_DIR}/standalone-cfg.cmake)

	include(${JVX_CMAKE_DIR}/macros.cmake)
	include(${JVX_CMAKE_DIR}/platform.cmake)

	set(JVX_PRODUCE_FOLDER_HIERARCHIE FALSE)

	get_property(JVX_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
	
	# Prepare Visual Studio project
	if("${CMAKE_GENERATOR}" MATCHES "Visual Studio 17 2022"
	OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 16 2019" 
	OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017" OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017 Win64" 
	OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 14 2015" OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 14 2015 Win64" 
	OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 11 2012 Win64" OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017 Win64"
	OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017")
		set_property(GLOBAL PROPERTY USE_FOLDERS ON)
		set(JVX_VS_TPL_PATH ${JVX_SDK_PATH}/misc/visualstudio/vs2015)
		set(JVX_REPLACE_ME "${JVX_SDK_RUNTIME_PATH}")
		set(JVX_PRODUCE_FOLDER_HIERARCHIE TRUE)
	endif()
	set(JVX_FOLDER_HIERARCHIE_BASE "${PROJECT_NAME}")

	# Collect all components for a possible app bundle Mac OS
	### set(JVX_COLLECT_COMPONENTS_MAC_BUNDLE "" CACHE INTERNAL "all components")

	set(INSTALL_PATH_RELEASE_ROOT "${JVX_SDK_PATH}")
	set(INSTALL_PATH_RELEASE_ROOT_MIN_SLASH "$JVX_SDK_PATH}/")

	set(INSTALL_PATH_LIB_STATIC "${INSTALL_PATH_RELEASE_ROOT}/lib")
	set(INSTALL_PATH_INCLUDE_STATIC "${INSTALL_PATH_RELEASE_ROOT}/include")
	set(INSTALL_PATH_COMPONENTS_STATIC "${INSTALL_PATH_RELEASE_ROOT}/lib/jvxComponents-static")
	set(INSTALL_PATH_INCLUDE_COMPONENTS_STATIC "${INSTALL_PATH_RELEASE_ROOT}/include/jvxComponents-static")

	set(INSTALL_PATH_RUNTIME "${JVX_SDK_RUNTIME_PATH}")
	set(INSTALL_PATH_BIN "${JVX_SDK_RUNTIME_PATH}/bin")
	set(INSTALL_PATH_SCRIPTS "${JVX_SDK_RUNTIME_PATH}")
	set(INSTALL_PATH_IMAGES "${JVX_SDK_RUNTIME_PATH}/images")
	set(INSTALL_PATH_LIB_SHARED "${JVX_SDK_RUNTIME_PATH}/lib")
	set(INSTALL_PATH_INCLUDE_SHARED "${JVX_SDK_RUNTIME_PATH}/include")
	set(INSTALL_PATH_COMPONENTS_SHARED "${JVX_SDK_RUNTIME_PATH}/jvxComponents")
	set(INSTALL_PATH_INCLUDE_COMPONENTS_SHARED "${JVX_SDK_RUNTIME_PATH}/include/jvxComponents-shared")
	set(INSTALL_PATH_MATLAB "${JVX_SDK_RUNTIME_PATH}/matlab")
	set(INSTALL_PATH_OCTAVE "${JVX_SDK_RUNTIME_PATH}/octave")

	set(INSTALL_PATH_RELEASE_RUNTIME "${JVX_SDK_RUNTIME_PATH}")
	
	set(JVX_BASE_LIBS_INCLUDE_PATH "${JVX_SDK_PATH}/include")
	set(JVX_BASE_BINARY_INCLUDE_PATH ${JVX_BASE_LIBS_INCLUDE_PATH})
	
	set(JVX_BASE_LIBS_LIB_PATH "${JVX_SDK_PATH}/lib")
	set(JVX_SUBPRODUCT_LIBS_INCLUDE_PATH "${JVX_SDK_PATH}/include")
	set(JVX_SUBPRODUCT_LIBS_LIB_PATH "${JVX_SDK_PATH}/lib")

	set(JVX_OSX_BUNDLE_HELPER_PATH "${JVX_SDK_RUNTIME_PATH}/")
	set(JVX_OSX_BUNDLE_HELPER_PATH_APPS "${JVX_SDK_RUNTIME_PATH}/")
	set(JVX_OSX_BUNDLE_HELPER_PATH_LIB "${JVX_SDK_RUNTIME_PATH}/lib/")
	
	# All header files as copied to the binary folder is copied to this path in SDK build
	set(JVX_SUBPRODUCT_BINARY_INCLUDE_PATH ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH})
	
	set(JVX_BASE_LIBS_MATOCT_GEN_LIB "${JVX_SDK_RUNTIME_PATH}/matlab/m-files")
	
	# common include directories
	include_directories(
		# ${JVX_BASE_ROOT}/software
		# ${JVX_SUBPRODUCT_ROOT}/software
		#${JVX_BASE_ROOT}/software/platform/${JVX_OS}
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-system-min/include/platform/${JVX_OS}
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-system-min/include
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-system-base/include
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-callprot/include
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-system-base/include/platform/${JVX_OS}
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-component-templates-min/include
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvxComponents-static
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-helpers/include
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-helpers-product/include
		${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-json/include/
		## ${CMAKE_CURRENT_BINARY_DIR}/base/sources/jvxLibraries/jvx-dsp-base
	)
  
	# If deploying full functionality source code, it is often required to deactivate
	# the SDP pre-compiled dsp-base lib. In that case, the involved c files can be added
	# locally in the SDK involved project
 
	if(NOT JVX_SUPPRESS_DSP_BASE_DEPENDENCIES)
		include_directories(
			${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base/include/)
	endif()
  
	set(INSTALL_PATH_MATLAB_SUBPROJECT "${INSTALL_PATH_MATLAB}/m-files/${JVX_PRODUCT}Properties")

	set(JVX_SYSTEM_LIBRARIES 
		${JVX_SYSTEM_LIBRARIES} 
		jvx-system-min_static
		jvx-system-base_static
		jvx-callprot_static
		jvx-helpers_static
		jvx-helpers-product_static
		jvx-json_static # jsmn
		jvx-component-templates-min_static
		)

	if(NOT JVX_SUPPRESS_DSP_BASE_DEPENDENCIES)
		set(JVX_SYSTEM_LIBRARIES ${JVX_SYSTEM_LIBRARIES} 
			jvx-dsp-base_static)
	endif()
###
# configure external packages
###

header_message("Configuring external tools")

# we always need an fft library
find_fft()

# configure matlab and/or octave
if(JVX_USE_PART_OCTAVE)
  find_octave("AUTO")
endif()
if(JVX_USE_PART_MATLAB)
  find_matlab("AUTO")
#   find_package(Matlab COMPONENTS MX_LIBRARY)
#   set(MATLAB_MEX_SUFFIX ".mexa64")
#   set(JVX_CXX_FLAGS_MEX_MATLAB "-D__STDC_UTF_16__")
#   message(${Matlab_FOUND})
#   message(${Matlab_ROOT_DIR})
# #  message(${Matlab_MAIN_PROGRAM})
#   message(${Matlab_INCLUDE_DIRS})
#   message(${Matlab_MEX_LIBRARY})
#   message(${Matlab_MX_LIBRARY})
# #  message(${Matlab_ENG_LIBRARY})
#   message(${Matlab_LIBRARIES})
# #  message(${Matlab_MEX_COMPILER})
endif()

if(JVX_USE_QT)
	find_qt()

else()
	message("--> Qt5 not used")

	# Deactivate the depending QT libraries
	set(JVX_USE_QWT FALSE)
	set(JVX_USE_QWT_POLAR FALSE)
endif()

if(JVX_RELEASE_OMAPL137 OR JVX_RELEASE_C674X OR JVX_RELEASE_ARMCORTEXA8)
	find_ccs()
elseif(JVX_RELEASE_ARMCORTEXM4)
	find_mcuxpresso()
endif()

# Extra options for gstreamer - if desired
if(JVX_AUDIO_WITH_GST)
	include(${JVX_CMAKE_DIR}/${JVX_OS}.gst.cmake)
	find_gst()
endif()

	# Add path to place where all the libraries are located
	link_directories(
		${JVX_SDK_PATH}/lib
		${JVX_SDK_PATH}/lib/jvxComponents-static
	)
	if(${JVX_OS} MATCHES "windows")

		# For nmake makefiles, we need to be accurate
		set(JVX_PCG "${JVX_SDK_PATH}/bin/jvxExPropC.exe")
		set(JVX_PCG_MATLAB "${JVX_SDK_PATH}/bin/jvxExPropMat.exe")
		set(JVX_MCG "${JVX_SDK_PATH}/bin/jvxExMatC.exe")
	else()
		set(JVX_PCG "${JVX_SDK_PATH}/bin/jvxExPropC")
		set(JVX_PCG_MATLAB "${JVX_SDK_PATH}/bin/jvxExPropMat")
		set(JVX_MCG "${JVX_SDK_PATH}/bin/jvxExMatC")
	endif()

	include(${JVX_SDK_PATH}/cmake/standalone.${JVX_PRODUCT}.cmake)
	
	

