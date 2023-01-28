set(PROJECT_NAME qcustomplot)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot)
	message("qcustomplot sub project not available. Starting script to download it.") 
	execute_process(COMMAND ${mysh} -c "cd ${CMAKE_CURRENT_SOURCE_DIR} ; ./prepareModules.sh; exit")
	message("qcustomplot installation complete.")
endif()

set(CMAKE_AUTOMOC ON)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot
)

set(LOCAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot/qcustomplot.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot/qcustomplot.cpp
  )

set(LOCAL_LIBS
    ${JVX_QT_LIBS}
	${JVX_OPENGL_LIBS}
	)
	

set(BUILD_STATIC TRUE)
#set(BUILD_SHARED TRUE)

# set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)

#set(LOCAL_COMPILE_DEFINITIONS_SHARED "QCUSTOMPLOT_COMPILE_LIBRARY;QCUSTOMPLOT_USE_OPENGL")

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

if(JVX_RELEASE_SDK)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot/qcustomplot.h DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/third_party/web/qcustomplot/qcustomplot) # FILES_MATCHING PATTERN "*.h")	
endif()
