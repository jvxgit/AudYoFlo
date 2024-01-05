function [a, b] = jvx_mesgq_callback(eventmask,param1, param2)

global jvx_host_call_global;
global jvx_global_control;

[suc vStopSequencer] = jvx_host_call_global('lookup_type_id__name', 'jvxReportEventType', 'JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_SHIFT');
[suc vUpdateWindow] = jvx_host_call_global('lookup_type_id__name', 'jvxReportEventType', 'JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT');
[suc vUpdatePropViewer] = jvx_host_call_global('lookup_type_id__name', 'jvxReportEventType', 'JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT');
[suc vUpdatePropViewerFull] = jvx_host_call_global('lookup_type_id__name', 'jvxReportEventType', 'JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT');
[suc vDataConnections] = jvx_host_call_global('lookup_type_id__name', 'jvxReportEventType', 'JVX_REPORT_REQUEST_UPDATE_DATACONNECTIONS_SHIFT');
[suc vUserMessage] = jvx_host_call_global('lookup_type_id__name', 'jvxReportEventType', 'JVX_REPORT_REQUEST_USER_MESSAGE_SHIFT');
[suc vUpdateProperties] = jvx_host_call_global('lookup_type_id__name', 'jvxReportEventType', 'JVX_REPORT_REQUEST_UPDATE_PROPERTIES_SHIFT');
[suc vTestChain] = jvx_host_call_global('lookup_type_id__name', 'jvxReportEventType', 'JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT');
[suc vReconfig] = jvx_host_call_global('lookup_type_id__name', 'jvxReportEventType', 'JVX_REPORT_REQUEST_RECONFIGURE_SHIFT');

if(bitand(eventmask, bitshift(1, vStopSequencer)))
    display('JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_SHIFT');
end

if(bitand(eventmask, bitshift(1, vUpdateWindow)))
    
    display('JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT');
    
    hObject = jvx_global_control.tasks.msgq.context.userdata;
    handles = guidata(hObject);
    
    % Refresh what you want to show!
    [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
    
    % Show current status
    [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    
    guidata(hObject, handles);

end

if(bitand(eventmask, bitshift(1, vUpdatePropViewer)))
    display('JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_SHIFT');
end

if(bitand(eventmask, bitshift(1, vUpdatePropViewerFull)))
    display('JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT');
end

if(bitand(eventmask, bitshift(1, vDataConnections)))
    display('JVX_REPORT_REQUEST_UPDATE_DATACONNECTIONS_SHIFT');
end

if(bitand(eventmask, bitshift(1, vUserMessage)))
    display('JVX_REPORT_REQUEST_USER_MESSAGE_SHIFT');
end

if(bitand(eventmask, bitshift(1, vUpdateProperties)))
    display('JVX_REPORT_REQUEST_UPDATE_PROPERTIES_SHIFT');
end

if(bitand(eventmask, bitshift(1, vTestChain)))
    % display('JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT');
    
    [suc, retVal] = jvx_host_call_global('test_connection_master', param1);
    if(suc)
        display(['Testing signal chain ok.']);
    else
        
        display(['Testing signal chain failed, reason: ' retVal]);
    end
    
    hObject = jvx_global_control.tasks.msgq.context.userdata;
    handles = guidata(hObject);
    
    % Update current status
    [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
    
    % Show current status
    [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    guidata(hObject, handles);

end

if(bitand(eventmask, bitshift(1, vReconfig)))
    display('JVX_REPORT_REQUEST_RECONFIGURE_SHIFT');
end
