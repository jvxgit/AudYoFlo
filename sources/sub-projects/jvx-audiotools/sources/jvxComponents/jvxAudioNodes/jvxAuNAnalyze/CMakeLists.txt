cmake_minimum_required (VERSION 2.8.11)
if(DEFINED JVX_SDK_PATH)
  include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvxAuNAnalyze)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${JVX_SUBPRODUCT_SIGNALS_LIBS_INCLUDE_PATH}/jvx-audiotools-cpp/include
  ${JVX_SUBPRODUCT_SIGNALS_LIBS_INCLUDE_PATH}/jvx-audiotools/include
  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp/include
  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-dsp-cpp/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base-cpp/include
  ${JVX_BASE_LIBS_INCLUDE_PATH}/jvxLthreads/include
  ${JVX_SUBPRODUCT_LIBS_INCLUDE_PATH}/jvx-signal-plot/include
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNAnalyze.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAuNAnalyze.h
  )


set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_node.pcg
  )

set(LOCAL_PCG_FILE_OPTIONS
	-I ${JVX_BASE_LIBS_INCLUDE_PATH}/jvx-dsp-base-cpp/pcg/jvxLogToFile
	-I ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-system-base/pcg/jvxVisualDisplay
	-D JVX_ALL_AUDIO_TO_LOGFILE 
	-D JVX_USE_SPECTRUM_PLOT
	-D JVX_INVOLVE_WAVEFORM_PLOTTER
	# -D JVX_DECLARE_PROP_REF_INSTALL_CALLBACK
	)

#${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-dsp-base-cpp/pcg/jvxLogToFile

# FIXME: to be handled via LOCAL_MATPCG_FILES variable
if(JVX_MATLAB_PROPERTY_GENERATOR)
  jvx_genMatProperties(${JVX_TARGET_NAME} "JVX_COMPONENT_AUDIO_NODE" "node" "${LOCAL_PCG_FILES}" "${LOCAL_PCG_FILE_OPTIONS}")
endif()

set(LOCAL_LIBS
  jvx-audiotools-cpp_static
  jvx-dsp-cpp_static
  jvx-dsp-base-cpp_static
  jvx-audiotools_static
  jvx-dsp_static
  jvx-component-templates-base_static
  jvx-dsp-base_static
  jvxLthreads_static
  jvx-signal-plot_static
  ${FFT_LIBRARIES}
  )

set(IS_JVX_COMPONENT TRUE)
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_ALL_AUDIO_TO_LOGFILE;JVX_USE_SPECTRUM_PLOT")

# set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_INPUTOUTPUTCONNECTOR_VERBOSE")

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
