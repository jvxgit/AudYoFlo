% All error messages should end up here. However, I have not modified the
% code at every single place. Here is how it can be done if I find the time
% one day:
% [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_is_file, handles.jvx_struct.properties.device.id_mat_in_is_file);
% if(a)
%      selection = jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD);
% else
%      jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'get_property_uniqueid', stb.DESCRIPTION_STRING, true);
%      selection = -1;
% end

function [] = jvx_display_error(origFile, errCode, errOperation, errDescr, errorsAsWarnings)

% origFile, errCode, errOperation, errTxt, errorsAsWarnings
global jvx_host_call_global;

if(nargin <= 3)
    errorsAsWarnings = false;
end

dd = dbstack;
[devnull errCodeTxt] = jvx_host_call_global('lookup_type_name__value', 'jvxErrorType', errCode);
errTxt = ['Operation <' errOperation '> failed, errorcode: ' errCodeTxt ', ' errDescr];

if(size(dd,1) >= 2)
    txt = ['Error in jvxrt host: File <a href="matlab: opentoline(which(''' dd(2).file '''),' num2str(dd(2).line) ')">' dd(2).file '</a>!' ', function <' dd(2).name '>: '];
else
    txt = ['Error in jvxrt host: File <a href="matlab: opentoline(which(''' origFile '''),1)">' origFile '</a>' ': '];
end

disp('');
disp('');
disp(txt);
disp(errTxt);
disp('');
disp('');


if(errorsAsWarnings)
    
    warning('Error while using jvxrt backend');
else
    
    error('Error while using jvxrt backend');
end

