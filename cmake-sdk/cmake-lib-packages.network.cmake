	message("--> Using Jvx SDK Network Package")
	set(LOCAL_LIBS ${LOCAL_LIBS}
		jvxLsockets_st_static
		jvx-sockets_static
		jvx-net-helpers_static
		)
	
	if(JVX_USE_PCAP)
		set(LOCAL_LIBS ${LOCAL_LIBS}
		${PCAP_LIBRARIES}
		${PCAPPA_LIBRARIES}
		)

	endif()
	
	if(JVX_USE_PART_WEBSERVER)
		set(LOCAL_LIBS ${LOCAL_LIBS} jvxTWebServer_static)
	endif()
	
	set(LOCAL_LIBS ${LOCAL_LIBS}
		${JVX_SOCKET_LIBRARIES}
		)
