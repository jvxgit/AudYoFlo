function [hObject, handles] = jvx_start_processing(hObject, handles)

% Callback for project specific actions
if(isfield(handles, 'local'))
    if(isa(handles.local.cb_start, 'function_handle'))
        try
            handles.local.private = handles.local.cb_start(handles.local.private);
        catch ME
            a = functions(handles.local.cb_start);
            warning(['Function <' a.function '> failed, reason: ' ME.message]);
            for(ind=1:size(ME.stack,1))
                disp([num2str(ind) ')--> ' ME.stack(ind).file ', ' ME.stack(ind).name ', line ' num2str(ME.stack(ind).line)]);
            end
        end
    end
end

set(handles.checkbox_force, 'Visible', 'on');

% Start sequencer
[a b] = handles.hostcall('start_process_sequence', 1000, -1, true);
if(~a)
    jvxJvxHost.jvx_display_error(13, mfilename, b.DESCRIPTION_STRING);
end

% Start variable inProcessing
global inProcessing;
inProcessing.jvx_start_handles = handles.local.private;

% Trigger once, all steps should happen within the one step in case of
% audio processing.
[a status_seq last_step return_val] = handles.hostcall('trigger_external_process', 0);
if(a)
    [devnull seq_stat_txt] = handles.hostcall('lookup_type_name__value', 'jvxSequencerStatus', status_seq);
    [devnull last_step_txt] = handles.hostcall('lookup_type_name__value', 'jvxSequenceElementType', last_step);
    [devnull return_code_txt] = handles.hostcall('lookup_type_name__value', 'jvxErrorType', return_val);
    disp(['Return values sequencer step: ' seq_stat_txt ' -- ' last_step_txt ' -- ' return_code_txt]);

else
    jvxJvxHost.jvx_display_error(14, mfilename, status_seq.DESCRIPTION_STRING);
end

[hObject, handles] = jvxJvxHostRt.jvx_refresh_props_run(hObject, handles, true);
handles.jvx_struct.properties_run.active = true;

[hObject, handles] = jvxJvxHostRt.jvx_update_ui_run_start(hObject, handles);
[hObject, handles] = jvxJvxHostRt.jvx_set_gains(hObject, handles);

% Pass handles to GUI object for timer callback
guidata(hObject, handles);

% Message queue thread is already running
global jvx_global_control;
jvx_global_control.tasks.msgq.update_mode = 1;

% Not needed ...
%[a b] = handles.hostcall('status_process_sequence');
%if(~a)
     %end