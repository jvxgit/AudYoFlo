function [hObject, handles] = jvx_set_to_neutral(hObject, handles)
[a b]= handles.hostcall('info');
if(a)
   
    % The host has been initialized before, re-initialize
    [a b]= handles.hostcall('status_process_sequence');
    if(~a)
        jvxHost_display_error('Error code 0x8', b);       
    else
        state_init_sequencer =  jvxJvxHost.jvx_lookup_type_id__name('jvxSequencerStatus', 'JVX_SEQUENCER_STATUS_NONE');
        if(b.STATUS_INT32 ~= state_init_sequencer)
            error('jvx_set_to_neutral: Stopping sequencer here not yet supported');
        end
    end
    
    [a b]= handles.hostcall('info_sequencer');
    if(~a)
        jvxHost_display_error('Error code 0x8', b);
    end
end