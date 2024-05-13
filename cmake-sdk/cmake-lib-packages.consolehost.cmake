	message("--> Using Jvx SDK ConsoleHost Package")
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvxLthreads_static	
		jvxTconfigProcessor_static
		jvxTDataConverter_static
		jvxTDataLogger_static
		jvxTResampler_static
		jvxTSocket_static			
		jvxTRemoteCall_static		
		jvxTrtAudioFileReader_static
		jvxTrtAudioFileWriter_static
		jvxAuTGenericWrapper_static
		jvxTLogRemoteHandler_static
		jvxTSystemTextLog_static
		jvxTWebServer_static
		jvxTThreads_static			
		jvxEStandalone_static
		jvxLFileIO_static		
		jvx-host-json_static
		jvx-json_static
		jvx-crc_static)
		

	if(JVX_CONSOLEHOST_STATIC)
		set(LOCAL_LIBS ${LOCAL_LIBS}
			jvx-app-host-static_static
			jvxLConsoleHost-static_static
			jvxLConsoleTools-static_static
			jvxHAppHost_static_nd
			jvxFHAppFactoryHost_static_nd		
			)
	else()
		set(LOCAL_LIBS ${LOCAL_LIBS}
			jvx-app-host-shared_static
			jvxLConsoleHost-shared_static
			jvxLConsoleTools-shared_static
			jvxHAppHost_static
			jvxFHAppFactoryHost_static
			)		
	endif()
	
	if(JVX_USE_PART_PAUDIO)
		set(LOCAL_LIBS ${LOCAL_LIBS}
			portAudio)
	endif()