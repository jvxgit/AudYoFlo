cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-dsp)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

configure_file(include/jvx_dsp.h.in ${CMAKE_CURRENT_BINARY_DIR}/jvx_dsp.h)

# add a target to generate API documentation with Doxygen
# FIXME: support in common foot for this?
if(DOXYGEN_FOUND AND LATEX_FOUND AND LATEX_DVIPS_FOUND)
  message("--> Running documentation generation in project <${PROJECT_NAME}>.")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
  add_custom_target(jvx-dsp-doc ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
    COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen" VERBATIM
    )
endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/os-${JVX_OS}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_beampattern/jvx_beampattern.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_powerAlignment/jvx_powerAlignment.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_pdm/jvx_fir_lut.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_pdm/jvx_cic.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_misc/jvx_levinson_durbin.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_limiter/jvx_limiter.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_fft_processing/jvx_fft_framework.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_fft_processing/jvx_fft_processing.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_fft_processing/jvx_fft_framework_prv.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_delay/jvx_delay.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_iir_fb/jvx_iir_fb.c

  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_beampattern/jvx_beampattern.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_powerAlignment/jvx_powerAlignment.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_pdm/jvx_fir_lut.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_pdm/jvx_cic.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_misc/jvx_levinson_durbin.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_limiter/jvx_limiter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_fft_processing/jvx_fft_framework.h
   ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_fft_processing/jvx_fft_processing.h
   ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_delay/jvx_delay.h
   ${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_iir_fb/jvx_iir_fb.h
  )

if(JVX_USE_PART_MATLAB AND JVX_GENERATE_HEADER_ON_BUILD)
	set(LOCAL_MATCONF_SCRIPTS
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_pdm/generate_config_header.m
		${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_speech/generate_config_header.m
		)
else()
	message("--> Copying files from pre-generated folder")
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pre-generated/jvx_irs_p341_16.h DESTINATION
		${CMAKE_CURRENT_BINARY_DIR}/generated)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pre-generated/jvx_irs_p48_16.h DESTINATION
		${CMAKE_CURRENT_BINARY_DIR}/generated)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pre-generated/jvx_irs_p48_8.h DESTINATION
		${CMAKE_CURRENT_BINARY_DIR}/generated)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pre-generated/jvx_fdesign_II_octConfig.h
		DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/generated)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pre-generated/jvx_fdesign_I_octConfig.h
		DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/generated)

endif()

set(LOCAL_LIBS jvx-dsp-base_static)

if(JVX_TARGET_GCC_ARM_YOCTO)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_TARGET_GCC_ARM_YOCTO")
endif()


set(BUILD_STATIC TRUE)
set(IS_C_LIB TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(INSTALL_DEDICATED_GENERATED_TREE TRUE)
set(INSTALL_DEDICATED_PCG_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
