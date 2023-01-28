function [jvx_handle] = jvxBeforeProcessOffline_local(jvx_handle)
     
    global inProcessing;
    global dbg;
    
    % dbg.fig = figure;
    
    if(jvx_handle.processing.offline)
    	% Nothing
    end