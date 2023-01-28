function [hObject, handles] = jvx_refresh_props_run(hObject, handles)

handles.jvx_struct.properties.device.id_in_gain = -1;

% ===========================================
% Obtain info from the sub system
% ===========================================
[a b]= jvxJvxHostRt('info');
if(a)
    
    % Copy struct to display current state of subsystem
    info = b;
    
    % Target states active and selected
    id_check_active = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_ACTIVE');
    handles.jvx_struct.hostState = info.STATE_INT32;
    
    % Check state of host
    if(jvxBitField.jvx_compare(info.STATE_BITFIELD, id_check_active))
                       
        % Obtain content of properties
        [a b]= jvxJvxHostRt('info_properties', handles.jvx_struct.technologies.comp_type);
        if(a)
            
            % Copy properties
            props = b;
            
            % Obtain property unique id for all props in use
            [handles.jvx_struct.properties.technology.id_input_files, handles.jvx_struct.properties.technology.cat_input_files, handles.jvx_struct.properties.technology.tp_input_files] = jvxJvxHost.jvx_find_property_id_descriptor(props, '/JVX_GENW_INPUT_FILES');
            [handles.jvx_struct.properties.technology.id_input_files_lengths, handles.jvx_struct.properties.technology.cat_input_files_lengths, handles.jvx_struct.properties.technology.tp_input_files_lengths] = jvxJvxHost.jvx_find_property_id_descriptor(props, '/JVX_GENW_INFILE_LENGTHS');
            [handles.jvx_struct.properties.technology.id_input_files_rates, handles.jvx_struct.properties.technology.cat_input_files_rates, handles.jvx_struct.properties.technology.tp_input_files_rates] = jvxJvxHost.jvx_find_property_id_descriptor(props, '/JVX_GENW_INFILE_RATES');
            [handles.jvx_struct.properties.technology.id_output_files, handles.jvx_struct.properties.technology.cat_output_files, handles.jvx_struct.properties.technology.tp_output_files] = jvxJvxHost.jvx_find_property_id_descriptor(props, '/JVX_GENW_OUTPUT_FILES');
                
            % In state active and higher, get properties
            [a b]= jvxJvxHostRt('info_properties', handles.jvx_struct.devices.comp_type);
            if(a)
                
                % Copy the property structs
                props = b;
                                                       
                [handles.jvx_struct.properties.device.id_in_gain, handles.jvx_struct.properties.device.cat_in_gain, handles.jvx_struct.properties.device.tp_in_gain] = jvxJvxHost.jvx_find_property_id_descriptor(props, '/JVX_GENW_DEV_IGAIN');
            else
                % ERROR
            end
        end        
    end
else
    jvxJvxHostRt.jvx_display_error('Error code 0x8', b);
end
