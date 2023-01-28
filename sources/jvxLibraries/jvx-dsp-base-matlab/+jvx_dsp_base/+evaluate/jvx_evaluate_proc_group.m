function [] = jvx_evaluate_proc_group(fn)
    [content] = jvxDataLogReader.jvx_read_file(fn);
    
    entries = content{1}.data{1}.data;
    timstamps = content{1}.data{1}.time_us;
    
    num_opers = 6;
    table_operations = {...
        'TRIGGER_PIPELINE_ENTER', ... 
        'TRIGGER_PIPELINE_LEAVE', ... 
        'START_ENTER' ...
        'START_LEAVE' ...
        'STOP_ENTER', ...
        'STOP_LEAVE', ...
        'PROCESSING_ENTER', ...
        'PROCESSING_LEAVE', ...
        'THREAD_ENTER', ...
        'THREAD_LEAVE', ...
        'THREAD_COMPLETE_ENTER', ...
        'THREAD_COMPLETE_LEAVE'};
    
    table_operations_plot = [...
        6, -6, 1, -1, 3, -3, 2, -2, 4, -4, 5, -5];
    
    table_operations_descr = { ...
        'start', 'process', 'stop', 'thread', 'complete' 'trigger'};
    
    num = size(entries,1);
    x_axis = zeros(1, 2*num);
    
    
    last_valid_state = 0;
    old_state = 0;
    cnt = 1;
    stack = [];
   
    for(ind = 1:num_opers)
        str.offset = ind*2-1;
        str.y =  zeros(1, 2*num);
        str.current_state = str.offset;
        str.descr = table_operations_descr{ind};
        operations{ind} = str;
    end
    
    for(ind=1:num)
        
        %% [COMPONENT_ID] [THREAD_ID] [OPERATION] [SUCCESS] [PROC_ID] [IDX_BUF_IN] [IDX_BUF_OUT] [THREAD_ID]
        oper_id = entries(ind, 3);
        disp(['Step #' num2str(ind) ' :: ' table_operations{oper_id}]);
        new_state = table_operations_plot(oper_id);
        
        lowtohigh = true;
        if(new_state < 0)
        	lowtohigh = false;
            new_state = - new_state;
        end
        
        x_axis(cnt) = timstamps(ind);
        x_axis(cnt+1) = timstamps(ind);
        for(ind = 1:num_opers)
            operations{ind}.y(cnt) = operations{ind}.current_state;  
            operations{ind}.y(cnt+1) = operations{ind}.current_state;  
        end
        
        if(lowtohigh)
            operations{new_state}.y(cnt+1) = operations{new_state}.offset+1;
            operations{new_state}.current_state = operations{new_state}.offset+1;
        else
            operations{new_state}.y(cnt+1) = operations{new_state}.offset;
            operations{new_state}.current_state = operations{new_state}.offset;
        end
        
        cnt = cnt + 2;
    end
    
    figure; 
    hold on;
    xlabel('msecs');
    for(ind = 1:num_opers)
        plot(x_axis/1000.0, operations{ind}.y);
    end
    legend(table_operations_descr);
    
    