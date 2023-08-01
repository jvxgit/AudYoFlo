PROJECT("myNewRtapProject")

# If working with automatic relative to absolute path generation,
# replace 
file(TO_CMAKE_PATH ${JVX_SDK_RUNTIME_PATH} JVX_SDK_RUNTIME_PATH)
file(TO_CMAKE_PATH ${JVX_SDK_PATH} JVX_SDK_PATH)

include(${JVX_SDK_PATH}/cmake/standalone.cmake)

set(JVX_SUBPRODUCT_MYNEWRTAPPROJECT_LIBS_INCLUDE_PATH "${JVX_SDK_PATH}/include")
set(JVX_SUBPRODUCT_MYNEWRTAPPROJECT_LIBS_LIB_PATH "${JVX_SDK_PATH}/lib")
set(JVX_SUBPRODUCT_MYNEWRTAPPROJECT_LIBS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/myNewRtapProject")
set(JVX_SUBPRODUCT_MYNEWRTAPPROJECT_LOCAL_LIBS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/myNewRtapLibrary/include")

# Either build algorithm or host
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/myNewRtapLibrary)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/myNewRtapLibrary/tests)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/myNewRtapLibrary-matlab/cmake/matlab)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/myNewRtapLibrary-matlab/cmake/octave)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/myNewRtapAudioNode)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/myNewRtapApplication)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT myNewRtapApplication)
