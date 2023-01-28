function [hObject, handles] = jvx_refresh_props(hObject, handles)

% ===========================================
% Set all property ids to neutral values
% ===========================================
handles.jvx_struct.configuredForDouble = true; % Assume double configured system

handles.jvx_struct.properties.technology.id = -1;
handles.jvx_struct.properties.technology.id_technologies = -1;
handles.jvx_struct.properties.technology.id_input_files = -1;
handles.jvx_struct.properties.technology.id_output_files = -1;
            
handles.jvx_struct.properties.device.id = -1;
handles.jvx_struct.properties.device.id_in_channels = -1;
handles.jvx_struct.properties.device.id_out_channels = -1;
handles.jvx_struct.properties.device.id_srate = -1;
handles.jvx_struct.properties.device.id_bsize = -1;
handles.jvx_struct.properties.device.id_formats = -1;

handles.jvx_struct.properties.device.id_in_gain = -1;
handles.jvx_struct.properties.device.id_out_gain = -1;
handles.jvx_struct.properties.device.id_direct_out = -1;

handles.jvx_struct.sequencer.iamprocessing = false;

handles.jvx_struct.sequencer.system_ready_for_processing.device_ready = false;
handles.jvx_struct.sequencer.system_ready_for_processing.node_ready = false;

handles.jvx_struct.properties.node.id = -1;

handles.jvx_struct.properties.node.feature_hookup_audio_node = false;
handles.jvx_struct.properties.node.offline_node.id_start_function = -1;
handles.jvx_struct.properties.node.offline_node.id_process_function = -1;
handles.jvx_struct.properties.node.offline_node.id_b4_process_function = -1;
handles.jvx_struct.properties.node.offline_node.id_stop_function = -1;
handles.jvx_struct.properties.node.offline_node.id_operation_mode = -1;
handles.jvx_struct.properties.node.offline_node.id_subproject_name = -1;
handles.jvx_struct.properties.node.offline_node.id_num_buffers = -1;
handles.jvx_struct.properties.node.offline_node.id_engange_matlab = -1;
handles.jvx_struct.properties.node.offline_node.id_nrt = -1;

% Flags etc propagated to sub ui
in_processing = false;
state_technology_active_higher = false;
state_device_active_higher = false;
props_tech = [];
props_dev = [];

% ===========================================
% Empty list of subcomponents
% ===========================================
handles.jvx_struct.properties.devices = {'--'};
handles.jvx_struct.properties.nodes = {'--'};

% ===========================================
% Obtain info from the sub system
% ===========================================
[a b]= handles.hostcall('info');
if(a)
    
    % Copy struct to display current state of subsystem
    info = b;
    
    % Find configuration of C/C++ system
    if(info.FLOAT_CONFIGURATION_ID == 1)
        handles.jvx_struct.configuredForDouble = true;
    else
        handles.jvx_struct.configuredForDouble = false;
    end
    
    % Target states active and selected
    id_check_active = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_ACTIVE');
    id_check_selected = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_SELECTED');
    
    handles.jvx_struct.hostState = info.STATE_BITFIELD;
    
    % Check state of host
    if(jvxBitField.jvx_compare(info.STATE_BITFIELD, id_check_active))
                
        % Check state of sequencer first...
        [a b] = handles.hostcall('status_process_sequence');
        if(~a)
        end
        
        % Check sequencer state
        state_init_sequencer =  jvxJvxHost.jvx_lookup_type_id__name('jvxSequencerStatus', 'JVX_SEQUENCER_STATUS_NONE');
        if(b.STATUS_INT32 ~= state_init_sequencer)
            handles.jvx_struct.sequencer.iamprocessing = true;
            in_processing = true;
        end            
 
        % Obtain state of active technology component
        [handles.jvx_struct.properties.technology.state, handles.jvx_struct.properties.technology.id] = jvxJvxHost.jvx_find_state_components(handles, info.COMPONENTS.JVX_COMPONENT_AUDIO_TECHNOLOGY);
        
        % Obtain state of active node component
        bset_feature_hookup_call = jvxJvxHost.jvx_lookup_type_id__name('jvxFeatureClassType', 'JVX_FEATURE_CLASS_HOOKUP_CALL');
        [handles.jvx_struct.properties.node.state, handles.jvx_struct.properties.node.id, handles.jvx_struct.properties.node.feature_hookup_audio_node] = jvxJvxHost.jvx_find_state_components(handles, info.COMPONENTS.JVX_COMPONENT_AUDIO_NODE, bset_feature_hookup_call);
        
        [handles.jvx_struct.properties.node.state, handles.jvx_struct.properties.node.id] = jvxJvxHost.jvx_find_state_components(handles, info.COMPONENTS.JVX_COMPONENT_AUDIO_NODE);
        handles.jvx_struct.properties.nodes = jvxJvxHost.jvx_find_names_components(info.COMPONENTS.JVX_COMPONENT_AUDIO_NODE);
        % If device is active, it might be started
                        
        if(jvxBitField.jvx_compare(handles.jvx_struct.properties.node.state, id_check_active))
            handles.jvx_struct.sequencer.system_ready_for_processing.node_ready = true;
            
            % In state active and higher, get properties
            [a b]= handles.hostcall('info_properties', handles.jvx_struct.nodes.comp_type);
            if(a)
                props_node = b;
                
                if(handles.jvx_struct.properties.node.feature_hookup_audio_node)
                    [handles.jvx_struct.properties.node.offline_node.id_num_buffers, handles.jvx_struct.properties.node.offline_node.cat_num_buffers, handles.jvx_struct.properties.node.offline_node.tp_num_buffers] = jvxJvxHost.jvx_find_property_id_descriptor(props_node, '/JVX_HOOKUP_NUM_BUFFERS');
                end
            end
        end
                        
        % +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        % Rules for technology states selected and higher
        % +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if(jvxBitField.jvx_value32(handles.jvx_struct.properties.technology.state) >= jvxBitField.jvx_value32(id_check_selected))
            
            % Obtain content of properties
            [a b]= handles.hostcall('info_properties', handles.jvx_struct.technologies.comp_type);
            if(a)
                
                % Copy properties
                props_tech = b;
                
                % Special rule for state selected
                if(jvxBitField.jvx_compare(handles.jvx_struct.properties.technology.state, id_check_selected))
                    [handles.jvx_struct.properties.technology.id_technologies handles.jvx_struct.properties.technology.cat_technologies] = jvxJvxHost.jvx_find_property_id_descriptor(props_tech, '/JVX_GENW_TECHNOLOGIES');
                else
                    % Special id for "restart configuration" option
                    handles.jvx_struct.properties.technology.id_technologies = -2;
                    handles.jvx_struct.properties.technology.cat_technologies = -1; % special rule since afterwards, property is invalid
                end
                    
                
                % Obtain property unique id for all props in use
                [handles.jvx_struct.properties.technology.id_input_files, handles.jvx_struct.properties.technology.cat_input_files, handles.jvx_struct.properties.technology.tp_input_files] = jvxJvxHost.jvx_find_property_id_descriptor(props_tech, '/select_files/input_file');
                %[handles.jvx_struct.properties.technology.id_input_files_lengths, handles.jvx_struct.properties.technology.cat_input_files_lengths, handles.jvx_struct.properties.technology.tp_input_files_lengths] = jvxJvxHost.jvx_find_property_id_descriptor(props_tech, '/JVX_GENW_INFILE_LENGTHS');
                %[handles.jvx_struct.properties.technology.id_input_files_rates, handles.jvx_struct.properties.technology.cat_input_files_rates, handles.jvx_struct.properties.technology.tp_input_files_rates] = jvxJvxHost.jvx_find_property_id_descriptor(props_tech, '/JVX_GENW_INFILE_RATES');
                [handles.jvx_struct.properties.technology.id_output_files, handles.jvx_struct.properties.technology.cat_output_files, handles.jvx_struct.properties.technology.tp_output_files] = jvxJvxHost.jvx_find_property_id_descriptor(props_tech, '/select_files/output_file');
                
                % Check state greater equal active
                if(jvxBitField.jvx_value32(handles.jvx_struct.properties.technology.state) >= jvxBitField.jvx_value32(id_check_active))
                    
                    % For sub ui
                    state_technology_active_higher = true;

                    % Get the names of all devices if available
                    handles.jvx_struct.properties.devices = jvxJvxHost.jvx_find_names_components(info.COMPONENTS.JVX_COMPONENT_AUDIO_DEVICE);
                                        
                    % Next, check device
                    [handles.jvx_struct.properties.device.state, handles.jvx_struct.properties.device.id] = jvxJvxHost.jvx_find_state_components(handles, info.COMPONENTS.JVX_COMPONENT_AUDIO_DEVICE);
                    if(jvxBitField.jvx_value32(handles.jvx_struct.properties.device.state) >= jvxBitField.jvx_value32(id_check_active))
        
                        % State device for sub ui
                        state_device_active_higher = true;                        
                        
                        % If device is active, it might be started
                        if(jvxBitField.jvx_compare(handles.jvx_struct.properties.device.state, id_check_active))
                            handles.jvx_struct.sequencer.system_ready_for_processing.device_ready = true;
                        
                            handles.jvx_struct.properties.device.is_ready = false;
                            handles.jvx_struct.properties.device.reason_if_not_ready = '';
                        end
                        
                        
                        % In state active and higher, get properties
                        [a b]= handles.hostcall('info_properties', handles.jvx_struct.devices.comp_type);
                        if(a)
                        
                            % Copy the property structs
                            props_dev = b;
                            
                            % Obtain property unique id for all props in use 
                            [handles.jvx_struct.properties.device.id_in_channels, handles.jvx_struct.properties.device.cat_in_channels, handles.jvx_struct.properties.device.tp_in_channels] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_AUDEV_INPUT_CHANNELS');
                            [handles.jvx_struct.properties.device.id_out_channels, handles.jvx_struct.properties.device.cat_out_channels, handles.jvx_struct.properties.device.tp_out_channels]  = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_AUDEV_OUTPUT_CHANNELS');
                            [handles.jvx_struct.properties.device.id_srate, handles.jvx_struct.properties.device.cat_srate, handles.jvx_struct.properties.device.tp_srate] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_AUDEV_SRATE');
                            [handles.jvx_struct.properties.device.id_bsize, handles.jvx_struct.properties.device.cat_bsize, handles.jvx_struct.properties.device.tp_bsize] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_AUDEV_BSIZE');
                            [handles.jvx_struct.properties.device.id_formats, handles.jvx_struct.properties.device.cat_formats, handles.jvx_struct.properties.device.tp_formats] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_AUDEV_FORMATS');
                            
                            [handles.jvx_struct.properties.device.id_in_gain, handles.jvx_struct.properties.device.cat_in_gain, handles.jvx_struct.properties.device.tp_in_gain] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW_DEV_IGAIN');
                            [handles.jvx_struct.properties.device.id_out_gain, handles.jvx_struct.properties.device.cat_out_gain, handles.jvx_struct.properties.device.tp_out_gain] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW_DEV_OGAIN');
                            [handles.jvx_struct.properties.device.id_direct_out, handles.jvx_struct.properties.device.cat_direct_out, handles.jvx_struct.properties.device.tp_direct_out] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW_DEV_IDIRECT');

                        else
                            % ERROR
                        end
                    end
                end
            else
                % ERROR
            end
        end
        
         % +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
         % State for sequencer
         % +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
         [a b]= handles.hostcall('status_process_sequence');
         if(a)
         end


    end
else
    jvxJvxHostRt.jvx_display_error('Error code 0x8', b);
end

 % ===============================================
 % Somespecific functions for the offline audio node
 % ===============================================
if(handles.jvx_struct.properties.node.feature_hookup_audio_node)
       [a b]= handles.hostcall('info_properties', handles.jvx_struct.nodes.comp_type);
       if(a)
           props_node = b;
           [handles.jvx_struct.properties.node.offline_node.id_start_function, handles.jvx_struct.properties.node.offline_node.cat_start_function, handles.jvx_struct.properties.node.offline_node.tp_start_function] = jvxJvxHost.jvx_find_property_id_descriptor(props_node, '/JVX_AUN_OFFLINE_START_FCT');
           [handles.jvx_struct.properties.node.offline_node.id_stop_function, handles.jvx_struct.properties.node.offline_node.cat_stop_function, handles.jvx_struct.properties.node.offline_node.tp_stop_function] = jvxJvxHost.jvx_find_property_id_descriptor(props_node, '/JVX_AUN_OFFLINE_STOP_FCT');
           [handles.jvx_struct.properties.node.offline_node.id_process_function, handles.jvx_struct.properties.node.offline_node.cat_process_function, handles.jvx_struct.properties.node.offline_node.tp_process_function] = jvxJvxHost.jvx_find_property_id_descriptor(props_node, '/JVX_AUN_OFFLINE_PROCESS_FCT');
           [handles.jvx_struct.properties.node.offline_node.id_b4_process_function, handles.jvx_struct.properties.node.offline_node.cat_b4_process_function, handles.jvx_struct.properties.node.offline_node.tp_b4_process_function] = jvxJvxHost.jvx_find_property_id_descriptor(props_node, '/JVX_AUN_OFFLINE_B4_PROCESS_FCT');
           [handles.jvx_struct.properties.node.offline_node.id_subproject_name, handles.jvx_struct.properties.node.offline_node.cat_subproject_name, handles.jvx_struct.properties.node.offline_node.tp_subproject_name] = jvxJvxHost.jvx_find_property_id_descriptor(props_node, '/JVX_AUN_OFFLINE_SUBFOLDER');
           [handles.jvx_struct.properties.node.offline_node.id_operation_mode, handles.jvx_struct.properties.node.offline_node.cat_operation_mode, handles.jvx_struct.properties.node.offline_node.tp_operation_mode] = jvxJvxHost.jvx_find_property_id_descriptor(props_node, '/JVX_AUN_OFFLINE_OPERATION_MODE');
           [handles.jvx_struct.properties.node.offline_node.id_engange_matlab, handles.jvx_struct.properties.node.offline_node.cat_engange_matlab, handles.jvx_struct.properties.node.offline_node.tp_engange_matlab] = jvxJvxHost.jvx_find_property_id_descriptor(props_node, '/JVX_AUN_ENGAGE_MATLAB');
           [handles.jvx_struct.properties.node.offline_node.id_nrt, handles.jvx_struct.properties.node.offline_node.cat_nrt, handles.jvx_struct.properties.node.offline_node.tp_nrt] = jvxJvxHost.jvx_find_property_id_descriptor(props_node, '/JVX_AUN_HOOKUP_LOAD');
       end
       
       % Activate the "hookup operation mode"
       jvxJvxHost.jvx_set_property_single_selection_mat_id(handles, handles.jvx_struct.nodes.comp_type,  handles.jvx_struct.properties.node.offline_node.cat_operation_mode, handles.jvx_struct.properties.node.offline_node.id_operation_mode, 2);
end

% Refresh properties in sub ui
hObject_subui = handles.subui.hdl;
handles_subui = guidata(hObject_subui);
handles_subui.jvx_struct.properties.node.offline_node = handles.jvx_struct.properties.node.offline_node;
[hObject_subui, handles_subui] = jvxJvxHostap.jvx_refresh_props(hObject_subui, handles_subui, in_processing, state_technology_active_higher, state_device_active_higher, props_tech, props_dev);
guidata(hObject_subui, handles_subui);
  
 