# Add a linux shell - even in Windows
if(CMAKE_HOST_WIN32)
# if(JVX_OS MATCHES "windows")
		
	get_filename_component(gitfolder ${GIT_EXECUTABLE} DIRECTORY)
	find_program(mysh sh PATHS ${gitfolder}/../bin NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_ENVIRONMENT_PATH)
	get_filename_component(gitfolder ${GIT_EXECUTABLE} DIRECTORY) 
	
	# Note: mysh is found when searching git - check gitversion.cmake
	FIND_PROGRAM(myshplus "msys2_shell.cmd" REQUIRED)
	
else()
	set(mysh bashXY)
endif()		

