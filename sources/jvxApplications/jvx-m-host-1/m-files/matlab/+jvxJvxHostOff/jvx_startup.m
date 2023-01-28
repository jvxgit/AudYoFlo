function [hObject, handles] = jvx_startup(hObject, handles)
    
    handles.subui2.hdl = jvxHost_mi();
    
    % Set reference to parent UI
    handles_subui2 = guidata(handles.subui2.hdl);
    handles_subui2.parent = hObject;
    handles_subui2.hostcall = handles.hostcall;
    guidata(handles.subui2.hdl, handles_subui2);
    
    % Initialize the host into a stable state
    [hObject, handles] = jvxJvxHostOff.jvx_init(hObject, handles);

    % Initialize the UI window to set some "always identical" properties
    [hObject, handles] = jvxJvxHostOff.jvx_init_ui(hObject, handles);
    
    % Obtain references to all relevant properties
    [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
       
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
             for(ind=1:size(ME.stack,1))
                 disp([num2str(ind) ')--> ' ME.stack(ind).file ', ' ME.stack(ind).name ', line ' num2str(ME.stack(ind).line)]);
             end
        end
        %a = functions(handles.local.cb_init);
        %if(~isempty(a.file))
    end
    
    % Obtain references to all relevant properties
    [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);

    % Verify that setting is valid for this session (files and workspace
    % vars)
     [hObject, handles] = jvxJvxHostOff.jvx_initialize_settings(hObject, handles);
    
    % Show current status
    [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    
