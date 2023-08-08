function [ ] = jvxStartOffline( jvx_bsize_in, jvx_srate_in, jvx_num_in, jvx_format_in, jvx_bsize_out, jvx_srate_out, jvx_num_out, jvx_format_out, jvx_is_offline, precision, jvx_add_hints )

    global inProcessing;

% Create handle
    jvx_handle = [];
    
    if(exist('jvxStartOffline_local', 'file') == 2)
        [ jvx_handle ] = jvxStartOffline_local(...
            jvx_bsize_in, jvx_srate_in, jvx_num_in, jvx_format_in, jvx_bsize_out, ...
            jvx_srate_out, jvx_num_out, jvx_format_out, jvx_is_offline, precision, ...
            jvx_add_hints);
    else
        inProcessing.triggerStop = true;
        inProcessing.reportError = 'Function <jvxStartOffline_local> does not exist. Maybe, you did not set the path to a development folder? Review path command for more infos.';
    end
	
	inProcessing.jvx_handle = jvx_handle;
