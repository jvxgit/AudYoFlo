function [hObject, handles] = settoinit(hObject, handles)

% Set into neutral state
[hObject, handles] = jvxJvxHostRt.jvx_set_to_neutral(hObject, handles);

handles.jvx_struct.properties.input_files.input.id_user = -1;
handles.jvx_struct.properties.output_files.input.id_user = -1;
handles.jvx_struct.properties.channels.output.id_user = -1;
handles.jvx_struct.properties.channels.input.id_user = -1;

[a b]= jvxJvxHostRt('info');
if(a)

    % The host has been initialized before, re-initialize
    [a b]= jvxJvxHostRt('terminate');
    if(~a)
        jvxJvxHostRt.jvx_display_error('Error code 0x1', b);
    end
end