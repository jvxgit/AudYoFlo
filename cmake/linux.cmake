header_message("Linux specific configuration")

# platform specific configuration options
set(SED "/bin/sed" CACHE PATH "Path to stream editor (sed) TO BE REMOVED")

# deactivate unsupported stuff
if (JVX_USE_PART_ASIO)
  message("XX> deactivating ASIO support (not supported on Linux)")
  set(JVX_USE_PART_ASIO OFF)
endif()
if (JVX_USE_PART_CORECONTROL)
  message("XX> deactivating corecontrol support (not supported on Linux)")
  set(JVX_USE_PART_CORECONTROL OFF)
endif()
if (JVX_USE_PART_COREAUDIO)
  message("XX> deactivating core audio support (not supported on Linux)")
  set(JVX_USE_PART_COREAUDIO OFF)
endif()

# pre-/suffixes
set(JVX_SHARED_EXTENSION so)
set(JVX_STATIC_EXTENSION a)
set(JVX_SCRIPT_EXTENSION ".sh")
set(JVX_EXECUTABLE_EXTENSION "")

# Gloab disable for shared libs (in case shared libs are not supported)
set(JVX_DISABLE_ALL_SHARED FALSE)

# use pkg-config for libraries that do not bring a cmake-package definition
find_package(PkgConfig)

# Needed to add this to suppress warnings due to ABI modifications
## set(JVX_COMPILE_FLAGS_GENERAL "-Wno-psabi -pg")
set(JVX_COMPILE_FLAGS_GENERAL "")
# Taken from here:
## https://foonathan.net/2018/10/cmake-warnings/
set(JVX_LANGUAGE_SPECIFIC_OPTIONS
  $<$<COMPILE_LANGUAGE:C>:-Wno-psabi>
  $<$<COMPILE_LANGUAGE:CXX>:-Wno-psabi>
)

# Flags for shared libraries

# These flags are ADDITIONAL and depend on the PROJECT type (C or C++ project). There is not
# difference between the TYPE of file if C and C++ are mixed. C/C++ specific flags must be set in CMAKE_CXX_FLAGS/CMAKE_C_FLAGS

# set(JVX_CMAKE_C_FLAGS_SHARED "--std=gnu99 -fPIC -pthread ${JVX_COMPILE_FLAGS_GENERAL}")
# set(JVX_CMAKE_CXX_FLAGS_SHARED "--std=c++11 -fPIC -pthread ${JVX_COMPILE_FLAGS_GENERAL}")

set(JVX_CMAKE_C_FLAGS_SHARED "${JVX_COMPILE_FLAGS_GENERAL}")
set(JVX_CMAKE_CXX_FLAGS_SHARED "${JVX_COMPILE_FLAGS_GENERAL}")
set(JVX_CMAKE_LINKER_FLAGS_SHARED "-Wl,--no-undefined -shared ${JVX_COMPILE_FLAGS_GENERAL}")

set(WHOLE_ARCHIVE "-Wl,--whole-archive")
set(NO_WHOLE_ARCHIVE "-Wl,--no-whole-archive")

if(JVX_GCC_LINKER_SYMBOLIC)
  # The following line to allow -fPIC linkage of ffmpeg
  # https://github.com/microsoft/vcpkg/issues/17292
  set(JVX_CMAKE_LINKER_FLAGS_SHARED "${JVX_CMAKE_LINKER_FLAGS_SHARED} -Wl,-Bsymbolic")
endif(JVX_GCC_LINKER_SYMBOLIC)

# Flags for shared objects with export file list
set(JVX_CMAKE_LINKER_FLAGS_SHARED_EXPORT_COMPONENTS "${JVX_CMAKE_LINKER_FLAGS_SHARED} -Wl,--retain-symbols-file=${JVX_BASE_ROOT}/software/exports/components/linux/exports.def")
set(JVX_CMAKE_LINKER_FLAGS_SHARED_EXPORT_LOCAL "${JVX_CMAKE_LINKER_FLAGS_SHARED} -Wl,--retain-symbols-file=${JVX_BASE_ROOT}/software/exports/components/linux/exports.def")

# Flags for static libraries

# These flags are ADDITIONAL and depend on the PROJECT type (C or C++ project). There is not
# difference between the TYPE of file if C and C++ are mixed. C/C++ specific flags must be set in CMAKE_CXX_FLAGS/CMAKE_C_FLAGS

# set(JVX_CMAKE_C_FLAGS_STATIC "--std=gnu99 -pthread ${JVX_COMPILE_FLAGS_GENERAL}")
# set(JVX_CMAKE_CXX_FLAGS_STATIC "--std=c++11 -pthread ${JVX_COMPILE_FLAGS_GENERAL}")

set(JVX_CMAKE_C_FLAGS_STATIC "${JVX_COMPILE_FLAGS_GENERAL}")
set(JVX_CMAKE_CXX_FLAGS_STATIC "${JVX_COMPILE_FLAGS_GENERAL}")
set(JVX_CMAKE_C_FLAGS_STATIC_PIC "${JVX_CMAKE_C_FLAGS_STATIC}")
set(JVX_CMAKE_CXX_FLAGS_STATIC_PIC "${JVX_CMAKE_CXX_FLAGS_STATIC}")
set(JVX_CMAKE_LINKER_FLAGS_STATIC "")
set(JVX_CMAKE_LINKER_FLAGS_STATIC_PIC "")

# Flags for executables

# These flags are ADDITIONAL and depend on the PROJECT type (C or C++ project). There is not
# difference between the TYPE of file if C and C++ are mixed. C/C++ specific flags must be set in CMAKE_CXX_FLAGS/CMAKE_C_FLAGS

# set(JVX_CMAKE_C_FLAGS_EXEC "--std=gnu99 -fPIC -pthread ${JVX_COMPILE_FLAGS_GENERAL}")
# set(JVX_CMAKE_CXX_FLAGS_EXEC "--std=c++11 -pthread ${JVX_COMPILE_FLAGS_GENERAL}")

set(JVX_CMAKE_C_FLAGS_EXEC "-fPIC -pthread ${JVX_COMPILE_FLAGS_GENERAL}")
set(JVX_CMAKE_CXX_FLAGS_EXEC "-pthread ${JVX_COMPILE_FLAGS_GENERAL}")
set(JVX_CMAKE_LINKER_FLAGS_EXEC "")

# set(CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
# set(CMAKE_CXX_FLAGS "--std=c++14 ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")

if(JVX_FORCE_CPP_14)
  message("--> Forcing build to involve C++14")
  set(CMAKE_CXX_STANDARD 14)
else()
  message("--> Default build involving C++17")
  set(CMAKE_CXX_STANDARD 17)
endif()
#set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(JVX_SYSTEM_LIBRARIES dl pthread m)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wno-psabi")

# It seems that Matlab (2017b) crashes in batch mode due to the "exit" at the end. A pause will help out
set(JVX_PRE_EXIT_MATLAB_HOOK "pause(1);")


###
# macros
###

# configure FFT library
macro (find_fft)
  message("--> Looking for fftw library")

  if(JVX_CROSS_COMPILE)
    message("--> ${CMAKE_SYSROOT}")
    include(FindPkgConfig)
    set(PKG_CONFIG_PATH_OLD "$ENV{PKG_CONFIG_PATH}")
    #set(ENV{PKG_CONFIG_SYSROOT_DIR} "${JVX_LINUX_CROSSCOMPILER_LIB_ROOT}")
    set(ENV{PKG_CONFIG_PATH} "${JVX_PACKAGE_ROOT_PATH}/pkgconfig")
    message("--> ${JVX_PACKAGE_ROOT_PATH}/pkgconfig")
  endif()

  pkg_check_modules(FFTF fftw3f REQUIRED)
  set(FFT_LIBRARYF ${FFTF_LIBRARIES})
  pkg_check_modules(FFT fftw3 REQUIRED)
  set(FFT_LIBRARYD ${FFT_LIBRARIES})
  set(FFT_LIBRARIES "${FFT_LIBRARYD};${FFT_LIBRARYF}")

  if(JVX_CROSS_COMPILE)
    set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH_OLD}")
    set(FFT_LIBDIR "${JVX_PACKAGE_ROOT_PATH}/${FFT_LIBDIR}")
    set(FFT_INCLUDEDIR "${JVX_PACKAGE_ROOT_PATH}/${FFT_INCLUDEDIR}")
  endif()

  set(FFT_COMPILE_DEFINITIONS "")
  message("     include path: ${FFT_INCLUDEDIR}")
  message("     lib path: ${FFT_LIBDIR}")
  message("     compile definitions: ${FFT_COMPILE_DEFINITIONS}")
  message("     lib float: ${FFT_LIBRARYF}")
  message("     lib double: ${FFT_LIBRARYD}")
  message("     lib: ${FFT_LIBRARIES}")
  #  message(FATAL_ERROR ${FFT_LIBRARYD})
endmacro (find_fft)

# Matlab path specifications
macro (find_matlab)
  set(MATLAB_ADDITIONAL_VERSIONS "R2017a=9.2;R2018a=9.4")

  find_package(Matlab COMPONENTS MAIN_PROGRAM)
  if(JVX_PLATFORM MATCHES "32bit")
    message("--> Looking for Matlab 32")
    set(MATLAB_PATH_32 ${Matlab_ROOT_DIR} CACHE PATH "Path to Matlab installation (32bit)")
    set(MATLAB_PATH ${MATLAB_PATH_32})
    if(IS_DIRECTORY "${MATLAB_PATH}")
      message("--> Matlab: ${MATLAB_PATH}")
      set(MATLAB_LIB_SUBDIR "glnx86")
      set(MATLAB_MEX_SUFFIX ".mexglx")
      set(MATLAB_MEX_SUBDIR "glnx86")
      set(Matlab_INCLUDE_DIRS ${MATLAB_PATH}/extern/include)
      set(MATLAB_PATH_LIB ${MATLAB_PATH}/bin/${MATLAB_MEX_SUBDIR})
      set(JVX_SYSTEM_MATLAB_MEX_LIBRARIES libmx.so libmat.so libmex.so)
    else(IS_DIRECTORY "${MATLAB_PATH}")
      message("XX> could not find Matlab, deactivating support")
      set(JVX_USE_PART_MATLAB OFF)
    endif(IS_DIRECTORY "${MATLAB_PATH}")
  else()
    message("--> Looking for Matlab 64")
    set(MATLAB_PATH_64 ${Matlab_ROOT_DIR} CACHE PATH "Path to Matlab installation (64bit)")
    set(MATLAB_PATH ${MATLAB_PATH_64})
    # message(FATAL_ERROR "${MATLAB_PATH} -- ${Matlab_ROOT_DIR}")
    if(IS_DIRECTORY "${MATLAB_PATH}")
      message("--> Matlab: ${MATLAB_PATH}")
      set(MATLAB_LIB_SUBDIR "glnxa64")
      set(MATLAB_MEX_SUFFIX ".mexa64")
      set(MATLAB_MEX_SUBDIR "glnxa64")
      set(Matlab_INCLUDE_DIRS ${MATLAB_PATH}/extern/include)
      set(MATLAB_PATH_LIB ${MATLAB_PATH}/bin/${MATLAB_MEX_SUBDIR})
      set(JVX_SYSTEM_MATLAB_MEX_LIBRARIES "${MATLAB_PATH_LIB}/libmex.so;${MATLAB_PATH_LIB}/libmx.so;${MATLAB_PATH_LIB}/libmat.so" )
      set(JVX_CMAKE_LINKER_FLAGS_MATLAB_MEX "-Wl,--no-undefined -Wl,--retain-symbols-file=${JVX_BASE_ROOT}/software/exports/components/linux/exports-mex.def -Wl,-rpath-link,${MATLAB_LIB_PATH}")
    else(IS_DIRECTORY "${MATLAB_PATH}")
      message("XX> could not find Matlab, deactivating support")
      set(JVX_USE_PART_MATLAB OFF)
    endif(IS_DIRECTORY "${MATLAB_PATH}")
  endif()
endmacro (find_matlab)

# Matlab path specifications
#macro (find_matlab)
#  if(NOT DEFINED MATLAB_PATH)
#    exec_program("matlab -n |grep  'MATLAB .*= .*'|cut -f 2 -d '='|cut -f 2 -d ' '" OUTPUT_VARIABLE MATLAB_PATH)
#  endif()
#  if(IS_DIRECTORY  "${MATLAB_PATH}")
#    message("--> found matlab at ${MATLAB_PATH}")
#    if(JVX_PLATFORM MATCHES "32bit")
#      Set(MATLAB_LIB_SUBDIR "glnx86")
#      set(MATLAB_MEX_SUFFIX ".mexglx")
#    else()
#      set(MATLAB_LIB_SUBDIR "glnxa64")
#      set(MATLAB_MEX_SUFFIX ".mexa64")
#    endif()
#    set(Matlab_INCLUDE_DIRS ${MATLAB_PATH}/extern/include)
#    set(MATLAB_PATH_LIB ${MATLAB_PATH}/bin/${MATLAB_LIB_SUBDIR})
#    set(JVX_SYSTEM_MATLAB_MEX_LIBRARIES mx mat mex)
#    set(JVX_CXX_FLAGS_MEX_MATLAB "-D__STDC_UTF_16__ -pthread")
#    set(JVX_CMAKE_LINKER_FLAGS_MEX "-Wl,--no-undefined -Wl,--retain-symbols-file=${JVX_BASE_ROOT}/software/exports/components/linux/exports-mex.def -Wl,-rpath-link,${MATLAB_LIB_PATH}")
#  else(IS_DIRECTORY  "${MATLAB_PATH}")
#    message("XX> could not find Matlab, deactivating support")
#    set(JVX_USE_PART_MATLAB OFF)
#  endif(IS_DIRECTORY  "${MATLAB_PATH}")
#endmacro (find_matlab)

# Octave path specifications
macro (find_octave)
  exec_program("octave-config" ARGS "-v" OUTPUT_VARIABLE OCTAVE_VERSION)
  exec_program("octave-config" ARGS "-p  BINDIR" OUTPUT_VARIABLE OCTAVE_PATH)
  if(IS_DIRECTORY "${OCTAVE_PATH}")
    message("--> found octave-${OCTAVE_VERSION}")
    if(${OCTAVE_VERSION} STRLESS 4.0.0)
      message(FATAL_ERROR "Octave must be at least version 4")
    endif()
    exec_program("octave-config" ARGS "-p OCTINCLUDEDIR" OUTPUT_VARIABLE Octave_INCLUDE_DIRS)
    exec_program("octave-config" ARGS "-p OCTLIBDIR" OUTPUT_VARIABLE OCTAVE_PATH_LIB)
    if(EXISTS ${OCTAVE_PATH}/octave-cli)
      set(OCTAVE ${OCTAVE_PATH}/octave-cli)
    else()
      set(OCTAVE ${OCTAVE_PATH}/octave)
    endif()
    set(OCTAVE_MEX_SUFFIX ".mex")
    #set(JVX_CXX_FLAGS_MEX_OCTAVE "-D__STDC_UTF_16__ -pthread")
    set(JVX_SYSTEM_OCTAVE_MEX_LIBRARIES ${OCTAVE_PATH_LIB}/liboctinterp.so ${OCTAVE_PATH_LIB}/liboctave.so)
    set(JVX_CMAKE_LINKER_FLAGS_OCTAVE_MEX "-Wl,--no-undefined -Wl,--retain-symbols-file=${JVX_BASE_ROOT}/software/exports/components/linux/exports-mex.def -Wl,-rpath-link,${OCTAVE_LIB_PATH}")
  else(IS_DIRECTORY "${OCTAVE_PATH}")
    message("XX> could not find Octave, deactivating support")
    set(JVX_USE_PART_OCTAVE OFF)
  endif(IS_DIRECTORY "${OCTAVE_PATH}")
endmacro (find_octave)

macro (find_pcap)
  message("--> Looking for libpcap")
  if(JVX_CROSS_COMPILE)
  else()
    exec_program("pcap-config --cflags | cut -c 3-" OUTPUT_VARIABLE PCAP_INCLUDEDIR)
    exec_program("pcap-config --libs | cut -f 1 -d ' ' | cut -c 3-" OUTPUT_VARIABLE PCAP_LIBDIR)
    exec_program("pcap-config --libs | cut -f 3 -d ' ' | cut -c 3-" OUTPUT_VARIABLE PCAP_LIBRARIES)
    #set(PCAP_LIBRARIES "${PCAP_LIBDIR}/${PCAP_LIBRARIES}")
  endif()

  message("-XX-> ${PCAP_INCLUDEDIR}")
  if(IS_DIRECTORY "${PCAP_INCLUDEDIR}")
    message("     include path: ${PCAP_INCLUDEDIR}")
    message("     library path: ${PCAP_LIBDIR}")
    message("     lib: ${PCAP_LIBRARIES}")
  else()
    message("     libpcap not available")
  endif()
endmacro (find_pcap)



# This macro lets you find executable programs on the host system
# -- identical to find_package for native linux builds
macro (find_host_package)
  if(JVX_CROSS_COMPILE)
    set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
    set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
  endif()
  find_package(${ARGN})
  if(JVX_CROSS_COMPILE)
    set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
    set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
  endif()
endmacro (find_host_package)

macro (find_ccs)
  find_program(CCS_ECLIPSE eclipse PATHS ${JVX_CCS_INSTALL_FOLDER}/eclipse NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_ENVIRONMENT_PATH)

  if(CCS_ECLIPSE)
    set(CCS_FOUND TRUE)
    get_filename_component(var1 ${CCS_ECLIPSE} DIRECTORY)
    get_filename_component(JVX_CCS_INSTALL_FOLDER "${var1}" DIRECTORY)
  else()
    set(CCS_FOUND FALSE)
    message(FATAL_ERROR "    TI CCS not found")
  endif()

  message("--> CCS Installation folder: ${JVX_CCS_INSTALL_FOLDER}")
endmacro (find_ccs)

macro (find_mcuxpresso)
  find_program(MCUXPRESSO mcuxpressoide PATHS ${JVX_MCUXPRESSO_INSTALL_FOLDER}/ide NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_ENVIRONMENT_PATH)

  if(MCUXPRESSO)
    set(MCUXPRESSO_FOUND TRUE)
    get_filename_component(var1 ${MCUXPRESSO} DIRECTORY)
    get_filename_component(JVX_MCUXPRESSO_INSTALL_FOLDER "${var1}" DIRECTORY)
  else()
    set(MCUXPRESSO_FOUND FALSE)
    message(FATAL_ERROR "    NXP MCUXPRESSO not found")
  endif()

  message("--> MCUXPRESSO Installation folder: ${JVX_MCUXPRESSO_INSTALL_FOLDER}")
endmacro (find_mcuxpresso)

macro(find_glut)
  message("--> Looking for libglut")
  find_package(GLUT REQUIRED)
  set(GLUT_INCLUDEDIR ${GLUT_INCLUDE_DIR}/GL)
  message("    include path: ${GLUT_INCLUDEDIR}")
  message("    lib: ${GLUT_LIBRARY}")
endmacro (find_glut)

macro(find_glew)

  message("--> Looking for libglew")

  if(JVX_CROSS_COMPILE)
    set(PKG_CONFIG_PATH_OLD "$ENV{PKG_CONFIG_PATH}")
    set(ENV{PKG_CONFIG_PATH} "${JVX_PACKAGE_ROOT_PATH}/pkgconfig")
  endif()

  pkg_check_modules(GLEW REQUIRED glew)

  if(JVX_CROSS_COMPILE)
    set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH_OLD}")

    set(GLEW_INCLUDEDIR "${JVX_PACKAGE_ROOT_PATH}/${GLEW_INCLUDE_DIRS}")
    # set(GLEW_LIBRARY_DIRS "${JVX_PACKAGE_ROOT_PATH}/${GLEW_LIBRARY_DIRS}")
    set(GLEW_LIBRARIES "${JVX_PACKAGE_ROOT_PATH}/${GLEW_LIBRARIES}")
    set(GLEW_LIBDIR "${JVX_PACKAGE_ROOT_PATH}/${GLEW_LIBDIR}")
  endif()

  message("    include path: ${GLEW_INCLUDEDIR}")
  #message("    libdirs: ${GLEW_LIBRARY_DIRS}")
  message("    library path: ${GLEW_LIBDIR}")
  message("    lib: ${GLEW_LIBRARIES}")

endmacro (find_glew)

macro(find_gl)
  message("--> Looking for opengl")

  if(JVX_CROSS_COMPILE)
    set(PKG_CONFIG_PATH_OLD "$ENV{PKG_CONFIG_PATH}")
    set(ENV{PKG_CONFIG_PATH} "${JVX_PACKAGE_ROOT_PATH}/pkgconfig")
  endif()

  pkg_check_modules(GL REQUIRED gl)

  message("-pkg-config -- FOUND-> ${GL_FOUND}")
  message("-pkg-config -- LIBRARIES-> ${GL_LIBRARIES}")
  message("-pkg-config -- LIBRARY_DIRS-> ${GL_LIBRARY_DIRS}")
  message("-pkg-config -- LDFLAGS-> ${GL_LDFLAGS}")
  message("-pkg-config -- LDFLAGS_OTHER-> ${GL_LDFLAGS_OTHER}")
  message("-pkg-config -- INCLUDE_DIRS-> ${GL_INCLUDE_DIRS}")
  message("-pkg-config -- CFLAGS-> ${GL_CFLAGS}")
  message("-pkg-config -- CFLAGS_OTHER-> ${GL_CFLAGS_OTHER}")
  message("-pkg-config -- VERSION-> ${GL_VERSION}")
  message("-pkg-config -- PREFIX-> ${GL_PREFIX}")
  message("-pkg-config -- INCLUDEDIR-> ${GL_INCLUDEDIR}")
  message("-pkg-config -- LIBDIR-> ${GL_LIBDIR}")

  if(JVX_CROSS_COMPILE)
    set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH_OLD}")

    set(GL_INCLUDEDIR "${JVX_PACKAGE_ROOT_PATH}/${GL_INCLUDE_DIRS}")
    #set(GL_LIBRARY_DIRS "${JVX_PACKAGE_ROOT_PATH}/${GL_LIBRARY_DIRS}")
    set(GL_LIBRARIES "${JVX_PACKAGE_ROOT_PATH}/${GL_LIBRARIES}")
    set(GL_LIBDIR "${JVX_PACKAGE_ROOT_PATH}/${GL_LIBDIR}")
  endif()

  message("    include path: ${GL_INCLUDEDIR}")
  #message("    libdirs: ${GL_LIBRARY_DIRS}")
  message("    libdir: ${GL_LIBDIR}")
  message("    lib: ${GL_LIBRARIES}")
endmacro(find_gl)

macro(find_opencv)
  message("--> Looking for opencv")

  if(JVX_CROSS_COMPILE)
    set(PKG_CONFIG_PATH_OLD "$ENV{PKG_CONFIG_PATH}")
    set(ENV{PKG_CONFIG_PATH} "${JVX_PACKAGE_ROOT_PATH}/pkgconfig")
  endif()

  pkg_check_modules(OPENCV REQUIRED opencv)

  message("-pkg-config -- FOUND-> ${OPENCV_FOUND}")
  message("-pkg-config -- LIBRARIES-> ${OPENCV_LIBRARIES}")
  message("-pkg-config -- LIBRARY_DIRS-> ${OPENCV_LIBRARY_DIRS}")
  message("-pkg-config -- LDFLAGS-> ${OPENCV_LDFLAGS}")
  message("-pkg-config -- LDFLAGS_OTHER-> ${OPENCV_LDFLAGS_OTHER}")
  message("-pkg-config -- INCLUDE_DIRS-> ${OPENCV_INCLUDE_DIRS}")
  message("-pkg-config -- CFLAGS-> ${OPENCV_CFLAGS}")
  message("-pkg-config -- CFLAGS_OTHER-> ${OPENCV_CFLAGS_OTHER}")
  message("-pkg-config -- VERSION-> ${OPENCV_VERSION}")
  message("-pkg-config -- PREFIX-> ${OPENCV_PREFIX}")
  message("-pkg-config -- INCLUDEDIR-> ${OPENCV_INCLUDEDIR}")
  message("-pkg-config -- LIBDIR-> ${OPENCV_LIBDIR}")
  if(JVX_CROSS_COMPILE)
    set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH_OLD}")

    set(OPENCV_INCLUDEDIR "${JVX_PACKAGE_ROOT_PATH}/${OPENCV_INCLUDEDIR}")
    #set(OPENCV_LIBRARY_DIRS "${JVX_PACKAGE_ROOT_PATH}/${OPENCV_LIBRARY_DIRS}")
    set(OPENCV_LIBRARIES "${JVX_PACKAGE_ROOT_PATH}/${OPENCV_LIBRARIES}")
    set(OPENCV_LIBDIR "${JVX_PACKAGE_ROOT_PATH}/${OPENCV_LIBDIR}")
  endif()

  message("    include path: ${OPENCV_INCLUDEDIR}")
  #message("    libdirs: ${OPENCV_LIBRARY_DIRS}")
  message("    libdir: ${OPENCV_LIBDIR}")
  message("    lib: ${OPENCV_LIBRARIES}")
endmacro(find_opencv)

macro(find_all_opengl_x11)

  find_gl()
  find_glut()
  find_glew()

endmacro(find_all_opengl_x11)

macro(find_boost)
  message("--> Boost is one the standard system path and consists of headers only")
  set(BOOST_LIBRARY_PATH "/usr/include")
  set(BOOST_INCLUDEDIR_OSGUESS ${BOOST_LIBRARY_PATH}/boost)
  find_path (BOOST_INCLUDEDIR config.hpp PATHS "${BOOST_INCLUDEDIR_OSGUESS}")

  if(BOOST_INCLUDEDIR)
    set(BOOST_FOUND TRUE)
    set(BOOST_INCLUDEDIR "${BOOST_INCLUDEDIR}/..")
    message("    include path: ${BOOST_INCLUDEDIR}")
  else()
    set(BOOST_FOUND FALSE)
    message(FATAL_ERROR "    lib boost not available, you need to specify the location in environment variable BOOST_LIBRARY_PATH.")
  endif()
endmacro (find_boost)

macro(find_eigen)

  message("--> Looking for Eigen")

  if(JVX_CROSS_COMPILE)
    set(PKG_CONFIG_PATH_OLD "$ENV{PKG_CONFIG_PATH}")
    set(ENV{PKG_CONFIG_PATH} "${JVX_PACKAGE_ROOT_PATH}/pkgconfig")
  endif()

  pkg_check_modules(EIGEN REQUIRED eigen3)
  if(${EIGEN_VERSION} STRLESS 3.3.0)
    message(FATAL_ERROR "EIGEN must be at least version 3.3")
  endif()
  # message("-pkg-config -- FOUND-> ${EIGEN_FOUND}")
  #  message("-pkg-config -- LIBRARIES-> ${EIGEN_LIBRARIES}")
  #  message("-pkg-config -- LIBRARY_DIRS-> ${EIGEN_LIBRARY_DIRS}")
  #  message("-pkg-config -- LDFLAGS-> ${EIGEN_LDFLAGS}")
  #  message("-pkg-config -- LDFLAGS_OTHER-> ${EIGEN_LDFLAGS_OTHER}")
  #  message("-pkg-config -- INCLUDE_DIRS-> ${EIGEN_INCLUDE_DIRS}")
  #  message("-pkg-config -- CFLAGS-> ${EIGEN_CFLAGS}")
  #  message("-pkg-config -- CFLAGS_OTHER-> ${EIGEN_CFLAGS_OTHER}")
  #  message("-pkg-config -- VERSION-> ${EIGEN_VERSION}")
  #  message("-pkg-config -- PREFIX-> ${EIGEN_PREFIX}")
  #  message("-pkg-config -- INCLUDEDIR-> ${EIGEN_INCLUDEDIR}")
  #  message("-pkg-config -- LIBDIR-> ${EIGEN_LIBDIR}")

  set(EIGEN_INCLUDEDIR ${EIGEN_INCLUDE_DIRS})

  if(JVX_CROSS_COMPILE)
    set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH_OLD}")

    set(EIGEN_INCLUDEDIR "${JVX_PACKAGE_ROOT_PATH}/${EIGEN_CFLAGS}")
    #set(EIGEN_LIBRARY_DIRS "${JVX_PACKAGE_ROOT_PATH}/${EIGEN_LIBRARY_DIRS}")
    set(EIGEN_LIBRARIES "${JVX_PACKAGE_ROOT_PATH}/${EIGEN_LIBRARIES}")
    set(EIGEN_LIBDIR "${JVX_PACKAGE_ROOT_PATH}/${EIGEN_LIBDIR}")
  endif()


  message("    include path: ${EIGEN_INCLUDEDIR}")
  #message("    libdirs: ${EIGEN_LIBRARY_DIRS}")
  message("    libdir: ${EIGEN_LIBDIR}")
  message("    lib: ${EIGEN_LIBRARIES}")

endmacro (find_eigen)
