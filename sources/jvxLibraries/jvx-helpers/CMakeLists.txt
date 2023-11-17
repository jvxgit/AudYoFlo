if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-helpers)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
#	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
)

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxBitfieldTools.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxMisc.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxJsonTools.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxArgs.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxSystem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxBase64.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxReadWriteFile.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxDataProcessor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxDataLinkDescriptor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxDataConnections.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxProperties.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxComponentPreselection_common.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tools/HjvxReadConfigEntries.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tools/HjvxWriteConfigEntries.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tools/HjvxRs232.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tools/HjvxRemoteCall.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tools/HjvxExternalCommand.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tools/HjvxCodeGeneration.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxComponentTempConfig.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxConnectionDropzone.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxConnectionDescription.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxHostsAndProperties.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxComponentPreselection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxHost.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxDataLogger.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxDataLogger_Otf.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HjvxSequencer.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxSequencer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxBitfieldTools.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxMisc.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxJsonTools.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxArgs.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxSystem.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxBase64.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxReadWriteFile.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxDataProcessor.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxDataLinkDescriptor.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxDataConnections.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxProperties.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxComponentPreselection_common.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tools/HjvxReadConfigEntries.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tools/HjvxWriteConfigEntries.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tools/HjvxRs232.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tools/HjvxRemoteCall.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tools/HjvxExternalCommand.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxComponentTempConfig.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxConnectionDropzone.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/CjvxConnectionDescription.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxHostsAndProperties.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxComponentPreselection.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxHost.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxDataLogger.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/HjvxDataLogger_Otf.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx-helpers.h

  ${CMAKE_CURRENT_SOURCE_DIR}/../jvx-helpers-cpp/include/HjvxCppMisc.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../jvx-helpers-cpp/include/HjvxCircBufferPass.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../jvx-helpers-cpp/include/HjvxCircBufferRead.h
  )

set(LOCAL_LIBS
  jvx-json_static
  jvx-dsp-base_static
  jvx-system-base_static
  jvx-system-min_static
  jvx-callprot_static
  )

set(BUILD_STATIC TRUE)
set(IS_C_LIB FALSE)

set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
