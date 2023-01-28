function [hObject, handles] = jvx_update_ui(hObject, handles)

txtField = {'ERROR'};
idSelect = -1;
enable = 'off';
nameFilePass = '';

technologiesShow = false;
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

tech_active_higher = false;
dev_active_higher = false;

if(handles.jvx_struct.hostState == id_check_active)
    if(handles.jvx_struct.properties.technology.state >= id_check_selected)
        technologiesShow = true;
    end
    nodesShow = true;
end

set(handles.pushbutton_add_input_file, 'Enable', 'off');
set(handles.pushbutton_add_output_file, 'Enable', 'off');
set(handles.pushbutton_remove_input_file, 'Enable', 'off');
set(handles.pushbutton_remove_output_file, 'Enable', 'off');

% ===============================================
% Get the options for the selection f sub technologies
% ===============================================
if(handles.jvx_struct.properties.technology.id_technologies == -1)
    txtField = {'--'};
    idSelect = 1;
    enable = 'off';
elseif(handles.jvx_struct.properties.technology.id_technologies == -2)
    txtField = {'Restart Selection'};
    idSelect = 1;
    
    if(handles.jvx_struct.sequencer.iamprocessing)
        enable = 'off';
    else
        enable = 'on';
    end
    
    devicesShow = true;
    filesShow = true;
    
else
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.technologies.comp_type, handles.jvx_struct.properties.technology.cat_technologies, handles.jvx_struct.properties.technology.id_technologies);
    if(a)
        txtField = {'None' b.SUBFIELD.OPTIONS{:}};
        idSelect = jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD);
        if(handles.jvx_struct.sequencer.iamprocessing)
            enable = 'off';
            
        else
            enable = 'on';
        end
    else
        error(['Unexpected error: '  b.DESCRIPTION_STRING]);
    end
end

set(handles.popupmenu_technology, 'String', txtField);
set(handles.popupmenu_technology, 'Value', idSelect);
set(handles.popupmenu_technology, 'Enable', enable);

% ===============================================
% Show input files
% ===============================================
enable = 'off';
idSelect = -1;

handles.jvx_struct.properties.input_files.input.id_user = -1;
if(handles.jvx_struct.properties.technology.id_input_files >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.technologies.comp_type, handles.jvx_struct.properties.technology.cat_input_files, handles.jvx_struct.properties.technology.id_input_files);
    if(a)
        txtField = b.SUBFIELD.OPTIONS;
        handles.jvx_struct.properties.input_files.input.id_user = ...
            jvxBitField.jvx_selectionid(b.SUBFIELD.SELECTION_BITFIELD);
        if(handles.jvx_struct.properties.input_files.input.id_user >= 0)
            nameFilePass = txtField{handles.jvx_struct.properties.input_files.input.id_user+1};
        end
        %if(handles.jvx_struct.sequencer.iamprocessing)
        %    enable = 'off';
        %else
            enable = 'on';
        %end
    end
    data = jvxJvxHost.jvx_get_uitable_list_in_files(txtField);
else
    data = {};
end

set(handles.uitable_infiles, 'Data', data);
set(handles.uitable_infiles, 'Enable', enable);
% Seems that index can not be set...: set(handles.uitable_infiles, 'Index',..);

% ===============================================
% Show output files
% ===============================================
enable = 'off';
idSelect = -1;

if(handles.jvx_struct.properties.technology.id_output_files >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.technologies.comp_type, handles.jvx_struct.properties.technology.cat_output_files, handles.jvx_struct.properties.technology.id_output_files);
    if(a)
        txtField = b.SUBFIELD.OPTIONS;
        idSelect = jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD);
        if(handles.jvx_struct.sequencer.iamprocessing)
            enable = 'off';
        else
            enable = 'on';
        end
        
    end
    data =txtField;
else
    data = {};
end

%% Obtain currently selected output file
handles.jvx_struct.properties.output_files.output.id_user = -1;
[a b] = handles.hostcall('get_property_descriptor', ...
        handles.jvx_struct.technologies.comp_type, ...
        '/select_files/output_file');
if(~a)
       error(['Unexpected error: '  b.DESCRIPTION_STRING]);
end
handles.jvx_struct.properties.output_files.output.id_user = ...
    jvxBitField.jvx_selectionid(b.SELECTION_BITFIELD);

set(handles.uitable_outfiles, 'Data', data);
set(handles.uitable_outfiles, 'Enable', enable);

% ++++++++++++++++++++++++++++++++++++++++++++++++
% ++++++++++++++++++++++++++++++++++++++++++++++++
% ++++++++++++++++++++++++++++++++++++++++++++++++
% ++++++++++++++++++++++++++++++++++++++++++++++++
% ++++++++++++++++++++++++++++++++++++++++++++++++





% ===============================================
% List all devices
% ===============================================
if(devicesShow)
    txtField =  {'None' handles.jvx_struct.properties.devices{:}};
    
    if(handles.jvx_struct.sequencer.iamprocessing)
        enable = 'off';
    else
        enable = 'on';
    end
    
else
    txtField = handles.jvx_struct.properties.devices;
    enable = 'off';
end

idSelect = 1;
if(handles.jvx_struct.properties.device.id >= 1)
    idSelect = handles.jvx_struct.properties.device.id + 1;
    
    rateShow = true;
    bsizeShow = true;
    formatsShow = true;
    channelsShow = true;
    
    % Device has been selected, show the buttons
    set(handles.pushbutton_add_input_file, 'Enable', 'on');
    set(handles.pushbutton_add_output_file, 'Enable', 'on');
    set(handles.pushbutton_remove_input_file, 'Enable', 'on');
    set(handles.pushbutton_remove_output_file, 'Enable', 'on');
    
end

set(handles.popupmenu_devices, 'String', txtField);
set(handles.popupmenu_devices, 'Value',idSelect );
set(handles.popupmenu_devices, 'Enable', enable);

% ===============================================
% List all audio nodes
% ===============================================
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

% ===============================================
% Show input channels
% ===============================================
enable = 'off';
idSelect = -1;

if(handles.jvx_struct.properties.device.id_in_channels >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_in_channels, handles.jvx_struct.properties.device.id_in_channels);
    if(a)
        
        txtField = b.SUBFIELD.OPTIONS;
        selection =b.SUBFIELD.SELECTION_BITFIELD;
        
        if(size(handles.jvx_struct.properties.device.local.directIn,2) ~= size(txtField,1))
            directInNew = zeros(1, size(txtField,1));
            szl = min(size(handles.jvx_struct.properties.device.local.directIn,2), size(directInNew,2));
            for(ind = 1:szl)
                directInNew(ind) = handles.jvx_struct.properties.device.local.directIn(ind);
            end
            handles.jvx_struct.properties.device.local.directIn = directInNew;
        end
        
        for(ind= 1:size(txtField,1))
            if(handles.jvx_struct.properties.device.local.directIn(ind) == 1)
                txtField{ind} = [txtField{ind} '*'];
            end
        end
        
        if(size(handles.jvx_struct.properties.device.local.gain_in,2) ~= size(txtField,1))
            gainInNew = ones(1, size(txtField,1));
            szl = min(size(handles.jvx_struct.properties.device.local.gain_in,2), size(gainInNew,2));
            for(ind = 1:szl)
                gainInNew(ind) = handles.jvx_struct.properties.device.local.gain_in(ind);
            end
            handles.jvx_struct.properties.device.local.gain_in = gainInNew;
        end
        
        %if(handles.jvx_struct.sequencer.iamprocessing)
        %    enable = 'off';
        %else
        enable = 'on';
        %end
        
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
        selection =b.SUBFIELD.SELECTION_BITFIELD;
        
        if(size(handles.jvx_struct.properties.device.local.gain_out,2) ~= size(txtField,1))
            gainOutNew = ones(1, size(txtField,1));
            szl = min(size(handles.jvx_struct.properties.device.local.gain_out,2), size(gainOutNew,2));
            for(ind = 1:szl)
                gainOutNew(ind) = handles.jvx_struct.properties.device.local.gain_out(ind);
            end
            handles.jvx_struct.properties.device.local.gain_out = gainOutNew;
        end
        %if(handles.jvx_struct.sequencer.iamprocessing)
        %    enable = 'off';
        % else
        enable = 'on';
        %end
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
else
    if(handles.jvx_struct.sequencer.system_ready_for_processing.device_ready && handles.jvx_struct.sequencer.system_ready_for_processing.node_ready)
        enable = 'on';
    end
end

set(handles.pushbutton_startstop, 'String', txtField);
set(handles.pushbutton_startstop, 'Enable', enable);

% Gain factor sliders
value = 0.0;
idS = handles.jvx_struct.properties.channels.input.id_user;
if(handles.jvx_struct.properties.channels.input.all)
    idS = 1;
end
if(idS <= size(handles.jvx_struct.properties.device.local.gain_in,2))
    value = handles.jvx_struct.properties.device.local.gain_in(idS);
end

valdB = 20*log10(value + 1e-10);
valDbl = valdB + 50;
valDbl = valDbl * 0.01;
valDbl = min(valDbl, 1.0);
valDbl = max(valDbl, 0.0);
set(handles.slider_level_in, 'Value', valDbl);
set(handles.text_gain_in_dB, 'String', [num2str(valdB, '%.1f') ' dB']);

value = 0.0;
idS = handles.jvx_struct.properties.channels.output.id_user;
if(handles.jvx_struct.properties.channels.output.all)
    idS = 1;
end
if(idS <= size(handles.jvx_struct.properties.device.local.gain_out,2))
    value = handles.jvx_struct.properties.device.local.gain_out(idS);
end

valdB = 20*log10(value + 1e-10);
valDbl = valdB + 50;
valDbl = valDbl * 0.01;
valDbl = min(valDbl, 1.0);
valDbl = max(valDbl, 0.0);
set(handles.slider_level_out, 'Value', valDbl);
set(handles.text_gain_out_dB, 'String', [num2str(valdB, '%.1f') ' dB']);



% Selection of channelselection
if(handles.jvx_struct.properties.channels.input.all)
    selected = 1;
else
    selected = 0;
end
set(handles.checkbox_in_lev_all, 'Value', selected);

if(handles.jvx_struct.properties.channels.output.all)
    selected = 1;
else
    selected = 0;
end
set(handles.checkbox_out_lev_all, 'Value', selected);

set(handles.text_in_0, 'BackgroundColor', [0 0.1 0]);
set(handles.text_in_1, 'BackgroundColor', [0 0.1 0]);
set(handles.text_in_2, 'BackgroundColor', [0 0.1 0]);
set(handles.text_in_3, 'BackgroundColor', [0 0.1 0]);
set(handles.text_in_4, 'BackgroundColor', [0 0.1 0]);
set(handles.text_in_5, 'BackgroundColor', [0 0.1 0]);
set(handles.text_in_6, 'BackgroundColor', [0 0.1 0]);
set(handles.text_in_7, 'BackgroundColor', [0 0.1 0]);
set(handles.text_in_avrg_lev, 'String', '--');
set(handles.text_in_max_lev, 'String','--');

set(handles.text_out_0, 'BackgroundColor', [0 0.1 0]);
set(handles.text_out_1, 'BackgroundColor', [0 0.1 0]);
set(handles.text_out_2, 'BackgroundColor', [0 0.1 0]);
set(handles.text_out_3, 'BackgroundColor', [0 0.1 0]);
set(handles.text_out_4, 'BackgroundColor', [0 0.1 0]);
set(handles.text_out_5, 'BackgroundColor', [0 0.1 0]);
set(handles.text_out_6, 'BackgroundColor', [0 0.1 0]);
set(handles.text_out_7, 'BackgroundColor', [0 0.1 0]);

set(handles.text_out_avrg_lev, 'String','--');
set(handles.text_out_max_lev, 'String', '--');

% Ready or not images
if(handles.jvx_struct.sequencer.iamprocessing)
    imagesc(handles.jvx_struct.images.proc, 'parent', handles.axes_device);
    axis(handles.axes_device, 'off');
    imagesc(handles.jvx_struct.images.proc, 'parent', handles.axes_node);
    axis(handles.axes_node, 'off');
else
    [a, b] = handles.hostcall('isready_component', handles.jvx_struct.devices.comp_type);
    if(a)
        if(b.ISREADY_BOOL)
            imagesc(handles.jvx_struct.images.ready, 'parent', handles.axes_device);
        else
            imagesc(handles.jvx_struct.images.notready, 'parent', handles.axes_device);
        end
        axis(handles.axes_device, 'off');
        
        set(handles.text_reason_dev, 'String', b.REASON_IF_NOT_READY_STRING);
    end
    [a, b] = handles.hostcall('isready_component', handles.jvx_struct.nodes.comp_type);
    if(a)
        if(b.ISREADY_BOOL)
            imagesc(handles.jvx_struct.images.ready, 'parent', handles.axes_node);
        else
            imagesc(handles.jvx_struct.images.notready, 'parent', handles.axes_node);
        end
        axis(handles.axes_node, 'off');
        
        set(handles.text_reason_node, 'String', b.REASON_IF_NOT_READY_STRING);
    end
end

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
if(handles.jvx_struct.properties.node.offline_node.id_num_buffers >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties.node.offline_node.cat_num_buffers, handles.jvx_struct.properties.node.offline_node.id_num_buffers);
    if(a)
        txtField = num2str(b.SUBFIELD);
    end
end
set(handles.edit_num_buffers, 'String', txtField);
set(handles.edit_lost, 'String', '--');

txtField = '--';
enable = 'off';
idSelect = 1;

if(handles.jvx_struct.properties.node.offline_node.id_subproject_name >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties.node.offline_node.cat_subproject_name, handles.jvx_struct.properties.node.offline_node.id_subproject_name);
    txtField = {'jvxDefault'};
    txtField = [txtField handles.jvx_struct.subprojects];
    enable = 'on';
    
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
        else
            enable = 'on';
        end
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
hObject_subui = handles.subui.hdl;
handles_subui = guidata(hObject_subui);
is_processing = handles.jvx_struct.sequencer.iamprocessing;
[hObject_subui, handles_subui] = jvxJvxHostap.jvx_update_ui(hObject_subui, handles_subui, ...
    is_processing, handles.jvx_struct.technologies.comp_type, ...
    handles.jvx_struct.devices.comp_type,handles.jvx_struct.properties.input_files.input.id_user,...
    handles.jvx_struct.properties.output_files.output.id_user, ...
    tech_active_higher, dev_active_higher, id_readonly_prop, nameFilePass );

guidata(hObject_subui, handles_subui);

% ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
% ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

% Sub UI control
hObject_subui2 = handles.subui2.hdl;
handles_subui2 = guidata(hObject_subui2);
[hObject_subui2, handles_subui2] = jvxJvxHostmi.jvx_update_ui(hObject_subui2, handles_subui2);

guidata(hObject_subui2, handles_subui2);

