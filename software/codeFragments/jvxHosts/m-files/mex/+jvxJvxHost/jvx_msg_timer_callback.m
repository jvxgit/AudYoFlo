function [] = jvx_msg_timer_callback(object,event)
    
    global jvx_host_call_global;
    doLoop = true;
    
    % Request trigger of next message queue processing until queue is empty
    while(doLoop)
        [doLoop err]= jvx_host_call_global('trigger_process_msgq');
        
        if(doLoop)
            
        else
            % Error code 6 means that there simpliy os no message in the
            % queue..
            if(err.ERRORID_INT32 ~= 6)
                warning(err.DESCRIPTION_STRING);
            end
        end
    end
