set(PROJECT_NAME jvxAuTAsio)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

set(LOCAL_PCG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/codeGen/exports_device.pcg
  )

# set(JVX_ASIOSDK_SUBFOLDER "ASIOSDK2")
set(JVX_ASIOSDK_SUBFOLDER "asiosdk_2.3.3_2019-06-14")

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/${JVX_ASIOSDK_SUBFOLDER})
	message("Could not link ASIO sdk. Due to copyright issues, you need to download it at https://new.steinberg.net/de/developers and install it under ${CMAKE_CURRENT_SOURCE_DIR}/lib/${JVX_ASIOSDK_SUBFOLDER}") 
	# file(DOWNLOAD "https://www.steinberg.net/asiosdk?_ga=2.34212833.1787253143.1617615529-938406583.1617615529" ${JVX_ASIOSDK_SUBFOLDER}.zip}
	message("Running installation in cmake run.")
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR}/lib ; ./prepareModules.sh; exit")
	message("Installation in cmake completed.")
endif()


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/target
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/${JVX_ASIOSDK_SUBFOLDER}/common
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/${JVX_ASIOSDK_SUBFOLDER}/host
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/${JVX_ASIOSDK_SUBFOLDER}/host/pc
  ${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-component-templates-base/include
  ${JVX_BASE_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-base/generated
  )

set(LOCAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/target/componentEntry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioAsioTechnology.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioAsioDevice.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioAsioTechnology.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CjvxAudioAsioDevice.h
  )

set(LOCAL_LIBS
  jvx-component-templates-base_static
  )

set(IS_JVX_COMPONENT TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)
