if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-flutter-native-static)

include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

## message(FATAL_ERROR "Flutter SDK path: ${FLUTTER_SDK_PATH}")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/target

	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-lib-host-app/include/binary-api
	${JVX_BASE_ROOT}/sources/jvxLibraries/jvx-app-host/include
	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-app-host/include
  
	${JVX_BASE_ROOT}/software/codeFragments/jvxHosts/common
	${JVX_SUBPRODUCT_ROOT}/software/codeFragments/jvxHosts/common
	${FLUTTER_SDK_PATH}/bin/cache/dart-sdk/include

	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/jvx-lib-host-app/generated
	
  )


  # Add local sources (header files only for project workspace generation!) 
set(LOCAL_SOURCES 
	
	# We do not need this here!
	# ${CMAKE_CURRENT_SOURCE_DIR}/exports/${JVX_OS}/exports-flutter-native.def 

	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_local.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_macros.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_api.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_host_config.h

	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_api_alloc.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_api_helpers.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_api_boot_shutdown.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_api_decode_requests.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_api_processes.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_api_periodic.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_api_sequencer.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_api_properties.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_api_components.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_api_config.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_types.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/flutter_native_type_translator.h

	${CMAKE_CURRENT_SOURCE_DIR}/src/flutter_native_boot_shutdown.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/flutter_native_decode_requests.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/flutter_native_periodic.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/flutter_native_processes.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/flutter_native_alloc.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/flutter_native_helpers.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/flutter_native_properties.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/flutter_native_components.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/flutter_native_config.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/flutter_native_sequencer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/flutter_native_report_callbacks.cpp	
	${CMAKE_CURRENT_SOURCE_DIR}/src/flutter_native_async_report.cpp

	# ${CMAKE_CURRENT_SOURCE_DIR}/exports/${JVX_OS}/exports-flutter-native.def
  )

# Macro JVX_LIB_SINGLETON: Allocate the lib object as a singelton. Dynamic alloc somehow
# causes a heap corruption even if we only allocate and deallocate without any interaction - strange :->
set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_HOST_USE_ONLY_STATIC_OBJECTS;JVX_LIB_SINGLETON")

set(LOCAL_LIBS 
	jvx-lib-host-app-binary-static_static)

## ======================================================================
## The audio libs
## ======================================================================

set(BUILD_STATIC TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(CREATE_OBJECT_LIBRARY TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)



