# =============================================================================
# Third party libraries
# =============================================================================

if(${JVX_OS} MATCHES "windows")
  ###
  # build 3rd party libraries
  # FFTW lib is only copied in runtime mode to have the dlls at the right place
  set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS}
    ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/web/fftw/fftw-3.3-win

    ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/speex-dsp/
  )
  if(JVX_USE_PCAP)
    set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS}
      ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/web/npcap)
  endif()

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
  endif()
  if(JVX_USE_QWT_POLAR)
    # set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS} ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/qwtPolar-${JVX_QWT_POLAR_VERSION}/cmake)
  endif()

  # set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS} ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/qcustomplot-wrapper/cmake )
  set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS} ${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/web/qcustomplot)

endif()

if(JVX_USE_PART_PAUDIO)
  if(${JVX_OS} MATCHES "windows" OR ${JVX_OS} MATCHES "macosx")

    # Portaudio is available in the linux system by apt-get
    set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS}
      ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/portaudio
    )
  else()
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
    find_package(portaudio)
  endif()
endif()

if(JVX_USE_PART_CMINPACK)
  set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS}
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/cminpack
  )
endif()

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
  option(LIBMYSOFA_BUILD_TESTS "Build test programs" OFF)
  option(LIBMYSOFA_BUILD_SHARED_LIBS "Build shared library" OFF)
  set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS}
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/mysofa)
endif()

if(JVX_USE_HDF5)
  # This part here only when downloading HDF5	 i windows
  set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS}
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/web/hdf5)
endif()

if(JVX_USE_ORC)
  set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS}
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/orc/)
endif()

if(JVX_USE_KISSFFT)
  # This part here only when downloading HDF5	 i windows
  set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS}
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/kissfft)
endif()

if(JVX_USE_OPENCV)
  set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS}
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/opencv/)
endif()

if(JVX_USE_GLEW_GLUT)
  set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS}
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/web/glew/
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/freeglut/)
endif()

if(JVX_USE_CBMP)
  set(JVX_BASE_3RDPARTY_LIBS ${JVX_BASE_3RDPARTY_LIBS}
    ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/cbmp)
endif()

