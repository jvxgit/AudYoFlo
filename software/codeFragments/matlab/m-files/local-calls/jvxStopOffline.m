function [ ] = jvxStopOffline()

    global inProcessing;
	jvx_handle = inProcessing.jvx_handle;

	jvxStopOffline_local( jvx_handle );
