function [hObject, handles] = jvx_init(hObject, handles)
    
    handles.user.id_select_connection = -1;
    handles.user.id_select_bridge_connection = -1;
    
    handles.system.connections = [];
    handles.system.bridges = [];
    handles.system.master = [];
    handles.system.path = [];