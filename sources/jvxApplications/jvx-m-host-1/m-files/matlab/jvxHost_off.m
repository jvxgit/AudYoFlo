function varargout = jvxHost_off(varargin)
% JVXHOST_OFF MATLAB code for jvxHost_off.fig
%      JVXHOST_OFF, by itself, creates a new JVXHOST_OFF or raises the existing
%      singleton*.
%
%      H = JVXHOST_OFF returns the handle to a new JVXHOST_OFF or the handle to
%      the existing singleton*.
%
%      JVXHOST_OFF('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in JVXHOST_OFF.M with the given input arguments.
%
%      JVXHOST_OFF('Property','Value',...) creates a new JVXHOST_OFF or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before jvxHost_off_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to jvxHost_off_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help jvxHost_off

% Last Modified by GUIDE v2.5 26-Sep-2022 15:11:13

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @jvxHost_off_OpeningFcn, ...
                   'gui_OutputFcn',  @jvxHost_off_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before jvxHost_off is made visible.
function jvxHost_off_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to jvxHost_off (see VARARGIN)

% Allow remote access
global jvxHost_off_remote;
jvxHost_off_remote.hObject = hObject;
jvxHost_off_remote.jvx_run = @jvxHost_off_remote_run;
jvxHost_off_remote.jvx_force_stop = @jvxHost_off_remote_force_stop;
jvxHost_off_remote.jvx_input_var = @jvxHost_off_remote_selection_in_variable;
jvxHost_off_remote.jvx_input_var_direct = @jvxHost_off_remote_selection_in_variable_direct;
jvxHost_off_remote.jvx_set_samplerate = @jvxHost_off_remote_set_samplerate;
jvxHost_off_remote.jvx_set_buffersize = @jvxHost_off_remote_set_buffersize;
jvxHost_off_remote.jvx_set_engage_matlab = @jvxHost_off_remote_set_engage_matlab;
jvxHost_off_remote.jvx_mesgq_immediate = @jvxHost_off_remote_trigger_mesgq_immediately;

% Set callback for all host system calls
global jvx_host_call_global;
jvx_host_call_global = @jvxHostOff;
handles.hostcall = @jvxHostOff;
global jvx_global_control;
handles.local = [];

% ===================================
% In case of restart, set to state in which host can be initialized
% afterwards
% ===================================
% ===================================
% In case of restart, set to state in which host can be initialized
% afterwards
% ===================================
if(~isempty(jvx_global_control))
    if(isfield(jvx_global_control, 'hObject'))
        if(~isempty(jvx_global_control.hObject))
            if(isvalid(jvx_global_control.hObject))
                handles_old = guidata(jvx_global_control.hObject);
                jvxJvxHostOff.jvx_shutdown(handles_old);
            end
        end
    end
end

jvx_global_control.hObject = hObject;

% ===================================
% Initialize Rtproc host
% ===================================
handles.jvx_struct.valid = false;

% Pull out Matlab specific configuration props
[varargin_filtered, handles.local] = jvxJvxHost.jvx_filter_mat_cmdline(handles.local, varargin{:});

% Start C host tool
[a b]=  handles.hostcall('initialize', varargin_filtered);
if(~a)
    jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, ...
        'initialize', b.DESCRIPTION_STRING, true); 
    jvxJvxHostOff.jvx_shutdown(handles);
    error('Error in host initialization.');
else
    [a b]=  handles.hostcall('activate');
    if(~a)
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, ...
        'activate', b.DESCRIPTION_STRING); 
        jvxJvxHostOff.jvx_shutdown(handles);
        error('Error in host initialization.');
    else
        handles.jvx_struct.valid = true;
    end
end

% ===================================
% Initialize the host into a stable state
% ===================================
if(handles.jvx_struct.valid == true)
    
    [hObject, handles] = jvxJvxHostOff.jvx_startup(hObject, handles);

end

% Choose default command line output for jvxHost_rt
handles.output = hObject;

% Update handles structure for message queue callback
guidata(hObject, handles);

% Set the message queue callback function
handles.hostcall('set_callback_msgq', 'jvxJvxHostOff.jvx_mesgq_callback');

% Start a global timer to poll message queue
jvxJvxHost.jvx_start_msgq(hObject, @jvxJvxHostOff.jvx_timer_callback, handles.local.msgq_period, handles.local.start_msgq_timer);

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes jvxHost_off wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = jvxHost_off_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;



function edit_in_text_Callback(hObject, eventdata, handles)
% hObject    handle to edit_in_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_in_text as text
%        str2double(get(hObject,'String')) returns contents of edit_in_text as a double


% --- Executes during object creation, after setting all properties.
function edit_in_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_in_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton_input.
function pushbutton_input_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_input (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[f, p] = uigetfile({'*.wav', 'Wav Audiofiles'});
if(ischar(p))
    if(handles.local.use_absolut_pathnames == false)
        p = jvxJvxHost.jvx_relative_path(p);
    end
    if(strcmp(p, ''))
        fname = f;
    else
        fname = [p '/' f];
    end
    
    if(exist(fname, 'file'))
        
        [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
        if(~a)
            % origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
        end
        
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_text, handles.jvx_struct.properties.device.id_mat_in_text, fname);
        if(~a)
            % origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
        end
        
        selection = jvxBitField.jvx_create(0);
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_is_file, handles.jvx_struct.properties.device.id_mat_in_is_file, selection);
        if(~a)
            % origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
        end
        
        [tt, r] = audioread(fname);
        
        handles.jvx_struct.data.input.data = tt';
        handles.jvx_struct.data.input.rate = r;
        
        if(handles.jvx_struct.properties.device.id_srate > 0)
            [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_srate, handles.jvx_struct.properties.device.id_srate,int32(handles.jvx_struct.data.input.rate));
            if(~a)
                % origFile, errCode, errOperation, errTxt, errorsAsWarnings
                jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
            end
        end
        
        if(handles.jvx_struct.properties.device.id_in_channels_set > 0)
            [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_in_channels_set, handles.jvx_struct.properties.device.id_in_channels_set,int32(size(handles.jvx_struct.data.input.data,1)));
            if(~a)
                % origFile, errCode, errOperation, errTxt, errorsAsWarnings
                jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
            end
        end
        
        [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
        if(~a)
            % origFile, errCode, errOperation, errTxt, errorsAsWarnings             
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
        end
        
        % Obtain references to all relevant properties
        [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
        
        % Show current status
        [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
        
        guidata(hObject, handles);
    end
end

% --- Executes on selection change in popupmenu_input.
function popupmenu_input_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_input (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_input contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_input
idSel = get(hObject,'Value');
if(idSel <= size(handles.jvx_struct.data.input.vars,1))
    
    [handles] = jvxHost_off_remote_offline_input_variable_core(hObject, handles, idSel);
    
    % Show current status
    [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    
    guidata(hObject, handles);
end


% --- Executes during object creation, after setting all properties.
function popupmenu_input_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_input (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit_project_samplerate_Callback(hObject, eventdata, handles)
% hObject    handle to edit_project_samplerate (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_project_samplerate as text
%        str2double(get(hObject,'String')) returns contents of edit_project_samplerate as a double
val = str2double(get(hObject,'String'));

[handles] = jvxHost_off_remote_offline_set_samplerate_core(hObject, handles, val); 

% Obtain references to all relevant properties
[hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);

% Show current status
[hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);

guidata(hObject, handles);


% --- Executes during object creation, after setting all properties.
function edit_project_samplerate_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_project_samplerate (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit_project_buffersize_Callback(hObject, eventdata, handles)
% hObject    handle to edit_project_buffersize (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_project_buffersize as text
%        str2double(get(hObject,'String')) returns contents of edit_project_buffersize as a double
val = str2double(get(hObject,'String'));

[handles] = jvxHost_off_remote_offline_set_buffersize_core(hObject, handles, val); 

% Obtain references to all relevant properties
[hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);

% Show current status
[hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);

guidata(hObject, handles);


% --- Executes during object creation, after setting all properties.
function edit_project_buffersize_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_project_buffersize (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in popupmenu_project_formats.
function popupmenu_project_formats_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_project_formats (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_project_formats contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_project_formats
idSelect = get(hObject, 'Value');
if(handles.jvx_struct.properties.device.id_formats >= 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type,handles.jvx_struct.properties.device.cat_formats, handles.jvx_struct.properties.device.id_formats);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings             
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'get_property_uniqueid', b.DESCRIPTION_STRING, false);
    end
    
    [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
    end
    
    if(idSelect < size(b.SUBFIELD.OPTIONS,1))
        jvxJvxHost.jvx_set_property_single_selection_mat_id(handles, handles.jvx_struct.devices.comp_type,handles.jvx_struct.properties.device.cat_formats, handles.jvx_struct.properties.device.id_formats, idSelect);
    end

    [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
    end
    
    % Obtain references to all relevant properties
    [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
    
    % Show current status
    [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    
    guidata(hObject, handles);
end
    
    
% --- Executes during object creation, after setting all properties.
function popupmenu_project_formats_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_project_formats (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in popupmenu_audionode.
function popupmenu_audionode_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_audionode (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_audionode contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_audionode
% ==============================================
% User selection
% ==============================================
idSelect = get(handles.popupmenu_audionode, 'Value');
idSelect = idSelect - 1; % First entry: none

if(handles.jvx_struct.properties.node.id >= 1)
    % ==============================================
    % Deactivate component
    % ==============================================
    [a b]= handles.hostcall('deactivate_component', handles.jvx_struct.nodes.comp_type);
    if(~a)
        % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'deactivate_component', b.DESCRIPTION_STRING, false);
    end

    % ==============================================
    % Unselect node
    % ==============================================
    [a b]= handles.hostcall('unselect_component', handles.jvx_struct.nodes.comp_type);
    if(~a)
        % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'unselect_component', b.DESCRIPTION_STRING, false);
    end
end

if(idSelect >= 1)
    
    % ==============================================
    % Activate selected component
    % ==============================================
    [a b]= handles.hostcall('select_component', handles.jvx_struct.nodes.comp_type, idSelect-1);% c notation idx
    if(~a)
        % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'select_component', b.DESCRIPTION_STRING, false);
    end

    [a b]= handles.hostcall('activate_component', handles.jvx_struct.nodes.comp_type);
    if(~a)
         % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'activate_component', b.DESCRIPTION_STRING, false);
    end
end

% Obtain references to all relevant properties
[hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);

% Show current status
[hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);

guidata(hObject, handles);


% --- Executes during object creation, after setting all properties.
function popupmenu_audionode_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_audionode (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton_startstop.
function pushbutton_startstop_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_startstop (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[handles] = jvxHost_off_remote_run_core(hObject, handles);
guidata(hObject, handles);


% --- Executes on button press in pushbutton_save.
function pushbutton_save_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_save (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[a b] = handles.hostcall('write_config_file');
if(~a)
end

% --- Executes on button press in pushbutton_save_output.
function pushbutton_save_output_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_save_output (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[f, p] = uiputfile({'*.wav', 'Wav Audiofiles'});
if(ischar(p))
    fname = [p f];
    if(~isempty(fname))
        if(handles.jvx_struct.data.channels.plot.id_user > size(handles.jvx_struct.data.input.data,1))
            oneBuf = handles.jvx_struct.data.output.data(handles.jvx_struct.data.channels.plot.id_user - size(handles.jvx_struct.data.input.data,1),:);
            rate = handles.jvx_struct.data.output.rate;
        else
            oneBuf = handles.jvx_struct.data.input.data(handles.jvx_struct.data.channels.plot.id_user,:);
            rate = handles.jvx_struct.data.input.rate;
        end
        audiowrite(fname, oneBuf, rate);
        disp(['Wrote file ' fname '.']);
    end
end

% --- Executes on button press in pushbutton_startplay.
function pushbutton_startplay_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_startplay (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if(handles.jvx_struct.data.channels.plot.id_user > 0)
     if(handles.jvx_struct.data.channels.plot.id_user > size(handles.jvx_struct.data.input.data,1))
        oneBuf1 = handles.jvx_struct.data.output.data(handles.jvx_struct.data.channels.plot.id_user - size(handles.jvx_struct.data.input.data,1),:);
        if(handles.jvx_struct.data.channels.plot.id_user - size(handles.jvx_struct.data.input.data,1) < size(handles.jvx_struct.data.output.data,1))
            oneBuf2 = handles.jvx_struct.data.output.data(handles.jvx_struct.data.channels.plot.id_user - size(handles.jvx_struct.data.input.data,1)+1,:);
        else
            oneBuf2 = oneBuf1;
        end
        rate = handles.jvx_struct.data.output.rate;
    else
        oneBuf1 = handles.jvx_struct.data.input.data(handles.jvx_struct.data.channels.plot.id_user,:);
         if(handles.jvx_struct.data.channels.plot.id_user < size(handles.jvx_struct.data.input.data,1))
            oneBuf2 = handles.jvx_struct.data.input.data(handles.jvx_struct.data.channels.plot.id_user+1,:);
        else
            oneBuf2 = oneBuf1;
        end
        rate = handles.jvx_struct.data.input.rate;
     end

     if( handles.jvx_struct.data.stereo)
        sig = [oneBuf1' oneBuf2'];
     else
         sig = oneBuf1;
     end
     handles.jvx_struct.data.player = audioplayer(sig, double(rate));
     play(handles.jvx_struct.data.player);
     guidata(hObject, handles);
end


% --- Executes on button press in pushbutton_export_output.
function pushbutton_export_output_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_export_output (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if(handles.jvx_struct.data.channels.plot.id_user > 0)
     if(handles.jvx_struct.properties.device.id_mat_export_text)
         [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_export_text, handles.jvx_struct.properties.device.id_mat_export_text);
         if(a)
             export_name = b.SUBFIELD;
             
             if(handles.jvx_struct.data.channels.plot.id_user > size(handles.jvx_struct.data.input.data,1))
                 oneBuf = handles.jvx_struct.data.output.data(handles.jvx_struct.data.channels.plot.id_user - size(handles.jvx_struct.data.input.data,1),:);
             else
                 oneBuf = handles.jvx_struct.data.input.data(handles.jvx_struct.data.channels.plot.id_user,:);
             end
             assignin('base', export_name, oneBuf);
         end
     end
end

function edit_export_name_Callback(hObject, eventdata, handles)
% hObject    handle to edit_export_name (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_export_name as text
%        str2double(get(hObject,'String')) returns contents of edit_export_name as a double
export_name = get(hObject,'String');
if(handles.jvx_struct.properties.device.id_mat_export_text)
    
    [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
    end
    
    [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_export_text, handles.jvx_struct.properties.device.id_mat_export_text, export_name);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
    end
    
    [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
    end
    % Obtain references to all relevant properties
    [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
            
    % Show current status
    [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);

    guidata(hObject, handles);
end

% --- Executes during object creation, after setting all properties.
function edit_export_name_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_export_name (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in popupmenu_what_to_plot.
function popupmenu_what_to_plot_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_what_to_plot (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_what_to_plot contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_what_to_plot
idSel = get(hObject,'Value');

handles.jvx_struct.data.channels.plot.id_user = idSel;
handles.jvx_struct.data.limits_valid = false;

% Show current status
[hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
[hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);

guidata(hObject, handles);


% --- Executes during object creation, after setting all properties.
function popupmenu_what_to_plot_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_what_to_plot (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton_update_plot.
function pushbutton_update_plot_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_update_plot (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in pushbutton_dec_out.
function pushbutton_dec_out_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_dec_out (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if(handles.jvx_struct.properties.device.id_out_channels_set > 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_out_channels_set, handles.jvx_struct.properties.device.id_out_channels_set);
    if(a)
        numChans = b.SUBFIELD;
        numChans  = numChans  - 1;
        if(numChans  < 0)
           numChans  = 0;
        end
        
        [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
        if(~a)
            % origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
        end
        
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_out_channels_set, handles.jvx_struct.properties.device.id_out_channels_set,numChans );
        if(~a)
             % origFile, errCode, errOperation, errTxt, errorsAsWarnings
             jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
        end
        
        [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
        if(~a)
            % origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
        end
        
        % Obtain references to all relevant properties
        [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
        
        % Show current status
        [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
        
        guidata(hObject, handles);        
    end
end

% --- Executes on button press in pushbutton_inc_out.
function pushbutton_inc_out_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_inc_out (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if(handles.jvx_struct.properties.device.id_out_channels_set > 0)
    [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_out_channels_set, handles.jvx_struct.properties.device.id_out_channels_set);
    if(a)
        numChans = b.SUBFIELD;
        numChans  = numChans  + 1;
        
        [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
        if(~a)
            % origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
        end
        
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_out_channels_set, handles.jvx_struct.properties.device.id_out_channels_set,numChans);
        if(~a)
            % origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false); 
        end
        
        [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
        if(~a)
            % origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
        end
        
        % Obtain references to all relevant properties
        [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
        
        % Show current status
        [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
        
        guidata(hObject, handles);       
    end
end



function edit_out_text_Callback(hObject, eventdata, handles)
% hObject    handle to edit_out_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_out_text as text
%        str2double(get(hObject,'String')) returns contents of edit_out_text as a double
txt = get(hObject,'String');

[a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
if(~a)
    % origFile, errCode, errOperation, errTxt, errorsAsWarnings
    jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
end

[a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_out_text, handles.jvx_struct.properties.device.id_mat_out_text, txt);
if(~a)
    % origFile, errCode, errOperation, errTxt, errorsAsWarnings             
    jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
end
selection = jvxBitField.jvx_create(-1);
[a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_out_is_file, handles.jvx_struct.properties.device.id_mat_out_is_file, selection);
if(~a)
    % origFile, errCode, errOperation, errTxt, errorsAsWarnings             
    jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
end
    
 [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
 if(~a)
     % origFile, errCode, errOperation, errTxt, errorsAsWarnings
     jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
 end
% Show current status
[hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
[hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    
guidata(hObject, handles);


% --- Executes during object creation, after setting all properties.
function edit_out_text_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_out_text (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton10.
function pushbutton10_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton10 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[f, p] = uiputfile({'*.wav', 'Wav Audiofiles'});
if(handles.local.use_absolut_pathnames == false)
    p = jvxJvxHost.jvx_relative_path(p);
end
if(strcmp(p, ''))
    fname = f;
else
    fname = [p '/' f];
end

[a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
if(~a)
    % origFile, errCode, errOperation, errTxt, errorsAsWarnings
    jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
end

[a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_out_text, handles.jvx_struct.properties.device.id_mat_out_text, fname);
if(~a)
    % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
    jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
end
selection = jvxBitField.jvx_create(0);
[a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_out_is_file, handles.jvx_struct.properties.device.id_mat_out_is_file, selection);
if(~a)
    % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
    jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
end
    
 [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
 if(~a)
     % origFile, errCode, errOperation, errTxt, errorsAsWarnings
     jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
 end
 
% Show current status
[hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
[hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    
guidata(hObject, handles);

% --- Executes on button press in checkbox_out_is_file.
function checkbox_out_is_file_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox_out_is_file (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox_out_is_file
% Show current status
[hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
[hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);

guidata(hObject, handles);

% --- Executes on button press in checkbox_in_is_file.
function checkbox_in_is_file_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox_in_is_file (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox_in_is_file
% Show current status
[hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
[hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);

guidata(hObject, handles);


% --- Executes when user attempts to close figure1.
function figure1_CloseRequestFcn(hObject, eventdata, handles)
% hObject    handle to figure1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
jvxJvxHostOff.jvx_shutdown(handles);

% Hint: delete(hObject) closes the figure
delete(hObject);



function edit_in_var_rate_Callback(hObject, eventdata, handles)
% hObject    handle to edit_in_var_rate (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_in_var_rate as text
%        str2double(get(hObject,'String')) returns contents of edit_in_var_rate as a double
rate =  str2double(get(hObject,'String'));

[handles] = jvxHost_off_remote_offline_set_samplerate_inmat(hObject, handles, rate); 

% Obtain references to all relevant properties
%[hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
% handles.jvx_struct.data.input.rate_user = rate;
% [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);

guidata(hObject, handles);


% --- Executes during object creation, after setting all properties.
function edit_in_var_rate_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_in_var_rate (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes when selected cell(s) is changed in uitable_inchannels.
function uitable_inchannels_CellSelectionCallback(hObject, eventdata, handles)
% hObject    handle to uitable_inchannels (see GCBO)
% eventdata  structure with the following fields (see UITABLE)
%	Indices: row and column indices of the cell(s) currently selecteds
% handles    structure with handles and user data (see GUIDATA)
if(~isempty(eventdata.Indices))
    rowIdx = eventdata.Indices(1);
    colIdx = eventdata.Indices(2);
    
    if(handles.jvx_struct.properties.device.id_in_channels >= 0)
        if(colIdx == 1)
            handles.jvx_struct.properties.channels.input.id_user = rowIdx;
        end
        
        if(colIdx == 2)
            handles.jvx_struct.properties.channels.input.id_user = rowIdx;
                        
            [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_in_channels, handles.jvx_struct.properties.device.id_in_channels);
            if(a)
                selection =b.SUBFIELD.SELECTION_BITFIELD;
                selection = jvxBitField.jvx_toggle(selection, rowIdx-1);
                
                [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
                if(~a)
                    % origFile, errCode, errOperation, errTxt, errorsAsWarnings
                    jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
                end

                
                [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_in_channels, handles.jvx_struct.properties.device.id_in_channels, selection);
                 if(~a)
                     % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
                    jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
                end
                
                [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
                if(~a)
                    % origFile, errCode, errOperation, errTxt, errorsAsWarnings
                    jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
                end
        
                % Show current status
                [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
                [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
            
            else
                % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
                jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'get_property_uniqueid', b.DESCRIPTION_STRING, false);
            end
        end
    
        [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
        [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    end
    guidata(hObject, handles);
end


% --- Executes when selected cell(s) is changed in uitable_outchannels.
function uitable_outchannels_CellSelectionCallback(hObject, eventdata, handles)
% hObject    handle to uitable_outchannels (see GCBO)
% eventdata  structure with the following fields (see UITABLE)
%	Indices: row and column indices of the cell(s) currently selecteds
% handles    structure with handles and user data (see GUIDATA)
if(~isempty(eventdata.Indices))
    rowIdx = eventdata.Indices(1);
    colIdx = eventdata.Indices(2);
    
    if(handles.jvx_struct.properties.device.id_out_channels >= 0)
        if(colIdx == 1)
            handles.jvx_struct.properties.channels.output.id_user = rowIdx;
        end
        
        if(colIdx == 2)
            handles.jvx_struct.properties.channels.output.id_user = rowIdx;
            [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_out_channels, handles.jvx_struct.properties.device.id_out_channels);
            if(a)
                selection =b.SUBFIELD.SELECTION_BITFIELD;
                selection = jvxBitField.jvx_toggle(selection, rowIdx-1);
                
                [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
                if(~a)
                    % origFile, errCode, errOperation, errTxt, errorsAsWarnings
                    jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
                end
        
                [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_out_channels, handles.jvx_struct.properties.device.id_out_channels, selection);
                if(~a)
                    % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
                    jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
                end
                
                [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
                if(~a)
                    % origFile, errCode, errOperation, errTxt, errorsAsWarnings
                    jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
                end
            
                % Show current status
                [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
                [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
            else
               % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
               jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'get_property_uniqueid', b.DESCRIPTION_STRING, false);
            end
        end
    
        [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
        [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    end
    guidata(hObject, handles);
end


% --- Executes on button press in pushbutton_plot_output.
function pushbutton_plot_output_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_plot_output (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if(handles.jvx_struct.data.channels.plot.id_user > 0)
    
     if(handles.jvx_struct.data.channels.plot.id_user > size(handles.jvx_struct.data.input.data,1))
        oneBuf = handles.jvx_struct.data.output.data(handles.jvx_struct.data.channels.plot.id_user - size(handles.jvx_struct.data.input.data,1),:);
    else
        oneBuf = handles.jvx_struct.data.input.data(handles.jvx_struct.data.channels.plot.id_user,:);
    end
    figure; plot(oneBuf);
end


% --- Executes on button press in pushbutton_stopplay.
function pushbutton_stopplay_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_stopplay (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if(~isempty(handles.jvx_struct.data.player))
    stop(handles.jvx_struct.data.player);
end


% --- Executes on button press in checkbox_stereo.
function checkbox_stereo_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox_stereo (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox_stereo
handles.jvx_struct.data.stereo = get(hObject,'Value');
if(handles.jvx_struct.properties.device.id_mat_play_stereo>= 0)
    if(handles.jvx_struct.data.stereo)
        selection = jvxBitField.jvx_create(0);
    else
        selection = jvxBitField.jvx_create(-1);
    end
    
    [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
    end
    
    [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_play_stereo, ...
        handles.jvx_struct.properties.device.id_mat_play_stereo, selection);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
    end
    
    [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
    end
    
    % Show current status
    [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
    [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    
    guidata(hObject, handles);
end


function edit_start_cb_Callback(hObject, eventdata, handles)
% hObject    handle to edit_start_cb (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_start_cb as text
%        str2double(get(hObject,'String')) returns contents of edit_start_cb as a double
newName = get(hObject,'String');
if(handles.jvx_struct.properties.node.offline_node.id_start_function)
   
    [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
    end
    
    [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties.node.offline_node.cat_start_function, handles.jvx_struct.properties.node.offline_node.id_start_function, newName);
    if(~a)
        % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
    end

    [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
    end
    
   % Obtain references to all relevant properties
    [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
    
    % Show current status
    [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    
    guidata(hObject, handles);
end

% --- Executes during object creation, after setting all properties.
function edit_start_cb_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_start_cb (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit_stop_cb_Callback(hObject, eventdata, handles)
% hObject    handle to edit_stop_cb (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_stop_cb as text
%        str2double(get(hObject,'String')) returns contents of edit_stop_cb as a double
newName = get(hObject,'String');
if(handles.jvx_struct.properties.node.offline_node.id_stop_function)
    
    [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
    end
    
    [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties.node.offline_node.cat_stop_function, handles.jvx_struct.properties.node.offline_node.id_stop_function, newName);
    if(~a)
        % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
    end

    [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
    end
    
   % Obtain references to all relevant properties
    [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
    
    % Show current status
    [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    
    guidata(hObject, handles);
end



% --- Executes during object creation, after setting all properties.
function edit_stop_cb_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_stop_cb (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit_process_cb_Callback(hObject, eventdata, handles)
% hObject    handle to edit_process_cb (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_process_cb as text
%        str2double(get(hObject,'String')) returns contents of edit_process_cb as a double
newName = get(hObject,'String');
if(handles.jvx_struct.properties.node.offline_node.id_process_function)
    
    [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
    end
    
    [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties.node.offline_node.cat_process_function, handles.jvx_struct.properties.node.offline_node.id_process_function, newName);
    if(~a)
        % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
    end

    [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
    end
   % Obtain references to all relevant properties
    [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
    
    % Show current status
    [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    
    guidata(hObject, handles);
end
    

% --- Executes during object creation, after setting all properties.
function edit_process_cb_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_process_cb (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton_force_stop.
function pushbutton_force_stop_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_force_stop (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[handles] = jvxHost_off_remote_force_stop_core(hObject, handles);
guidata(hObject, handles);


% --- Executes on selection change in popupmenu_subproj.
function popupmenu_subproj_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_subproj (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_subproj contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_subproj
idSel = get(hObject,'Value');
if(handles.jvx_struct.properties.node.offline_node.id_subproject_name >= 0)
    if(idSel == 1)
        nm = 'jvxDefault';
    else
        nm = handles.jvx_struct.subprojects{idSel-1};
    end
    
    [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
    end
    
     [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties.node.offline_node.cat_subproject_name, handles.jvx_struct.properties.node.offline_node.id_subproject_name, nm);
     if(~a)
        % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
     end
    
     [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
    end
    
    % If we have modified the subproject the host has not set the right
    % path. You need to restart to take effect
    handles.jvx_struct.allowStart = false;
    guidata(hObject, handles);
    
        % Obtain references to all relevant properties
    [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
    
    % Show current status
    [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    
    guidata(hObject, handles);
end

% --- Executes during object creation, after setting all properties.
function popupmenu_subproj_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_subproj (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit_b4_process_cb_Callback(hObject, eventdata, handles)
% hObject    handle to edit_b4_process_cb (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_b4_process_cb as text
%        str2double(get(hObject,'String')) returns contents of edit_b4_process_cb as a double
newName = get(hObject,'String');
if(handles.jvx_struct.properties.node.offline_node.id_b4_process_function)
    
    [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
    end
    
    [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.nodes.comp_type, handles.jvx_struct.properties.node.offline_node.cat_b4_process_function, handles.jvx_struct.properties.node.offline_node.id_b4_process_function, newName);
    if(~a)
        % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
    end

    [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
    end
    
   % Obtain references to all relevant properties
    [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
    
    % Show current status
    [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    
    guidata(hObject, handles);
end

% --- Executes during object creation, after setting all properties.
function edit_b4_process_cb_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_b4_process_cb (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
   
function edit_ylow_Callback(hObject, eventdata, handles)
% hObject    handle to edit_ylow (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_ylow as text
%        str2double(get(hObject,'String')) returns contents of edit_ylow as a double
newVal = str2double(get(hObject,'String'));
handles.jvx_struct.data.ylow = newVal;
[hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
[hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
 guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function edit_ylow_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_ylow (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit_yhigh_Callback(hObject, eventdata, handles)
% hObject    handle to edit_yhigh (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_yhigh as text
%        str2double(get(hObject,'String')) returns contents of edit_yhigh as a double
newVal = str2double(get(hObject,'String'));
handles.jvx_struct.data.yhigh = newVal;
[hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
 [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
 guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function edit_yhigh_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_yhigh (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit_xlow_Callback(hObject, eventdata, handles)
% hObject    handle to edit_xlow (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_xlow as text
%        str2double(get(hObject,'String')) returns contents of edit_xlow as a double
newVal = str2double(get(hObject,'String'));
handles.jvx_struct.data.xlow = newVal;
[hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
 [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
 guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function edit_xlow_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_xlow (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit_xhigh_Callback(hObject, eventdata, handles)
% hObject    handle to edit_xhigh (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_xhigh as text
%        str2double(get(hObject,'String')) returns contents of edit_xhigh as a double
newVal = str2double(get(hObject,'String'));
handles.jvx_struct.data.xhigh = newVal;
[hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
 [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
 guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function edit_xhigh_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_xhigh (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in checkbox4.
function checkbox4_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox4
% TODO: Engage/disengage Matlab processing
selection = get(hObject,'Value');
if(handles.jvx_struct.properties.node.offline_node.id_engange_matlab)
    
    val16 = int16(0);
    if(selection)
        val16 = int16(1);
    end
      
    jvxHost_off_remote_offline_set_engage_matlab_core(hObject, handles, val16);

    % Obtain references to all relevant properties
    [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
    
    % Show current status
    [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    
    guidata(hObject, handles);
end

 % ===================================================
 % ===================================================
 % The core functions
function [handles] = jvxHost_off_remote_run_core(hObject, handles)

    global inProcessing;
    skip_shutdown_seq = true;
    statRunning = jvxJvxHost.jvx_lookup_type_id__name('jvxSequencerStatus', 'JVX_SEQUENCER_STATUS_IN_OPERATION');
    
    if(jvxJvxHost.jvx_ready_for_start(handles))
        
        skip_save_data = true;
        [status_seq, hObject, handles] = jvxJvxHostOff.jvx_start_processing(hObject, handles);
                
        if(status_seq == statRunning)
            [hObject, handles] = jvxJvxHostOff.jvx_core_processing(hObject, handles);
            skip_save_data = false;
            skip_shutdown_seq = false;
        end
        
        [hObject, handles] = jvxJvxHostOff.jvx_stop_processing(hObject, handles, skip_shutdown_seq);
        [hObject, handles] = jvxJvxHostOff.jvx_post_processing(hObject, handles, skip_save_data);
        
    else
        
        inProcessing.isRunning = false;
        
    end
    
    % Obtain references to all relevant properties
    [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
    
    % Show current status
    [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);

 % #########################################################
 
function [handles] = jvxHost_off_remote_force_stop_core(hObject, handles)

    if(~jvxJvxHost.jvx_ready_for_start(handles))
        [hObject, handles] = jvxJvxHostOff.jvx_stop_processing(hObject, handles, false);
    
        % Obtain references to all relevant properties
        [hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);
        
        % Show current status
        [hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);
    end

 % #########################################################
 
function [handles] = jvxHost_off_remote_offline_input_variable_core(hObject, handles, idSel, rate)
    
    [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
    end
    
    [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_text, handles.jvx_struct.properties.device.id_mat_in_text, handles.jvx_struct.data.input.vars{idSel});
    if(~a)
        % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
    end
    selection = jvxBitField.jvx_create(-1);
    [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_is_file, handles.jvx_struct.properties.device.id_mat_in_is_file, selection);
    if(~a)
        % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
    end
    
    if(nargin < 4)
        if(handles.jvx_struct.properties.device.id_mat_in_rate)
            [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_rate, handles.jvx_struct.properties.device.id_mat_in_rate);
            if(a)
                rate = b.SUBFIELD;
            end
        else
            [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_rate, handles.jvx_struct.properties.device.id_mat_in_rate, rate);
            if(~a)
                % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
                jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
            end
        end
    end
    
    eval(['global ' handles.jvx_struct.data.input.vars{idSel} ';']);
    disp(['Reading variable ' handles.jvx_struct.data.input.vars{idSel} ' to collect input data.']); 
    eval(['handles.jvx_struct.data.input.data = ' handles.jvx_struct.data.input.vars{idSel} ';']);
    handles.jvx_struct.data.input.rate = double(rate);
    
    if(handles.jvx_struct.properties.device.id_srate > 0)
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_srate, handles.jvx_struct.properties.device.id_srate,int32(handles.jvx_struct.data.input.rate));
        if(a)
        end
    end
    
    if(handles.jvx_struct.properties.device.id_in_channels_set > 0)
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_in_channels_set, handles.jvx_struct.properties.device.id_in_channels_set,int32(size(handles.jvx_struct.data.input.data,1)));
        if(a)
        end
    end
    
    [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
    end        
    % #########################################################
 
function [handles] = jvxHost_off_remote_offline_input_variable_direct_core(hObject, handles, var, rate)
    
    [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
    end
    
    [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_text, handles.jvx_struct.properties.device.id_mat_in_text, 'direct');
    if(~a)
        % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
    end
    selection = jvxBitField.jvx_create(-1);
    [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_is_file, handles.jvx_struct.properties.device.id_mat_in_is_file, selection);
    if(~a)
        % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
    end
    
    if(nargin < 4)
        if(handles.jvx_struct.properties.device.id_mat_in_rate)
            [a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_rate, handles.jvx_struct.properties.device.id_mat_in_rate);
            if(a)
                rate = b.SUBFIELD;
            end
        else
            [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_mat_in_rate, handles.jvx_struct.properties.device.id_mat_in_rate, rate);
            if(~a)
                % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
                jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
            end
        end
    end
    
    disp(['Specifying input variable directly.']); 
    handles.jvx_struct.data.input.data = var;
    handles.jvx_struct.data.input.rate = double(rate);
    
    if(handles.jvx_struct.properties.device.id_srate > 0)
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_srate, handles.jvx_struct.properties.device.id_srate,int32(handles.jvx_struct.data.input.rate));
        if(a)
        end
    end
    
    if(handles.jvx_struct.properties.device.id_in_channels_set > 0)
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_in_channels_set, handles.jvx_struct.properties.device.id_in_channels_set,int32(size(handles.jvx_struct.data.input.data,1)));
        if(a)
        end
    end
    
    [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
    end        
    % #########################################################
    
function [handles] = jvxHost_off_remote_offline_set_samplerate_core(hObject, handles, rate)
    
    if(handles.jvx_struct.properties.device.id_srate >= 0)
        
        [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
        if(~a)
            % origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
        end
        
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type,handles.jvx_struct.properties.device.cat_srate, handles.jvx_struct.properties.device.id_srate, int32(rate));
        if(~a)
            % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
        end
        
        [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
        if(~a)
            % origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
        end
    end

        % #########################################################

 function [handles] = jvxHost_off_remote_offline_set_samplerate_inmat(hObject, handles, rate)
    
    if(handles.jvx_struct.properties.device.id_mat_in_rate >= 0)
        
        [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
        if(~a)
            % origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
        end
        
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type,handles.jvx_struct.properties.device.cat_mat_in_rate, handles.jvx_struct.properties.device.id_mat_in_rate, int32(rate));
        if(~a)
            % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
        end
        
        [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
        if(~a)
            % origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
        end
    end

 
    % #########################################################
    
function [handles] = jvxHost_off_remote_offline_set_buffersize_core(hObject, handles, bsize)
    
    if(handles.jvx_struct.properties.device.id_srate >= 0)
        [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
        if(~a)
            % origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
        end
        [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.devices.comp_type,handles.jvx_struct.properties.device.cat_bsize, handles.jvx_struct.properties.device.id_bsize, int32(bsize));
        if(~a)
            % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings
            jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
        end
        
        [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
    end
    end
   
function [handles] = jvxHost_off_remote_offline_set_engage_matlab_core(hObject, handles, doEngageI16)
    
    [a stb]= handles.hostcall('start_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), stb.ERRORID_INT32, 'start_property_group', stb.DESCRIPTION_STRING, false);
    end
    
    [a b]= handles.hostcall('set_property_uniqueid', handles.jvx_struct.nodes.comp_type, ...
        handles.jvx_struct.properties.node.offline_node.cat_engange_matlab, handles.jvx_struct.properties.node.offline_node.id_engange_matlab, doEngageI16);
    if(~a)
        % REPLACEME origFile, errCode, errOperation, errTxt, errorsAsWarnings             
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'set_property_uniqueid', b.DESCRIPTION_STRING, false);
    end
    
    [a b]= handles.hostcall('stop_property_group', handles.jvx_struct.devices.comp_type);
    if(~a)
        % origFile, errCode, errOperation, errTxt, errorsAsWarnings
        jvxJvxHost.jvx_display_error(mfilename('fullpath'), b.ERRORID_INT32, 'stop_property_group', b.DESCRIPTION_STRING, false);
    end
    
% --- Executes on button press in pushbutton14.
function pushbutton14_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton14 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
jvxJvxHost.jvx_view_sequencer(handles);

% ======================================================
% ======================================================
% Remote function calls


% Some remote access functions
function [res] = jvxHost_off_remote_run()
    res = true;
    global jvxHost_off_remote;
    handles = guidata(jvxHost_off_remote.hObject);
    [handles]  =  jvxHost_off_remote_run_core(jvxHost_off_remote.hObject, handles);
    guidata(jvxHost_off_remote.hObject, handles);

    % ======================================================
 function [res] = jvxHost_off_remote_force_stop()
     res = true;
     global jvxHost_off_remote;
    handles = guidata(jvxHost_off_remote.hObject);
    [handles]  =  jvxHost_off_remote_force_stop_core(jvxHost_off_remote.hObject, handles);
    guidata(jvxHost_off_remote.hObject, handles);   
    
    % ======================================================
    
function [res] = jvxHost_off_remote_trigger_mesgq_immediately()
    global jvx_host_call_global;
    [res err] = jvx_host_call_global('trigger_process_msgq');
    if(~res)
        
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
    % ======================================================
    
function [res] = jvxHost_off_remote_selection_in_variable(name_var,rate)
     res = true;
     global jvxHost_off_remote;
    handles = guidata(jvxHost_off_remote.hObject);
    
      % Obtain references to all relevant properties
    [jvxHost_off_remote.hObject, handles] = jvxJvxHostOff.jvx_refresh_props(jvxHost_off_remote.hObject, handles);
    
    % Show current status
    [jvxHost_off_remote.hObject, handles] = jvxJvxHostOff.jvx_update_ui(jvxHost_off_remote.hObject, handles);
    
    idSel = -1;
    for(ind=1:size(handles.jvx_struct.data.input.vars,1))
        if(strcmp(handles.jvx_struct.data.input.vars{ind}, name_var))
            idSel = ind;
            break;
        end
    end
    
    if(idSel > 0)
        if(nargin < 2)
            [handles]  =  jvxHost_off_remote_offline_input_variable_core(jvxHost_off_remote.hObject, handles, idSel);
        else
            [handles]  =  jvxHost_off_remote_offline_input_variable_core(jvxHost_off_remote.hObject, handles, idSel, rate);
        end
        
        % Obtain references to all relevant properties
        [jvxHost_off_remote.hObject, handles] = jvxJvxHostOff.jvx_refresh_props(jvxHost_off_remote.hObject, handles);
        
        % Show current status
        [jvxHost_off_remote.hObject, handles] = jvxJvxHostOff.jvx_update_ui(jvxHost_off_remote.hObject, handles);
        
        guidata(jvxHost_off_remote.hObject, handles);
    else
        warning(['Unable to set variable with index ' num2str(idSel) ' as input data.']);
        res = false;
    end
 
    % ======================================================
    
    function [res] = jvxHost_off_remote_selection_in_variable_direct(var,rate)
        res = true;
        global jvxHost_off_remote;
        handles = guidata(jvxHost_off_remote.hObject);
        
        % Obtain references to all relevant properties
        [jvxHost_off_remote.hObject, handles] = jvxJvxHostOff.jvx_refresh_props(jvxHost_off_remote.hObject, handles);
        
        % Show current status
        [jvxHost_off_remote.hObject, handles] = jvxJvxHostOff.jvx_update_ui(jvxHost_off_remote.hObject, handles);
        
        if(nargin < 2)
            [handles]  =  jvxHost_off_remote_offline_input_variable_direct_core(jvxHost_off_remote.hObject, handles, var);
        else
            [handles]  =  jvxHost_off_remote_offline_input_variable_direct_core(jvxHost_off_remote.hObject, handles, var, rate);
        end
        
        % Obtain references to all relevant properties
        [jvxHost_off_remote.hObject, handles] = jvxJvxHostOff.jvx_refresh_props(jvxHost_off_remote.hObject, handles);
        
        % Show current status
        [jvxHost_off_remote.hObject, handles] = jvxJvxHostOff.jvx_update_ui(jvxHost_off_remote.hObject, handles);
        
        guidata(jvxHost_off_remote.hObject, handles);
        
        % ======================================================
 function [res] = jvxHost_off_remote_set_samplerate(rate)
     res = true;
     global jvxHost_off_remote;
    handles = guidata(jvxHost_off_remote.hObject);
    [handles]  =  jvxHost_off_remote_offline_set_samplerate_core(jvxHost_off_remote.hObject, handles, rate);
    guidata(jvxHost_off_remote.hObject, handles);   
    
    % ======================================================

     function [res] = jvxHost_off_remote_set_buffersize(bsize)
         res = true;
         global jvxHost_off_remote;
         handles = guidata(jvxHost_off_remote.hObject);
         [handles]  =  jvxHost_off_remote_offline_set_buffersize_core(jvxHost_off_remote.hObject, handles, bsize);
         guidata(jvxHost_off_remote.hObject, handles);
     
     
     function [res] = jvxHost_off_remote_set_engage_matlab(doEngage)
         res = true;
         global jvxHost_off_remote;
         handles = guidata(jvxHost_off_remote.hObject);
         doEngageI16 = int16(0);
         if(nargin == 0)
             doEngageI16 = int16(1);
         else
             if(doEngage)
                 doEngageI16 = int16(1);
             end
         end
         [handles]  =  jvxHost_off_remote_offline_set_engage_matlab_core(jvxHost_off_remote.hObject, handles, doEngageI16);
         guidata(jvxHost_off_remote.hObject, handles);
     


