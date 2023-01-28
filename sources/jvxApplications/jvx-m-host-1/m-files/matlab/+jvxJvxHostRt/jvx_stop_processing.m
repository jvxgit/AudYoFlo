function [hObject, handles] = jvx_stop_processing(hObject, handles, errorsAsWarnings)

    if(nargin <= 2)
        errorsAsWarnings = false;
    end
    
    % Switch back the mode
    global jvx_global_control;
    jvx_global_control.tasks.msgq.update_mode = 0;

     
      [hObject, handles] = jvxJvxHostRt.jvx_refresh_props_run(hObject, handles, false);
     % Trigger shutdown of sequencer
     [a b] = handles.hostcall('trigger_stop_process');
     if(~a)
         jvxJvxHost.jvx_display_error(15, mfilename, b.DESCRIPTION_STRING, errorsAsWarnings); 
     end
     
     % Move processing forward in one step for audio
     [a stat_sequencer last_step return_val] = handles.hostcall('trigger_external_process', 0);
     if(a)
         [devnull seq_stat_txt] = handles.hostcall('lookup_type_name__value', 'jvxSequencerStatus', stat_sequencer);
         [devnull last_step_txt] = handles.hostcall('lookup_type_name__value', 'jvxSequenceElementType', last_step);
         [devnull return_code_txt] = handles.hostcall('lookup_type_name__value', 'jvxErrorType', return_val);
         disp(['Return values sequencer step: ' seq_stat_txt ' -- ' last_step_txt ' -- ' return_code_txt]);
         
     else
         jvxJvxHost.jvx_display_error(16, mfilename, stat_sequencer.DESCRIPTION_STRING, errorsAsWarnings); 
     end
     
    % Wait for completion
     [a b] = handles.hostcall('wait_completion_process', 0);
     if(~a)
         jvxJvxHost.jvx_display_error(17, mfilename, b.DESCRIPTION_STRING, errorsAsWarnings); 
     end
     
    % Callback for project specific actions
     if(isfield(handles, 'local'))
         if(isa(handles.local.cb_stop, 'function_handle'))
             try
                 handles.local.private = handles.local.cb_stop(handles.local.private);
             catch ME
                 a = functions(handles.local.cb_start);
                 warning(['Function <' a.function '> failed, reason: ' ME.message]);
                 for(ind=1:size(ME.stack,1))
                     disp([num2str(ind) ')--> ' ME.stack(ind).file ', ' ME.stack(ind).name ', line ' num2str(ME.stack(ind).line)]);
                 end
             end
         end
     end

     set(handles.checkbox_force, 'Visible', 'off');
     % Not needed
     %[a b] = handles.hostcall('status_process_sequence');
     %if(~a)
     %end