function [jvx_out_frame] =jvxProcessOffline(jvx_in_frame, jvx_xchg_cnt)
    
	global inProcessing;
	jvx_handle = inProcessing.jvx_handle;

	[jvx_handle, jvx_out_frame] =jvxProcessOffline_local(jvx_handle, jvx_in_frame);

	inProcessing.jvx_handle = jvx_handle;
