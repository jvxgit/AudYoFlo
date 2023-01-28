function [hObject, handles] = jvx_set_to_init(hObject, handles)

global jvx_host_call_global;

% Set into neutral state
[hObject, handles] = jvxJvxHostOff.jvx_set_to_neutral(hObject, handles);

% Means: no selection yet
handles.jvx_struct.input.selection = 0;

handles.jvx_struct.input.fName = '';
handles.jvx_struct.input.vName = '';

[a b]= handles.hostcall('info');
if(a)

    % The host has been initialized before, re-initialize
    [a b]= handles.hostcall('deactivate');
    if(~a)
        %errCode, origFile, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(1, mfilename('fullpath'), ['<deactivate failed>, reason: ' b.DESCRIPTION_STRING], false);
    end
    [a b]= handles.hostcall('terminate');
    if(~a)
        % errCode, origFile, errOperation, errTxt, errorsAsWarnings
        %% origFile, errCode, errOperation, errDescr, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'terminate', b.DESCRIPTION_STRING, false);
    end
end