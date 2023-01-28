function[hObject, handles] = jvx_init_ui(hObject, handles)

columnname = {'Channel','Selection'};
columnformat = {'char','char'};
columnwidth = {150,100};
set(handles.uitable_inchannels, 'ColumnName', columnname);
set(handles.uitable_inchannels, 'ColumnFormat', columnformat);
set(handles.uitable_inchannels, 'ColumnWidth', columnwidth);

columnname = {'Channel','Selection'};
columnformat = {'char','char'};
columnwidth = {150,100};
set(handles.uitable_outchannels, 'ColumnName', columnname);
set(handles.uitable_outchannels, 'ColumnFormat', columnformat);
set(handles.uitable_outchannels, 'ColumnWidth', columnwidth);

handles.jvx_struct.data.ylow = -1;
handles.jvx_struct.data.yhigh = 1;
handles.jvx_struct.data.xlow = -1;
handles.jvx_struct.data.xhigh = 1;
handles.jvx_struct.data.limits_valid = false;