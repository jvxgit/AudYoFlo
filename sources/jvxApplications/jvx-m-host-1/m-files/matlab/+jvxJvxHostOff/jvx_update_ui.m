function [hObject, handles] = jvx_update_ui(hObject, handles)

txtField = {'ERROR'};
idSelect = -1;
enable = 'off';

devicesShow = false;
nodesShow = false;

rateShow = false;
bsizeShow = false;
formatsShow = false;
channelsShow = false;
filesShow = false;

id_check_active = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_ACTIVE');
id_check_selected = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_SELECTED');
id_readonly_prop = jvxJvxHost.jvx_lookup_type_id__name('jvxPropertyAccessType', 'JVX_PROPERTY_ACCESS_READ_ONLY');

dev_active_higher = false;

if(handles.jvx_struct.hostState == id_check_active)
    nodesShow = true;
end

% ++++++++++++++++++++++++++++++++++++++++++++++++
% ++++++++++++++++++++++++++++++++++++++++++++++++
% ++++++++++++++++++++++++++++++++++++++++++++++++
% ++++++++++++++++++++++++++++++++++++++++++++++++
% ++++++++++++++++++++++++++++++++++++++++++++++++
if(nodesShow)
    txtField =  {'None' handles.jvx_struct.properties.nodes{:}};
    if(handles.jvx_struct.sequencer.iamprocessing)
        enable = 'off';
    else
        enable = 'on';
    end
    
else
    txtField = handles.jvx_struct.properties.nodes;
    enable = 'off';
end

idSelect = 1;
if(handles.jvx_struct.properties.node.id >= 1)
    idSelect = handles.jvx_struct.properties.node.id + 1;
end

set(handles.popupmenu_audionode, 'String', txtField);
set(handles.popupmenu_audionode, 'Value', idSelect);
set(handles.popupmenu_audionode, 'Enable', enable);

% ===============================================
% Show samplerate
% ===============================================
txtField = '--';
enable = 'off';
if(handles.jvx_struct.properties.device.id_srate >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_srate, handles.jvx_struct.properties.device.id_srate);
    if(a)
        txtField = num2str(b.SUBFIELD);
        if(handles.jvx_struct.sequencer.iamprocessing)
            enable = 'off';
        else
            enable = 'on';
        end
    end
end
set(handles.edit_project_samplerate, 'String', txtField);
set(handles.edit_project_samplerate, 'Enable', enable);

% ===============================================
% Show buffersize
% ===============================================
txtField = '--';
enable = 'off';
if(handles.jvx_struct.properties.device.id_bsize >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_bsize, handles.jvx_struct.properties.device.id_bsize);
    if(a)
        txtField = num2str(b.SUBFIELD);
        if(handles.jvx_struct.sequencer.iamprocessing)
            enable = 'off';
        else
            enable = 'on';
        end
    end
end
set(handles.edit_project_buffersize, 'String', txtField);
set(handles.edit_project_buffersize, 'Enable', enable);

% ===============================================
% Show processing formats
% ===============================================
txtField = '--';
enable = 'off';
idSelect = 1;
if(handles.jvx_struct.properties.device.id_formats >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_formats, handles.jvx_struct.properties.device.id_formats);
    if(a)
        txtField = b.SUBFIELD.OPTIONS;
        idSelect = jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD);
        if(handles.jvx_struct.sequencer.iamprocessing)
            enable = 'off';
        else
            enable = 'on';
        end
        
    end
end
set(handles.popupmenu_project_formats, 'String', txtField);
set(handles.popupmenu_project_formats, 'Enable', enable);
set(handles.popupmenu_project_formats, 'Value', idSelect);

% ===============================================
% Show input channels
% ===============================================
enable = 'off';
idSelect = -1;

if(handles.jvx_struct.properties.device.id_in_channels >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_in_channels, handles.jvx_struct.properties.device.id_in_channels);
    if(a)
        
        txtField = b.SUBFIELD.OPTIONS;
        selection = b.SUBFIELD.SELECTION_BITFIELD;
        
        if(handles.jvx_struct.sequencer.iamprocessing)
            enable = 'off';
        else
            enable = 'on';
        end
        
    end
    data = jvxJvxHost.jvx_get_uitable_list_multiple_selection(txtField, selection);
else
    data = {};
end

if(size(data,1) == 0)
    handles.jvx_struct.properties.channels.input.id_user = -1;
else
    if(handles.jvx_struct.properties.channels.input.id_user < 1) || (handles.jvx_struct.properties.channels.input.id_user> size(data,1))
        handles.jvx_struct.properties.channels.input.id_user = 1;
    end
end

set(handles.uitable_inchannels, 'Data', data);
set(handles.uitable_inchannels, 'Enable', enable);
%if(handles.jvx_struct.properties.channels.input.id_user >= 1)
%    set(handles.uitable_inchannels, 'Value', handles.jvx_struct.properties.channels.input.id_user);
%end

% ===============================================
% Show output channels
% ===============================================
enable = 'off';
idSelect = -1;

if(handles.jvx_struct.properties.device.id_out_channels >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_out_channels, handles.jvx_struct.properties.device.id_out_channels);
    if(a)
        txtField = b.SUBFIELD.OPTIONS;
        selection = b.SUBFIELD.SELECTION_BITFIELD;
        
        if(handles.jvx_struct.sequencer.iamprocessing)
            enable = 'off';
        else
            enable = 'on';
        end
    end
    data = jvxJvxHost.jvx_get_uitable_list_multiple_selection(txtField, selection);
    
else
    data = {};
end

if(size(data,1) == 0)
    handles.jvx_struct.properties.channels.output.id_user = -1;
else
    if(handles.jvx_struct.properties.channels.output.id_user < 1) || (handles.jvx_struct.properties.channels.output.id_user> size(data,1))
        handles.jvx_struct.properties.channels.output.id_user = 1;
    end
end

set(handles.uitable_outchannels, 'Data', data);
set(handles.uitable_outchannels, 'Enable', enable);

% Set start/stop button text and enable
txtField = 'start';
enable = 'off';

if(handles.jvx_struct.sequencer.iamprocessing)
    enable = 'on';
    txtField = 'stop';
    set(handles.text_reason_node, 'String', 'Processing');
else
    if(handles.jvx_struct.sequencer.system_ready_for_processing.device_ready && ...
            handles.jvx_struct.sequencer.system_ready_for_processing.node_ready && ...
            handles.jvx_struct.sequencer.system_ready_for_processing.chain_ready)
        enable = 'on';       
    end
    set(handles.text_reason_node, 'String', handles.jvx_struct.sequencer.system_ready_for_processing.chain_ready_reason_if_not);
end

% Ready or not images
if(handles.jvx_struct.sequencer.iamprocessing)
    imagesc(handles.jvx_struct.images.proc, 'parent', handles.axes_node);
    axis(handles.axes_node, 'off');
else
    try
        if(handles.jvx_struct.sequencer.system_ready_for_processing.node_ready)
        imagesc(handles.jvx_struct.images.ready, 'parent', handles.axes_node);
        else
           imagesc(handles.jvx_struct.images.notready, 'parent', handles.axes_node);
        end
    catch ME
        handles.jvx_struct.images
        handles.axes_node
        error('Found the always present error location!');
        rethrow(ME)
    end
    axis(handles.axes_node, 'off');
end

set(handles.pushbutton_startstop, 'String', txtField);
set(handles.pushbutton_startstop, 'Enable', enable);

txtField = {};
allVariables = whos('global');
for(ind=1:size(allVariables,1))
    
    oneVar = allVariables(ind);
    
    if(oneVar.global)
        if(strcmp(oneVar.class, 'double'))
            txtField = [txtField; oneVar.name];
        end
        if(strcmp(oneVar.class, 'single'))
            txtField = [txtField; oneVar.name];
        end
        if(strcmp(oneVar.class, 'int32'))
            txtField = [txtField; oneVar.name];
        end
        if(strcmp(oneVar.class, 'int64'))
            txtField = [txtField; oneVar.name];
        end
    end
end

handles.jvx_struct.data.input.vars = txtField;
if(isempty(txtField))
    txtField = 'None';
end

% Popupmenu to list all global variables in workspace
set(handles.popupmenu_input, 'String', txtField);

txtField = '--';
if(~handles.jvx_struct.sequencer.iamprocessing)
    % Text based on which to obtain input samples
    txtField = 'Error';
    if(handles.jvx_struct.properties.device.id_mat_in_text)
        [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_text, handles.jvx_struct.properties.device.id_mat_in_text);
        if(a)
            txtField = b.SUBFIELD;
        end
    end
    
    % Do not update text fild while in processing
    set(handles.edit_in_text, 'String', txtField);
    set(handles.edit_in_text, 'Enable', 'off');
end

% Check if input text specifies a file
if(~handles.jvx_struct.sequencer.iamprocessing)
    selection = -1;
    if(handles.jvx_struct.properties.device.id_mat_in_is_file)

        [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_is_file, handles.jvx_struct.properties.device.id_mat_in_is_file);
        if(a)
            selection = jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD);
        else
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'get_property_uniqueid', b.DESCRIPTION_STRING, true);
            selection = -1;
        end
    end
    
    if(selection > 0)
        set(handles.checkbox_in_is_file, 'Value', selection);
        set(handles.checkbox_in_is_file, 'Enable', 'on');
    else
        set(handles.checkbox_in_is_file, 'Enable', 'off');
    end
else
    set(handles.checkbox_in_is_file, 'Value', 0);
    set(handles.checkbox_in_is_file, 'Enable', 'off');
end

% Specify input rate (in case variable from workspace)
if(~handles.jvx_struct.sequencer.iamprocessing)
    rate = 0;
    if(handles.jvx_struct.properties.device.id_mat_in_rate)
        [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_rate, handles.jvx_struct.properties.device.id_mat_in_rate);
        if(a)
            rate = b.SUBFIELD;
        end
    end
    set(handles.edit_in_var_rate, 'String', num2str(rate));
    set(handles.edit_in_var_rate, 'Enable', 'on');
else
    set(handles.edit_in_var_rate, 'String', '--');
    set(handles.edit_in_var_rate, 'Enable', 'off');
end

% Text based on which to store output samples
if(~handles.jvx_struct.sequencer.iamprocessing)
    txtField = 'Error';
    if(handles.jvx_struct.properties.device.id_mat_out_text)
        [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_out_text, handles.jvx_struct.properties.device.id_mat_out_text);
        if(a)
            txtField = b.SUBFIELD;
        end
    end
    set(handles.edit_out_text, 'String', txtField);
    set(handles.edit_out_text, 'Enable', 'on');
else
    set(handles.edit_out_text, 'String', '--');
    set(handles.edit_out_text, 'Enable', 'off');
end

% Check if input text specifies a file
if(~handles.jvx_struct.sequencer.iamprocessing)
    selection = 0;
    if(handles.jvx_struct.properties.device.id_mat_out_is_file)
        [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_out_is_file, handles.jvx_struct.properties.device.id_mat_out_is_file);
        if(a)
            selection = jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD);
        end
    end
    set(handles.checkbox_out_is_file, 'Value', selection);
    set(handles.checkbox_out_is_file, 'Enable', 'on');
else
    set(handles.checkbox_out_is_file, 'Value', 0);
    set(handles.checkbox_out_is_file, 'Enable', 'off');
end


set(handles.edit_ylow, 'String', '--');
set(handles.edit_yhigh, 'String', '--');
set(handles.edit_xlow, 'String', '--');
set(handles.edit_xhigh, 'String', '--');

% Some data specific UI stuff
if(~handles.jvx_struct.sequencer.iamprocessing)
    txtField = {};
    for(ind = 1:size(handles.jvx_struct.data.input.data,1))
        txtField = [txtField ['input#' num2str(ind)]];
    end
    for(ind = 1:size(handles.jvx_struct.data.output.data,1))
        txtField = [txtField ['output#' num2str(ind)]];
    end
    
    if(handles.jvx_struct.data.channels.plot.id_user < 1)
        if(size(txtField,2) > 0)
            handles.jvx_struct.data.channels.plot.id_user = 1;
        end
    else
        if(handles.jvx_struct.data.channels.plot.id_user > size(txtField,2))
            if( size(txtField,2) > 0)
                handles.jvx_struct.data.channels.plot.id_user = 1;
            else
                handles.jvx_struct.data.channels.plot.id_user = -1;
            end
        end
    end
    
    if(isempty(txtField))
        txtField = 'None';
    end
    set(handles.popupmenu_what_to_plot, 'String', txtField);
    if(handles.jvx_struct.data.channels.plot.id_user >= 1)
        set(handles.popupmenu_what_to_plot, 'Value', handles.jvx_struct.data.channels.plot.id_user);
    end
    
    oneBuf = [];
    if(handles.jvx_struct.data.channels.plot.id_user > 0)
        if(handles.jvx_struct.data.channels.plot.id_user > size(handles.jvx_struct.data.input.data,1))
            oneBuf = handles.jvx_struct.data.output.data(handles.jvx_struct.data.channels.plot.id_user - size(handles.jvx_struct.data.input.data,1),:);
        else
            oneBuf = handles.jvx_struct.data.input.data(handles.jvx_struct.data.channels.plot.id_user,:);
        end
    end
    
    if(size(oneBuf,2) > 0)
        oneBufTime = [0:size(oneBuf,2)-1] * 1.0 / double(handles.jvx_struct.data.input.rate);
        plot(handles.axes_view, oneBufTime, oneBuf);
        if(handles.jvx_struct.data.limits_valid == false)
            handles.jvx_struct.data.ylow = -1;
            handles.jvx_struct.data.yhigh = 1;
            handles.jvx_struct.data.xlow = oneBufTime(1);
            handles.jvx_struct.data.xhigh = oneBufTime(end);
            handles.jvx_struct.data.xhigh = max(handles.jvx_struct.data.xhigh, handles.jvx_struct.data.xlow + eps);
            handles.jvx_struct.data.limits_valid = true;
        end
        ylim(handles.axes_view, [handles.jvx_struct.data.ylow handles.jvx_struct.data.yhigh]);
        xlim(handles.axes_view, [handles.jvx_struct.data.xlow handles.jvx_struct.data.xhigh]);
        
        set(handles.edit_ylow, 'String', num2str(handles.jvx_struct.data.ylow));
        set(handles.edit_yhigh, 'String', num2str(handles.jvx_struct.data.yhigh));
        set(handles.edit_xlow, 'String', num2str(handles.jvx_struct.data.xlow));
        set(handles.edit_xhigh, 'String', num2str(handles.jvx_struct.data.xhigh));
    else
        plot(handles.axes_view, [0], [0]);
        axis(handles.axes_view, 'off');
    end
end

% Text based on which to export variables
if(~handles.jvx_struct.sequencer.iamprocessing)
    txtField = 'Error';
    if(handles.jvx_struct.properties.device.id_mat_export_text)
        [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_export_text, handles.jvx_struct.properties.device.id_mat_export_text);
        if(a)
            txtField = b.SUBFIELD;
        end
    end
    set(handles.edit_export_name, 'String', txtField);
    set(handles.edit_export_name, 'Enable', 'on');
else
    set(handles.edit_export_name, 'String', '--');
    set(handles.edit_export_name, 'Enable', 'off');
end

if(~handles.jvx_struct.sequencer.iamprocessing)
    selection = 0;
    if(handles.jvx_struct.properties.device.id_mat_play_stereo)
        [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_play_stereo, handles.jvx_struct.properties.device.id_mat_play_stereo);
        if(a)
            selection = jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD);
        end
    end
    set(handles.checkbox_stereo, 'Value', selection);
    set(handles.checkbox_stereo, 'Enable', 'on');
    
else
    set(handles.checkbox_stereo, 'Value', 0);
    set(handles.checkbox_stereo, 'Enable', 'off');
end

if(handles.jvx_struct.sequencer.iamprocessing)
    set(handles.pushbutton_export_output, 'Enable', 'off');
    set(handles.pushbutton_save_output, 'Enable', 'off');
    set(handles.pushbutton_plot_output, 'Enable', 'off');
    set(handles.pushbutton_startplay, 'Enable', 'off');
    set(handles.pushbutton_stopplay, 'Enable', 'off');
else
    set(handles.pushbutton_export_output, 'Enable', 'on');
    set(handles.pushbutton_save_output, 'Enable', 'on');
    set(handles.pushbutton_plot_output, 'Enable', 'on');
    set(handles.pushbutton_startplay, 'Enable', 'on');
    set(handles.pushbutton_stopplay, 'Enable', 'on');
end

set(handles.text_progress_0, 'BackgroundColor', [0 0.1 0]);
set(handles.text_progress_1, 'BackgroundColor', [0 0.1 0]);
set(handles.text_progress_2, 'BackgroundColor', [0 0.1 0]);
set(handles.text_progress_3, 'BackgroundColor', [0 0.1 0]);
set(handles.text_progress_4, 'BackgroundColor', [0 0.1 0]);
set(handles.text_progress_5, 'BackgroundColor', [0 0.1 0]);
set(handles.text_progress_6, 'BackgroundColor', [0 0.1 0]);
set(handles.text_progress_7, 'BackgroundColor', [0 0.1 0]);
set(handles.text_percent_progress, 'String', ['--']);

% In case of offline processing node, show the Matlab Callbacks
set(handles.edit_process_cb, 'Enable', 'on');
set(handles.edit_start_cb, 'Enable', 'on');
set(handles.edit_b4_process_cb, 'Enable', 'on');
set(handles.edit_stop_cb, 'Enable','on');
set(handles.popupmenu_subproj, 'Enable', 'on');

txtField = '--';
if(handles.jvx_struct.properties.node.offline_node.id_start_function >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties.node.offline_node.cat_start_function, handles.jvx_struct.properties.node.offline_node.id_start_function);
    if(a)
        txtField = b.SUBFIELD;
    end
end
set(handles.edit_start_cb, 'String', txtField);

txtField = '--';
if(handles.jvx_struct.properties.node.offline_node.id_stop_function >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties.node.offline_node.cat_stop_function, handles.jvx_struct.properties.node.offline_node.id_stop_function);
    if(a)
        txtField = b.SUBFIELD;
    end
end
set(handles.edit_stop_cb, 'String', txtField);

txtField = '--';
if(handles.jvx_struct.properties.node.offline_node.id_process_function >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties.node.offline_node.cat_process_function, handles.jvx_struct.properties.node.offline_node.id_process_function);
    if(a)
        txtField = b.SUBFIELD;
    end
end
set(handles.edit_process_cb, 'String', txtField);

txtField = '--';
if(handles.jvx_struct.properties.node.offline_node.id_b4_process_function >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties.node.offline_node.cat_b4_process_function, handles.jvx_struct.properties.node.offline_node.id_b4_process_function);
    if(a)
        txtField = b.SUBFIELD;
    end
end
set(handles.edit_b4_process_cb, 'String', txtField);

txtField = '--';
enable = 'off';
idSelect = 1;

if(handles.jvx_struct.properties.node.offline_node.id_subproject_name >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties.node.offline_node.cat_subproject_name, handles.jvx_struct.properties.node.offline_node.id_subproject_name);
    txtField = {'jvxDefault'};
    txtField = [txtField handles.jvx_struct.subprojects];
    
    if(a)
        for(ind = 1:size(handles.jvx_struct.subprojects,2))
            if(strcmp(handles.jvx_struct.subprojects{ind}, b.SUBFIELD))
                idSelect = ind+1;
            end
        end
        if(idSelect == 1)
            
            % Correct setting to specify default directory for
            % talkthrough
            handles.hostcall('set_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties.node.offline_node.cat_subproject_name, handles.jvx_struct.properties.node.offline_node.id_subproject_name, 'jvxDefault');
        end
        enable = 'on';
    end
end
set(handles.popupmenu_subproj, 'String', txtField);
set(handles.popupmenu_subproj, 'Value', idSelect);
set(handles.popupmenu_subproj, 'Enable', enable);

% Variable to activate / deactivate Matlab processing in C code
matlabEngaged = false;
set(handles.checkbox4, 'Value', 0);
set(handles.checkbox4, 'Enable', 'off');
set(handles.checkbox4, 'ForegroundColor', [0.62 0.62 0.62]);
if(handles.jvx_struct.properties.node.offline_node.id_engange_matlab >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties.node.offline_node.cat_engange_matlab, ...
        handles.jvx_struct.properties.node.offline_node.id_engange_matlab);
    if(a == true)
        set(handles.checkbox4, 'Value', b.SUBFIELD);
        set(handles.checkbox4, 'Enable', 'on');
        if(b.SUBFIELD)
            matlabEngaged = true;
        end
    else
        set(handles.checkbox4, 'Value', 1);
        matlabEngaged = true;
    end
    
    if(matlabEngaged)
        set(handles.checkbox4, 'ForegroundColor', [1 0 0]);
    else
        % It is left to the algorithm developer to use this feature
        % or not - therefore, we do not make any further modification
        %set(handles.edit_process_cb, 'Enable', 'off');
        %set(handles.edit_start_cb, 'Enable', 'off');
        %set(handles.edit_b4_process_cb, 'Enable', 'off');
        %set(handles.edit_stop_cb, 'Enable', 'off');
        %set(handles.popupmenu_subproj, 'Enable', 'off');
    end
end

% ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

% Sub UI control
hObject_subui2 = handles.subui2.hdl;
handles_subui2 = guidata(hObject_subui2);
[hObject_subui2, handles_subui2] = jvxJvxHostmi.jvx_update_ui(hObject_subui2, handles_subui2);

guidata(hObject_subui2, handles_subui2);

