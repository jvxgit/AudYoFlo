function[hObject, handles] = jvx_init_ui(hObject, handles)

global jvx_host_call_global;
[a b] = jvx_host_call_global('lookup_const__name', 'jvxPropertyLookaheadSizes');
if(~a) error('Call of host function <jvx_host_call_global(''lookup_const__name'', ''jvxPropertyLookaheadSizes'')> failed.'); end

data = {};
for(ind=1:size(b,2))
    data = [data ; num2str(b(ind))];
end

set(handles.popupmenu_input_lahead, 'String', data);
set(handles.popupmenu_input_lahead, 'Enable', 'off');

set(handles.popupmenu_output_lahead, 'String', data);
set(handles.popupmenu_output_lahead, 'Enable', 'off');


