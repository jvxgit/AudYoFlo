function [] = jvxBeforeProcessOffline()
     
	global inProcessing;
	jvx_handle = inProcessing.jvx_handle;

	[jvx_handle] = jvxBeforeProcessOffline_local(jvx_handle);
	
	inProcessing.jvx_handle = jvx_handle;

	 