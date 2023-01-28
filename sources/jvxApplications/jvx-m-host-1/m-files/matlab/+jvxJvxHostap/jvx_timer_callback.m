function [] = jvx_timer_callback(object,event)

subHandles = get(object, 'UserData');
handles = guidata( subHandles.hObject_mainWindow);

if(handles.jvx_struct.properties_run.active)
   [subHandles.hObject_mainWindow, handles] = jvxJvxHostRt.jvx_update_ui_run(subHandles.hObject_mainWindow, handles);
end