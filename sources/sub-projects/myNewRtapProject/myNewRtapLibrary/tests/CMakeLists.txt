#set(PROJECT_NAME myNewRtapLibrary)
#project ("Tests")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  ${CMAKE_CURRENT_SOURCE_DIR}/../include/os-${JVX_OS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
  ${CMAKE_CURRENT_BINARY_DIR}
  )

set(TEST_SOURCES 
# Examples:
#  ${CMAKE_CURRENT_SOURCE_DIR}/test_myComponent.cpp


  )

set(LOCAL_SOURCES
# Examples:
#  ${CMAKE_CURRENT_SOURCE_DIR}/../src/myComponent.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/../include/myComponent.h

  )

set(LOCAL_LIBS jvx-dsp_static jvx-dsp-base_static ${FFT_LIBRARIES})

set(LOCAL_COMPILE_DEFINITIONS "${FFT_COMPILE_DEFINITIONS}")

source_group("cmake" FILES "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")

#file(GLOB TEST_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

#Run through each source
foreach(testSrc ${TEST_SOURCES})
        #Extract the filename without an extension (NAME_WE)
        get_filename_component(testName ${testSrc} NAME_WE)

        #Add compile target
        add_executable(${testName} ${testSrc} ${LOCAL_SOURCES})
		target_link_libraries(${testName} ${LOCAL_LIBS} ${ADDITIONAL_LIBS} ${JVX_SYSTEM_LIBRARIES})

		target_compile_definitions(${testName} PRIVATE ${LOCAL_COMPILE_DEFINITIONS} ${ADDITIONAL_COMPILE_DEFINITIONS} ${GLOBAL_COMPILE_DEFINITIONS})
		set_target_properties(${testName} PROPERTIES
			COMPILE_FLAGS "${CMAKE_CXX_FLAGS}  ${JVX_CMAKE_LINKER_FLAGS_EXEC} ${LOCAL_CXX_FLAGS}"
			LINK_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${JVX_CMAKE_LINKER_FLAGS_EXEC} ${LOCAL_LINKER_FLAGS}")
		
		jvx_displayFlags( ${testName})
		set_property(TARGET ${testName} PROPERTY FOLDER "Tests")
endforeach(testSrc)



