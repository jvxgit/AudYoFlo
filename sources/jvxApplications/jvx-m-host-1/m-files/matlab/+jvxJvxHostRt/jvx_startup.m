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

    % Start UI for sub-control
    handles.subui.hdl = jvxHost_ap();
    
    % Set reference to parent UI
    handles_subui = guidata(handles.subui.hdl);
    handles_subui.parent = hObject;
    handles_subui.hostcall = handles.hostcall;
    guidata(handles.subui.hdl, handles_subui);
    
    
    handles.subui2.hdl = jvxHost_mi();
    
    % Set reference to parent UI
    handles_subui2 = guidata(handles.subui2.hdl);
    handles_subui2.parent = hObject;
    handles_subui2.hostcall = handles.hostcall;
    guidata(handles.subui2.hdl, handles_subui2);

    
    % Initialize the host into a stable state
    [hObject, handles] = jvxJvxHostRt.jvx_init(hObject, handles);

    % Initialize the UI window to set some "always identical" properties
    [hObject, handles] = jvxJvxHostRt.jvx_init_ui(hObject, handles);
    
    % Obtain references to all relevant properties
    [hObject, handles] = jvxJvxHostRt.jvx_refresh_props(hObject, handles);

    % Show current status
    [hObject, handles] = jvxJvxHostRt.jvx_update_ui(hObject, handles);
    
     % Set hook function calls - if desired to subproject version
    gotSubProject = false;
    subproject = 'jvxDefault';
    if(handles.jvx_struct.properties.node.offline_node.id_subproject_name >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties.node.offline_node.cat_subproject_name, handles.jvx_struct.properties.node.offline_node.id_subproject_name);
        if(a)
            for(ind=1:size(handles.jvx_struct.subprojects, 2))
                if(strcmp(handles.jvx_struct.subprojects{ind}, b.SUBFIELD))
                    gotSubProject = true;
                    break;
                end
            end
        end
    end
    if(gotSubProject)
        subproject = b.SUBFIELD;
    end
    
    % If desired, add some sequencer entries here! All default sequeners
    % are provided in C system
    if(isa(handles.local.cb_seq, 'function_handle'))
        try
            handles.local.cb_seq();
        catch ME
            a = functions(handles.local.cb_seq);
            disp(['Function <' a.function '> failed, reason: ' ME.message]);
            for(ind=1:size(ME.stack,1))
                disp([num2str(ind) ')--> ' ME.stack(ind).file ', ' ME.stack(ind).name ', line ' num2str(ME.stack(ind).line)]);
            end
        end
    end
    
    nm = [subproject  '.jvx_init_callback'];
    handles.local.cb_init = str2func(nm);
    nm = [subproject  '.jvx_start_callback'];
    handles.local.cb_start = str2func(nm);
    nm = [subproject  '.jvx_stop_callback'];
    handles.local.cb_stop = str2func(nm);
    nm = [subproject  '.jvx_term_callback'];
    handles.local.cb_term = str2func(nm);
    
    handles.local.private = [];
    if(isa(handles.local.cb_init, 'function_handle'))
        try
            handles.local.private = handles.local.cb_init(hObject, handles);
        catch ME
            a = functions(handles.local.cb_init);
             warning(['Function <' a.function '> failed, reason: ' ME.message]);             
        end
        %a = functions(handles.local.cb_init);
        %if(~isempty(a.file))
    end
    
    % Obtain references to all relevant properties
    [hObject, handles] = jvxJvxHostRt.jvx_refresh_props(hObject, handles);

    % Show current status
    [hObject, handles] = jvxJvxHostRt.jvx_update_ui(hObject, handles);
        
