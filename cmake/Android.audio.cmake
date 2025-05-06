header_message("Linux specific configuration (submodule audio)")

###
# macros
###

# configure sndfile
macro (find_sndfile)
  message("--> Looking for libsndfile")

  if(JVX_CROSS_COMPILE)
    set(PKG_CONFIG_PATH_OLD "$ENV{PKG_CONFIG_PATH}")
    set(ENV{PKG_CONFIG_PATH} "${JVX_PACKAGE_ROOT_PATH}/usr/lib")
  endif()

  pkg_check_modules(SNDFILE sndfile)

  if(JVX_CROSS_COMPILE)
      set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH_OLD}")
      set(SNDFILE_LIBDIR "${JVX_PACKAGE_ROOT_PATH}/${SNDFILE_LIBDIR}")
      set(SNDFILE_INCLUDEDIR "${JVX_PACKAGE_ROOT_PATH}/${SNDFILE_INCLUDEDIR}")

      #find_library (SNDFILEPOST sndfile HINTS ${SNDFILE_LIBDIR})
      #message("FIND ME ${SNDFILEPOST}")
      #set(SNDFILE_LIBRARIES "${SNDFILE_LIBDIR}/libsndfile.so.1.0.25")
      #set(SNDFILE_LIBDIR "")
  endif()

  if(SNDFILE_FOUND)
    message("     include path: ${SNDFILE_INCLUDEDIR}")
    message("     lib path: ${SNDFILE_LIBDIR}")
    message("     lib: ${SNDFILE_LIBRARIES}")
  else()
    message("     libsndfile not available")
  endif()
endmacro (find_sndfile)

# configure alsa
macro (find_platform_specific)

  message("--> Looking for libalsa")

  if(JVX_CROSS_COMPILE)
    set(PKG_CONFIG_PATH_OLD "$ENV{PKG_CONFIG_PATH}")
    set(ENV{PKG_CONFIG_PATH} "${JVX_PACKAGE_ROOT_PATH}/pkgconfig")
  endif()

  pkg_check_modules(ASOUND alsa)

  if(JVX_CROSS_COMPILE)
      set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH_OLD}")

      set(ASOUND_INCLUDEDIR "${JVX_PACKAGE_ROOT_PATH}/${ASOUND_INCLUDEDIR}")
      set(ASOUND_LIBDIR "${JVX_PACKAGE_ROOT_PATH}/${ASOUND_LIBDIR}")

      #find_library (ASOUNDPOST HINTS ${ASOUND_LIBDIR} NAMES ${ASOUND_LIBRARIES}.${CMAKE_SHARED_LIBRARY_SUFFIX})
      #message("FIND ME ${ASOUNDPOST}")
      #set(ASOUND_LIBRARIES ${ASOUND_LIBDIR}/libasound.so.2.0.0)
      #set(ASOUND_LIBDIR "")
  endif()

  if(ASOUND_FOUND)
    message("     include path: ${ASOUND_INCLUDEDIR}")
    message("     lib path: ${ASOUND_LIBDIR}")
    message("     lib: ${ASOUND_LIBRARIES}")
  else()
    message("     libasound not available")
  endif()
endmacro (find_platform_specific)
