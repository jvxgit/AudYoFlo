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

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}/generated
  ${FFT_INCLUDEDIR}
  )

if(${FFT_COMPILE_DEFINITIONS} MATCHES "JVX_FFT_APPLE")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-apple)
else()
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-fftw)
endif()

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

  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_conv/jvx_conv.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_conv/jvx_conv.h

  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_resampler/jvx_fixed_resampler.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_matrix/jvx_matrix.h

  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_windows/jvx_windows.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_math/jvx_complex.h
)

if(JVX_USE_FFTW)
	set(LOCAL_SOURCES ${LOCAL_SOURCES}
		${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_fft_tools/jvx_firfft.h
		${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_fft_tools/jvx_firfft_cf.h

		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_fft_tools/jvx_firfft_prv.h

		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_fft_tools/jvx_fft_tools.c
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_fft_tools/jvx_firfft.c
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_fft_tools/jvx_firfft_cf.c
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_circbuffer/jvx_circbuffer_fft.c
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_spectrumEstimation/jvx_spectrumEstimation.c
		)
endif()
  
if(${FFT_COMPILE_DEFINITIONS} MATCHES "JVX_FFT_APPLE")

set(LOCAL_SOURCES ${LOCAL_SOURCES}
		  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-apple/jvx_fft_core.c)

else()

	if(JVX_USE_FFTW)
		set(LOCAL_SOURCES ${LOCAL_SOURCES}
		  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx-fft-fftw/jvx_fft_core.c)
	endif()

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

set(LOCAL_LIBS ${FFT_LIBRARIES})

set(LOCAL_COMPILE_DEFINITIONS ${FFT_COMPILE_DEFINITIONS})

set(BUILD_STATIC TRUE)
set(IS_C_LIB TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(INSTALL_DEDICATED_PCG_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
