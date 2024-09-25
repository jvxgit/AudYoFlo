set(LOCAL_SOURCES ${LOCAL_SOURCES}
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstarter-components.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstarter-connectionrules.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstarter-hostconfig.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/ayfstarter-sequencer.cpp
	../common/local_config.cmake
	)
	
include_directories(

	${SUBPRODUCT_AYFSTARTER_COMPONENTS_BINARY_DIR}/AudioNodes/ayfAuNStarter/    

	${JVX_SUBPRODUCT_ROOT}/sources/jvxLibraries/jvx-component-templates-product/include
	${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxLibraries/jvx-component-templates-product/generated
)
 
set(LOCAL_LIBS ${LOCAL_LIBS}
	ayfAuNStarter_static )
  
# Link against either audio host lib or develop host lib
include(${JVX_SUBPRODUCT_ROOT}/cmake/hostlink.audio.cmake)

## ======================================================================
## The audio libs
# Pull in all audio components
include(${JVX_CMAKE_DIR}/packages/cmake-audio-components.cmake)

# Macos x specific configurations
#if(JVX_OS MATCHES "macosx")
#  set(JVX_MAC_OS_CREATE_BUNDLE TRUE)
#else(JVX_OS MATCHES "macosx")
#  set(CONFIGURE_LOCAL_START_SCRIPT TRUE)
#endif(JVX_OS MATCHES "macosx")
