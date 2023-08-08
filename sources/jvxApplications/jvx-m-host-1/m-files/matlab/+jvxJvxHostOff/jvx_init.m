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

handles.jvx_struct.images.notready = imread([jvx_start_path '/images/icon_notready.png'], 'png');
handles.jvx_struct.images.ready = imread([jvx_start_path '/images/icon_ready.png'], 'png');
handles.jvx_struct.images.proc = imread([jvx_start_path '/images/icon_proc.png'], 'png');

handles.jvx_struct.subprojects = {};
handles.jvx_struct.subpath = [fileparts(path) '/subprojects-audionode'];
%subfolders = ls(handles.jvx_struct.subpath);
subfolders = dir(handles.jvx_struct.subpath);
for(ind=1:size(subfolders,1))
    if(subfolders(ind).isdir)
        oneEntry = strtrim(subfolders(ind).name);
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
end

handles.jvx_struct.allowStart = true;

% Processing data
handles.jvx_struct.properties_run.active = false;

handles.jvx_struct.data.input.data = [];
handles.jvx_struct.data.input.rate = 48000;
handles.jvx_struct.data.input.vars = '';

handles.jvx_struct.data.output.data = [];
handles.jvx_struct.data.output.rate = 48000;

handles.jvx_struct.data.channels.plot.id_user = -1;

handles.jvx_struct.properties.channels.input.id_user = -1;
handles.jvx_struct.properties.channels.output.id_user = -1;

handles.jvx_struct.data.player = [];
handles.jvx_struct.data.stereo = true;

handles.jvx_struct.data.output.channels.selection = -1;
handles.jvx_struct.data.output.channels.data = [];

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
    jvxJvxHostOff.jvx_display_error('Error code 0x3',b);
else
    handles.jvx_struct.info = b;
end

% ============================================================
% Select the generic wrapper audio technology
% ============================================================
handles.jvx_struct.technologies.allTechnologies = struct2cell(handles.jvx_struct.info.COMPONENTS.JVX_COMPONENT_AUDIO_TECHNOLOGY);
if(size(handles.jvx_struct.technologies.allTechnologies,2) == 1)
    idSelectedTechnology = -1;
    idStateSelectedTechnology = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_NONE');
    
    descrTech = handles.jvx_struct.technologies.allTechnologies{1}.DESCRIPTOR_STRING;
    if(strcmp(descrTech, 'JVX_OFFLINE_AUDIO') == 1)
        idSelectedTechnology = jvxJvxHost.jvx_mat_2_c_idx(1);
        idStateSelectedTechnology = handles.jvx_struct.technologies.allTechnologies{1}.STATE_BITFIELD;
    else
        error('Unexpected');
    end
end

% ===========================================
% Select the offline audio technology - if not done so before
% ===========================================
if(idSelectedTechnology == 0)
    
    % Right technology picked before, let us check if we need to
    % select it at first to be at least in min state
    id_check_selected = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_SELECTED');
    id_check_active = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_ACTIVE');
    
    if(jvxBitField.jvx_value32(idStateSelectedTechnology) < jvxBitField.jvx_value32(id_check_selected))
        [a b]= handles.hostcall('select_component', handles.jvx_struct.technologies.comp_type, idSelectedTechnology);
        if(~a)
            jvxJvxHost.jvx_display_error(4, 'jvxJvxHost.init', b.DESCRIPTION_STRING);
        end
    end
    
    if(jvxBitField.jvx_value32(idStateSelectedTechnology) < jvxBitField.jvx_value32(id_check_active))
        [a b]= handles.hostcall('activate_component', handles.jvx_struct.technologies.comp_type);
        if(~a)
            jvxJvxHostOff.jvx_display_error(4, 'jvxJvxHost.init', b.DESCRIPTION_STRING);
        end
    end
    
    % ========================================================
    % Next step: Activate the single offline device
    % ========================================================
    % Read in the current system status struct
    [a b]= handles.hostcall('info');
    if(~a)
        jvxJvxHostOff.jvx_display_error('Error code 0x6',b);
    else
        handles.jvx_struct.info = b;
    end
    
    handles.jvx_struct.technologies.allDevices = struct2cell(handles.jvx_struct.info.COMPONENTS.JVX_COMPONENT_AUDIO_DEVICE);
    if(size(handles.jvx_struct.technologies.allDevices,2) == 1)
        idSelectedDevice = -1;
        idStateSelectedDevice = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_NONE');
        
        descrDev = handles.jvx_struct.technologies.allDevices{1}.DESCRIPTOR_STRING;
        if(strcmp(descrDev, 'JVX_OFFLINE_AUDIO_DEVICE') == 1)
            idSelectedDevice = jvxJvxHost.jvx_mat_2_c_idx(1);
            idStateSelectedDevice = handles.jvx_struct.technologies.allTechnologies{1}.STATE_BITFIELD;
        else
            error('Unexpected');
        end
        
        
        if(jvxBitField.jvx_value32(idStateSelectedDevice) < jvxBitField.jvx_value32(id_check_selected))
            [a b]= handles.hostcall('select_component', handles.jvx_struct.devices.comp_type, idSelectedDevice);
            if(~a)
                jvxJvxHostOff.jvx_display_error(4, 'jvxJvxHost.init', b.DESCRIPTION_STRING);
            end
        end
        
        if(jvxBitField.jvx_value32(idStateSelectedTechnology) < jvxBitField.jvx_value32(id_check_active))
            [a b]= handles.hostcall('activate_component', handles.jvx_struct.devices.comp_type);
            if(~a)
                jvxJvxHostOff.jvx_display_error(4, 'jvxJvxHost.init', b.DESCRIPTION_STRING);
            end
        end
    else %if(size(handles.jvx_struct.technologies.allDevices,2) == 1)
        error('Unexpected');
    end %if(size(handles.jvx_struct.technologies.allDevices,2) == 1)
    
    [a b]= handles.hostcall('info');
    if(~a)
        jvxJvxHostOff.jvx_display_error('Error code 0x6',b);
    else
        handles.jvx_struct.info = b;
    end
else
    jvxJvxHostOff.jvx_display_error('Error code 0x7');
end
 
 % =========================================================
 % Sub UI control
 % =========================================================
   hObject_subui2 = handles.subui2.hdl;
  handles_subui2 = guidata(hObject_subui2);
  [hObject_subui2, handles_subui2] = jvxJvxHostmi.jvx_init(hObject_subui2, handles_subui2);
  
  guidata(hObject_subui2, handles_subui2);

