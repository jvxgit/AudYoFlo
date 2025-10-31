	message("--> Using Jvx SDK AudioHost Package")
	set(LOCAL_LIBS ${LOCAL_LIBS}		
		
		jvxLthreads_static
		jvxTconfigProcessor_static
		jvxTGlobalLock_static
		jvxTDataConverter_static
		jvxTDataLogger_static
		jvxTResampler_static		
		jvxTSocket_static	
		jvxTSystemTextLog_static		
		jvxTRemoteCall_static		
		jvxTrtAudioFileReader_static
		jvxTrtAudioFileWriter_static
		jvxAuTGenericWrapper_static
		jvxTLogRemoteHandler_static
		jvx-qt-helpers_static
		jvx-remote-call_static
		jvx-crc_static
		jvxLFileIO_static
		jvx-dsp-base_static
		jvx-dsp-base-cpp_static
		jvx-dsp_static
		jvx-qt-host-widgets_static
		jvx-qt-helpers_static	
		qwt_static
		qcustomplot_static
	)
	if(JVX_AUDIOHOST_SHARED)
		set(LOCAL_LIBS ${LOCAL_LIBS}
			jvx-app-host-shared_static
			jvxTThreads_static
			jvxHAppHost_static
			jvxFHAppFactoryHost_static
			)
	else()
		set(LOCAL_LIBS ${LOCAL_LIBS}
			jvx-app-host-static_static
			jvxTThreads_static
			jvxHAppHost_static_nd
			jvxFHAppFactoryHost_static_nd
			)
	endif()
	
	
    if(${JVX_OS} MATCHES "windows")
		if(JVX_USE_PART_RS232)
			set(LOCAL_LIBS ${LOCAL_LIBS}
				jvxTRs232Win32_static)
		endif()
	endif()
	
    if(${JVX_OS} MATCHES "linux")
		if(JVX_USE_PART_RS232)
			set(LOCAL_LIBS ${LOCAL_LIBS}
				jvxTRs232Glnx_static)
		endif()
	endif()
	if(JVX_USE_PART_PAUDIO)
		set(LOCAL_LIBS ${LOCAL_LIBS}
			portAudio)
	endif()

	if(JVX_USE_PART_WEBSERVER)
		set(LOCAL_LIBS ${LOCAL_LIBS}
		jvxTWebServer_static
		)		
	endif()

	if(JVX_OS MATCHES "windows")
		if(JVX_USE_PART_CORECONTROL)
			set(LOCAL_LIBS ${LOCAL_LIBS} imm32 Setupapi jvxTThreadController_static)
		endif()
	endif()
