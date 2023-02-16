header_message("Windows specific configuration")

# platform specific configuration options
set(SED "sed" CACHE PATH "Path to stream editor (sed) TO BE REMOVED")
set(MATLAB_PATH_64 $ENV{MATLAB_PATH_64} CACHE PATH "Path to Matlab installation (64bit)")
set(MATLAB_PATH_32 $ENV{MATLAB_PATH_32} CACHE PATH "Path to Matlab installation (32bit)")
set(OCTAVE_PATH_64 $ENV{OCTAVE_PATH_64} CACHE PATH "Path to Octave installation (64bit)")
set(OCTAVE_PATH_32 $ENV{OCTAVE_PATH_32} CACHE PATH "Path to Octave installation (32bit)")

# Configure Qt
if(NOT QT_INSTALL_PATH)
	set(QT_PATH_64 $ENV{QT_PATH_64} CACHE PATH "Path to QT installation (64bit)")
	set(QT_PATH_32 $ENV{QT_PATH_32} CACHE PATH "Path to QT installation (32bit)")
		
	if(JVX_PLATFORM MATCHES "32bit")
		set(QT_INSTALL_PATH ${QT_PATH_32})
	else()
		set(QT_INSTALL_PATH ${QT_PATH_64})
	endif()
endif()

# deactivate unsupported stuff
if (JVX_USE_PART_ALSA)
  message("XX> deactivating ALSA support (not supported on Windows)")
  set(JVX_USE_PART_ALSA OFF)
endif()
if (JVX_USE_PART_COREAUDIO)
  message("XX> deactivating core audio support (not supported on Windows)")
  set(JVX_USE_PART_COREAUDIO OFF)
endif()

# Set the maximum path length to just less than the allowed 250 due to bug in cmake
set(CMAKE_OBJECT_PATH_MAX 240)

# pre-/suffixes
set(JVX_SHARED_EXTENSION dll)
set(JVX_STATIC_EXTENSION lib)
set(JVX_SCRIPT_EXTENSION ".bat")
set(JVX_EXECUTABLE_EXTENSION ".exe")

# Global disable for shared libs (in case shared libs are not supported)
set(JVX_DISABLE_ALL_SHARED FALSE)

if(JVX_USES_SDK)
	set(JVX_LINKER_DISABLE_WARNING "/IGNORE:4099")
endif()

#if(JVX_SYSTEM_USE_DATA_FORMAT_FLOAT)
#  set(JVX_COMPILER_SPECIFIC_OPTIONS "${JVX_COMPILER_SPECIFIC_OPTIONS} 
#	$<$<CXX_COMPILER_ID:MSVC>:/wd4244> 
#	$<$<CXX_COMPILER_ID:MSVC>:/wd4305>")
#endif()

if(JVX_SYSTEM_USE_DATA_FORMAT_FLOAT)
	# Taken from here:
	## https://foonathan.net/2018/10/cmake-warnings/
	set(JVX_LANGUAGE_SPECIFIC_OPTIONS 
		$<$<COMPILE_LANGUAGE:C>:/wd4244 /wd4305>
		$<$<COMPILE_LANGUAGE:CXX>:/wd4244>
		$<$<COMPILE_LANGUAGE:CXX>:/wd4305>
		)
endif()

# /D_CRT_SECURE_NO_WARNINGS /D_WINSOCK_DEPRECATED_NO_WARNINGS_
set(GLOBAL_COMPILE_DEFINITIONS "${GLOBAL_COMPILE_DEFINITIONS};_CRT_SECURE_NO_WARNINGS;_WINSOCK_DEPRECATED_NO_WARNINGS_")

if(JVX_FORCE_CPP_14)
	set(CMAKE_CXX_STANDARD 14)
else()
	set(CMAKE_CXX_STANDARD 17)
endif()

## https://stackoverflow.com/questions/45957830/gdipluspath-throws-ambiguous-byte-for-cstddef-and-rpcndr-h
set(GLOBAL_COMPILE_DEFINITIONS "${GLOBAL_COMPILE_DEFINITIONS};_HAS_STD_BYTE=0")

set(JVX_SHARED_EXPORT "__declspec(dllexport)")

# Flags for shared libraries
set(JVX_CMAKE_C_FLAGS_SHARED "")
set(JVX_CMAKE_CXX_FLAGS_SHARED "")
set(JVX_CMAKE_LINKER_FLAGS_SHARED "${JVX_LINKER_DISABLE_WARNING}")

# Flags for shared objects with export file list
# set(JVX_CMAKE_LINKER_FLAGS_SHARED_EXPORT_COMPONENTS "/DEF:\"${JVX_BASE_ROOT}/software/exports/components/windows/exports.def\"")
# set(JVX_CMAKE_LINKER_FLAGS_SHARED_EXPORT_LOCAL "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/exports/windows/exports.def\"")
set(JVX_EXPORTS_SHARED_COMPONENT "${JVX_BASE_ROOT}/software/exports/components/${JVX_OS}/exports.def")
set(JVX_EXPORTS_SHARED_LOCAL "${CMAKE_CURRENT_SOURCE_DIR}/exports/${JVX_OS}/exports.def")

# Flags for static libraries
set(JVX_CMAKE_C_FLAGS_STATIC "")
set(JVX_CMAKE_CXX_FLAGS_STATIC "")
set(JVX_CMAKE_C_FLAGS_STATIC_PIC "${JVX_CMAKE_C_FLAGS_STATIC}")
set(JVX_CMAKE_CXX_FLAGS_STATIC_PIC "${JVX_CMAKE_CXX_FLAGS_STATIC}")
set(JVX_CMAKE_LINKER_FLAGS_STATIC "${JVX_LINKER_DISABLE_WARNING}")
set(JVX_CMAKE_LINKER_FLAGS_STATIC_PIC "${JVX_LINKER_DISABLE_WARNING}")

# Flags for executables
set(JVX_CMAKE_C_FLAGS_EXEC "")
set(JVX_CMAKE_CXX_FLAGS_EXEC "")
set(JVX_CMAKE_LINKER_FLAGS_EXEC "${JVX_LINKER_DISABLE_WARNING}")

set(JVX_SYSTEM_LIBRARIES "")
set(JVX_SOCKET_LIBRARIES "ws2_32;wsock32")

if(NOT JVX_USE_CONSOLE_APPS)
	set(JVX_APP_TYPE_TOKEN "WIN32") 
endif()
	
set(JVX_OPENGL_LIBS "Opengl32")
set(GLOBAL_COMPILE_DEFINITIONS "${GLOBAL_COMPILE_DEFINITIONS};QCUSTOMPLOT_USE_OPENGL")
set(GLOBAL_COMPILE_DEFINITIONS "${GLOBAL_COMPILE_DEFINITIONS};NOMINMAX ")

set(JVX_VS_GENERATOR FALSE)
if("${CMAKE_GENERATOR}" MATCHES "Visual Studio 16 2019" OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 17 2022"
	OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017" OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017 Win64" 
	OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 14 2015" OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 14 2015 Win64" 
	OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 11 2012 Win64" OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017 Win64"
	OR "${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017")
	set(JVX_VS_GENERATOR TRUE)
endif()
	
if(JVX_USE_QT)
	if(NOT JVX_COMPILE_BUILDTOOLS)
		if(NOT JVX_RELEASE_SDK AND NOT JVX_USES_SDK)
		
			#install(DIRECTORY ${QT_INSTALL_PATH}/bin/ DESTINATION ${INSTALL_PATH_RUNTIME_LIBS} FILES_MATCHING PATTERN "*.dll")
			# We must be careful to copy only a subset of the QT dlls: It may be that we use another qt lib on the
			# development machine and then forget to copy the dll. In that case, program will run on development machine
			# but will fail at the customers machine
			set(JVX_QT_FILE_EXTENSION "")
			
					set(JVX_QT_FILE_EXTENSION "d")
			
				install(FILES
					${QT_INSTALL_PATH}/bin/Qt5Core${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5Gui${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5Widgets${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5PrintSupport${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5Svg${JVX_QT_FILE_EXTENSION}.dll
					DESTINATION ${INSTALL_PATH_LIB_SHARED}
					CONFIGURATIONS Debug UnstableDebug 
					COMPONENT qtdllsmin)
				
				install(FILES
					${QT_INSTALL_PATH}/bin/libGLESv2${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/libEGL${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5Concurrent${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5OpenGL${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5Gamepad${JVX_QT_FILE_EXTENSION}.dll				
					${QT_INSTALL_PATH}/bin/Qt5Network${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt5Websockets${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DAnimation${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DCore${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DExtras${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DInput${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DLogic${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuick${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickAnimation${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickExtras${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickInput${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickRender${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickScene2D${JVX_QT_FILE_EXTENSION}.dll
					${QT_INSTALL_PATH}/bin/Qt53DRender${JVX_QT_FILE_EXTENSION}.dll				
				DESTINATION ${INSTALL_PATH_LIB_SHARED}
				CONFIGURATIONS Debug UnstableDebug 
				COMPONENT qtdlls)
			
				install(FILES
					${QT_INSTALL_PATH}/bin/Qt5Core.dll
					${QT_INSTALL_PATH}/bin/Qt5Gui.dll
					${QT_INSTALL_PATH}/bin/Qt5Widgets.dll
					${QT_INSTALL_PATH}/bin/Qt5PrintSupport.dll
					${QT_INSTALL_PATH}/bin/Qt5Svg.dll
					DESTINATION ${INSTALL_PATH_LIB_SHARED}
					CONFIGURATIONS Release UnstableRelease
					COMPONENT qtdllsmin)
				
				install(FILES
					${QT_INSTALL_PATH}/bin/libGLESv2.dll
					${QT_INSTALL_PATH}/bin/libEGL.dll
					${QT_INSTALL_PATH}/bin/Qt5Concurrent.dll
					${QT_INSTALL_PATH}/bin/Qt5OpenGL.dll
					${QT_INSTALL_PATH}/bin/Qt5Gamepad.dll				
					${QT_INSTALL_PATH}/bin/Qt5Network.dll
					${QT_INSTALL_PATH}/bin/Qt5Websockets.dll
					${QT_INSTALL_PATH}/bin/Qt53DAnimation.dll
					${QT_INSTALL_PATH}/bin/Qt53DCore.dll
					${QT_INSTALL_PATH}/bin/Qt53DExtras.dll
					${QT_INSTALL_PATH}/bin/Qt53DInput.dll
					${QT_INSTALL_PATH}/bin/Qt53DLogic.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuick.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickAnimation.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickExtras.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickInput.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickRender.dll
					${QT_INSTALL_PATH}/bin/Qt53DQuickScene2D.dll
					${QT_INSTALL_PATH}/bin/Qt53DRender.dll				
				DESTINATION ${INSTALL_PATH_LIB_SHARED}
				CONFIGURATIONS Release UnstableRelease
				COMPONENT qtdlls)
			#install(FILES
			#	${QT_INSTALL_PATH}/plugins/platforms/qminimald.dll
			#	${QT_INSTALL_PATH}/plugins/platforms/qoffscreend.dll
			#	${QT_INSTALL_PATH}/plugins/platforms/qwindowsd.dll
			#	DESTINATION ${INSTALL_PATH_BIN}/platforms
			#	CONFIGURATIONS Debug UnstableDebug)

			#install(FILES
			#	${QT_INSTALL_PATH}/plugins/platforms/qminimal.dll
			#	${QT_INSTALL_PATH}/plugins/platforms/qoffscreen.dll
			#	${QT_INSTALL_PATH}/plugins/platforms/qwindows.dll
			#	DESTINATION ${INSTALL_PATH_BIN}/platforms
			#	CONFIGURATIONS Release UnstableRelease)
				
			#
			set(JVX_QT_PLUGINS_DIR_COPY "platforms;styles;imageformats")
			jvx_install_qt_plugins(${QT_INSTALL_PATH} ${INSTALL_PATH_BIN}/plugins "JVX_QT_PLUGINS_DIR_COPY")			
		endif()
	endif()
endif()

###
# macros
###

include(${JVX_CMAKE_DIR}/install-libs.cmake)

# configure FFT library
macro (find_fft)
  set(FFTW_VERSION "3.3")
  set(FFTW_VERSION_LIB "3-3")
  set(FFTWF_VERSION_LIB "3f-3")
  if(JVX_PLATFORM MATCHES "32bit")
	set(FFTW_INCLUDE_PATH_OSGUESS_PREFIX third_party/web/fftw/fftw-${FFTW_VERSION}-win/Win32)
    set(FFTW_INCLUDE_PATH_OSGUESS ${JVX_BASE_LIBS_INCLUDE_PATH}/${FFTW_INCLUDE_PATH_OSGUESS_PREFIX})
    set(FFTW_LIB_PATH_OSGUESS ${JVX_BASE_LIBS_LIB_PATH}/${FFTW_INCLUDE_PATH_OSGUESS_PREFIX})
  else()
	set(FFTW_INCLUDE_PATH_OSGUESS_PREFIX third_party/web/fftw/fftw-${FFTW_VERSION}-win/x64)
    set(FFTW_INCLUDE_PATH_OSGUESS ${JVX_BASE_LIBS_INCLUDE_PATH}/${FFTW_INCLUDE_PATH_OSGUESS_PREFIX})
    set(FFTW_LIB_PATH_OSGUESS ${JVX_BASE_LIBS_LIB_PATH}/${FFTW_INCLUDE_PATH_OSGUESS_PREFIX})
  endif()
  #set(FFTW_INCLUDE_PATH_OSGUESS "O:/jvx/base/sources/jvxLibraries/third_party/fftw-3.3-win/x64")
  #message("    XXX: ${FFTW_INCLUDE_PATH_OSGUESS}")
  #message(" ${CMAKE_PREFIX_PATH}")
  #message(" ${CMAKE_FRAMEWORK_PATH}")
  set(FFTW_LIBNAME_OS libfftw${FFTW_VERSION_LIB}.lib)
  set(FFTWF_LIBNAME_OS libfftw${FFTWF_VERSION_LIB}.lib)
  message("--> Looking for fftw library")

  # Hint here: cmake in the latest version searches the environment varable entries and replaces <path>/bin by <path>/include to find headers, HK, 06042017
  # find_path (FFT_INCLUDEDIR fftw3.h PATHS "${FFTW_INCLUDE_PATH_OSGUESS}" NO_DEFAULT_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
  find_path (FFT_INCLUDEDIR fftw3.h PATHS "${FFTW_INCLUDE_PATH_OSGUESS}" NO_SYSTEM_ENVIRONMENT_PATH)
  if(FFT_INCLUDEDIR)
    find_library (FFT_LIBRARYD libfftw${FFTW_VERSION_LIB}.lib PATHS "${FFTW_LIB_PATH_OSGUESS}")
	find_library (FFT_LIBRARYF libfftw${FFTWF_VERSION_LIB}.lib PATHS "${FFTW_LIB_PATH_OSGUESS}")
	set(FFT_LIBRARIES ${FFT_LIBRARYD} ${FFT_LIBRARYF}) 
    set(FFT_COMPILE_DEFINITIONS "")
    message("    include path: ${FFT_INCLUDEDIR}")
    message("    compile definitions: ${FFT_COMPILE_DEFINITIONS}")
    message("    lib: ${FFT_LIBRARIES}")
  else()
    message(FATAL_ERROR "could not find fftw library")
  endif()
endmacro (find_fft)

# Matlab path specifications
macro (find_matlab arg1)
  if(JVX_PLATFORM MATCHES "32bit")
    set(MATLAB_PATH ${MATLAB_PATH_32})
  else()
    set(MATLAB_PATH ${MATLAB_PATH_64})
  endif()
  if(IS_DIRECTORY "${MATLAB_PATH}")
    message("--> Matlab: ${MATLAB_PATH}")
    if(JVX_PLATFORM MATCHES "32bit")
      set(MATLAB_LIB_SUBDIR "win32")
      set(MATLAB_MEX_SUFFIX ".mexw32")
      set(MATLAB_MEX_SUBDIR "win32")
	  set(MATLAB_PATH_CONFIG "%MATLAB_PATH_32%")
    else()
      set(MATLAB_LIB_SUBDIR "win64")
      set(MATLAB_MEX_SUFFIX ".mexw64")
      set(MATLAB_MEX_SUBDIR "win64")
	  set(MATLAB_PATH_CONFIG "%MATLAB_PATH_64%")
    endif()
    set(Matlab_INCLUDE_DIRS ${MATLAB_PATH}/extern/include)
    set(MATLAB_PATH_LIB ${MATLAB_PATH}/extern/lib/${MATLAB_MEX_SUBDIR}/microsoft)
    set(JVX_SYSTEM_MATLAB_MEX_LIBRARIES "${MATLAB_PATH_LIB}/libmx.lib" "${MATLAB_PATH_LIB}/libmat.lib" "${MATLAB_PATH_LIB}/libmex.lib")
    set(JVX_CMAKE_LINKER_FLAGS_MATLAB_MEX "\"/DEF:${JVX_BASE_ROOT}/software/exports/components/windows/exports-mex.def\"")
	set(Matlab_MAIN_PROGRAM "${MATLAB_PATH}/bin/matlab.exe")
  else(IS_DIRECTORY "${MATLAB_PATH}")
    if(${arg1} MATCHES "FATAL")
		message(FATAL_ERROR "XX> could not find Matlab, option must be deactivated manually to procede.")
	else()
		message("XX> could not find Matlab, option is deactivated - no Matlab modules will be built.")
	endif()
    set(JVX_USE_PART_MATLAB OFF)
  endif(IS_DIRECTORY "${MATLAB_PATH}")
endmacro (find_matlab)

# Octave path specifications
macro (find_octave arg1)
  if(JVX_PLATFORM MATCHES "32bit")
    set(OCTAVE_PATH ${OCTAVE_PATH_32})
	set(OCTAVE_PATH_CONFIG "${OCTAVE_PATH}/mingw32")
  else()
    set(OCTAVE_PATH ${OCTAVE_PATH_64})
	set(OCTAVE_PATH_CONFIG "${OCTAVE_PATH}/mingw64")	
  endif()
  if(OCTAVE_PATH)
	message("Found Octave path ${OCTAVE_PATH}")
	message("Found Octave config path ${OCTAVE_PATH_CONFIG}")
  endif() 
  exec_program("${OCTAVE_PATH_CONFIG}/bin/octave-config" ARGS "-v" OUTPUT_VARIABLE OCTAVE_VERSION)
  exec_program("${OCTAVE_PATH_CONFIG}/bin/octave-config" ARGS "-p  BINDIR" OUTPUT_VARIABLE OCTAVE_BIN_PATH)
  if(IS_DIRECTORY "${OCTAVE_BIN_PATH}")
    message("--> found octave-${OCTAVE_VERSION}")
    exec_program("${OCTAVE_PATH_CONFIG}/bin/octave-config" ARGS "-p OCTINCLUDEDIR" OUTPUT_VARIABLE OCTAVE_INCLUDE_PATH)
    exec_program("${OCTAVE_PATH_CONFIG}/bin/octave-config" ARGS "-p OCTLIBDIR" OUTPUT_VARIABLE OCTAVE_PATH_LIB)
    if(EXISTS ${OCTAVE_PATH_CONFIG}/bin/octave-cli.exe)
      set(OCTAVE ${OCTAVE_PATH_CONFIG}/bin/octave-cli.exe)
    else()
      set(OCTAVE ${OCTAVE_PATH_CONFIG}/bin/octave.exe)
    endif()
    set(OCTAVE_MEX_SUFFIX ".mex")
    set(JVX_SYSTEM_OCTAVE_MEX_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/octinterp.lib" "${CMAKE_CURRENT_BINARY_DIR}/octave.lib")
    set(JVX_CMAKE_LINKER_FLAGS_OCTAVE_MEX "\"/DEF:${JVX_BASE_ROOT}/software/exports/components/windows/exports-mex.def\"")

	# Need to skip this path - otherwise it will re-define math.h
	# set(OCTAVE_INCLUDE_PATH "")
	# message(FATAL_ERROR "->${OCTAVE_INCLUDE_PATH}  ${OCTAVE_PATH_LIB}")
	
    # workaround for octave 4
    if(OCTAVE_VERSION VERSION_GREATER "3.9.9")
      message("--> Copying octave libs to build tree (workaround for Octave 4.0.0)")
      exec_program(${CMAKE_COMMAND} ARGS "-E copy \"${OCTAVE_PATH_LIB}/liboctinterp.dll.a\" \"${CMAKE_CURRENT_BINARY_DIR}/octinterp.lib\"")
      exec_program(${CMAKE_COMMAND} ARGS "-E copy \"${OCTAVE_PATH_LIB}/liboctave.dll.a\" \"${CMAKE_CURRENT_BINARY_DIR}/octave.lib\"")
      set(OCTAVE_PATH_LIB ${CMAKE_CURRENT_BINARY_DIR})
    endif()
  else(IS_DIRECTORY "${OCTAVE_BIN_PATH}")
    if(${arg1} MATCHES "FATAL")
		message(FATAL_ERROR "XX> could not find Octave, option must be deactivated manually to procede.")
	else()
		message("XX> could not find Octave, option is deactivated - no Octave modules will be built.")
	endif()
    set(JVX_USE_PART_OCTAVE OFF)
  endif(IS_DIRECTORY "${OCTAVE_BIN_PATH}")
endmacro (find_octave)

# Configure PCAP Library

macro(find_pcap arg1)
  message("--> Looking for libpcap")
  if(NOT PCAP_LIBRARY_PATH)
	set(PCAP_LIBRARY_PATH $ENV{PCAP_LIBRARY_PATH})
  endif()
  if(JVX_PLATFORM MATCHES "32bit")
    set(PCAP_INCLUDEDIR_OSGUESS ${PCAP_LIBRARY_PATH}/Include)
    set(PCAP_LIB_PATH_OSGUESS ${PCAP_LIBRARY_PATH}/Lib)
  else()
    set(PCAP_INCLUDEDIR_OSGUESS ${PCAP_LIBRARY_PATH}/Include)
    set(PCAP_LIB_PATH_OSGUESS ${PCAP_LIBRARY_PATH}/Lib/x64)
  endif()
  set(PCAP_LIBNAME_OS "wpcap")
  set(PCAPPA_LIBNAME_OS "packet")
  find_path (PCAP_INCLUDEDIR pcap.h PATHS "${PCAP_INCLUDEDIR_OSGUESS}")

  if(PCAP_INCLUDEDIR)
    set(PCAP_FOUND TRUE)
    find_library (PCAP_LIBRARIES NAMES ${PCAP_LIBNAME_OS} PATHS "${PCAP_LIB_PATH_OSGUESS}")
    find_library (PCAPPA_LIBRARIES NAMES ${PCAPPA_LIBNAME_OS} PATHS "${PCAP_LIB_PATH_OSGUESS}")
    message("    include path: ${PCAP_INCLUDEDIR}")
    message("    lib: ${PCAP_LIBRARIES}")
    message("    lib: ${PCAPPA_LIBRARIES}")
    add_library(${PCAP_LIBNAME_OS} SHARED IMPORTED)
    add_library(${PCAPPA_LIBNAME_OS} SHARED IMPORTED)
  else()
    set(PCAP_FOUND FALSE)
	if(${arg1} MATCHES "FATAL")
		message(FATAL_ERROR "XX> could not find PCAP, option must be deactivated manually to procede.")
	else()
		message("XX> could not find PCAP, option is deactivated - no PCAP modules will be built.")
	endif()
	set(JVX_USE_PCAP OFF)
  endif()
endmacro (find_pcap)

# This macro lets you find executable programs on the host system
# -- identical to find_package for native windows builds
macro (find_host_package)
  find_package(${ARGN})
endmacro (find_host_package)

macro (find_ccs)

	if(NOT DEFINED JVX_CCS_INSTALL_FOLDER)
		set(JVX_CCS_INSTALL_FOLDER "C:/develop/ti/ccsv6")
	endif()

	if(DEFINED ENV{CCS_INSTALL_PATH})
		set(JVX_CCS_INSTALL_FOLDER $ENV{CCS_INSTALL_PATH})
	endif()

	find_program(CCS_ECLIPSE eclipsec PATHS ${JVX_CCS_INSTALL_FOLDER}/eclipse NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_ENVIRONMENT_PATH)

	if(CCS_ECLIPSE)
		set(CCS_FOUND TRUE)
		get_filename_component(var1 ${CCS_ECLIPSE} DIRECTORY)
		get_filename_component(JVX_CCS_INSTALL_FOLDER "${var1}" DIRECTORY)
	else()
		set(CCS_FOUND FALSE)
		message(FATAL_ERROR "    TI ccs not found")
	endif()

	message("--> CCS Installation folder: ${JVX_CCS_INSTALL_FOLDER}")
endmacro (find_ccs)

macro (find_mcuxpresso)

	if(NOT DEFINED JVX_MCUXPRESSO_INSTALL_FOLDER)
		set(JVX_MCUXPRESSO_INSTALL_FOLDER "C:/nxp/MCUXpressoIDE_10.0.0_344")
	endif()

	if(DEFINED ENV{MCUXPRESSO_INSTALL_PATH})
		set(JVX_MCUXPRESSO_INSTALL_FOLDER $ENV{MCUXPRESSO_INSTALL_PATH})
	endif()

	message("Try to find mcu xpresso in ${JVX_MCUXPRESSO_INSTALL_FOLDER}")
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
  set(GLUT_LIBRARY_PATH $ENV{GLUT_LIBRARY_PATH})
  if(JVX_PLATFORM MATCHES "32bit")
    set(GLUT_INCLUDEDIR_OSGUESS ${GLUT_LIBRARY_PATH}/include/GL)
    set(GLUT_LIB_PATH_OSGUESS ${GLUT_LIBRARY_PATH}/lib)
	set(GLUT_LIB_PATH_INSTALL_GUESS ${GLUT_LIBRARY_PATH}/bin)
  else()
    set(GLUT_INCLUDEDIR_OSGUESS ${GLUT_LIBRARY_PATH}/include/GL)
    set(GLUT_LIB_PATH_OSGUESS ${GLUT_LIBRARY_PATH}/lib/x64)
	set(GLUT_LIB_PATH_INSTALL_GUESS ${GLUT_LIBRARY_PATH}/bin/x64)
  endif()
  set(GLUT_LIBNAME_OS "freeglut")
  find_path (GLUT_INCLUDEDIR glut.h PATHS "${GLUT_INCLUDEDIR_OSGUESS}")

  if(GLUT_INCLUDEDIR)
    set(GLUT_FOUND TRUE)
    find_library (GLUT_LIBRARIES NAMES ${GLUT_LIBNAME_OS} PATHS "${GLUT_LIB_PATH_OSGUESS}")
    message("    include path: ${GLUT_INCLUDEDIR}")
    message("    lib: ${GLUT_LIBRARIES}")
    add_library(${GLUT_LIBNAME_OS} SHARED IMPORTED)
	set(GLUT_INSTALL_LIB_NAME ${GLUT_LIB_PATH_INSTALL_GUESS}/freeglut.${JVX_SHARED_EXTENSION})
	string(REGEX REPLACE "\\\\" "/" GLUT_INSTALL_LIB_NAME ${GLUT_INSTALL_LIB_NAME})
  else()
    set(GLUT_FOUND FALSE)
    message(FATAL_ERROR "    lib glut not available, you need to specify the location in environment variable GLUT_LIBRARY_PATH.")
  endif()
endmacro (find_glut)

macro(find_glew)
  message("--> Looking for libglew")
  set(GLEW_LIBRARY_PATH $ENV{GLEW_LIBRARY_PATH})
  if(JVX_PLATFORM MATCHES "32bit")
    set(GLEW_INCLUDEDIR_OSGUESS ${GLEW_LIBRARY_PATH}/include/GL)
    set(GLEW_LIB_PATH_OSGUESS ${GLEW_LIBRARY_PATH}/lib/Release/Win32)
	set(GLEW_LIB_PATH_INSTALL_GUESS ${GLEW_LIBRARY_PATH}/bin/Release/Win32)
  else()
    set(GLEW_INCLUDEDIR_OSGUESS ${GLEW_LIBRARY_PATH}/include/GL)
    set(GLEW_LIB_PATH_OSGUESS ${GLEW_LIBRARY_PATH}/lib/Release/x64)
    set(GLEW_LIB_PATH_INSTALL_GUESS ${GLEW_LIBRARY_PATH}/bin/Release/x64)
  endif()
  set(GLEW_LIBNAME_OS "glew32")
  find_path (GLEW_INCLUDEDIR glew.h PATHS "${GLEW_INCLUDEDIR_OSGUESS}")

  if(GLEW_INCLUDEDIR)
    set(GLEW_FOUND TRUE)
    find_library (GLEW_LIBRARIES NAMES ${GLEW_LIBNAME_OS} PATHS "${GLEW_LIB_PATH_OSGUESS}")
    message("    include path: ${GLEW_INCLUDEDIR}")
    message("    lib: ${GLEW_LIBRARIES}")
    add_library(${GLEW_LIBNAME_OS} SHARED IMPORTED)
	set(GLEW_INSTALL_LIB_NAME ${GLEW_LIB_PATH_INSTALL_GUESS}/glew32.${JVX_SHARED_EXTENSION})
	string(REGEX REPLACE "\\\\" "/" GLEW_INSTALL_LIB_NAME ${GLEW_INSTALL_LIB_NAME})
  else()
    set(GLEW_FOUND FALSE)
    message(FATAL_ERROR "    lib glew not available, you need to specify the location in environment variable GLEW_LIBRARY_PATH.")
  endif()
endmacro (find_glew)

macro(find_all_opengl_x11)

	# Open GL is a system wide thing, therefore, we need no directory
	find_package(OpenGL REQUIRED)
	find_glut()
	find_glew()
	
	# We need to point cmake to the opencv lib folder
	#set(OPENCV_LIBRARY_PATH $ENV{OPENCV_LIBRARY_PATH})
	
	## OpenCV currently not used!!
	## set(OpenCV_DIR $ENV{OPENCV_LIBRARY_PATH})
	## message("Looking for opencv in ${OpenCV_DIR}")
	## find_package(OpenCV REQUIRED)
	## message("Open CV include path: ${OpenCV_INCLUDE_DIRS})
	#message("Open CV lib: ${OpenCV_LIBRARY_DIRS})
endmacro(find_all_opengl_x11)

macro(find_pybind)

	
	message("--> Looking for Python/pybind11. You may use the environment variable PYBIND_PATH to link, e.g., PYBIND_PATH=C:\\python-packages\\site-packages\\pybind11\\share\\cmake\\pybind11")
	set(PYBIND_PATH "" CACHE STRING "Pybind find path")
	if(NOT EXISTS ${PYBIND_PATH})
		set(PYBIND_PATH $ENV{PYBIND_PATH})
	endif()
	message("--> Using variable PYBIND_PATH=${PYBIND_PATH}")
	set(pybind11_DIR ${PYBIND_PATH})
	find_package(pybind11 CONFIG REQUIRED)
endmacro(find_pybind)
