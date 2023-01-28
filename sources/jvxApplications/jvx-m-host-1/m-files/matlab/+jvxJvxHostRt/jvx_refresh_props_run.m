function [hObject, handles] = jvx_refresh_props_run(hObject, handles, onStart)

handles.jvx_struct.properties_run.device.id_out_act_level = -1;
handles.jvx_struct.properties_run.device.id_in_level_avrg = -1;
handles.jvx_struct.properties_run.device.id_out_level_avrg = -1;
handles.jvx_struct.properties_run.device.id_in_level_max = -1;
handles.jvx_struct.properties_run.device.id_out_level_max = -1;
handles.jvx_struct.properties_run.device.id_in_gain = -1;
handles.jvx_struct.properties_run.device.id_out_gain = -1;
handles.jvx_struct.properties_run.device.id_direct_out = -1;
handles.jvx_struct.properties_run.node.offline_node.id_lost_buffers = -1;

        
props_tech = [];
props_dev = [];

if(onStart)
    % ===========================================
    % Obtain info from the sub system
    % ===========================================
    [a b]= handles.hostcall('info');
    if(a)
        
        % Copy struct to display current state of subsystem
        info = b;
        
        % Target states active and selected
        id_check_active = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_ACTIVE');
        id_check_selected = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_SELECTED');
        handles.jvx_struct.hostState = info.STATE_BITFIELD;
        
        % Check state of host
        if(jvxBitField.jvx_compare(info.STATE_BITFIELD, id_check_active))
            
            % Obtain content of properties
            [a b]= handles.hostcall('info_properties', handles.jvx_struct.technologies.comp_type);
            if(a)
                
                % Copy properties
                props_tech = b;
                
                % Obtain property unique id for all props in use
                
                % In state active and higher, get properties
                [a b]= handles.hostcall('info_properties', handles.jvx_struct.devices.comp_type);
                if(a)
                    
                    % Copy the property structs
                    props_dev = b;
                    
                    % Activate update of input/output level 
                    [handles.jvx_struct.properties_run.device.id_in_act_level, handles.jvx_struct.properties_run.device.cat_in_act_level, handles.jvx_struct.properties_run.device.tp_in_act_level] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW_ACT_ILEVEL');
                    [handles.jvx_struct.properties_run.device.id_out_act_level, handles.jvx_struct.properties_run.device.cat_out_act_level, handles.jvx_struct.properties_run.device.tp_out_act_level] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW_ACT_OLEVEL');
                    
                    [handles.jvx_struct.properties_run.device.id_in_level_avrg, handles.jvx_struct.properties_run.device.cat_in_level_avrg, handles.jvx_struct.properties_run.device.tp_in_level_avrg] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW_DEV_ILEVEL');
                    [handles.jvx_struct.properties_run.device.id_out_level_avrg, handles.jvx_struct.properties_run.device.cat_out_level_avrg, handles.jvx_struct.properties_run.device.tp_out_level_avrg] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW_DEV_OLEVEL');
                    [handles.jvx_struct.properties_run.device.id_in_level_max, handles.jvx_struct.properties_run.device.cat_in_level_max, handles.jvx_struct.properties_run.device.tp_in_level_max] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW_DEV_ILEVEL_MAX');
                    [handles.jvx_struct.properties_run.device.id_out_level_max, handles.jvx_struct.properties_run.device.cat_out_level_max, handles.jvx_struct.properties_run.device.tp_out_level_max] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW_DEV_OLEVEL_MAX');
                end
                
                [a b]= handles.hostcall('info_properties', handles.jvx_struct.nodes.comp_type);
                if(a)
                    props_node = b;
                    if(handles.jvx_struct.properties.node.feature_hookup_audio_node)
                        [handles.jvx_struct.properties_run.node.offline_node.id_lost_buffers,handles.jvx_struct.properties_run.node.offline_node.cat_lost_buffers, handles.jvx_struct.properties_run.node.offline_node.tp_lost_buffers] = jvxJvxHost.jvx_find_property_id_descriptor(props_node, '/JVX_HOOKUP_LOST_FRAMES');
                    end
                else
                    % ERROR
                end
            end
        end
    else
        jvxJvxHostRt.jvx_display_error('Error code 0x8', b);
    end
end

% Refresh properties in sub ui
hObject_subui = handles.subui.hdl;
handles_subui = guidata(hObject_subui);
[hObject_subui, handles_subui] = jvxJvxHostap.jvx_refresh_props_run(hObject_subui, handles_subui, onStart, props_tech, props_dev);
guidata(hObject_subui, handles_subui);
  

