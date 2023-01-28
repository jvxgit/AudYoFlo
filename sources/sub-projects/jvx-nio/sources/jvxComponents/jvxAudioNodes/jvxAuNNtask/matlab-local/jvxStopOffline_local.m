function [ ] = jvxStopOffline_local( jvx_handle )

    global inProcessing;
    
    global myResults;
    
    if(jvx_handle.processing.matlab_engaged)
        myResults.mat_eng.sig = inProcessing.out_data;
    else
        myResults.mat_noteng.sig = inProcessing.out_data;    
    end


    
