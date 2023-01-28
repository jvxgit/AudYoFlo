header_message("Mac OS specific configuration (submodule audio)")


###
# macros
###

# configure sndfile
macro (find_sndfile)
  message("--> Looking for libsndfile")
  set(SNDFILE_INCLUDEDIR_OSGUESS /opt/local/include)
  set(SNDFILE_LIB_PATH_OSGUESS /opt/local/lib)
  set(SNDFILE_LIBNAME_OS libsndfile.dylib)
  find_path (SNDFILE_INCLUDEDIR sndfile.h PATHS "${SNDFILE_INCLUDEDIR_OSGUESS}")
  if(SNDFILE_INCLUDEDIR)
    set(SNDFILE_FOUND TRUE)
    find_library (SNDFILE_LIBRARIES ${SNDFILE_LIBNAME_OS} PATHS "${SNDFILE_LIB_PATH_OSGUESS}")
    message("    include path: ${SNDFILE_INCLUDEDIR}")
    message("    lib: ${SNDFILE_LIBRARIES}")
    add_library(${SNDFILE_LIBNAME_OS} SHARED IMPORTED)
  else()
    set(SNDFILE_FOUND FALSE)
    message("    lib sndfile not available")
  endif()
endmacro (find_sndfile)

macro (find_platform_specific)
  # nothing...
endmacro (find_platform_specific)
