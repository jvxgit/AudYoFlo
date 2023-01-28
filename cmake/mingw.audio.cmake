header_message("Windows specific configuration (submodule audio)")

###
# macros
###

# configure SNDFILE
macro(find_sndfile)
  message("--> Looking for libsndfile")
  if(JVX_PLATFORM MATCHES "32bit")
    set(SNDFILE_INCLUDEDIR_OSGUESS ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/libsndfile-win/Win32)
    set(SNDFILE_LIB_PATH_OSGUESS ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/libsndfile-win/Win32/)
  else()
    set(SNDFILE_INCLUDEDIR_OSGUESS ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/libsndfile-win/x64)
    set(SNDFILE_LIB_PATH_OSGUESS ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/libsndfile-win/x64/)
  endif()
  set(SNDFILE_LIBNAME_OS libsndfile-1)
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
