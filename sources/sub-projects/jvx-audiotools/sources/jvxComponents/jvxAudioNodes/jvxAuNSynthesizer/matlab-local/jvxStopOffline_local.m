function [ ] = jvxStopOffline_local( jvx_handle )

    global inProcessing;

    global interm_result;

    interm_result = inProcessing.algorithm.dbg;

	DOPLOT = false;

    if(DOPLOT)
        if(jvx_handle.processing.offline)
            x = (0:inProcessing.sig_length-1) *1/inProcessing.in_rate;
            
            figure;
            for ind = 1:inProcessing.out_channels
                subplot(inProcessing.out_channels, 1, ind);
                y = inProcessing.out_data(ind,:);
                
                plot(x,y);
            end
        end
        
        c1 = inProcessing.in_data(1, :);
        c2 = inProcessing.algorithm.dbg.m1_delayed;
        c1d = [zeros(1, inProcessing.algorithm.dbg.eq_delay_one_stage) c1];
        minL = min(size(c1d, 2), size(c2, 2));
        
        numPlots = 5;
        figure;
        subplot(numPlots,1,1);
        plot(c2(1:minL));
        hold on;
        plot(c1d(1:minL), 'g:');
        legend({'Input 1 - delayed'});
        
        subplot(numPlots,1,2);
        plot(inProcessing.algorithm.dbg.m2_delayed);
        legend({'Input 2 - delayed'});
        
        
        subplot(numPlots,1,3);
        plot(inProcessing.algorithm.dbg.m1_x00filtered);
        hold on;
        plot(inProcessing.algorithm.dbg.m1_delayed, 'g:');
        legend({'Input 1 - filtered x00', 'Input 1 - delayed'});
        
        subplot(numPlots,1,4);
        plot(inProcessing.algorithm.dbg.m2_x00filtered);
        hold on;
        plot(inProcessing.algorithm.dbg.m2_delayed, 'g:');
        legend({'Input 2 - filtered x00', 'Input 2 - delayed'});
        
        subplot(numPlots,1,5);
        plot(inProcessing.algorithm.dbg.m1_x0cfiltered);
        hold on;
        plot(inProcessing.algorithm.dbg.m1_delayed, 'g:');
        legend({'Input 2 - filtered x0c', 'Input 1 - delayed'});
        
        interm_result.response_fwd = inProcessing.algorithm.dbg.signal_fwd;
        
        %subplot(2,1,2);
        %plot(c1d(1:minL) - c2(1:minL));
        %legend({'Channel 1, delayed vs filtered'});
    end
    
