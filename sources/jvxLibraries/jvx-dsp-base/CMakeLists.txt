if(DEFINED JVX_SDK_PATH)
  include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-dsp-base)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

# add a target to generate API documentation with Doxygen
# FIXME: support in common foot for this?
if(DOXYGEN_FOUND AND LATEX_FOUND AND LATEX_DVIPS_FOUND)
  message("--> Running documentation generation in project <${PROJECT_NAME}>.")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
  add_custom_target(jvx-dsp-base-doc ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
    COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen" VERBATIM
    )
endif()

# message(FATAL_ERROR "FFT_INCLUDEDIR = ${FFT_INCLUDEDIR}")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}/generated
  )

# Include path for OS builds - Linux, Windows, Apple
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/common)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_startHere/jvx_startHere.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_misc/jvx_rangecheck.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_misc/jvx_id.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_misc/jvx_printf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_misc/jvx_convert.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_misc/jvx_allocate_fields.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_buffer_estimation/jvx_estimate_buffer_fillheight.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_math/jvx_math_extensions.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_math/jvx_complex.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_windows/jvx_windows.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_circbuffer/jvx_circbuffer.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_resampler/jvx_adaptive_resampler.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_resampler/jvx_resampler_plotter.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_resampler/jvx_sample_by_sample_resampler.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_resampler/jvx_adaptive_resampler_frac.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_resampler/jvx_fixed_resampler.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_signal/jvx_signal.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_matrix/jvx_matrix.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_matrix/jvx_matrix_mult.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_matrix/jvx_matrix_rot.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_quat/jvx_quat.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_vector/jvx_vector.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_profiler/jvx_profiler_data_entry.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_allocators/jvx_allocators.c

  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_conv/jvx_conv.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_conv/jvx_conv.h

  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_cart_polar/jvx_cart_polar.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_cart_polar/jvx_cart_polar.h

  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_resampler/jvx_fixed_resampler.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_matrix/jvx_matrix.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_matrix/jvx_matrix_mult.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_matrix/jvx_matrix_rot.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_quat/jvx_quat.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_vector/jvx_vector.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_windows/jvx_windows.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_math/jvx_complex.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_allocators/jvx_allocators.h

  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_fxlms/jvx_fxlms.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_fxlms/jvx_fxlms.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_fft_tools/jvx_firfft.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_fft_tools/jvx_firfft_cf.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_fft_tools/jvx_firfft_cf_nout.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_fft_tools/jvx_firfft_cf_cvrt.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_fft_tools/jvx_fft_share_global.h
		
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_fft_tools/jvx_firfft_prv.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_fft_tools/jvx_fft_tools.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_fft_tools/jvx_firfft.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_fft_tools/jvx_firfft_cf.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_fft_tools/jvx_firfft_cf_nout.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_fft_tools/jvx_firfft_cf_cvrt.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_fft_tools/jvx_fft_share_global.c

	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_circbuffer/jvx_circbuffer_fft.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_spectrumEstimation/jvx_spectrumEstimation.c
	)

  
if(JVX_FFT_TYPE_NAME MATCHES "fftw" AND JVX_USE_FFTW)

	set(LOCAL_SOURCES ${LOCAL_SOURCES} 
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-fftw/jvx_fft_core.c
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-fftw/jvx_fft_core_typedefs.h
		)
	include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-fftw
		${FFT_INCLUDEDIR}
	)
	set(LOCAL_LIBS ${FFT_LIBRARIES})
	set(LOCAL_COMPILE_DEFINITIONS ${FFT_COMPILE_DEFINITIONS})

elseif(JVX_FFT_TYPE_NAME MATCHES "kissfft" AND JVX_USE_KISSFFT)

	set(LOCAL_SOURCES ${LOCAL_SOURCES} 
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-kissfft/jvx_fft_core.c
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-kissfft/jvx_fft_core_typedefs.h
		)
	
	include_directories(
		${AYF_KISSFFT_INCLUDE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-kissfft
		)
	set(LOCAL_LIBS ${AYF_KISSFFT_LIBRARIES})
	set(LOCAL_COMPILE_DEFINITIONS ${AYF_KISSFFT_COMPILE_FLAGS})
	
elseif(JVX_FFT_TYPE_NAME MATCHES "fftreal" AND JVX_USE_FFTREAL)

	set(LOCAL_SOURCES ${LOCAL_SOURCES} 
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-fftreal/jvx_fft_core.c
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-fftreal/jvx_fft_core_typedefs.h
		)
		
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-fftreal)
	
elseif(JVX_FFT_TYPE_NAME MATCHES "JVX_FFT_APPLE")

	set(LOCAL_SOURCES ${LOCAL_SOURCES} 
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-apple/jvx_fft_core.c
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-apple/jvx_fft_core_typedefs.h
		)
		
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-apple)
	set(LOCAL_COMPILE_DEFINITIONS ${FFT_COMPILE_DEFINITIONS})

else()

	message(FATAL_ERROR "No valid fft implementation specified!")
	
endif()


if(JVX_USE_PART_OCTAVE AND JVX_GENERATE_HEADER_ON_BUILD)
	set(LOCAL_OCTCONF_SCRIPTS
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_startHere/generate_config_header.m
		)
else()
  	message("--> Copying files from pre-generated folder")
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pre-generated/jvx_startHere_octConfig.h DESTINATION
		${CMAKE_CURRENT_BINARY_DIR}/generated)
endif()

# if(JVX_USE_PART_MATLAB)
if(JVX_USE_PART_MATLAB AND JVX_GENERATE_HEADER_ON_BUILD)
	set(LOCAL_MATCONF_SCRIPTS
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_resampler/generate_config_header.m
		)
else()
	message("--> Copying files from pre-generated folder")
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pre-generated/jvx_fx_resampler_design_audio8_2_audio32_octConfig.h DESTINATION
		${CMAKE_CURRENT_BINARY_DIR}/generated)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pre-generated/jvx_fx_resampler_design_audio32_2_audio8_octConfig.h DESTINATION
		${CMAKE_CURRENT_BINARY_DIR}/generated)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pre-generated/jvx_fx_resampler_design_audio48_2_us192_octConfig.h DESTINATION
		${CMAKE_CURRENT_BINARY_DIR}/generated)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pre-generated/jvx_fx_resampler_design_us192_2_audio48_octConfig.h DESTINATION
		${CMAKE_CURRENT_BINARY_DIR}/generated)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pre-generated/jvx_fx_resampler_design_audio16_2_us192_octConfig.h DESTINATION
		${CMAKE_CURRENT_BINARY_DIR}/generated)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pre-generated/jvx_fx_resampler_design_us192_2_audio16_octConfig.h DESTINATION
		${CMAKE_CURRENT_BINARY_DIR}/generated)
endif()

set(BUILD_STATIC TRUE)
set(IS_C_LIB TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(INSTALL_DEDICATED_PCG_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

# message(FATAL_ERROR "Stop Here!")