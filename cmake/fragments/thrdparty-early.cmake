message("Running code to install third party libs at very early time.")

if(JVX_USE_QT)
  
	if(JVX_INSTALL_QT_ON_BUILD)
		set(JVX_BASE_3RDPARTY_LIBS_EARLY ${JVX_BASE_3RDPARTY_LIBS_EARLY} 
			${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/web/qt)
	endif()
endif()

if(JVX_FLUTTER_UI)
	# Check for invalid Flutter activation on win32 builds
    if(${JVX_OS} MATCHES "windows")
		if(${JVX_PLATFORM} MATCHES "32bit")
			message(FATAL_ERROR "Flutter is not supported for win32! Please deactivate the option <JVX_FLUTTER_UI>.")
		endif()
	endif()
	
	if(JVX_INSTALL_FLUTTER_ON_BUILD)
		set(JVX_BASE_3RDPARTY_LIBS_EARLY ${JVX_BASE_3RDPARTY_LIBS_EARLY} 
			${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/web/flutter)
	endif()
endif()
	
if(JVX_USE_EIGEN)
    # This part here only when downloading EIGEN - find_boost will run in main CMakeLists file
    # This option is true if the instal-libs.cmake file is invluded in the platform cmake file
    set(JVX_BASE_3RDPARTY_LIBS_EARLY ${JVX_BASE_3RDPARTY_LIBS_EARLY}
      ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/eigen)
endif()

if(JVX_USE_BOOST)
    # This part here only when downloading BOOST - find_boost will run in main CMakeLists file
    # This option is true if the instal-libs.cmake file is invluded in the platform cmake file
    set(JVX_BASE_3RDPARTY_LIBS_EARLY ${JVX_BASE_3RDPARTY_LIBS_EARLY}
      ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/web/boost)
endif()

if(JVX_USE_TRACY)
  if(NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message(WARNING "Tracy Profiler should be used with the RelWithDebInfo build profile")
  endif()
endif()

if(${JVX_OS} MATCHES "Android")

	# Download and install fftw libs. this must be in early phase as we need this lib in dsb-base
	set(JVX_BASE_3RDPARTY_LIBS_EARLY ${JVX_BASE_3RDPARTY_LIBS_EARLY}
		${JVX_BASE_ROOT}/sources/jvxLibraries/third_party/web/fftw/fftw-3.3)
endif()

# We download Tracy even if JVX_USE_TRACY is not set so Tracy.hpp is avaible
# with the empty macros. This avoids all the `#ifdef TRACY_ENABLE .. #endif` blocks.
# The library is not built or linked in that case. It's only a cmake interface library
# to set the include directory.
set(JVX_BASE_3RDPARTY_LIBS_EARLY ${JVX_BASE_3RDPARTY_LIBS_EARLY}
  ${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/third_party/git/tracy)
