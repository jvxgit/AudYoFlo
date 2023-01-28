function [hObject, handles] = jvx_refresh_props(hObject, handles, in_processing, ...
    state_tech_active_higher, state_dev_active_higher, props_tech, props_dev)

% ===========================================
% Set all property ids to neutral values
% ===========================================
handles.jvx_struct.properties.technology.id_input_file_lookahead = -1;
handles.jvx_struct.properties.technology.id_input_file_loop = -1;
handles.jvx_struct.properties.technology.id_input_file_boost = -1;
handles.jvx_struct.properties.technology.id_input_file_rate = -1; %'/properties_selected_input_file/samplerate'
handles.jvx_struct.properties.technology.id_input_file_length = -1; %'/properties_selected_input_file/file_length'
handles.jvx_struct.properties.technology.id_input_file_mode = -1; %'/properties_selected_input_file/mode'
handles.jvx_struct.properties.technology.id_output_file_rate = -1;
handles.jvx_struct.properties.technology.id_output_file_chans = -1;
handles.jvx_struct.properties.technology.id_output_file_lookahead = -1;
handles.jvx_struct.properties.technology.id_output_file_boost = -1;
handles.jvx_struct.properties.technology.id_output_file_mode = .1;

handles.jvx_struct.properties.device.id_aud_dev_rates = -1;
handles.jvx_struct.properties.device.id_aud_dev_rate = -1;
handles.jvx_struct.properties.device.id_aud_dev_bsizes = -1;
handles.jvx_struct.properties.device.id_aud_dev_bsize = -1;
handles.jvx_struct.properties.device.id_aud_dev_formats = -1;
handles.jvx_struct.properties.device.id_qresampler_in = -1;
handles.jvx_struct.properties.device.id_qresampler_out = -1;
handles.jvx_struct.properties.device.id_resampler = -1;
handles.jvx_struct.properties.device.id_rebuffer = -1;
handles.jvx_struct.properties.device.id_auto_hw = -1;
handles.jvx_struct.properties.device.id_ctrl_panel = -1;

handles.jvx_struct.properties.device.id_alsa_access_type = -1;
handles.jvx_struct.properties.device.id_alsa_data_type = -1;
handles.jvx_struct.properties.device.id_alsa_periods_in = -1;
handles.jvx_struct.properties.device.id_alsa_periods_out = -1;

handles.jvx_struct.properties.node.id_nrt = -1;

handles.jvx_struct.state_tech_active_higher =  state_tech_active_higher;
handles.jvx_struct.state_dev_active_higher =  state_dev_active_higher;

% ===========================================
% Set ids for properties
% ===========================================
if(handles.jvx_struct.state_tech_active_higher)
    % Obtain property unique id for all props in use
    %/properties_selected_input_file/num_channels'%
    %'/properties_selected_input_file/samplerate'
    %'/properties_selected_input_file/file_length'
    [handles.jvx_struct.properties.technology.id_input_file_lookahead, handles.jvx_struct.properties.technology.cat_input_file_lookahead, handles.jvx_struct.properties.technology.tp_input_file_lookahead] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_tech,  '/properties_selected_input_file/lookahead');
    [handles.jvx_struct.properties.technology.id_input_file_loop, handles.jvx_struct.properties.technology.cat_input_file_loop, handles.jvx_struct.properties.technology.tp_input_file_loop] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_tech, '/properties_selected_input_file/loop');
    [handles.jvx_struct.properties.technology.id_input_file_boost, ...
        handles.jvx_struct.properties.technology.cat_input_file_boost, ...
        handles.jvx_struct.properties.technology.tp_input_file_boost] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_tech, '/properties_selected_input_file/boost_prio');
    
    [handles.jvx_struct.properties.technology.id_input_file_rate, ...
        handles.jvx_struct.properties.technology.cat_input_file_rate, ...
        handles.jvx_struct.properties.technology.tp_input_file_rate] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_tech, '/properties_selected_input_file/samplerate');

    [handles.jvx_struct.properties.technology.id_input_file_length, ...
        handles.jvx_struct.properties.technology.cat_input_file_length, ...
        handles.jvx_struct.properties.technology.tp_input_file_length] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_tech, '/properties_selected_input_file/file_length');
    [handles.jvx_struct.properties.technology.id_input_file_mode, ...
        handles.jvx_struct.properties.technology.cat_input_file_mode, ...
        handles.jvx_struct.properties.technology.tp_input_file_mode] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_tech, '/properties_selected_input_file/mode');

    [handles.jvx_struct.properties.technology.id_output_file_rate, handles.jvx_struct.properties.technology.cat_output_file_rate, handles.jvx_struct.properties.technology.tp_output_file_rate] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_tech, '/properties_selected_output_file/samplerate');
    [handles.jvx_struct.properties.technology.id_output_file_chans, handles.jvx_struct.properties.technology.cat_output_file_chans, handles.jvx_struct.properties.technology.tp_output_file_chans] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_tech, '/properties_selected_output_file/num_channels');
    [handles.jvx_struct.properties.technology.id_output_file_lookahead, handles.jvx_struct.properties.technology.cat_output_file_lookahead, handles.jvx_struct.properties.technology.tp_output_file_lookahead] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_tech,  '/properties_selected_output_file/lookahead');
    [handles.jvx_struct.properties.technology.id_output_file_boost, handles.jvx_struct.properties.technology.cat_output_file_boost, handles.jvx_struct.properties.technology.tp_output_file_boost] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_tech, '/properties_selected_output_file/boost_prio');
    [handles.jvx_struct.properties.technology.id_output_file_mode, handles.jvx_struct.properties.technology.cat_output_file_mode, ...
        handles.jvx_struct.properties.technology.tp_output_file_mode] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_tech, '/properties_selected_output_file/mode');
    
end
                
if(handles.jvx_struct.state_dev_active_higher)
    [handles.jvx_struct.properties.device.id_aud_dev_rates, handles.jvx_struct.properties.device.cat_aud_dev_rates, handles.jvx_struct.properties.device.tp_aud_dev_rates, ...
        handles.jvx_struct.properties.device.at_aud_dev_rates] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW/JVX_AUDEV_RATES');
    [handles.jvx_struct.properties.device.id_aud_dev_rate, handles.jvx_struct.properties.device.cat_aud_dev_rate, handles.jvx_struct.properties.device.tp_aud_dev_rate, ...
        handles.jvx_struct.properties.device.at_aud_dev_rate] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW/JVX_AUDEV_SRATE');
    [handles.jvx_struct.properties.device.id_aud_dev_bsizes, handles.jvx_struct.properties.device.cat_aud_dev_bsizes, handles.jvx_struct.properties.device.tp_aud_dev_bsizes, ...
        handles.jvx_struct.properties.device.at_aud_dev_bsizes] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW/JVX_AUDEV_BSIZES');
    [handles.jvx_struct.properties.device.id_aud_dev_bsize, handles.jvx_struct.properties.device.cat_aud_dev_bsize, handles.jvx_struct.properties.device.tp_aud_dev_bsize, ...
        handles.jvx_struct.properties.device.at_aud_dev_bsize] = jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW/JVX_AUDEV_BSIZE');
    [handles.jvx_struct.properties.device.id_aud_dev_formats, handles.jvx_struct.properties.device.cat_aud_dev_formats, handles.jvx_struct.properties.device.tp_aud_dev_formats] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW/JVX_AUDEV_FORMATS');    
    [handles.jvx_struct.properties.device.id_qresampler_in, handles.jvx_struct.properties.device.cat_qresampler_in, handles.jvx_struct.properties.device.tp_qresampler_in] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW_DEV_QRESAMPLER_IN');
    [handles.jvx_struct.properties.device.id_qresampler_out, handles.jvx_struct.properties.device.cat_qresampler_out, handles.jvx_struct.properties.device.tp_qresampler_out] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW_DEV_QRESAMPLER_OUT');
    [handles.jvx_struct.properties.device.id_resampler, handles.jvx_struct.properties.device.cat_resampler, handles.jvx_struct.properties.device.tp_resampler] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW_DEV_RESAMPLER');
    [handles.jvx_struct.properties.device.id_rebuffer, handles.jvx_struct.properties.device.cat_rebuffer, handles.jvx_struct.properties.device.tp_rebuffer] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW_DEV_REBUFFER');
    [handles.jvx_struct.properties.device.id_auto_hw, handles.jvx_struct.properties.device.cat_auto_hw, handles.jvx_struct.properties.device.tp_auto_hw] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW_DEV_AUTOHW');
    [handles.jvx_struct.properties.device.id_ctrl_panel, handles.jvx_struct.properties.device.cat_ctrl_panel, handles.jvx_struct.properties.device.tp_ctrl_panel] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW/JVX_CTRL_PANEL');
    
    [handles.jvx_struct.properties.device.id_alsa_access_type, handles.jvx_struct.properties.device.cat_alsa_access_type, handles.jvx_struct.properties.device.tp_alsa_access_type] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW/JVX_ALSA_ACCESSTYPE');
    [handles.jvx_struct.properties.device.id_alsa_data_type, handles.jvx_struct.properties.device.cat_alsa_data_type, handles.jvx_struct.properties.device.tp_alsa_data_type] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW/JVX_ALSA_DATATYPE');    
    [handles.jvx_struct.properties.device.id_alsa_periods_in, handles.jvx_struct.properties.device.cat_alsa_periods_in, handles.jvx_struct.properties.device.tp_alsa_periods_in] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW/JVX_ALSA_PERIODS_IN');
    [handles.jvx_struct.properties.device.id_alsa_periods_out, handles.jvx_struct.properties.device.cat_alsa_periods_out, handles.jvx_struct.properties.device.tp_alsa_periods_out] = ...
        jvxJvxHost.jvx_find_property_id_descriptor(props_dev, '/JVX_GENW/JVX_ALSA_PERIODS_OUT');
    
end


    