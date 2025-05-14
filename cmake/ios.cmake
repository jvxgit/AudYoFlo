header_message("iOS specific configuration")

# deactivate unsupported stuff
if (JVX_USE_PART_ASIO)
  message("XX> deactivating ASIO support (not supported on iOS)")
  set(JVX_USE_PART_ASIO OFF)
endif()
if (JVX_USE_PART_RS232)
  message("XX> deactivating RS232 support (not supported on iOS)")
  set(JVX_USE_PART_RS232 OFF)
endif()
if (JVX_USE_PART_CORECONTROL)
  message("XX> deactivating corecontrol support (not supported on iOS)")
  set(JVX_USE_PART_CORECONTROL OFF)
endif()
if (JVX_USE_PART_ALSA)
  message("XX> deactivating alsa audio support (not supported on iOS)")
  set(JVX_USE_PART_ALSA OFF)
endif()
if (JVX_USE_PART_ANDROIDAUDIO)
  message("XX> deactivating android audio support (not supported on iOS)")
  set(JVX_USE_PART_ANDROIDAUDIO OFF)
endif()

if (JVX_USE_PART_MATLAB)
  message("XX> deactivating Matlab support (not supported on iOS)")
  set(JVX_USE_PART_MATLAB OFF)
endif()
if (JVX_USE_PART_OCTAVE)
  message("XX> deactivating Octave support (not supported on iOS)")
  set(JVX_USE_PART_OCTAVE OFF)
endif()

set(JVX_CROSS_COMPILE TRUE)

# Find the required mac framework libraries
find_library(ACCELERATE Accelerate)
if (NOT ACCELERATE)
    message(FATAL_ERROR "Accelerate not found")
endif()

find_library(AUDIOUNIT AudioUnit)
if (NOT AUDIOUNIT)
    message(FATAL_ERROR "AudioUnit not found")
endif()

# pre-/suffixes
set(JVX_SHARED_EXTENSION dylib)
set(JVX_STATIC_PREFIX lib)
set(JVX_STATIC_EXTENSION a)
set(JVX_SCRIPT_EXTENSION ".sh")
set(JVX_EXECUTABLE_EXTENSION "")
set(JVX_DISABLE_ALL_SHARED TRUE)


# Flags for shared libraries
set(JVX_CMAKE_C_FLAGS_SHARED "--std=gnu99 -fPIC -pthread -Wno-switch -Wno-deprecated-register -Wno-comment -Wno-unused-function -Wno-unused-variable")
set(JVX_CMAKE_CXX_FLAGS_SHARED "--std=c++0x -fPIC -pthread -Wno-switch -Wno-deprecated-register -Wno-comment -Wno-unused-function -Wno-unused-variable")
set(JVX_CMAKE_LINKER_FLAGS_SHARED "-Wl,-undefined,error")

# Flags for shared objects with export file list
set(JVX_CMAKE_LINKER_FLAGS_SHARED_EXPORT_COMPONENTS "${JVX_CMAKE_LINKER_FLAGS_SHARED} -Wl,-exported_symbols_list,${JVX_BASE_ROOT}/software/exports/components/macosx/exports.def")
set(JVX_CMAKE_LINKER_FLAGS_SHARED_EXPORT_LOCAL "${JVX_CMAKE_LINKER_FLAGS_SHARED} -Wl,-exported_symbols_list,${JVX_BASE_ROOT}/software/exports/components/macosx/exports.def")

# Flags for static libraries
set(JVX_CMAKE_C_FLAGS_STATIC "-DDEBUG=1 --std=gnu99 -Wno-switch -Wno-deprecated-register -Wno-comment -Wno-unused-function -Wno-unused-variable")
set(JVX_CMAKE_CXX_FLAGS_STATIC "-DDEBUG=1 --std=c++0x -Wno-switch -Wno-deprecated-register -Wno-comment -Wno-unused-function -Wno-unused-variable")
set(JVX_CMAKE_C_FLAGS_STATIC_PIC "${JVX_CMAKE_C_FLAGS_STATIC} -fPIC")
set(JVX_CMAKE_CXX_FLAGS_STATIC_PIC "${JVX_CMAKE_CXX_FLAGS_STATIC} -fPIC")
set(JVX_CMAKE_LINKER_FLAGS_STATIC "")
set(JVX_CMAKE_LINKER_FLAGS_STATIC_PIC "")

# Flags for executables
set(JVX_CMAKE_C_FLAGS_EXEC "--std=gnu99 -fPIC -Wno-switch -Wno-deprecated-register -Wno-comment -Wno-unused-function -Wno-unused-variable")
set(JVX_CMAKE_CXX_FLAGS_EXEC "--std=c++0x -Wno-switch -Wno-deprecated-register -Wno-comment -Wno-unused-function -Wno-unused-variable")
set(JVX_CMAKE_LINKER_FLAGS_EXEC "")

set(JVX_SYSTEM_LIBRARIES dl pthread)

###
# macros
###

# configure FFT library
macro (find_fft)
  message("--> Configuring FFT library")
  set(FFT_INCLUDEDIR "")
  set(FFT_COMPILE_DEFINITIONS "JVX_FFT_APPLE")
  set(FFT_LIBRARIES ${ACCELERATE})
  message("    include path: ${FFT_INCLUDEDIR}")
  message("    compile definitions: ${FFT_COMPILE_DEFINITIONS}")
  message("    lib: ${FFT_LIBRARIES}")
endmacro (find_fft)

# This macro lets you find executable programs on the host system
macro (find_host_package)
	set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
	set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
	set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
	set (IOS FALSE)

	find_package(${ARGN})

	set (IOS TRUE)
	set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
	set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endmacro (find_host_package)
