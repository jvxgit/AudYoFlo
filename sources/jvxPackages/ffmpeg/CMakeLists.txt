   
set(JVX_FFMPEG_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(JVX_FFMPEG_PACKAGE ${JVX_FFMPEG_PACKAGE}

	# set(SDL_LIBRARY_PATH  "<no-sdl>/libsdl/SDL2-2.26.3" CACHE INTERNAL "libsdl lib path (windows)")
	jvxLibraries/third_party/web/libsdl
	jvxLibraries/third_party/git/ffmpeg
	jvxLibraries/jvx-ffmpeg-helpers
	jvxComponents/jvxAudioTechnologies/jvxAuTFFMpegReader
	jvxComponents/jvxAudioTechnologies/jvxAuTFFMpegWriter
	jvxComponents/jvxAudioCodecs/jvxAuCFfmpeg
	jvxComponents/jvxPackages/jvxPaFfmpeg)
	
if(NOT JVX_RELEASE_SDK)
	if(JVX_LINUX_NATIVE_INSTALLED_LIBS)
	else()
		set(JVX_FFMPEG_PACKAGE ${JVX_FFMPEG_PACKAGE}
			jvxApplications/ffoutput
			jvxApplications/ffmpeg-app
			jvxApplications/ffplay-app)
	endif()
endif()

jvx_addSubdirs("Configuring ffmpeg package" "${JVX_FFMPEG_PACKAGE}" "${JVX_FOLDER_HIERARCHIE_BASE}/ffmpeg")
