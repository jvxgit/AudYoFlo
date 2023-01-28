	# =============================================================================
	# Third party libraries
	# =============================================================================
		
	if(JVX_USE_EAQUAL)
		set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS}
			${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/eaqual
			${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/eaqual/eaqual/src/libEAQUAL
			${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/eaqual/eaqual/src/libSndFile)
	endif()
	
  if(${JVX_OS} MATCHES "windows")
	###
	# build 3rd party libraries
	# FFTW lib is only copied in runtime mode to have the dlls at the right place
	set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS}
		${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/web/fftw/fftw-3.3-win

		${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/speex-dsp/
	)		
  endif()
	   
   if(JVX_USE_PART_WEBSERVER)
   
	   # Some pre-config settings
	   
	   # Some pre-config settings
	   
	   # Activate web sockets
	   # set(CIVETWEB_ENABLE_WEBSOCKETS TRUE)
	   option(CIVETWEB_ENABLE_WEBSOCKETS "Enable websockets connections" ON)
	   
	   # Define SSL API
	   # set(CIVETWEB_SSL_OPENSSL_API_1_1 TRUE)
	   option(CIVETWEB_SSL_OPENSSL_API_1_1 "Use the OpenSSL 1.1 API" ON)
	   
	   # Deactivate sanitizer feature - I am missing the corresponding dll on my PC
	   # set(CIVETWEB_ENABLE_ASAN FALSE)
	   option(CIVETWEB_ENABLE_ASAN "Enable ASAN in debug mode" OFF)
	   
	   # Deactivate debug output
	   # set(CIVETWEB_ENABLE_DEBUG_TOOLS FALSE)
	   option(CIVETWEB_ENABLE_DEBUG_TOOLS "For Debug builds enable verbose logging and assertions" OFF)
	   	   
	   set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS} 
			${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/git/civetweb)
   endif()
   
  if(JVX_USE_QT)
    if(JVX_USE_QWT)
      # set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS} ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/qwt-${JVX_QWT_VERSION}/cmake)
      set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS} ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/git/qwt)
    endif(JVX_USE_QWT)
    if(JVX_USE_QWT_POLAR)
      # set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS} ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/qwtPolar-${JVX_QWT_POLAR_VERSION}/cmake)
    endif(JVX_USE_QWT_POLAR)

    # set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS} ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/qcustomplot-wrapper/cmake )
	set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS} ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/web/qcustomplot)

  endif()
  
  if(JVX_USE_PART_PAUDIO)
	set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS}
		${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/portaudio
		)
  endif(JVX_USE_PART_PAUDIO)

  if(JVX_USE_PART_CMINPACK)
   set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS}
		${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/cminpack
		)	
  endif(JVX_USE_PART_CMINPACK)

  if(JVX_INCLUDE_EBUR128)
		set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS} 
			${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/ebur128)
endif()

if (JVX_USE_LIBMYSOFA)
	
	# Note: In Windows, the sofa lib only builds if the cmake configuration 
	# is as follows:
	# -DJVX_USE_LIBMYSOFA=TRUE
	# -DLIBMYSOFA_BUILD_TESTS=FALSE
	# OPTIONAL: -DBUILD_SHARED_LIBS=FALSE
	# The author has added the dllexport specification in the function definition.
	# This is not allowed in Visual Studio
	set(LIBMYSOFA_BUILD_TESTS FALSE)
	set(LIBMYSOFA_BUILD_SHARED_LIBS FALSE)
	set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS} 
			${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/mysofa)
endif()

	# =============================================================================
	# =============================================================================
