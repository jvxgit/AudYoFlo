function [] = jvx_shutdown(handles)

% ==============================================
% Terminate Rtproc host on close
% ==============================================

% If sequencer is running, shut it down
% Trigger shutdown of sequencer
[a b] = jvxJvxHostRt('status_process_sequence');
if(a)

    state_init_sequencer =  jvxJvxHost.jvx_lookup_type_id__name('jvxSequencerStatus', 'JVX_SEQUENCER_STATUS_NONE');
    
    % Start only if seqeuncer is in neutral init state
    if(b.STATUS_INT32 ~= state_init_sequencer)
        
        [a b] = jvxJvxHostRt('trigger_stop_process');
        if(~a)
        end
        
        % Move processing forward in one step for audio
        [a stat_sequencer last_step return_val] = jvxJvxHostRt('trigger_external_process', 0);
        if(a)
            [devnull seq_stat_txt] = handles.hostcall('lookup_type_name__value', 'jvxSequencerStatus', stat_sequencer);
            [devnull last_step_txt] = handles.hostcall('lookup_type_name__value', 'jvxSequenceElementType', last_step);
            [devnull return_code_txt] = handles.hostcall('lookup_type_name__value', 'jvxErrorType', return_val);
            disp(['Return values sequencer step: ' seq_stat_txt ' -- ' last_step_txt ' -- ' return_code_txt]);
        end
        
        % Wait for completion
        [a b] = jvxJvxHostRt('wait_completion_process', 0);
        if(~a)
        end
    end
end

% ++++++++++++++++++++++++++++++++++++++++++++++

% Terminate host
[a b]= jvxJvxHostRt('terminate');

% ++++++++++++++++++++++++++++++++++++++++++++++   
% ++++++++++++++++++++++++++++++++++++++++++++++

% Clear host
clear jvxJvxHostRt;