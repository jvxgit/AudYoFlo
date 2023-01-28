function [ ] = jvxStopOffline_local( jvx_handle )

    global inProcessing
    global interm_result;

    if(bitand(jvx_handle.operation_mode, 2))
        
    end
    
    if(jvx_handle.processing.offline)
    	% Nothing
    end
    
