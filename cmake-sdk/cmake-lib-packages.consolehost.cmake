	message("--> Using Jvx SDK ConsoleHost Package")
	set(LOCAL_LIBS ${LOCAL_LIBS}		
		jvxTconfigProcessor_static
		jvxTSystemTextLog_static
		jvxTWebServer_static
		jvx-json_static
		jvx-crc_static)
		

	if(JVX_CONSOLEHOST_STATIC)
		set(LOCAL_LIBS ${LOCAL_LIBS}
			jvx-app-host-static_static
			)
	else()
		set(LOCAL_LIBS ${LOCAL_LIBS}
			jvx-app-host-shared_static
			)		
	endif()