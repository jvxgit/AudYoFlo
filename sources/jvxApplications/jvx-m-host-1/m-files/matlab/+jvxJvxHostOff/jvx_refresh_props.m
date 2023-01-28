function [hObject, handles] = jvx_refresh_props(hObject, handles)

% ===========================================
% Set all property ids to neutral values
% ===========================================           

handles.jvx_struct.configuredForDouble = true; % Assume double configured system

handles.jvx_struct.properties.device.id = -1;
handles.jvx_struct.properties.device.id_in_channels = -1;
handles.jvx_struct.properties.device.id_out_channels = -1;
handles.jvx_struct.properties.device.id_srate = -1;
handles.jvx_struct.properties.device.id_bsize = -1;
handles.jvx_struct.properties.device.id_formats = -1;
handles.jvx_struct.properties.device.id_in_channels_set = -1;
handles.jvx_struct.properties.device.id_out_channels_set = -1;

handles.jvx_struct.properties.device.id_mat_in_text = -1;
handles.jvx_struct.properties.device.id_mat_in_is_file = -1;
handles.jvx_struct.properties.device.id_mat_in_rate = -1;

handles.jvx_struct.properties.device.id_mat_out_text = -1;
handles.jvx_struct.properties.device.id_mat_out_is_file = -1;

handles.jvx_struct.properties.device.id_mat_play_stereo = -1;
handles.jvx_struct.properties.device.id_mat_export_text = -1;
                                 
handles.jvx_struct.sequencer.iamprocessing = false;
handles.jvx_struct.sequencer.system_ready_for_processing.node_ready = false;

handles.jvx_struct.sequencer.system_ready_for_processing.chain_ready = false;

handles.jvx_struct.properties.node.feature_ext_call_audio_node  = false;
handles.jvx_struct.properties.node.id = -1;

handles.jvx_struct.properties.node.offline_node.id_start_function = -1;
handles.jvx_struct.properties.node.offline_node.id_process_function = -1;
handles.jvx_struct.properties.node.offline_node.id_b4_process_function = -1;
handles.jvx_struct.properties.node.offline_node.id_stop_function = -1;
handles.jvx_struct.properties.node.offline_node.id_subproject_name = -1;
handles.jvx_struct.properties.node.offline_node.id_operation_mode = -1;
handles.jvx_struct.properties.node.offline_node.id_engange_matlab = -1;

% Flags etc propagated to sub ui
in_processing = false;
state_technology_active_higher = false;
state_device_active_higher = false;
props_dev = [];

% ===========================================
% Empty list of subcomponents
% ===========================================
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
        [handles.jvx_struct.properties.technology.state, handles.jvx_struct.properties.technology.id] = ...
			jvxJvxHost.jvx_find_state_components(handles, info.COMPONENTS.JVX_COMPONENT_AUDIO_TECHNOLOGY);
        
        % ========================================
        % Obtain state of active node component  
        % ========================================
        
        % Obtain target property value for external call feature
        bset_feature_external_call = jvxJvxHost.jvx_lookup_type_id__name('jvxFeatureClassType', 'JVX_FEATURE_CLASS_EXTERNAL_CALL');
        
        % Get the state and the id of the current node. ALso, the component
        % feature set is evaluated to find out if the feature for external
        % calls is supported
        [handles.jvx_struct.properties.node.state, handles.jvx_struct.properties.node.id, handles.jvx_struct.properties.node.feature_ext_call_audio_node] = ...
			jvxJvxHost.jvx_find_state_components(handles, info.COMPONENTS.JVX_COMPONENT_AUDIO_NODE, bset_feature_external_call);

        % List of all components
        handles.jvx_struct.properties.nodes = jvxJvxHost.jvx_find_names_components(info.COMPONENTS.JVX_COMPONENT_AUDIO_NODE);
        
        % ========================================
        % If device is active, it might be started
        % ========================================
                        
        if(jvxBitField.jvx_compare(handles.jvx_struct.properties.node.state, id_check_active))
            handles.jvx_struct.sequencer.system_ready_for_processing.node_ready = true;
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
                
                % Check state greater equal active
                if(jvxBitField.jvx_value32(handles.jvx_struct.properties.technology.state) >= jvxBitField.jvx_value32(id_check_active))
                    
                    % For sub ui
                    state_technology_active_higher = true;

                    % Get the names of all devices if available
                    handles.jvx_struct.properties.devices = jvxJvxHost.jvx_find_names_components(info.COMPONENTS.JVX_COMPONENT_AUDIO_DEVICE);
                                        
                    % Next, check device
                    [handles.jvx_struct.properties.device.state, handles.jvx_struct.properties.device.id] = jvxJvxHost.jvx_find_state_components(handles, info.COMPONENTS.JVX_COMPONENT_AUDIO_DEVICE);
                    if(jvxBitField.jvx_value32(handles.jvx_struct.properties.device.state) >= jvxBitField.jvx_value32(id_check_active))
        
                        if(jvxBitField.jvx_value32(handles.jvx_struct.properties.device.state) >= jvxBitField.jvx_value32(id_check_active))
                            % State device for sub ui
                            state_device_active_higher = true;
                        end
                        
                        % If device is active, it might be started
                        if(handles.jvx_struct.properties.device.state == id_check_active)
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
                            [handles.jvx_struct.properties.device.id_in_channels, handles.jvx_struct.properties.device.cat_in_channels, handles.jvx_struct.properties.device.tp_in_channels] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/system/sel_input_channels');
                            [handles.jvx_struct.properties.device.id_out_channels, handles.jvx_struct.properties.device.cat_out_channels, handles.jvx_struct.properties.device.tp_out_channels]  = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/system/sel_output_channels');
                            [handles.jvx_struct.properties.device.id_srate, handles.jvx_struct.properties.device.cat_srate, handles.jvx_struct.properties.device.tp_srate] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/system/rate');
                            [handles.jvx_struct.properties.device.id_bsize, handles.jvx_struct.properties.device.cat_bsize, handles.jvx_struct.properties.device.tp_bsize] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/system/framesize');
                            [handles.jvx_struct.properties.device.id_formats, handles.jvx_struct.properties.device.cat_formats, handles.jvx_struct.properties.device.tp_formats] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/system/sel_dataformat');      

                            [handles.jvx_struct.properties.device.id_in_channels_set, handles.jvx_struct.properties.device.cat_in_channels_set, handles.jvx_struct.properties.device.tp_in_channels_set] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_IN_CHANNELS_MATLAB');      
                            [handles.jvx_struct.properties.device.id_out_channels_set, handles.jvx_struct.properties.device.cat_out_channels_set, handles.jvx_struct.properties.device.tp_out_channels_set] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_OUT_CHANNELS_MATLAB');      
                            
                            [handles.jvx_struct.properties.device.id_mat_in_text, handles.jvx_struct.properties.device.cat_mat_in_text, handles.jvx_struct.properties.device.tp_mat_in_text] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_MAT_IN_TEXT');  
                            [handles.jvx_struct.properties.device.id_mat_in_is_file, handles.jvx_struct.properties.device.cat_mat_in_is_file, handles.jvx_struct.properties.device.tp_mat_in_is_file] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_MAT_IN_IS_FILE');  
                            [handles.jvx_struct.properties.device.id_mat_in_rate, handles.jvx_struct.properties.device.cat_mat_in_rate, handles.jvx_struct.properties.device.tp_mat_in_rate] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_MAT_IN_RATE');  

                            [handles.jvx_struct.properties.device.id_mat_out_text, handles.jvx_struct.properties.device.cat_mat_out_text, handles.jvx_struct.properties.device.tp_mat_out_text] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_MAT_OUT_TEXT');  
                            [handles.jvx_struct.properties.device.id_mat_out_is_file, handles.jvx_struct.properties.device.cat_mat_out_is_file, handles.jvx_struct.properties.device.tp_mat_out_is_file] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_MAT_OUT_IS_FILE');  
                            
                             [handles.jvx_struct.properties.device.id_mat_play_stereo, handles.jvx_struct.properties.device.cat_mat_play_stereo, handles.jvx_struct.properties.device.tp_mat_play_stereo] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_MAT_PLAY_STEREO');
                             [handles.jvx_struct.properties.device.id_mat_export_text, handles.jvx_struct.properties.device.cat_mat_export_text, handles.jvx_struct.properties.device.tp_mat_export_text] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_MAT_EXPORT_TEXT');
                            


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
    jvxJvxHostOff.jvx_display_error('Error code 0x8', b);
end

[a, b] = handles.hostcall('check_ready_for_start');
if(a)
    if(b.ISREADY_BOOL)
        handles.jvx_struct.sequencer.system_ready_for_processing.chain_ready = true;
    end
    handles.jvx_struct.sequencer.system_ready_for_processing.chain_ready_reason_if_not = b.REASON_IF_NOT_READY_STRING;
end     

 % ===============================================
 % Somespecific functions for the offline audio node
 % ===============================================
if(handles.jvx_struct.properties.node.feature_ext_call_audio_node)
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

       end
       
       % Activate the "offline operation mode"
       jvxJvxHost.jvx_set_property_single_selection_mat_id(handles, handles.jvx_struct.nodes.comp_type,  handles.jvx_struct.properties.node.offline_node.cat_operation_mode, handles.jvx_struct.properties.node.offline_node.id_operation_mode, 1);
end

      
 