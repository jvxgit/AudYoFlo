function [hObject, handles] = jvx_init(hObject, handles)
    
% ===========================================
% Prepare the system tables
% ===========================================
[path name ext] = fileparts(mfilename('fullpath'));
global jvx_start_path;

handles.jvx_struct.technologies.comp_type = jvxJvxHost.jvx_lookup_type_id__name(...
    'jvxComponentType', 'JVX_COMPONENT_AUDIO_TECHNOLOGY');
handles.jvx_struct.devices.comp_type = jvxJvxHost.jvx_lookup_type_id__name(...
    'jvxComponentType', 'JVX_COMPONENT_AUDIO_DEVICE');
handles.jvx_struct.nodes.comp_type = jvxJvxHost.jvx_lookup_type_id__name(...
    'jvxComponentType', 'JVX_COMPONENT_AUDIO_NODE');

id_check_selected = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_SELECTED');

handles.jvx_struct.images.notready = imread([jvx_start_path '/images/icon_notready.png'], 'png');
handles.jvx_struct.images.ready = imread([jvx_start_path '/images/icon_ready.png'], 'png');
handles.jvx_struct.images.proc = imread([jvx_start_path '/images/icon_proc.png'], 'png');

% Matlab subproject folders
handles.jvx_struct.subprojects = {};
handles.jvx_struct.subpath = [fileparts(path) '/subprojects-audionode'];
subfolders = ls(handles.jvx_struct.subpath);
for(ind=1:size(subfolders,1))
    oneEntry = strtrim(subfolders(ind,:));
    if(...
        (strcmp(oneEntry, '.') ~= 1) & (strcmp(oneEntry, '..') ~= 1))
        theFolder = [handles.jvx_struct.subpath '/' oneEntry];
        if(exist(theFolder, 'dir'))
            if(strcmp( oneEntry(1), '+'))
                handles.jvx_struct.subprojects = [handles.jvx_struct.subprojects oneEntry(2:end)];
            end
        end
    end
end

 % =========================================================
% Program connections
% =========================================================

% Default connection: audio device -> audio node -> audio device
% Default connection rule is handled by underlying C system
% It may however be possible to add a rule here for the first time

%[suc, err] = handles.hostcall('add_connection_rule', 'The Default Audio Connection Rule', ...
%	'audio device<dontcare,dontcare>', '*', 'default',   ...
%	'Bridge_dev_to_node' , 'audio device<retain,retain>', '*', 'default',  'audio node<retain,retain>', '*', 'default', ...
%	'Bridge_node_to_dev' , 'audio node<retain,retain>', '*', 'default',  'audio device<retain,retain>', '*', 'default');
%if(~suc)
%    jvxJvxHost.jvx_display_error(12, 'jvx_init.m', ['Failed to set default connection rule: ' err.DESCRIPTION_STRING]);
%end
if(isa(handles.local.cb_conn, 'function_handle'))
    try
        handles.local.cb_conn();
    catch ME
        a = functions(handles.local.cb_conn);
        disp(['Function <' a.function '> failed, reason: ' ME.message]);
        for(ind=1:size(ME.stack,1))
            disp([num2str(ind) ')--> ' ME.stack(ind).file ', ' ME.stack(ind).name ', line ' num2str(ME.stack(ind).line)]);
        end
    end
end

 [a b]=  handles.hostcall('read_config_file');
 if(~a)
     jvxJvxHost.jvx_display_error(1, mfilename, b.DESCRIPTION_STRING);
 else
     handles.jvx_struct.valid = true;
 end
 
% ============================================================
% Obtain information about host
% ============================================================
[a b]= handles.hostcall('info');
if(~a)
    jvxJvxHost.jvx_display_error('Error code 0x3',b);
else
    handles.jvx_struct.info = b;
end

% ============================================================
% Select the generic wrapper audio technology
% ============================================================
handles.jvx_struct.technologies.allTechnologies = struct2cell(handles.jvx_struct.info.COMPONENTS.JVX_COMPONENT_AUDIO_TECHNOLOGY);
handles.jvx_struct.technologies.id_wrapper = 0;
idSelectedTechnology = 0;
idStateSelectedTechnology = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_NONE');
for(ind=1:size(handles.jvx_struct.technologies.allTechnologies,1))
    descrTech = handles.jvx_struct.technologies.allTechnologies{ind}.DESCRIPTOR_STRING;
    if(strcmp(descrTech, 'JVX_GENERIC_WRAPPER') == 1)
        handles.jvx_struct.technologies.id_wrapper = jvxJvxHost.jvx_mat_2_c_idx(ind);
    end
    if(jvxBitField.jvx_value32(handles.jvx_struct.technologies.allTechnologies{ind}.STATE_BITFIELD) >= jvxBitField.jvx_value32(id_check_selected))
        if(idSelectedTechnology == 0)
            idSelectedTechnology =jvxJvxHost.jvx_mat_2_c_idx(ind);
            idStateSelectedTechnology = handles.jvx_struct.technologies.allTechnologies{ind}.STATE_BITFIELD;
        else
            error('Unexpected');
        end
    end
end

% ===========================================
% Select the generic wrapper audio technology - if not done so before
% ===========================================
if(handles.jvx_struct.technologies.id_wrapper >= 1)

    if(idSelectedTechnology > 0)
        
        % In this case, there was a selected technology
        if(idSelectedTechnology ~= handles.jvx_struct.technologies.id_wrapper )
            
            % If not the generic wrapper, unselect it at first
            disp(['Warning: need to unselect previously selected technology ' handles.jvx_struct.technologies.allTechnologies{jvxJvxHost.jvx_c_2_mat_idx(idSelectedTechnology)}.DESCRIPTION_STRING ' at first.']);
             [a b]= handles.hostcall('unselect_component', handles.jvx_struct.technologies.comp_type);
             if(~a)
             end
             idStateSelectedTechnology =  jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_NONE');
        else
            
            % Right technology picked before, let us check if we need to
            % select it at first to be at least in min state
        end
    end
    
    if(jvxBitField.jvx_value32(idStateSelectedTechnology) < jvxBitField.jvx_value32(id_check_selected))
        [a b]= handles.hostcall('select_component', handles.jvx_struct.technologies.comp_type, handles.jvx_struct.technologies.id_wrapper);
        if(~a)
            jvxJvxHost.jvx_display_error(4, 'jvxJvxHost.init', b.DESCRIPTION_STRING);
        end
    end
        
    % Read in the current system status struct
    [a b]= handles.hostcall('info');
    if(~a)
        jvxJvxHost.jvx_display_error('Error code 0x6',b);
    else
        handles.jvx_struct.info = b;
    end
    
else
    jvxJvxHost.jvx_display_error('Error code 0x7');
end
 
 % =========================================================
 % Sub UI control
 % =========================================================

 hObject_subui = handles.subui.hdl;
  handles_subui = guidata(hObject_subui);
  [hObject_subui, handles_subui] = jvxJvxHostap.jvx_init(hObject_subui, handles_subui, ...
      handles.jvx_struct.technologies.comp_type, ...
      handles.jvx_struct.devices.comp_type);
  
  guidata(hObject_subui, handles_subui);
  
   % =========================================================
   hObject_subui2 = handles.subui2.hdl;
  handles_subui2 = guidata(hObject_subui2);
  [hObject_subui2, handles_subui2] = jvxJvxHostmi.jvx_init(hObject_subui2, handles_subui2);
  
  guidata(hObject_subui2, handles_subui2);
  