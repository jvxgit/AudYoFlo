function [hObject, handles] = jvx_update_ui(hObject, handles)

% Reset everything except for user selections
handles.system.connections = [];
handles.system.bridges = [];
handles.system.master = [];
handles.system.path = [];
    
strconnections = {};
strbridges = {};
strpath = '';
strmaster = '';
strbridge = '';
strbfrom = '';
strbto = '';

% ==============================================================
[a, b] = handles.hostcall('list_connections');
if(~a)
    error('Failed to run command <list_connections>.');
else
    handles.system.connections = b;
end

% ======================================================


if(isempty(handles.system.connections))
    handles.user.id_select_connection = -1;
    handles.user.id_select_bridge_connection = -1;
else
    hh = struct2cell(handles.system.connections);
    for(ind = 1: size(hh,1))
        oneconnection = hh{ind};
        strconnections = [strconnections; oneconnection.CONNECTION_PROCESS_DESCRIPTION];
    end
    
    % At least there is one connection, mark this
    if(handles.user.id_select_connection > size(hh,1))
        handles.user.id_select_connection = 1;
    end
    if(handles.user.id_select_connection < 1)
        handles.user.id_select_connection = 1;
    end
    
    handles.system.master = hh{handles.user.id_select_connection}.CONNECTION_PROCESS_MASTER;
    handles.system.bridges = hh{handles.user.id_select_connection}.CONNECTION_PROCESS_BRIDGES;
    handles.system.path = hh{handles.user.id_select_connection}.CONNECTION_PROCESS_PATH_DESCRIPTION;
    
    strpath = handles.system.path;
    cpTp = [ handles.system.master.CONNECTION_PROCESS_MASTER_FACTORY_COMP_ID.COMPONENT_IDENTIFICATION_TYPE ...
            '<' num2str(handles.system.master.CONNECTION_PROCESS_MASTER_FACTORY_COMP_ID.COMPONENT_IDENTIFICATION_SLOT_ID) ...
            ',' num2str(handles.system.master.CONNECTION_PROCESS_MASTER_FACTORY_COMP_ID.COMPONENT_IDENTIFICATION_SLOT_SUB_ID) '>'];
    strmaster = [handles.system.master.CONNECTION_PROCESS_MASTER_FACTORY_NAME '::' handles.system.master.CONNECTION_PROCESS_MASTER_NAME '[' cpTp ']'];
    if(isempty(handles.system.bridges))
        handles.user.id_select_bridge_connection = -1;
    else
        bb = struct2cell(handles.system.bridges);
        for(jnd = 1:size(bb,1))
            strbridges = [strbridges; bb{jnd}.CONNECTION_PROCESS_BRIDGE_DESCRIPTOR];
        end            
        if(handles.user.id_select_bridge_connection > size(bb,1))
            handles.user.id_select_bridge_connection = 1;
        end
        if(handles.user.id_select_bridge_connection < 1)
            handles.user.id_select_bridge_connection = 1;
        end
        
        strbridge = bb{handles.user.id_select_bridge_connection}.CONNECTION_PROCESS_BRIDGE_DESCRIPTOR;
        ocon = bb{handles.user.id_select_bridge_connection}.CONNECTION_PROCESS_BRIDGE_OUTPUT_CONNECTOR;
        cpTp = [ ocon.CONNECTION_PROCESS_OUTPUT_CONNECTOR_FACTORY_COMP_ID.COMPONENT_IDENTIFICATION_TYPE ...
            '<' num2str(ocon.CONNECTION_PROCESS_OUTPUT_CONNECTOR_FACTORY_COMP_ID.COMPONENT_IDENTIFICATION_SLOT_ID) ...
            ',' num2str(ocon.CONNECTION_PROCESS_OUTPUT_CONNECTOR_FACTORY_COMP_ID.COMPONENT_IDENTIFICATION_SLOT_SUB_ID) '>'];
        strbfrom = [ocon.CONNECTION_PROCESS_OUTPUT_CONNECTOR_FACTORY_NAME '::' ocon.CONNECTION_PROCESS_OUTPUT_CONNECTOR_DESCRIPTION '[' cpTp ']'];
        
        icon = bb{handles.user.id_select_bridge_connection}.CONNECTION_PROCESS_BRIDGE_INPUT_CONNECTOR;
        cpTp = [ icon.CONNECTION_PROCESS_INPUT_CONNECTOR_FACTORY_COMP_ID.COMPONENT_IDENTIFICATION_TYPE ...
            '<' num2str(icon.CONNECTION_PROCESS_INPUT_CONNECTOR_FACTORY_COMP_ID.COMPONENT_IDENTIFICATION_SLOT_ID) ...
            ',' num2str(icon.CONNECTION_PROCESS_INPUT_CONNECTOR_FACTORY_COMP_ID.COMPONENT_IDENTIFICATION_SLOT_SUB_ID) '>'];
        strbto = [icon.CONNECTION_PROCESS_INPUT_CONNECTOR_FACTORY_NAME '::' icon.CONNECTION_PROCESS_INPUT_CONNECTOR_DESCRIPTION '[' cpTp ']'];

    end
end
    
set(handles.listbox1, 'String', strconnections);
if(handles.user.id_select_connection > 0)
    set(handles.listbox1, 'Value', handles.user.id_select_connection);
end

set(handles.listbox2, 'String', strbridges);
if(handles.user.id_select_bridge_connection > 0)
    set(handles.listbox2, 'Value', handles.user.id_select_bridge_connection);
end

set(handles.edit1, 'String', strmaster);
set(handles.edit2, 'String', strbridge);
set(handles.edit3, 'String', strbfrom);
set(handles.edit4, 'String', strbto);


