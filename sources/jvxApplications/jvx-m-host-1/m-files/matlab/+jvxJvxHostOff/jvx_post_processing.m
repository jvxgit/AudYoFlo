function     [hObject, handles] = jvx_post_processing(hObject, handles, skip_save_data)

global inProcessing;

if(~skip_save_data)
    handles.jvx_struct.data.output.data =  jvxJvxHost.jvx_data_format_field_set(inProcessing.out_data,  inProcessing.format_id, false, handles.jvx_struct.configuredForDouble);
    handles.jvx_struct.data.output.rate = inProcessing.in_rate;
    
    txt = '';
    if(handles.jvx_struct.properties.device.id_mat_out_text >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_out_text, handles.jvx_struct.properties.device.id_mat_out_text);
        if(~a)
            error('Unexpected');
        else
            txt =b.SUBFIELD;
        end
    end
    
    isFile = false;
    if(handles.jvx_struct.properties.device.id_mat_out_is_file)
        [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_out_is_file, handles.jvx_struct.properties.device.id_mat_out_is_file);
        if(~a)
            error('Unexpected');
        else
            isFile = (jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD) > 0);
        end
    end
    
    if(~isempty(txt))
        if(isFile)
            audiowrite(txt, handles.jvx_struct.data.output.data', handles.jvx_struct.data.output.rate);
        else
            eval(['global ' txt ';']);
            eval([txt ' = handles.jvx_struct.data.output.data;']);
        end
    end
end

% Obtain references to all relevant properties
[hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);

% Show current status
[hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);

