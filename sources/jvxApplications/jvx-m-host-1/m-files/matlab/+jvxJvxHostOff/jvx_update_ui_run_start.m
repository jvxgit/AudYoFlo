function  [hObject, handles] = jvx_update_ui_run_start(hObject, handles)

if(handles.jvx_struct.properties_run.device.id_in_act_level  >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties_run.device.cat_in_act_level, handles.jvx_struct.properties_run.device.id_in_act_level);
    if(a)
        theProp = ones(size(b.SUBFIELD));;
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties_run.device.cat_in_act_level, handles.jvx_struct.properties_run.device.id_in_act_level, theProp);
    else
        % Error case
    end
end

if(handles.jvx_struct.properties_run.device.id_out_act_level >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties_run.device.cat_out_act_level, handles.jvx_struct.properties_run.device.id_out_act_level);
    if(a)
        theProp = ones(size(b.SUBFIELD));;
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties_run.device.cat_out_act_level, handles.jvx_struct.properties_run.device.id_out_act_level, theProp);
    else
        % Error case
    end
end

% ====================================================
% Some channel selection specific stuff
% ====================================================

if(handles.jvx_struct.properties.device.id_in_channels >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_in_channels, handles.jvx_struct.properties.device.id_in_channels);
    if(a)
        txtField = b.SUBFIELD.OPTIONS;
        selection = b.SUBFIELD.SELECTION_BITFIELD;
        handles.jvx_struct.properties_run.channel_map_in = zeros(1, size(txtField,1));
        cnt = 1;
        for(ind = 1:size(txtField,1))
            if(jvxJvxHost.jvx_test_bit(selection, ind-1))
                handles.jvx_struct.properties_run.channel_map_in(ind) = cnt;
                cnt = cnt + 1;
            end
        end
    end
end

if(handles.jvx_struct.properties.device.id_out_channels >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_out_channels, handles.jvx_struct.properties.device.id_out_channels);
    if(a)
        txtField = b.SUBFIELD.OPTIONS;
        selection = b.SUBFIELD.SELECTION_BITFIELD;
        handles.jvx_struct.properties_run.channel_map_out = zeros(1, size(txtField,1));
        cnt = 1;
        for(ind = 1:size(txtField,1))
            if(jvxJvxHost.jvx_test_bit(selection, ind-1))
                handles.jvx_struct.properties_run.channel_map_out(ind) = cnt;
                cnt = cnt + 1;
            end
        end
    end
end



