function [hObject, handles] = jvx_init(hObject, handles, comp_tech, comp_dev)
    
handles.jvx_struct.technologies.comp_type = comp_tech;
handles.jvx_struct.devices.comp_type = comp_dev;
handles.jvx_struct.isRunning = false;