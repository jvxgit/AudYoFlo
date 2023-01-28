###
# configure external packages
###

header_message("Configuring external tools")

# bison and flex (needed on compiling host)
find_host_package(BISON REQUIRED)
find_host_package(FLEX REQUIRED)

# we always need an fft library
if(JVX_USE_FFTW)
	find_fft()
endif()

# configure matlab and/or octave
if(JVX_USE_PART_OCTAVE)
   find_octave(JVX_FIND_ERR)
endif()

if(JVX_USE_PART_MATLAB)
  find_matlab(JVX_FIND_ERR)
  #   find_package(Matlab COMPONENTS MX_LIBRARY)
  #   set(MATLAB_MEX_SUFFIX ".mexa64")
  #   set(JVX_CXX_FLAGS_MEX_MATLAB "-D__STDC_UTF_16__")
  #   message(${Matlab_FOUND})
  #   message(${Matlab_ROOT_DIR})
  #  message(${Matlab_MAIN_PROGRAM})
  #   message(${Matlab_INCLUDE_DIRS})
  #   message(${Matlab_MEX_LIBRARY})
  #   message(${Matlab_MX_LIBRARY})
  # #  message(${Matlab_ENG_LIBRARY})
  #   message(${Matlab_LIBRARIES})
  # #  message(${Matlab_MEX_COMPILER})
endif()

if(JVX_USE_PYTHON)
	find_pybind()
endif()
