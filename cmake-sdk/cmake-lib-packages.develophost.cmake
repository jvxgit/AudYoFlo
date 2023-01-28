	message("--> Using Jvx SDK DevelopHost Package")
	set(LOCAL_LIBS ${LOCAL_LIBS}		
		qwt_static
		jvx-qt-helpers_static
		jvxHJvx_static
		jvxLthreads_static
		jvx-qt-host-widgets_static
		jvx-qt-helpers_static	
	)

	if(JVX_DEVELOPHOST_STATIC)
		set(LOCAL_LIBS ${LOCAL_LIBS}
			jvx-app-host-static_static
			jvxHJvx_static_nd
			jvxFHDefault_static_nd
			)
	else()
		set(LOCAL_LIBS ${LOCAL_LIBS}
			jvx-app-host-shared_static
			jvxHJvx_static
			jvxFHDefault_static
			)		
	endif()