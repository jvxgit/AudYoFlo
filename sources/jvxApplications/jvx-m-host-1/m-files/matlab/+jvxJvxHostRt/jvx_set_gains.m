function  [hObject, handles] = jvx_set_gains(hObject, handles)

if(handles.jvx_struct.properties.device.id_in_gain >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_in_gain, handles.jvx_struct.properties.device.id_in_gain);
    if(a)
        theProp = b.SUBFIELD;
        value = 0.0;
        
        for(ind=1:size(handles.jvx_struct.properties.device.local.gain_in,2))
            mapTo = handles.jvx_struct.properties_run.channel_map_in(ind);
            if(mapTo > 0)
                theProp(mapTo) = handles.jvx_struct.properties.device.local.gain_in(ind);
            end
        end         
         
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_in_gain, handles.jvx_struct.properties.device.id_in_gain, theProp);
    else
        % Error case
    end
end

if(handles.jvx_struct.properties.device.id_out_gain >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_out_gain, handles.jvx_struct.properties.device.id_out_gain);
    if(a)
        theProp = b.SUBFIELD;
        value = 0.0;
        
        for(ind=1:size(handles.jvx_struct.properties.device.local.gain_out,2))
            mapTo = handles.jvx_struct.properties_run.channel_map_out(ind);
            if(mapTo > 0)
                theProp(mapTo) = handles.jvx_struct.properties.device.local.gain_out(ind);
            end
        end         
         
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_out_gain, handles.jvx_struct.properties.device.id_out_gain, theProp);
    else
        % Error case
    end
end

if(handles.jvx_struct.properties.device.id_direct_out >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_direct_out, handles.jvx_struct.properties.device.id_direct_out);
    if(a)
        theProp = b.SUBFIELD;
        value = 0.0;
        
        for(ind=1:size(handles.jvx_struct.properties.device.local.directIn,2))
            mapTo = handles.jvx_struct.properties_run.channel_map_in(ind);
            if(mapTo > 0)
                theProp(mapTo) = handles.jvx_struct.properties.device.local.directIn(ind);
            end
        end         
         
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_direct_out, handles.jvx_struct.properties.device.id_direct_out, theProp);
    else
        % Error case
    end
end
