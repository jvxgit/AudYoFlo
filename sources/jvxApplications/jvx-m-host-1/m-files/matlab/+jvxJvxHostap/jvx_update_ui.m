function [hObject, handles] = jvx_update_ui(hObject, handles, is_processing, tp_tech, tp_dev, id_input_file_selected_mat, ...
    id_output_file_selected_mat, tech_active_higher, dev_active_higher, readonlyProp, text_input_file)

txtField = {'ERROR'};
idSelect = -1;
enable = 'off';

% Update all elements
if(handles.jvx_struct.state_tech_active_higher)
    if(id_input_file_selected_mat >= 0)
        [a b]= handles.hostcall('get_property_uniqueid_o', tp_tech, ...
            handles.jvx_struct.properties.technology.cat_input_file_length, ...
            handles.jvx_struct.properties.technology.id_input_file_length, ...
            id_input_file_selected_mat);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        llFile = b.SUBFIELD;
        
        [a b]= handles.hostcall('get_property_uniqueid_o', tp_tech, ...
            handles.jvx_struct.properties.technology.cat_input_file_rate, ...
            handles.jvx_struct.properties.technology.id_input_file_rate, ...
            id_input_file_selected_mat);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        rrFile = b.SUBFIELD;
        
        [a b]= handles.hostcall('get_property_uniqueid_o', tp_tech, ...
            handles.jvx_struct.properties.technology.cat_input_file_mode, ...
            handles.jvx_struct.properties.technology.id_input_file_mode, ...
            id_input_file_selected_mat);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        idxFile = jvxBitField.jvx_selectionid(b.SUBFIELD.SELECTION_BITFIELD);
        mmFile = b.SUBFIELD.OPTIONS{idxFile+1};
        
        text_input_file = [text_input_file '; duration: ' num2str(llFile/rrFile) ' sec; mode: ' mmFile];
        set(handles.text_selectedfilename, 'String', text_input_file);
    end
end
% ===================================================================
% popupmenu_lahead_input, handles.jvx_struct.properties.technology.id_input_file_lookahead
% ===================================================================
txtField = '--';
enable = 'off';
idSelect = 1;

if(handles.jvx_struct.state_tech_active_higher)
    if(id_input_file_selected_mat >= 0)
        if(handles.jvx_struct.properties.technology.id_input_file_lookahead >= 0)
            [a b]= handles.hostcall('get_property_uniqueid_o', tp_tech, ...
                handles.jvx_struct.properties.technology.cat_input_file_lookahead, ...
                handles.jvx_struct.properties.technology.id_input_file_lookahead, ...
                id_input_file_selected_mat);
            if(~a)
                error(['Unexpected error: '  b.DESCRIPTION_STRING]);
            end
            
            idSelect = double(b.SUBFIELD + 1);
            enable = 'off';
            if(~is_processing)
                enable = 'on';
            end
        end
    end
end
set(handles.popupmenu_input_lahead, 'Value', idSelect);
set(handles.popupmenu_input_lahead, 'Enable', enable);


% ===================================================================
% checkbox_input_loop, handles.jvx_struct.properties.technology.id_input_file_loop
% ===================================================================
enable = 'off';
selected = 0;

if(handles.jvx_struct.state_tech_active_higher)
    if(id_input_file_selected_mat >= 0)
        if(handles.jvx_struct.properties.technology.id_input_file_loop >= 0)
            [a b]= handles.hostcall('get_property_uniqueid_o', tp_tech, ...
                handles.jvx_struct.properties.technology.cat_input_file_loop, ...
                handles.jvx_struct.properties.technology.id_input_file_loop, ...
                id_input_file_selected_mat);
            if(~a)
                error(['Unexpected error: '  b.DESCRIPTION_STRING]);
            end
            
            selected = double(b.SUBFIELD);
            enable = 'off';
            if(~is_processing)
                enable = 'on';
            end
        end
    end
end
set(handles.checkbox_input_loop, 'Value', selected);
set(handles.checkbox_input_loop, 'Enable', enable);

% ===================================================================
% checkbox_input_boost,
% handles.jvx_struct.properties.technology.id_input_file_boost
% ===================================================================
enable = 'off';
selected = 0;

if(handles.jvx_struct.state_tech_active_higher)
    if(id_input_file_selected_mat >= 0)
        if(handles.jvx_struct.properties.technology.id_input_file_boost >= 0)
            [a b]= handles.hostcall('get_property_uniqueid_o', tp_tech, ...
                handles.jvx_struct.properties.technology.cat_input_file_boost, ...
                handles.jvx_struct.properties.technology.id_input_file_boost, ...
                id_input_file_selected_mat);
            if(~a)
                error(['Unexpected error: '  b.DESCRIPTION_STRING]);
            end
            
            selected = double(b.SUBFIELD);
            enable = 'off';
            if(~is_processing)
                enable = 'on';
            end
        end
    end
end


set(handles.checkbox_input_boost, 'Value', selected);
set(handles.checkbox_input_boost, 'Enable', enable);

enable = 'off';
if(is_processing)
    enable = 'on';
end
set(handles.pushbutton_input_restart, 'Enable', enable)
set(handles.pushbutton_input_rewind, 'Enable', enable)
set(handles.pushbutton_input_play, 'Enable', enable)
set(handles.pushbutton_input_pause, 'Enable', enable)
set(handles.pushbutton_input_fwd, 'Enable', enable)

set(handles.text_input_progress_0, 'BackgroundColor', [0 0.1 0]);
set(handles.text_input_progress_1, 'BackgroundColor', [0 0.1 0]);
set(handles.text_input_progress_2, 'BackgroundColor', [0 0.1 0]);
set(handles.text_input_progress_3, 'BackgroundColor', [0 0.1 0]);
set(handles.text_input_progress_4, 'BackgroundColor', [0 0.1 0]);
set(handles.text_input_progress_5, 'BackgroundColor', [0 0.1 0]);
 
set(handles.text_progress, 'String', '--');
% ===================================================

% popupmenu_lahead_output
txtField = '--';
enable = 'off';
idSelect = 1;

if(handles.jvx_struct.state_tech_active_higher)
    if(id_output_file_selected_mat >= 0)
        if(handles.jvx_struct.properties.technology.id_output_file_lookahead >= 0)
            [a b]= handles.hostcall('get_property_uniqueid_o', tp_tech, ...
                handles.jvx_struct.properties.technology.cat_output_file_lookahead, ...
                handles.jvx_struct.properties.technology.id_output_file_lookahead, ...
                id_output_file_selected_mat);
            if(~a)
                error(['Unexpected error: '  b.DESCRIPTION_STRING]);
            end
            
            idSelect = double(b.SUBFIELD + 1);
            enable = 'off';
            if(~is_processing)
                enable = 'on';
            end
        end
    end
end
set(handles.popupmenu_output_lahead, 'Value', idSelect);
set(handles.popupmenu_output_lahead, 'Enable', enable);

% ===================================================
% popupmenu_output_chans, id_output_chans
% ===================================================
txtField = '--';
enable = 'off';
idSelect = 1;

if(handles.jvx_struct.state_tech_active_higher)
    if(id_output_file_selected_mat >= 0)
        if(handles.jvx_struct.properties.technology.id_output_file_chans >= 0)
            [a b]= handles.hostcall('get_property_uniqueid_o', tp_tech, ...
                handles.jvx_struct.properties.technology.cat_output_file_chans, ...
                handles.jvx_struct.properties.technology.id_output_file_chans, ...
                id_output_file_selected_mat);
            if(~a)
                error(['Unexpected error: '  b.DESCRIPTION_STRING]);
            end
            
            txtField = num2str(double(b.SUBFIELD));
            enable = 'off';
            if(~is_processing)
                enable = 'on';
            end
        end
    end
end

set(handles.edit_output_channels, 'Value', idSelect);
set(handles.edit_output_channels, 'String', txtField);
set(handles.edit_output_channels, 'Enable', 'off');

% ===================================================
% edit_output_rate, id_output_rate
% ===================================================
txtField = '--';
enable = 'off';

if(handles.jvx_struct.state_tech_active_higher)
    if(id_output_file_selected_mat >= 0)
        if(handles.jvx_struct.properties.technology.id_output_file_rate >= 0)
            [a b]= handles.hostcall('get_property_uniqueid_o', tp_tech, ...
                handles.jvx_struct.properties.technology.cat_output_file_rate, ...
                handles.jvx_struct.properties.technology.id_output_file_rate, ...
                id_output_file_selected_mat);
            if(~a)
                error(['Unexpected error: '  b.DESCRIPTION_STRING]);
            end
            
            txtField = num2str(double(b.SUBFIELD));
            enable = 'off';
            if(~is_processing)
                enable = 'on';
            end
        end
    end
end

set(handles.edit_output_rate, 'String', txtField);
set(handles.edit_output_rate, 'String', txtField);
set(handles.edit_output_rate, 'Enable', enable);

% ===================================================
% checkbox_output_boost, id_output_boost
% ===================================================
enable = 'off';
selected = 0;

if(handles.jvx_struct.state_tech_active_higher)
    if(id_output_file_selected_mat >= 0)
        if(handles.jvx_struct.properties.technology.id_output_file_boost >= 0)
            [a b]= handles.hostcall('get_property_uniqueid_o', tp_tech, ...
                handles.jvx_struct.properties.technology.cat_output_file_boost, ...
                handles.jvx_struct.properties.technology.id_output_file_boost, ...
                id_output_file_selected_mat);
            if(~a)
                error(['Unexpected error: '  b.DESCRIPTION_STRING]);
            end
                        
            selected = b.SUBFIELD;
            enable = 'off';
            if(~is_processing)
                enable = 'on';
            end
        end
    end
end
set(handles.checkbox_output_boost, 'Value', selected);
set(handles.checkbox_output_boost, 'Enable', enable);

% ===================================================
% checkbox_output_mode, id_output_32bit
% ===================================================
txtField = '--';
enable = 'off';
idSelect = 1;

if(handles.jvx_struct.state_tech_active_higher)
    if(id_output_file_selected_mat >= 0)
        if(handles.jvx_struct.properties.technology.id_output_file_mode >= 0)
            [a b]= handles.hostcall('get_property_uniqueid', tp_tech, ...
                handles.jvx_struct.properties.technology.cat_output_file_mode, ...
                handles.jvx_struct.properties.technology.id_output_file_mode);
            if(~a)
                error(['Unexpected error: '  b.DESCRIPTION_STRING]);
            end
            
            if(id_output_file_selected_mat <= size(b.SUBFIELD,2))
                txtField = b.SUBFIELD.OPTIONS;
                idSelect = jvxBitField.jvx_selectionid(b.SUBFIELD.SELECTION_BITFIELD) + 1; % To Matlab idx
                enable = 'off';
                if(~is_processing)
                    enable = 'on';
                end
            else
                error(['Unexpected error: '  b.DESCRIPTION_STRING]);
            end
        end
    end
end

set(handles.popupmenu_outmode, 'String', txtField);
set(handles.popupmenu_outmode, 'Value', idSelect);
set(handles.popupmenu_outmode, 'Enable', enable);

if(~is_processing)

    enable = 'off';
    if(id_output_file_selected_mat >= 0)
        enable = 'on';
    end
end

set(handles.pushbutton_output_channels_dec, 'Enable', enable);
set(handles.pushbutton_output_channels_inc, 'Enable', enable);


% ===================================================
% popupmenu_audiodev_rates,, id_aud_dev_rates
% ===================================================
txtField = '--';
enable = 'off';
idSelect = 1;

if(handles.jvx_struct.state_dev_active_higher)
    if(handles.jvx_struct.properties.device.id_aud_dev_rates >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', tp_dev, ...
            handles.jvx_struct.properties.device.cat_aud_dev_rates, ...
            handles.jvx_struct.properties.device.id_aud_dev_rates);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        txtField = b.SUBFIELD.OPTIONS;
        idSelect = double(jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD));
        enable = 'off';
        if(~is_processing)
            if(handles.jvx_struct.properties.device.at_aud_dev_rates ~= readonlyProp)
                enable = 'on';
            end
        end
    end
end

set(handles.popupmenu_audiodev_rates, 'String', txtField);
set(handles.popupmenu_audiodev_rates, 'Value', idSelect);
set(handles.popupmenu_audiodev_rates, 'Enable', enable);

% ===================================================
% edit_audiodev_rate, id_aud_dev_rate
% ===================================================
txtField = '--';
enable = 'off';

if(handles.jvx_struct.state_dev_active_higher)
    if(handles.jvx_struct.properties.device.id_aud_dev_rate >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', tp_dev, ...
            handles.jvx_struct.properties.device.cat_aud_dev_rate, ...
            handles.jvx_struct.properties.device.id_aud_dev_rate);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        txtField = num2str(b.SUBFIELD);
        enable = 'off';
        if(~is_processing)
            if(handles.jvx_struct.properties.device.at_aud_dev_rate ~= readonlyProp)
                enable = 'on';
            end
        end
    end
end

set(handles.edit_audiodev_rate, 'String', txtField);
set(handles.edit_audiodev_rate, 'Enable', enable);
     
% ===================================================
% popupmenu_audiodev_bsizes, id_aud_dev_bsizes
% ===================================================
txtField = '--';
enable = 'off';
idSelect = 1;

if(handles.jvx_struct.state_dev_active_higher)
    if(handles.jvx_struct.properties.device.id_aud_dev_bsizes >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', tp_dev, ...
            handles.jvx_struct.properties.device.cat_aud_dev_bsizes, ...
            handles.jvx_struct.properties.device.id_aud_dev_bsizes);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        txtField = b.SUBFIELD.OPTIONS;
        idSelect = double(jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD));
        enable = 'off';
        if(isempty(txtField))
            txtField = '--';
            enable = 'off';
            idSelect = 1;
        else
            if(~is_processing)
                if(handles.jvx_struct.properties.device.at_aud_dev_bsizes ~= readonlyProp)
                    enable = 'on';
                end
            end
        end
    end
end

set(handles.popupmenu_audiodev_bsizes, 'String', txtField);
set(handles.popupmenu_audiodev_bsizes, 'Value', idSelect);
set(handles.popupmenu_audiodev_bsizes, 'Enable', enable);

% ===================================================
% edit_audiodev_rate, id_aud_dev_bsize
% ===================================================
txtField = '--';
enable = 'off';


if(handles.jvx_struct.state_dev_active_higher)
    if(handles.jvx_struct.properties.device.id_aud_dev_bsize >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', tp_dev, ...
            handles.jvx_struct.properties.device.cat_aud_dev_bsize, ...
            handles.jvx_struct.properties.device.id_aud_dev_bsize);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        txtField = num2str(b.SUBFIELD);
        enable = 'off';
        if(isempty(txtField))
            txtField = '--';
            enable = 'off';
            idSelect = 1;
        else
            if(~is_processing)
                if(handles.jvx_struct.properties.device.at_aud_dev_bsize ~= readonlyProp)
                    enable = 'on';
                end
            end
        end
    end
end

set(handles.edit_audiodev_bsize, 'String', txtField);
set(handles.edit_audiodev_bsize, 'Enable', enable);
 
% ===================================================
% popupmenu_audiodev_formats, id_aud_dev_formats
% ===================================================
txtField = '--';
enable = 'off';
idSelect = 1;

if(handles.jvx_struct.state_dev_active_higher)
    if(handles.jvx_struct.properties.device.id_aud_dev_formats >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', tp_dev, ...
            handles.jvx_struct.properties.device.cat_aud_dev_formats, ...
            handles.jvx_struct.properties.device.id_aud_dev_formats);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        txtField = b.SUBFIELD.OPTIONS;
        idSelect = double(jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD));
        enable = 'off';
        if(~is_processing)
            enable = 'on';
        end
    end
end

set(handles.popupmenu_audiodev_formats, 'String', txtField);
set(handles.popupmenu_audiodev_formats, 'Value', idSelect);
set(handles.popupmenu_audiodev_formats, 'Enable', enable);

% ===================================================
% popupmenu_qresampler_in, id_qresampler_in
% ===================================================
txtField = '--';
enable = 'off';
idSelect = 1;

if(handles.jvx_struct.state_dev_active_higher)
    if(handles.jvx_struct.properties.device.id_qresampler_in >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', tp_dev, ...
            handles.jvx_struct.properties.device.cat_qresampler_in, ...
            handles.jvx_struct.properties.device.id_qresampler_in);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        txtField = b.SUBFIELD.OPTIONS;
        idSelect = double(jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD));
        enable = 'off';
        if(~is_processing)
            enable = 'on';
        end
    end
end

set(handles.popupmenu_qresampler_in, 'String', txtField);
set(handles.popupmenu_qresampler_in, 'Value', idSelect);
set(handles.popupmenu_qresampler_in, 'Enable', enable);

% ===================================================
% popupmenu_qresampler_out, id_qresampler_out
% ===================================================
txtField = '--';
enable = 'off';
idSelect = 1;

if(handles.jvx_struct.state_dev_active_higher)
    if(handles.jvx_struct.properties.device.id_qresampler_out >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', tp_dev, ...
            handles.jvx_struct.properties.device.cat_qresampler_out, ...
            handles.jvx_struct.properties.device.id_qresampler_out);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        txtField = b.SUBFIELD.OPTIONS;
        idSelect = double(jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD));
        enable = 'off';
        if(~is_processing)
            enable = 'on';
        end
    end
end

set(handles.popupmenu_qresampler_out, 'String', txtField);
set(handles.popupmenu_qresampler_out, 'Value', idSelect);
set(handles.popupmenu_qresampler_out, 'Enable', enable);
    
% ===================================================
% text_audiodev_resample, id_resample
% ===================================================
selection = 0;
enable = 'off';

if(handles.jvx_struct.state_dev_active_higher)
    if(handles.jvx_struct.properties.device.id_resampler >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', tp_dev, ...
            handles.jvx_struct.properties.device.cat_resampler, ...
            handles.jvx_struct.properties.device.id_resampler);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        % First bit: resampling on; second bit: resampling off
        if(jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD) == 1)
            selection = 1;
        else
            selection = 0;
        end
        enable = 'off';
        if(~is_processing)
            enable = 'on';
        end
    end
end

if(selection)
    set(handles.text_audiodev_resample, 'BackgroundColor', [0 1 0]); 
else
    set(handles.text_audiodev_resample, 'BackgroundColor', [0 0.1 0]);
end

% ===================================================
% text_audiodev_rebuffer, id_rebuffer
% ===================================================
selection = 0;
enable = 'off';

if(handles.jvx_struct.state_dev_active_higher)
    if(handles.jvx_struct.properties.device.id_rebuffer >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', tp_dev, ...
            handles.jvx_struct.properties.device.cat_rebuffer, ...
            handles.jvx_struct.properties.device.id_rebuffer);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        if(jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD) == 1)
            selection = 1;
        else
            selection = 0;
        end
        enable = 'off';
        if(~is_processing)
            enable = 'on';
        end
    end
end

if(selection)
    set(handles.text_audiodev_rebuffer, 'BackgroundColor', [0 1 0]); 
else
    set(handles.text_audiodev_rebuffer, 'BackgroundColor', [0 0.1 0]);
end

% ===================================================
% checkbox_audiodev_auto, id_auto_hw
% ===================================================
enable = 'off';
selected = 0;

if(handles.jvx_struct.state_dev_active_higher)
    if(handles.jvx_struct.properties.device.id_auto_hw >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', tp_dev, ...
            handles.jvx_struct.properties.device.cat_auto_hw, ...
            handles.jvx_struct.properties.device.id_auto_hw);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        if(jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD) > 0)
            selected = 1;
        end
        enable = 'off';
        if(~is_processing)
            enable = 'on';
        end
    end
end
set(handles.checkbox_audiodev_auto, 'Value', selected);
set(handles.checkbox_audiodev_auto, 'Enable', enable);

% ===================================================
% pushbutton_asio_open, id_auto_hw
% ===================================================
enable = 'off';
selected = 0;

if(handles.jvx_struct.state_dev_active_higher)
    if(handles.jvx_struct.properties.device.id_ctrl_panel >= 0)
        if(~is_processing)
            enable = 'on';
        end
    end
end
set(handles.pushbutton_asio_open, 'Enable', enable);


set(handles.text_load_0, 'BackgroundColor', [0 0.1 0]); 
set(handles.text_load_1, 'BackgroundColor', [0 0.1 0]); 
set(handles.text_load_2, 'BackgroundColor', [0 0.1 0]); 
set(handles.text_load_3, 'BackgroundColor', [0 0.1 0]); 
set(handles.text_load_4, 'BackgroundColor', [0 0.1 0]); 
set(handles.text_load_5, 'BackgroundColor', [0 0.1 0]); 

set(handles.text_load, 'String', '--');

% ===================================================
% popupmenu_alsa_access, id_qresampler_out
% ===================================================
txtField = '--';
enable = 'off';
idSelect = 1;

if(handles.jvx_struct.state_dev_active_higher)
    if(handles.jvx_struct.properties.device.id_alsa_access_type >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', tp_dev, ...
            handles.jvx_struct.properties.device.cat_alsa_access_type, ...
            handles.jvx_struct.properties.device.id_alsa_access_type);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        txtField = b.SUBFIELD.OPTIONS;
        idSelect = double(jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD));
        enable = 'off';
        if(~is_processing)
            enable = 'on';
        end
    end
end

set(handles.popupmenu_alsa_access, 'String', txtField);
set(handles.popupmenu_alsa_access, 'Value', idSelect);
set(handles.popupmenu_alsa_access, 'Enable', enable);

% ===================================================
% popupmenu_alsa_int_datatype, id_alsa_data_type
% ===================================================
txtField = '--';
enable = 'off';
idSelect = 1;

if(handles.jvx_struct.state_dev_active_higher)    
    if(handles.jvx_struct.properties.device.id_alsa_data_type >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', tp_dev, ...
            handles.jvx_struct.properties.device.cat_alsa_data_type, ...
            handles.jvx_struct.properties.device.id_alsa_data_type);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        txtField = b.SUBFIELD.OPTIONS;
        idSelect = double(jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD));
        enable = 'off';
        if(~is_processing)
            enable = 'on';
        end
    end
end

set(handles.popupmenu_alsa_int_datatype, 'String', txtField);
set(handles.popupmenu_alsa_int_datatype, 'Value', idSelect);
set(handles.popupmenu_alsa_int_datatype, 'Enable', enable);

% ===================================================
% edit_alsa_num_per_in, id_alsa_periods_in
% ===================================================
txtField = '--';
enable = 'off';


if(handles.jvx_struct.state_dev_active_higher)
    if(handles.jvx_struct.properties.device.id_alsa_periods_in >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', tp_dev, ...
            handles.jvx_struct.properties.device.cat_aud_alsa_periods_in, ...
            handles.jvx_struct.properties.device.id_alsa_periods_in);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        error('Not yet supported');
        txtField = num2str(b.SUBFIELD.value);
        enable = 'off';
        if(~is_processing)
            enable = 'on';
        end
    end
end

set(handles.edit_alsa_num_per_in, 'String', txtField);
set(handles.edit_alsa_num_per_in, 'Enable', enable);

% ===================================================
% popupmenu_alsa_num_per_out, id_alsa_periods_out
% ===================================================
% ===================================================
% edit_alsa_num_per_in, id_alsa_periods_in
% ===================================================
txtField = '--';
enable = 'off';


if(handles.jvx_struct.state_dev_active_higher)
    if(handles.jvx_struct.properties.device.id_alsa_periods_out >= 0)
        [a b]= handles.hostcall('get_property_uniqueid', tp_dev, ...
            handles.jvx_struct.properties.device.cat_aud_alsa_periods_out, ...
            handles.jvx_struct.properties.device.id_alsa_periods_out);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        error('Not yet supported');
        txtField = num2str(b.SUBFIELD.value);
        enable = 'off';
        if(~is_processing)
            enable = 'on';
        end
    end
end

set(handles.edit_alsa_num_per_out, 'String', txtField);
set(handles.edit_alsa_num_per_out, 'Enable', enable);

txtField = '--';
enable = 'off';

set(handles.edit_nrt, 'String', txtField);
set(handles.edit_nrt, 'Enable', enable);

% ===================================================
% uitable_data = {};
% cnty = 1;
% cntC = 1;
% [a b]= handles.hostcall('list_connections');
% if(a)
%    if(~isempty(b))
%        connections = struct2cell(b);
%        for(ind=1:size(connections, 2))
%            cntx = 1;
%            uitable_data{cnty,cntx} = connections{ind}.CONNECTION_PROCESS_DESCRIPTION;
%            cnty = cnty + 1;
            
%            cntx = 1;
%            uitable_data{cnty,cntx} = '    --> Master'; 
%            cntx = cntx + 1;
%            uitable_data{cnty,cntx} = connections{ind}.CONNECTION_PROCESS_MASTER.CONNECTION_PROCESS_MASTER_FACTORY_NAME;
            
%            bridges = struct2cell(connections{ind}.CONNECTION_PROCESS_BRIDGES);
%            for(jnd = 1:size(bridges,2))
%                cntx = 1;
%                cnty = cnty + 1;
%                uitable_data{cnty,cntx} = ['    --> Bridge #' num2str(jnd)]; 
%                cntx = cntx + 1;
%                uitable_data{cnty,cntx} = bridges{jnd}.CONNECTION_PROCESS_BRIDGE_DESCRIPTOR;
%                cntx = cntx + 1;
%                uitable_data{cnty,cntx} = ...
%                    ['From ' bridges{jnd}.CONNECTION_PROCESS_BRIDGE_OUTPUT_CONNECTOR.CONNECTION_PROCESS_OUTPUT_CONNECTOR_FACTORY_NAME '<' ...
%                    bridges{jnd}.CONNECTION_PROCESS_BRIDGE_OUTPUT_CONNECTOR.CONNECTION_PROCESS_OUTPUT_CONNECTOR_DESCRIPTION '>'];
%                cntx = cntx + 1;
%                uitable_data{cnty,cntx} = ...
%                    ['To ' bridges{jnd}.CONNECTION_PROCESS_BRIDGE_INPUT_CONNECTOR.CONNECTION_PROCESS_INPUT_CONNECTOR_FACTORY_NAME '<' ...
%                    bridges{jnd}.CONNECTION_PROCESS_BRIDGE_INPUT_CONNECTOR.CONNECTION_PROCESS_INPUT_CONNECTOR_DESCRIPTION '>'];
                
%            end
            
%            cntx = 1;
%            uitable_data{cnty,cntx} = '    --> Path';
%            cntx = cntx + 1;
%            uitable_data{cnty,cntx} = connections{ind}.CONNECTION_PROCESS_PATH_DESCRIPTION;
%        end
%    end
% end
% set(handles.uitable_connections, 'Data', uitable_data);

