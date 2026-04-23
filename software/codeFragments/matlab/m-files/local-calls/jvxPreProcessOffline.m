function [jvx_in_frame] =jvxPreProcessOffline(jvx_in_frame)

    global inProcessing;
	jvx_handle = inProcessing.jvx_handle;

	[jvx_handle, jvx_in_frame] = jvxPreProcessOffline_local(jvx_handle, jvx_in_frame);
	
	inProcessing.jvx_handle = jvx_handle;
    