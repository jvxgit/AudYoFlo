function [hObject, handles] = jvx_initialize_settings(hObject, handles)

txtField = '';
isFile = false;

[a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
if(~a)
    % origFile, errCode, errOperation, errTxt, errorsAsWarnings
    jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
end

if(handles.jvx_struct.properties.device.id_mat_in_text)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_text, handles.jvx_struct.properties.device.id_mat_in_text);
    if(a)
        txtField = b.SUBFIELD;
    end
end

if(handles.jvx_struct.properties.device.id_mat_in_is_file)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_is_file, handles.jvx_struct.properties.device.id_mat_in_is_file);
    if(a)
        isFile = (jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD) > 0);
    end
end

if(~isempty(txtField))
    if(isFile)
        if(exist(txtField, 'file'))
            
			%[tt, r] = audioread(txtField);
            % handles.jvx_struct.data.input.data = tt';
			% Two versions: we use the jvx-version since it should be identical to the c version
			% The jvxversion opens data in correct format
			[tt, r] = jvxReadWav(txtField);            
            handles.jvx_struct.data.input.data = tt;           
            handles.jvx_struct.data.input.rate = r;
            
        else
            disp(['Warning: Input file ' txtField ' does not exist.']);
            
            [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_text, handles.jvx_struct.properties.device.id_mat_in_text, '');
            if(~a)
                error('Unexpected');
            end
            
            [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_is_file, handles.jvx_struct.properties.device.id_mat_in_is_file, '0x0');
            if(~a)
                error('Unexpected');
            end
            
        end
        
    else
        a = whos( txtField, 'global');
        if(~isempty(a))
            if( ...
                    (strcmp(a.class, 'double') == 1) | ...
                    (strcmp(a.class, 'int64') == 1) | ...
                    (strcmp(a.class, 'int32') == 1) | ...
                    (strcmp(a.class, 'int16') == 1) | ...
                    (strcmp(a.class, 'int8') == 1))
                rate = handles.jvx_struct.data.input.rate;
                if(handles.jvx_struct.properties.device.id_mat_in_rate)
                    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_rate, handles.jvx_struct.properties.device.id_mat_in_rate);
                    if(a)
                        rate = b.SUBFIELD;
                    end
                end
                
                eval(['global ' txtField ';']);
                eval(['handles.jvx_struct.data.input.data = ' txtField ';']);
                handles.jvx_struct.data.input.rate = double(rate);
            else
                disp(['Warning: Global variable ' txtField ' is not of corect type.']);
                [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_text, handles.jvx_struct.properties.device.id_mat_in_text, '');
                if(~a)
                    warning('Failed to read property <in_text>.');
                end
                
                [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_is_file, handles.jvx_struct.properties.device.id_mat_in_is_file, 0);
                if(~a)
                    warning('Failed to read property <is_in_file>.');
                end
            end
        else
            disp(['Warning: Global variable ' txtField ' does not exist.']);
            [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_text, handles.jvx_struct.properties.device.id_mat_in_text, '');
            if(~a)
                warning('Failed to read property <in_text>.');
            end
            
            bf = jvxBitField.jvx_create();
            [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_is_file, handles.jvx_struct.properties.device.id_mat_in_is_file, bf);
            if(~a)
                warning('Failed to read property <is_in_file>.');
            end
        end
    end
    
    if(handles.jvx_struct.properties.device.id_srate > 0)
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_srate, handles.jvx_struct.properties.device.id_srate,int32(handles.jvx_struct.data.input.rate));
        if(a)
        end
    end
    
    % Setting the number of outchannels will also modify the channel set
    % mask. Hence we need to update that.
    if(handles.jvx_struct.properties.device.id_in_channels_set > 0)
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, ...
            handles.jvx_struct.properties.device.cat_in_channels_set,...
            handles.jvx_struct.properties.device.id_in_channels_set,int32(size(handles.jvx_struct.data.input.data,1)));
        if(a)
        end
    end
    
    % Obtain references to all relevant properties
    [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
    
    % Show current status
    [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    
end

[a stb]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
if(~a)
    % origFile, errCode, errOperation, errTxt, errorsAsWarnings
    jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'stop_property_group', stb.DESCRIPTION_STRING, false);
end