if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp)
	message("speexdsp sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
	message("speexdsp installation complete.")
endif()

set(PROJECT_NAME jvx-speex-dsp)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/include
  )

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/preprocess.c
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/jitter.c
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/mdf.c
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/fftwrap.c
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/filterbank.c
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/resample.c
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/buffer.c
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/scal.c
  
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/kiss_fft.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/_kiss_fft_guts.h 
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/kiss_fft.h 
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/kiss_fftr.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/kiss_fftr.h
  
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/arch.h 	
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/bfin.h 
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/fixed_arm4.h 
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/fixed_arm5e.h 	
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/fixed_bfin.h
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/fixed_debug.h 	
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/math_approx.h
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/misc_bfin.h 	
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/stack_alloc.h
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/fftwrap.h 
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/filterbank.h 
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/fixed_generic.h 
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/os_support.h 
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/pseudofloat.h 
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/smallft.h 
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/vorbis_psy.h 
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/resample_sse.h 
	${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/libspeexdsp/resample_neon.h

  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/include/speex/speex_echo.h 	
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/include/speex/speex_jitter.h 	
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/include/speex/speex_buffer.h 	
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/include/speex/speex_preprocess.h 	
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/include/speex/speex_resampler.h 	
  ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/include/speex/speexdsp_types.h
  )

   
# set(LOCAL_LIBS jvx-dsp-base_static)

set(LOCAL_COMPILE_DEFINITIONS "FLOATING_POINT;EXPORT=/*none*/;USE_KISS_FFT")

set(BUILD_STATIC TRUE)
set(IS_C_LIB TRUE)
# set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
# set(INSTALL_DEDICATED_GENERATED_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

if(JVX_RELEASE_SDK)
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/speexdsp/include DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/third_party/git/speex-dsp/speexdsp FILES_MATCHING PATTERN "*.h")	
endif()

