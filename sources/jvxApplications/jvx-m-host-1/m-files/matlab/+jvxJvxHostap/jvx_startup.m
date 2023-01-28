function [hObject, handles] = jvx_startup(hObject, handles)
    
    % Processing not running yet
    handles.jvx_struct.properties_run.active = false;
    handles.jvx_struct.properties.channels.input.id_user = -1;
    handles.jvx_struct.properties.channels.input.all = true;
    handles.jvx_struct.properties.channels.output.id_user = -1;
    handles.jvx_struct.properties.channels.output.all = true;
    handles.jvx_struct.properties.device.local.directIn = [];
    handles.jvx_struct.properties.device.local.gain_in = [];
    handles.jvx_struct.properties.device.local.gain_out = [];

    % Initialize the host into a stable state
    [hObject, handles] = jvxJvxHostRt.jvx_init(hObject, handles);

    % Initialize the UI window to set some "always identical" properties
    [hObject, handles] = jvxJvxHostRt.jvx_init_ui(hObject, handles);
    
    % Obtain references to all relevant properties
    [hObject, handles] = jvxJvxHostRt.jvx_refresh_props(hObject, handles);

    % Show current status
    [hObject, handles] = jvxJvxHostRt.jvx_update_ui(hObject, handles);
    
    
