header_message("Mac OS X specific configuration")

# platform specific configuration options
set(SED "/usr/bin/sed" CACHE PATH "Path to stream editor (sed) TO BE REMOVED")

# deactivate unsupported stuff
if (JVX_USE_PART_ASIO)
  message("XX> deactivating ASIO support (not supported on Mac OS X)")
  set(JVX_USE_PART_ASIO OFF)
endif()
if (JVX_USE_PART_RS232)
  message("XX> deactivating RS232 support (not supported on Mac OS X)")
  set(JVX_USE_PART_RS232 OFF)
endif()
if (JVX_USE_PART_CORECONTROL)
  message("XX> deactivating corecontrol support (not supported on Mac OS X)")
  set(JVX_USE_PART_CORECONTROL OFF)
endif()
if (JVX_USE_PART_ALSA)
  message("XX> deactivating alsa audio support (not supported on Mac OS X)")
  set(JVX_USE_PART_ALSA OFF)
endif()

# Find the required core audio libraries
find_library(COREAUDIO CoreAudio)
if (NOT COREAUDIO)
    message(FATAL_ERROR "CoreAudio not found")
endif(NOT COREAUDIO)

find_library(COREAUDIOKIT CoreAudioKit)
if (NOT COREAUDIOKIT)
    message(FATAL_ERROR "CoreAudioKit not found")
endif(NOT COREAUDIOKIT)

find_library(COREFOUNDATION coreFoundation)
if (NOT COREFOUNDATION)
    message(FATAL_ERROR "CoreFoundation not found")
endif(NOT COREFOUNDATION)

find_library(FOUNDATION Foundation)
if (NOT FOUNDATION)
    message(FATAL_ERROR "Foundation not found")
endif(NOT FOUNDATION)

find_library(AVFOUNDATION AVFoundation)
if (NOT AVFOUNDATION)
    message(FATAL_ERROR "AVFoundation not found")
endif(NOT AVFOUNDATION)

find_library(CORESERVICES CoreServices)
if (NOT CORESERVICES)
    message(FATAL_ERROR "CoreServices not found")
endif(NOT CORESERVICES)

find_library(AUDIOUNIT AudioUnit)
if (NOT AUDIOUNIT)
    message(FATAL_ERROR "AudioUnit not found")
endif(NOT AUDIOUNIT)

find_library(AUDIOTOOLBOX AudioToolbox)
if (NOT AUDIOTOOLBOX)
    message(FATAL_ERROR "AudioToolbox not found")
endif(NOT AUDIOTOOLBOX)

find_library(ACCELERATE Accelerate)
if (NOT ACCELERATE)
    message(FATAL_ERROR "Accelerate not found")
endif(NOT ACCELERATE)

# Do not use RPATHs if you want to create deployable bundles
set(CMAKE_MACOSX_RPATH 0)

# pre-/suffixes
set(JVX_SHARED_EXTENSION dylib)
set(JVX_STATIC_EXTENSION a)
set(JVX_SCRIPT_EXTENSION ".sh")
set(JVX_EXECUTABLE_EXTENSION "")

# Global disable for shared libs (in case shared libs are not supported)
set(JVX_DISABLE_ALL_SHARED FALSE)

# Do not print out warnings related to unmatched cases in switch: -Wno-switch
set(JVX_COMPILE_FLAGS_GENERAL "-Wno-switch")

set(JVX_CMAKE_C_FLAGS_SHARED "${JVX_COMPILE_FLAGS_GENERAL}")
set(JVX_CMAKE_CXX_FLAGS_SHARED "${JVX_COMPILE_FLAGS_GENERAL}")
set(JVX_CMAKE_LINKER_FLAGS_SHARED "-Wl,-undefined,error")

# Flags for shared objects with export file list
set(JVX_CMAKE_LINKER_FLAGS_SHARED_EXPORT_COMPONENTS "${JVX_CMAKE_LINKER_FLAGS_SHARED} -Wl,-exported_symbols_list,${JVX_BASE_ROOT}/software/exports/components/macosx/exports.def")
set(JVX_CMAKE_LINKER_FLAGS_SHARED_EXPORT_LOCAL "${JVX_CMAKE_LINKER_FLAGS_SHARED} -Wl,-exported_symbols_list,${JVX_BASE_ROOT}/software/exports/components/macosx/exports.def")

set(JVX_CMAKE_C_FLAGS_STATIC "${JVX_COMPILE_FLAGS_GENERAL}")
set(JVX_CMAKE_CXX_FLAGS_STATIC "${JVX_COMPILE_FLAGS_GENERAL}")
set(JVX_CMAKE_C_FLAGS_STATIC_PIC "${JVX_CMAKE_C_FLAGS_STATIC}")
set(JVX_CMAKE_CXX_FLAGS_STATIC_PIC "${JVX_CMAKE_CXX_FLAGS_STATIC}")
set(JVX_CMAKE_LINKER_FLAGS_STATIC "")
set(JVX_CMAKE_LINKER_FLAGS_STATIC_PIC "")

# Flags for executables
set(JVX_CMAKE_C_FLAGS_EXEC "-fPIC -pthread ${JVX_COMPILE_FLAGS_GENERAL}")
set(JVX_CMAKE_CXX_FLAGS_EXEC "-pthread ${JVX_COMPILE_FLAGS_GENERAL}")
set(JVX_CMAKE_LINKER_FLAGS_EXEC "")

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











# Flags for shared libraries
##set(JVX_CMAKE_C_FLAGS_SHARED "--std=gnu99 -fPIC -pthread -Wno-switch -Wno-deprecated-register -Wno-comment")
##set(JVX_CMAKE_CXX_FLAGS_SHARED "--std=c++17 -fPIC -pthread -Wno-switch -Wno-deprecated-register -Wno-comment")


# Flags for static libraries
##set(JVX_CMAKE_C_FLAGS_STATIC "--std=gnu99 -Wno-switch -Wno-deprecated-register -Wno-comment")
##set(JVX_CMAKE_CXX_FLAGS_STATIC "--std=c++17 -Wno-switch -Wno-deprecated-register -Wno-comment")
##set(JVX_CMAKE_C_FLAGS_STATIC_PIC "${JVX_CMAKE_C_FLAGS_STATIC} -fPIC")
##set(JVX_CMAKE_CXX_FLAGS_STATIC_PIC "${JVX_CMAKE_CXX_FLAGS_STATIC} -fPIC")
##set(JVX_CMAKE_LINKER_FLAGS_STATIC "")
##set(JVX_CMAKE_LINKER_FLAGS_STATIC_PIC "")

# Flags for executables
##set(JVX_CMAKE_C_FLAGS_EXEC "--std=gnu99 -fPIC -Wno-switch -Wno-deprecated-register -Wno-comment")
##set(JVX_CMAKE_CXX_FLAGS_EXEC "--std=c++17 -Wno-switch -Wno-deprecated-register -Wno-comment")
##set(JVX_CMAKE_LINKER_FLAGS_EXEC "")

set(JVX_SYSTEM_LIBRARIES dl pthread)

###
# macros
###

include(${JVX_CMAKE_DIR}/install-libs.cmake)

# configure FFT library
macro (find_fft)
  if(JVX_MACOSX_RFFTW)
    message("--> Looking for rfftw library")

    # configure fftw3
    set(FFTW_VERSION "3.3")
    set(FFTW_VERSION_LIB "3-3")
    set(FFTWF_VERSION_LIB "3f-3")
    set(FFTW_INCLUDE_PATH_OSGUESS /opt/local/include)
    set(FFTW_LIB_PATH_OSGUESS /opt/local/lib)
    set(FFTW_LIBNAME_OS fftw3)
    set(FFTWF_LIBNAME_OS fftw3f)

    find_path (FFT_INCLUDEDIR fftw3.h PATHS "${FFTW_INCLUDE_PATH_OSGUESS}")
    find_library (FFT_LIBRARIES ${FFTW_LIBNAME_OS} PATHS "${FFTW_LIB_PATH_OSGUESS}")
    find_library (FFTWF_LIBRARIES ${FFTWF_LIBNAME_OS} PATHS "${FFTW_LIB_PATH_OSGUESS}")

      set(FFT_COMPILE_DEFINITIONS "")
      message("     include path: ${FFT_INCLUDEDIR}")
      message("     lib path: ${FFT_LIBDIR}")
      message("     compile definitions: ${FFT_COMPILE_DEFINITIONS}")
      message("     lib: ${FFT_LIBRARIES}")
else()
      message("--> Configuring Accelerate FFT library")
      set(FFT_INCLUDEDIR "")
      set(FFT_COMPILE_DEFINITIONS "JVX_FFT_APPLE")
      set(FFT_LIBRARIES ${ACCELERATE})
      message("    include path: ${FFT_INCLUDEDIR}")
      message("    compile definitions: ${FFT_COMPILE_DEFINITIONS}")
      message("    lib: ${FFT_LIBRARIES}")
  endif()
endmacro (find_fft)

# Matlab path specifications
macro (find_matlab)
   set(MATLAB_ADDITIONAL_VERSIONS "R2017a=9.2")

  find_package(Matlab COMPONENTS MAIN_PROGRAM)
  set(MATLAB_PATH_64 ${Matlab_ROOT_DIR} CACHE PATH "Path to Matlab installation (64bit)")
  set(MATLAB_PATH ${MATLAB_PATH_64})
  if(IS_DIRECTORY "${MATLAB_PATH}")
    message("--> Matlab: ${MATLAB_PATH}")
    set(MATLAB_LIB_SUBDIR "maci64")
    set(MATLAB_MEX_SUFFIX ".mexmaci64")
    set(MATLAB_MEX_SUBDIR "maci64")
    set(Matlab_INCLUDE_DIRS ${MATLAB_PATH}/extern/include)
    set(MATLAB_PATH_LIB ${MATLAB_PATH}/bin/${MATLAB_MEX_SUBDIR})
    set(JVX_SYSTEM_MATLAB_MEX_LIBRARIES ${MATLAB_PATH_LIB}/libmx.dylib ${MATLAB_PATH_LIB}/libmat.dylib ${MATLAB_PATH_LIB}/libmex.dylib)
    set(JVX_CMAKE_LINKER_FLAGS_MATLAB_MEX "-Wl,-undefined,error -Wl,-exported_symbols_list,${JVX_BASE_ROOT}/software/exports/components/macosx/exports-mex.def")
  else(IS_DIRECTORY "${MATLAB_PATH}")
    message("XX> could not find Matlab, deactivating support")
    set(JVX_USE_PART_MATLAB OFF)
  endif(IS_DIRECTORY "${MATLAB_PATH}")
endmacro (find_matlab)

# Octave path specifications
macro (find_octave)
  exec_program("octave-config" ARGS "-v" OUTPUT_VARIABLE OCTAVE_VERSION)
  exec_program("octave-config" ARGS "-p  BINDIR" OUTPUT_VARIABLE OCTAVE_PATH)
  if(IS_DIRECTORY "${OCTAVE_PATH}")
    message("--> found octave-${OCTAVE_VERSION}")
    exec_program("octave-config" ARGS "-p OCTINCLUDEDIR" OUTPUT_VARIABLE OCTAVE_INCLUDE_PATH)
    exec_program("octave-config" ARGS "-p OCTLIBDIR" OUTPUT_VARIABLE OCTAVE_PATH_LIB)
    set(OCTAVE ${OCTAVE_PATH}/octave)
    set(OCTAVE_MEX_SUFFIX ".mex")
    set(JVX_SYSTEM_OCTAVE_MEX_LIBRARIES "${OCTAVE_PATH_LIB}/liboctinterp.dylib;${OCTAVE_PATH_LIB}/liboctave.dylib")
    set(JVX_CMAKE_LINKER_FLAGS_OCTAVE_MEX "-Wl,-undefined,error -Wl,-exported_symbols_list,${JVX_BASE_ROOT}/software/exports/components/macosx/exports-mex.def")
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
  set(GLUT_INCLUDEDIR ${GLUT_INCLUDE_DIR})
  message("    include path: ${GLUT_INCLUDEDIR}")
  message("    lib: ${GLUT_LIBRARY}")
endmacro (find_glut)

macro(find_glew)

  message("--> Looking for libglew")

  find_package(GLEW REQUIRED)
  set(GLEW_INCLUDEDIR ${GLEW_INCLUDE_DIRS})
  set(GLEW_LIBRARIES ${GLEW_LIBRARIES})
  message("    include path: ${GLEW_INCLUDEDIR}")
  #message("    libdirs: ${GLEW_LIBRARY_DIRS}")
  message("    library path: ${GLEW_LIBDIR}")
  message("    lib: ${GLEW_LIBRARIES}")

endmacro (find_glew)

macro(find_gl)
    message("--> Looking for opengl")

    find_package(opengl REQUIRED)
    set(GL_INCLUDDIR ${OPENGL_INCLUDE_DIR})
    set(GL_LIBRARIES ${OPENGL_LIBRARIES})
    
  message("    include path: ${GL_INCLUDEDIR}")
  #message("    libdirs: ${GL_LIBRARY_DIRS}")
  message("    libdir: ${GL_LIBDIR}")
  message("    lib: ${GL_LIBRARIES}")
endmacro(find_gl)

macro(find_opencv)
    message("--> Looking for opencv")

    find_package(OPENCV REQUIRED)

    set(OPENCV_INCLUDEDIR ${OPENCV_INCLUDE_DIR})
    set(OPENCV_LIBRARIES ${OPENCV_LIBRARIES})

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

#macro(find_boost)
#  message("--> Boost is one the standard system path and consists of headers only")
#  set(BOOST_LIBRARY_PATH "/usr/local/include")
#  set(BOOST_INCLUDEDIR_OSGUESS ${BOOST_LIBRARY_PATH}/boost)
#  find_path (BOOST_INCLUDEDIR config.hpp PATHS "${BOOST_INCLUDEDIR_OSGUESS}")

 # if(BOOST_INCLUDEDIR)
#    set(BOOST_FOUND TRUE)
#	set(BOOST_INCLUDEDIR "${BOOST_INCLUDEDIR}/..")
#    message("    include path: ${BOOST_INCLUDEDIR}")
 # else()
#    set(BOOST_FOUND FALSE)
#    message(FATAL_ERROR "    lib boost not available, you need to specify the location in environment variable BOOST_LIBRARY_PATH.")
#  endif()
#endmacro (find_boost)

#macro(find_eigen)

#  message("--> Looking for Eigen")
 #   find_package(Eigen3 3.3 REQUIRED NO_MODULE)
 #   set(EIGEN_INCLUDEDIR ${EIGEN3_INCLUDE_DIR})

#  message("    include path: ${EIGEN_INCLUDEDIR}")
  #message("    libdirs: ${EIGEN_LIBRARY_DIRS}")
  #message("    libdir: ${EIGEN_LIBDIR}")
  #message("    lib: ${EIGEN_LIBRARIES}")

#  endmacro (find_eigen)




# macro (find_pcap)
#  message("--> Looking for libpcap")
#  exec_program("pcap-config --libs | cut -f 1 -d ' ' | cut -c 3-" OUTPUT_VARIABLE PCAP_LIBRARIES)
#  message("     lib: ${PCAP_LIBRARIES}")
# endmacro (find_pcap)

# This macro lets you find executable programs on the host system
# -- identical to find_package for native macosx builds
# macro (find_host_package)
#   find_package(${ARGN})
# endmacro (find_host_package)

# Collect all components for a possible app bundle Mac OS
set(JVX_COLLECT_COMPONENTS_MAC_BUNDLE "" CACHE INTERNAL "all components")
set(JVX_COLLECT_MISC_LIBS_MAC_BUNDLE  "" CACHE INTERNAL "all misc libs")
set(JVX_COLLECT_MISC_QT_LIBS_MAC_BUNDLE  "" CACHE INTERNAL "all misc qt libs")
set(JVX_COLLECT_MAC_BUNDLES "" CACHE INTERNAL "all bundles")
