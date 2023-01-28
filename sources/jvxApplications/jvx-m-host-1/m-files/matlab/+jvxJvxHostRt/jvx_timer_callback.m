function [] = jvx_timer_callback(object,event)

%subHandles = get(object, 'UserData');
%handles = guidata( subHandles.hObject_mainWindow);

%if(handles.jvx_struct.properties_run.active)
%   [subHandles.hObject_mainWindow, handles] = jvxJvxHostRt.jvx_update_ui_run(subHandles.hObject_mainWindow, handles);
%end


global jvx_global_control;
global jvx_host_call_global;

switch(jvx_global_control.tasks.msgq.update_mode)
    case 0
        
        [success err] = jvx_host_call_global('trigger_process_msgq');
        if(~success)
            
            [devnull, errElementNotFound] = jvx_host_call_global('lookup_type_id__name', 'jvxErrorType', 'JVX_ERROR_ELEMENT_NOT_FOUND');
            if(err.ERRORID_INT32 == errElementNotFound)
                
                % Here, everything is ok - there simply was no message to
                % be processed
            else
                % origFile, errCode, errOperation, errDescr, errorsAsWarnings
                % mfilename('fullpath'), 3, 'what', 'hier', true
                jvxJvxHost.jvx_display_error(mfilename('fullpath'), ...
                    err.ERRORID_INT32, 'trigger_process_msgq', ...
                     err.DESCRIPTION_STRING, true);
            end
        end
    case 1
        
        % Update window while running
        hObject = jvx_global_control.tasks.msgq.context.userdata;
        handles = guidata(hObject);
        [hObject, handles] = jvxJvxHostRt.jvx_update_ui_run(hObject, handles);
        guidata(hObject, handles);
end
