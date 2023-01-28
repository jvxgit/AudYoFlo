function [status_seq, hObject, handles] = jvx_start_processing(hObject, handles)

status_seq = -1;
errorFound = false;

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
            errorFound = true;
        end
    end
end
    
if(~errorFound)
    % Start sequencer
    [a b] = handles.hostcall('start_process_sequence', 1000, -1, true);
    if(~a)
        jvxJvxHost.jvx_display_error(13, mfilename, b.DESCRIPTION_STRING);
    end
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

