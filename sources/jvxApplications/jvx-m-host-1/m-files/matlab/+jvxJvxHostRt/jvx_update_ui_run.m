function  [hObject, handles] = jvx_update_ui_run(hObject, handles)

if(handles.jvx_struct.properties_run.device.id_in_level_avrg)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties_run.device.cat_in_level_avrg, handles.jvx_struct.properties_run.device.id_in_level_avrg);
    if(a)
        theProp = b.SUBFIELD;
        
        value = 0.0;
                
         idS = handles.jvx_struct.properties.channels.input.id_user;
         if(handles.jvx_struct.properties.channels.input.all)
             % Average
             value = sum(theProp) / size(theProp,2);
         else
             
             id = 0;
             if(handles.jvx_struct.properties.channels.input.id_user <= size(handles.jvx_struct.properties_run.channel_map_in,2))
                 id = handles.jvx_struct.properties_run.channel_map_in(idS);
             end
             if(id > 0)
                 if(id <= size(theProp,2))
                     value = theProp(id);
                 end
             end
         end
         
        value_dB = 10*log10(value + 1e-10);
        set(handles.text_in_0, 'BackgroundColor', [0 0.1 0]);
        set(handles.text_in_1, 'BackgroundColor', [0 0.1 0]);
        set(handles.text_in_2, 'BackgroundColor', [0 0.1 0]);
        set(handles.text_in_3, 'BackgroundColor', [0 0.1 0]);
        set(handles.text_in_4, 'BackgroundColor', [0 0.1 0]);
        set(handles.text_in_5, 'BackgroundColor', [0 0.1 0]);
        set(handles.text_in_6, 'BackgroundColor', [0 0.1 0]);
        set(handles.text_in_7, 'BackgroundColor', [0 0.1 0]);
        
        if(value_dB > -80)
            set(handles.text_in_0, 'BackgroundColor', [0 1 0])
        end
        if(value_dB > -70)
            set(handles.text_in_1, 'BackgroundColor', [0 1 0])
        end
        if(value_dB > -60)
            set(handles.text_in_2, 'BackgroundColor', [0 1 0])
        end
        if(value_dB > -50)
            set(handles.text_in_3, 'BackgroundColor', [0 1 0])
        end
        if(value_dB > -40)
            set(handles.text_in_4, 'BackgroundColor', [0 1 0])
        end
        if(value_dB > -30)
            set(handles.text_in_5, 'BackgroundColor', [0 1 0])
        end
        if(value_dB > -20)
            set(handles.text_in_6, 'BackgroundColor', [0 1 0])
        end
        if(value_dB > -10)
            set(handles.text_in_7, 'BackgroundColor', [0 1 0])
        end
        set(handles.text_in_avrg_lev, 'String', [num2str(value_dB,  '%.1f') ' dB']);

     else
        
    end
end

if(handles.jvx_struct.properties_run.device.id_out_level_avrg)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties_run.device.cat_out_level_avrg, handles.jvx_struct.properties_run.device.id_out_level_avrg);
    if(a)
         theProp = b.SUBFIELD;
         value = 0.0;
         idS = handles.jvx_struct.properties.channels.output.id_user;
         if(handles.jvx_struct.properties.channels.output.all)
             % Average
             value = sum(theProp) / size(theProp,2);
         else
             
             id = 0;
             if(handles.jvx_struct.properties.channels.output.id_user <= size(handles.jvx_struct.properties_run.channel_map_out,2))
                 id = handles.jvx_struct.properties_run.channel_map_out(idS);
             end
             if(id > 0)
                 if(id <= size(theProp,2))
                     value = theProp(id);
                 end
             end
         end
         
         value_dB = 10*log10(value + 1e-10);
        set(handles.text_out_0, 'BackgroundColor', [0 0.1 0]);
        set(handles.text_out_1, 'BackgroundColor', [0 0.1 0]);
        set(handles.text_out_2, 'BackgroundColor', [0 0.1 0]);
        set(handles.text_out_3, 'BackgroundColor', [0 0.1 0]);
        set(handles.text_out_4, 'BackgroundColor', [0 0.1 0]);
        set(handles.text_out_5, 'BackgroundColor', [0 0.1 0]);
        set(handles.text_out_6, 'BackgroundColor', [0 0.1 0]);
        set(handles.text_out_7, 'BackgroundColor', [0 0.1 0]);
        
        if(value_dB > -80)
            set(handles.text_out_0, 'BackgroundColor', [0 1 0])
        end
        if(value_dB > -70)
            set(handles.text_out_1, 'BackgroundColor', [0 1 0])
        end
        if(value_dB > -60)
            set(handles.text_out_2, 'BackgroundColor', [0 1 0])
        end
        if(value_dB > -50)
            set(handles.text_out_3, 'BackgroundColor', [0 1 0])
        end
        if(value_dB > -40)
            set(handles.text_out_4, 'BackgroundColor', [0 1 0])
        end
        if(value_dB > -30)
            set(handles.text_out_5, 'BackgroundColor', [0 1 0])
        end
        if(value_dB > -20)
            set(handles.text_out_6, 'BackgroundColor', [0 1 0])
        end
        if(value_dB > -10)
            set(handles.text_out_7, 'BackgroundColor', [0 1 0])
        end
        
        set(handles.text_out_avrg_lev, 'String', [num2str(value_dB, '%.1f') ' dB']);
    end
end

if(handles.jvx_struct.properties_run.device.id_in_level_max)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties_run.device.cat_in_level_max, handles.jvx_struct.properties_run.device.id_in_level_max);
    if(a)
         theProp = b.SUBFIELD;
         value = 0.0;

         idS = handles.jvx_struct.properties.channels.input.id_user;
         if(handles.jvx_struct.properties.channels.input.all)
             value = max(theProp);
         else

             id = 0;
             if(handles.jvx_struct.properties.channels.input.id_user <= size(handles.jvx_struct.properties_run.channel_map_in,2))
                 id = handles.jvx_struct.properties_run.channel_map_in(idS);
             end
             if(id > 0)
                 if(id <= size(theProp,2))
                     value = theProp(id);
                 end
             end
         end
         
        value_dB = 10*log10(value + 1e-10);
        set(handles.text_in_max_lev, 'String', [num2str(value_dB,  '%.1f') ' dB']);
        set(handles.text_in_max_lev, 'BackgroundColor', [1 1 1]);
        if(value > 0.98)
            set(handles.text_in_max_lev, 'BackgroundColor', [1 0 0]);
        end
        [a b] = handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties_run.device.cat_in_level_max, handles.jvx_struct.properties_run.device.id_in_level_max, theProp*0.0);
    end
end
    
if(handles.jvx_struct.properties_run.device.id_out_level_max)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties_run.device.cat_out_level_max, handles.jvx_struct.properties_run.device.id_out_level_max);
    if(a)
         theProp = b.SUBFIELD;
        value = 0.0;
        
         idS = handles.jvx_struct.properties.channels.output.id_user;
         if(handles.jvx_struct.properties.channels.output.all)
            value = max(theProp);
         else

             id = 0;
             if(handles.jvx_struct.properties.channels.output.id_user <= size(handles.jvx_struct.properties_run.channel_map_out,2))
                 id = handles.jvx_struct.properties_run.channel_map_out(idS);
             end
             if(id > 0)
                 if(id <= size(theProp,2))
                     value = theProp(id);
                 end
             end
         end
         
        value_dB = 10*log10(value + 1e-10);
        set(handles.text_out_max_lev, 'String', [num2str(value_dB, '%.1f') ' dB']);
        set(handles.text_out_max_lev, 'BackgroundColor', [1 1 1]);
        if(value > 0.98)
            set(handles.text_out_max_lev, 'BackgroundColor', [1 0 0]);
        end
         [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties_run.device.cat_out_level_max, handles.jvx_struct.properties_run.device.id_out_level_max, theProp*0.0);
    else
        
    end
end

txtField = '--';
if(handles.jvx_struct.properties_run.node.offline_node.id_lost_buffers >= 0)
     [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties_run.node.offline_node.cat_lost_buffers, handles.jvx_struct.properties_run.node.offline_node.id_lost_buffers);
     if(a)
         txtField = num2str(b.SUBFIELD);
     end
end
set(handles.edit_lost, 'String', txtField);

% Sub UI control
    inf_id_user = -1;
    [a b] = handles.hostcall('get_property_descriptor', ...
            handles.jvx_struct.technologies.comp_type, ...
            '/select_files/input_file');
    if(~a)
       error(['Unexpected error: '  b.DESCRIPTION_STRING]);
    end
    inf_id_user = jvxBitField.jvx_selectionid(b.SELECTION_BITFIELD);
    
  hObject_subui = handles.subui.hdl;
  handles_subui = guidata(hObject_subui);
  [hObject_subui, handles_subui] = jvxJvxHostap.jvx_update_ui_run(hObject_subui, handles_subui, ...
      inf_id_user, ...
      handles.jvx_struct.technologies.comp_type, ...
      handles.jvx_struct.devices.comp_type, ...
      handles.jvx_struct.nodes.comp_type); 
  guidata(hObject_subui, handles_subui);
  
  