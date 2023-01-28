function [ ] = jvxStopOffline( jvx_handle )
    
	% Obtain processing handle
	global inProcessing;
	jvx_handle = inProcessing.jvx_handle;

    close(inProcessing.processing.hdlUi);
    if(jvx_handle.processing.offline)
        x = [0:inProcessing.sig_length-1] *1/inProcessing.in_rate;
    
        figure;
        for(ind = 1:inProcessing.out_channels)
            subplot(inProcessing.out_channels, 1, ind);
            y = inProcessing.out_data(ind,:);
        
            plot(x,y);
        end
    end

