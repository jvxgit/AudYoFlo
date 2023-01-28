
# version tag generation
find_host_package(Git)
if(JVX_MYVERSION_OLD)
	# message("++> Version of build, old value: <${JVX_MYVERSION_OLD}")
else()
	set(JVX_MYVERSION_OLD "--invalid--")
	set(JVX_FORCE_GENERATE_NEW_VERSION_TAG TRUE)
endif()

if(JVX_MYRELEASE_DATE_OLD)
	# message("++> Date of build, old value: <${JVX_MYRELEASE_DATE_OLD}")
else()
	set(JVX_MYRELEASE_DATE_OLD "--invalid--")
	set(JVX_FORCE_GENERATE_NEW_VERSION_TAG TRUE)
endif()

execute_process(COMMAND
  # "${GIT_EXECUTABLE}" describe --tags --dirty=-dirty  --always
  "${GIT_EXECUTABLE}" describe --tags --always
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  RESULT_VARIABLE res
  OUTPUT_VARIABLE out
  #ERROR_QUIET
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
if(NOT res EQUAL 0)
  set(myversion "FAILEDTOACCESSGIT")
else()
  set(myversion "${out}")
endif()
message("++> Git version: ${myversion}")

string(TIMESTAMP myreleasedate)
message("++> Compile date: ${myreleasedate}")

if(NOT ${myversion} MATCHES ${JVX_MYVERSION_OLD})
	message("++> Requesting to generate system tag header since version tag has changed.")
	set(JVX_FORCE_GENERATE_NEW_VERSION_TAG TRUE)
endif()
#if(NOT ${myreleasedate} MATCHES ${JVX_MYRELEASE_DATE_OLD})
#	message("++> Requesting to generate system tag header since date tag has changed.")
#	set(JVX_FORCE_GENERATE_NEW_VERSION_TAG TRUE)
#endif()

if(JVX_FORCE_GENERATE_NEW_VERSION_TAG)
	message("++> Generation of system tag header required.")
else()
	message("++> Generation of system tag header NOT necessary.")
endif()
	
set(JVX_MYVERSION_OLD ${myversion} CACHE INTERNAL "")
set(JVX_MYRELEASE_DATE_OLD ${myreleasedate} CACHE INTERNAL "")

# message("Version of build, new value: <${JVX_MYVERSION_OLD}")
# message("Version of build, new value: <${JVX_MYRELEASE_DATE_OLD}")

# Copy the versio for those projects using the (old) product version
set(myproductversion ${myversion})

# Add a linux shell - even in Windows
if(JVX_OS MATCHES "windows")
		
	get_filename_component(gitfolder ${GIT_EXECUTABLE} DIRECTORY)
	find_program(mysh sh PATHS ${gitfolder}/../bin NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_ENVIRONMENT_PATH)
	get_filename_component(gitfolder ${GIT_EXECUTABLE} DIRECTORY) 
else()
	set(mysh bash)
endif()		