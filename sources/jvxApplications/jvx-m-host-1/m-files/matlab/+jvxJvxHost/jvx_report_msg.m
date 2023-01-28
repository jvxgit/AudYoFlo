function [] = jvx_report_msg(id_msg, sub_id_msg, load)
	
    global jvxHost_rt_remote;

    % 'JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_SHIFT'
    % 'JVX_REPORT_REQUEST_TRY_TRIGGER_START_SEQUENCER_SHIFT'
    % 'JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT'
    % 'JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT'
    % 'JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT'
    % 'JVX_REPORT_REQUEST_UPDATE_DATACONNECTIONS_SHIFT'
    % 'JVX_REPORT_REQUEST_USER_MESSAGE_SHIFT'
    % 'JVX_REPORT_REQUEST_UPDATE_PROPERTIES_SHIFT'
    
    id_check_stop_shift = jvxJvxHost.jvx_lookup_type_id__name('jvxReportEventType', 'JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_SHIFT');
    id_check_start_shift = jvxJvxHost.jvx_lookup_type_id__name('jvxReportEventType', 'JVX_REPORT_REQUEST_TRY_TRIGGER_START_SEQUENCER_SHIFT');
    id_check_update_ui_shift = jvxJvxHost.jvx_lookup_type_id__name('jvxReportEventType', 'JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT');
    id_check_user_shift = jvxJvxHost.jvx_lookup_type_id__name('jvxReportEventType', 'JVX_REPORT_REQUEST_USER_MESSAGE_SHIFT');
    
    if(bitand(id_msg, bitshift(1, id_check_stop_shift)))
        jvxHost_rt_remote.jvx_start_stop();
    end
    
    if(bitand(id_msg, bitshift(1, id_check_start_shift)))
        jvxHost_rt_remote.jvx_start_stop();
    end

    if(bitand(id_msg, bitshift(1, id_check_update_ui_shift)))
        jvxHost_rt_remote.jvx_update_ui();
    end
    
    if(bitand(id_msg, bitshift(1, id_check_user_shift)))
        switch(sub_id_msg)
            % What to do here?
        end
    end
