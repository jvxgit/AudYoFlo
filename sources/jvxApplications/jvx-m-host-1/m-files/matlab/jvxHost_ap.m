function varargout = jvxHost_ap(varargin)
% JVXHOST_AP MATLAB code for jvxHost_ap.fig
%      JVXHOST_AP, by itself, creates a new JVXHOST_AP or raises the existing
%      singleton*.
%
%      H = JVXHOST_AP returns the handle to a new JVXHOST_AP or the handle to
%      the existing singleton*.
%
%      JVXHOST_AP('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in JVXHOST_AP.M with the given input arguments.
%
%      JVXHOST_AP('Property','Value',...) creates a new JVXHOST_AP or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before jvxHost_ap_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to jvxHost_ap_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help jvxHost_ap

% Last Modified by GUIDE v2.5 05-Mar-2018 09:20:05

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @jvxHost_ap_OpeningFcn, ...
                   'gui_OutputFcn',  @jvxHost_ap_OutputFcn, ...
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


% --- Executes just before jvxHost_ap is made visible.
function jvxHost_ap_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to jvxHost_ap (see VARARGIN)

% Choose default command line output for jvxHost_ap
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes jvxHost_ap wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = jvxHost_ap_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on selection change in popupmenu_input_lahead.
function popupmenu_input_lahead_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_input_lahead (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_input_lahead contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_input_lahead
idSelect = get(hObject, 'Value');
if(handles.jvx_struct.properties.technology.id_input_file_lookahead >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    if(handles_parent.jvx_struct.properties.input_files.input.id_user >= 0)
        idSelect = idSelect - 1;
        % ==============================================
        % Set new selection processing format
        % ==============================================
        
        [a b]= handles.hostcall('set_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
            handles.jvx_struct.properties.technology.cat_input_file_lookahead, ...
            handles.jvx_struct.properties.technology.id_input_file_lookahead, ...
            handles_parent.jvx_struct.properties.input_files.input.id_user, ...
            int16(idSelect));
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        % ==============================================
        % Entry point for update is the parental window
        % ==============================================
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    end
     guidata(hObject_parent, handles_parent);
end


% --- Executes during object creation, after setting all properties.
function popupmenu_input_lahead_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_input_lahead (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in checkbox_input_boost.
function checkbox_input_boost_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox_input_boost (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox_input_boost
toggle = get(hObject, 'Value');
if(handles.jvx_struct.properties.technology.id_input_file_boost >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    if(handles_parent.jvx_struct.properties.input_files.input.id_user >= 0)
        
        % ==============================================
        % Set new selection processing format
        % ==============================================
        
        selection = int16(0);
        if(toggle)
            selection = int16(1);
        end
        [a b]= handles.hostcall('set_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
            handles.jvx_struct.properties.technology.cat_input_file_boost, ...
            handles.jvx_struct.properties.technology.id_input_file_boost, ...
            handles_parent.jvx_struct.properties.input_files.input.id_user, ...
            selection);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        % ==============================================
        % Entry point for update is the parental window
        % ==============================================
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    end
     guidata(hObject_parent, handles_parent);
end


% --- Executes on button press in checkbox_input_loop.
function checkbox_input_loop_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox_input_loop (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox_input_loop
toggle = get(hObject, 'Value');
if(handles.jvx_struct.properties.technology.id_input_file_loop >= 0)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    if(handles_parent.jvx_struct.properties.input_files.input.id_user >= 1)
        
        selection = int16(0);
        if(toggle)
            selection = int16(1);
        end
        [a b]= handles.hostcall('set_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
            handles.jvx_struct.properties.technology.cat_input_file_loop, ...
            handles.jvx_struct.properties.technology.id_input_file_loop, ...
            handles_parent.jvx_struct.properties.input_files.input.id_user, ...
             selection);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        % ==============================================
        % Entry point for update is the parental window
        % ==============================================
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    end
     guidata(hObject_parent, handles_parent);
end

% --- Executes on button press in pushbutton_input_restart.
function pushbutton_input_restart_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_input_restart (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if(handles.jvx_struct.isRunning)
    if(handles.jvx_struct.properties.technology.id_file_restart >= 0)
        
        hObject_parent = handles.parent;
        handles_parent = guidata(hObject_parent);
        
        if(handles_parent.jvx_struct.properties.input_files.input.id_user >= 0)
            
            selection = int16(1);
            
            [a b]= handles.hostcall('set_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
                handles.jvx_struct.properties.technology.cat_file_restart, ...
                handles.jvx_struct.properties.technology.id_file_restart, ...
                handles_parent.jvx_struct.properties.input_files.input.id_user, ...
                selection);
            if(~a)
                error(['Unexpected error: '  b.DESCRIPTION_STRING]);
            end
        end
        guidata(hObject_parent, handles_parent);
    end
end
        

% --- Executes on button press in pushbutton_input_rewind.
function pushbutton_input_rewind_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_input_rewind (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if(handles.jvx_struct.isRunning)
    if(handles.jvx_struct.properties.technology.id_file_bwd >= 0)
        
        hObject_parent = handles.parent;
        handles_parent = guidata(hObject_parent);
        
        if(handles_parent.jvx_struct.properties.input_files.input.id_user >= 0)
            
            selection = int16(1);
            
            [a b]= handles.hostcall('set_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
                handles.jvx_struct.properties.technology.cat_file_bwd, ...
                handles.jvx_struct.properties.technology.id_file_bwd, ...
                handles_parent.jvx_struct.properties.input_files.input.id_user, ...
                selection);
            if(~a)
                error(['Unexpected error: '  b.DESCRIPTION_STRING]);
            end
        end
        guidata(hObject_parent, handles_parent);
    end
end
        

% --- Executes on button press in pushbutton_input_play.
function pushbutton_input_play_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_input_play (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if(handles.jvx_struct.isRunning)
    if(handles.jvx_struct.properties.technology.id_file_play >= 0)
        
        hObject_parent = handles.parent;
        handles_parent = guidata(hObject_parent);
        
        if(handles_parent.jvx_struct.properties.input_files.input.id_user >= 0)
            
            selection = int16(1);
            
            [a b]= handles.hostcall('set_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
                handles.jvx_struct.properties.technology.cat_file_play, ...
                handles.jvx_struct.properties.technology.id_file_play, ...
                handles_parent.jvx_struct.properties.input_files.input.id_user, ...
                selection);
            if(~a)
                error(['Unexpected error: '  b.DESCRIPTION_STRING]);
            end
        end
        guidata(hObject_parent, handles_parent);
    end
end
        

% --- Executes on selection change in popupmenu_output_lahead.
function popupmenu_output_lahead_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_output_lahead (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_output_lahead contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_output_lahead
idSelect = get(hObject, 'Value');
if(handles.jvx_struct.properties.technology.id_output_file_lookahead >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    if(handles_parent.jvx_struct.properties.output_files.output.id_user >= 0)
        idSelect = idSelect - 1;
        % ==============================================
        % Set new selection processing format
        % ==============================================
        selection = int16(idSelect);
        
        [a b]= handles.hostcall('set_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
            handles.jvx_struct.properties.technology.cat_output_file_lookahead, ...
            handles.jvx_struct.properties.technology.id_output_file_lookahead, ...
            handles_parent.jvx_struct.properties.output_files.output.id_user, ...
            selection);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        % ==============================================
        % Entry point for update is the parental window
        % ==============================================
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    end
     guidata(hObject_parent, handles_parent);
end

% --- Executes during object creation, after setting all properties.
function popupmenu_output_lahead_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_output_lahead (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton_input_pause.
function pushbutton_input_pause_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_input_pause (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if(handles.jvx_struct.isRunning)
    if(handles.jvx_struct.properties.technology.id_file_pause >= 0)
        
        hObject_parent = handles.parent;
        handles_parent = guidata(hObject_parent);
        
        if(handles_parent.jvx_struct.properties.input_files.input.id_user >= 0)
            
            selection = int16(1);
            
            [a b]= handles.hostcall('set_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
                handles.jvx_struct.properties.technology.cat_file_pause, ...
                handles.jvx_struct.properties.technology.id_file_pause, ...
                handles_parent.jvx_struct.properties.input_files.input.id_user, ...
                selection);
            if(~a)
                error(['Unexpected error: '  b.DESCRIPTION_STRING]);
            end
        end
        guidata(hObject_parent, handles_parent);
    end
end
        

% --- Executes on button press in pushbutton_input_fwd.
function pushbutton_input_fwd_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_input_fwd (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if(handles.jvx_struct.isRunning)
    if(handles.jvx_struct.properties.technology.id_file_fwd >= 0)
        
        hObject_parent = handles.parent;
        handles_parent = guidata(hObject_parent);
        
        if(handles_parent.jvx_struct.properties.input_files.input.id_user >= 0)
            
            selection = int16(1);
            
            [a b]= handles.hostcall('set_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
                handles.jvx_struct.properties.technology.cat_file_fwd, ...
                handles.jvx_struct.properties.technology.id_file_fwd, ...
                handles_parent.jvx_struct.properties.input_files.input.id_user, ...
                selection);
            
            if(~a)
                error(['Unexpected error: '  b.DESCRIPTION_STRING]);
            end
        end
        guidata(hObject_parent, handles_parent);
    end
end
        


function edit_audiodev_rate_Callback(hObject, eventdata, handles)
% hObject    handle to edit_audiodev_rate (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_audiodev_rate as text
%        str2double(get(hObject,'String')) returns contents of edit_audiodev_rate as a double
value = str2double(get(hObject,'String'));
if(handles.jvx_struct.properties.device.id_aud_dev_rate >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    [a b]= handles.hostcall('set_property_uniqueid',handles.jvx_struct.devices.comp_type, ...
        handles.jvx_struct.properties.device.cat_aud_dev_rate, ...
        handles.jvx_struct.properties.device.id_aud_dev_rate, ...
        int32(value));
    if(~a)
        error(['Unexpected error: '  b.DESCRIPTION_STRING]);
    end
        
    % ==============================================
    % Entry point for update is the parental window
    % ==============================================
    [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
    [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    guidata(hObject_parent, handles_parent);
end



% --- Executes during object creation, after setting all properties.
function edit_audiodev_rate_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_audiodev_rate (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in popupmenu_audiodev_rates.
function popupmenu_audiodev_rates_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_audiodev_rates (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_audiodev_rates contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_audiodev_rates
idSelect = get(hObject, 'Value');
if(handles.jvx_struct.properties.device.id_aud_dev_rates >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    % ==============================================
    % Set new selection processing format
    % ==============================================
    [a b]= handles.hostcall('get_property_uniqueid',handles.jvx_struct.devices.comp_type, ...
        handles.jvx_struct.properties.device.cat_aud_dev_rates, ...
        handles.jvx_struct.properties.device.id_aud_dev_rates);
    if(~a)
        error(['Unexpected error: '  b.DESCRIPTION_STRING]);
    end
    
    jvxJvxHost.jvx_set_property_single_selection_mat_id(handles_parent, ...
        handles.jvx_struct.devices.comp_type, ...
        handles.jvx_struct.properties.device.cat_aud_dev_rates, ...
        handles.jvx_struct.properties.device.id_aud_dev_rates, idSelect);
    if(~a)
        error(['Unexpected error: '  b.DESCRIPTION_STRING]);
    end
    
    % ==============================================
    % Entry point for update is the parental window
    % ==============================================
    [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
    [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    guidata(hObject_parent, handles_parent);
end

% --- Executes during object creation, after setting all properties.
function popupmenu_audiodev_rates_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_audiodev_rates (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit_audiodev_bsize_Callback(hObject, eventdata, handles)
% hObject    handle to edit_audiodev_bsize (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_audiodev_bsize as text
%        str2double(get(hObject,'String')) returns contents of edit_audiodev_bsize as a double


% --- Executes during object creation, after setting all properties.
function edit_audiodev_bsize_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_audiodev_bsize (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in popupmenu_audiodev_bsizes.
function popupmenu_audiodev_bsizes_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_audiodev_bsizes (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_audiodev_bsizes contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_audiodev_bsizes
idSelect = get(hObject, 'Value');
if(handles.jvx_struct.properties.device.id_aud_dev_bsizes >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    % ==============================================
    % Set new selection processing format
    % ==============================================
    [a b]= handles.hostcall('get_property_uniqueid',handles.jvx_struct.devices.comp_type, ...
        handles.jvx_struct.properties.device.cat_aud_dev_bsizes, ...
        handles.jvx_struct.properties.device.id_aud_dev_bsizes);
    if(~a)
        error(['Unexpected error: '  b.DESCRIPTION_STRING]);
    end
    
    jvxJvxHost.jvx_set_property_single_selection_mat_id(handles_parent, ...
        handles.jvx_struct.devices.comp_type, ...
        handles.jvx_struct.properties.device.cat_aud_dev_bsizes, ...
        handles.jvx_struct.properties.device.id_aud_dev_bsizes, idSelect);
    if(~a)
        error(['Unexpected error: '  b.DESCRIPTION_STRING]);
    end
    
    % ==============================================
    % Entry point for update is the parental window
    % ==============================================
    [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
    [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    guidata(hObject_parent, handles_parent);
end

% --- Executes during object creation, after setting all properties.
function popupmenu_audiodev_bsizes_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_audiodev_bsizes (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit_output_channels_Callback(hObject, eventdata, handles)
% hObject    handle to edit_output_channels (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_output_channels as text
%        str2double(get(hObject,'String')) returns contents of edit_output_channels as a double


% --- Executes during object creation, after setting all properties.
function edit_output_channels_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_output_channels (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton_output_channels_dec.
function pushbutton_output_channels_dec_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_output_channels_dec (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if(handles.jvx_struct.properties.technology.id_output_file_chans >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    if( handles_parent.jvx_struct.properties.output_files.output.id_user >= 0)
        % ==============================================
        % Set new selection processing format
        % ==============================================
        [a b]= handles.hostcall('get_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
            handles.jvx_struct.properties.technology.cat_output_file_chans, ...
            handles.jvx_struct.properties.technology.id_output_file_chans, ...
             handles_parent.jvx_struct.properties.output_files.output.id_user);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        selection = b.SUBFIELD;
        selection = selection - 1;
        if(selection == 0)
            selection = 1;
        end
        
        [a b]= handles.hostcall('set_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
            handles.jvx_struct.properties.technology.cat_output_file_chans, ...
            handles.jvx_struct.properties.technology.id_output_file_chans, ...
             handles_parent.jvx_struct.properties.output_files.output.id_user, ...
             selection);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        % ==============================================
        % Entry point for update is the parental window
        % ==============================================
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    end
     guidata(hObject_parent, handles_parent);
end


% --- Executes on button press in pushbutton_output_channels_inc.
function pushbutton_output_channels_inc_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_output_channels_inc (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if(handles.jvx_struct.properties.technology.id_output_file_chans >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    if(handles_parent.jvx_struct.properties.output_files.output.id_user >= 0)
        % ==============================================
        % Set new selection processing format
        % ==============================================
        [a b]= handles.hostcall('get_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
            handles.jvx_struct.properties.technology.cat_output_file_chans, ...
            handles.jvx_struct.properties.technology.id_output_file_chans, ...
            handles_parent.jvx_struct.properties.output_files.output.id_user);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        selection = b.SUBFIELD;
        selection = selection + 1;
        
        [a b]= handles.hostcall('set_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
            handles.jvx_struct.properties.technology.cat_output_file_chans, ...
            handles.jvx_struct.properties.technology.id_output_file_chans, ...
            handles_parent.jvx_struct.properties.output_files.output.id_user, ...
            selection);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        % ==============================================
        % Entry point for update is the parental window
        % ==============================================
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    end
     guidata(hObject_parent, handles_parent);
end



function edit_output_rate_Callback(hObject, eventdata, handles)
% hObject    handle to edit_output_rate (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_output_rate as text
%        str2double(get(hObject,'String')) returns contents of edit_output_rate as a double
rate = str2double(get(hObject,'String'));
if(handles.jvx_struct.properties.technology.id_output_file_rate >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    if(handles_parent.jvx_struct.properties.output_files.output.id_user >= 0)
        % ==============================================
        % Set new selection processing format
        % ==============================================
        selection = int32(rate);
        
        [a b]= handles.hostcall('set_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
            handles.jvx_struct.properties.technology.cat_output_file_rate, ...
            handles.jvx_struct.properties.technology.id_output_file_rate, ...
            handles_parent.jvx_struct.properties.output_files.output.id_user, ...
            selection);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        % ==============================================
        % Entry point for update is the parental window
        % ==============================================
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    end
     guidata(hObject_parent, handles_parent);
end


% --- Executes during object creation, after setting all properties.
function edit_output_rate_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_output_rate (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in checkbox_output_boost.
function checkbox_output_boost_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox_output_boost (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox_output_boost
toggle = get(hObject, 'Value');
if(handles.jvx_struct.properties.technology.id_output_file_boost >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    if(handles_parent.jvx_struct.properties.output_files.output.id_user >= 0)
        
        % ==============================================
        % Set new selection processing format
        % ==============================================
        
        if(toggle)
            selection = int16(1);
        else
            selection = int16(0);
        end
        [a b]= handles.hostcall('set_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
            handles.jvx_struct.properties.technology.cat_output_file_boost, ...
            handles.jvx_struct.properties.technology.id_output_file_boost, ...
            handles_parent.jvx_struct.properties.output_files.output.id_user, ...
            selection);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        % ==============================================
        % Entry point for update is the parental window
        % ==============================================
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    end
     guidata(hObject_parent, handles_parent);
end


% --- Executes on button press in checkbox_output_32bit.
function checkbox_output_32bit_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox_output_32bit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox_output_32bit
toggle = get(hObject, 'Value');
if(handles.jvx_struct.properties.technology.id_output_file_32bit >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    fileId = handles_parent.jvx_struct.properties.output_files.output.id_user;
    
    if(fileId >= 1)
        
        % ==============================================
        % Set new selection processing format
        % ==============================================
        [a b]= handles.hostcall('get_property_uniqueid',handles.jvx_struct.technologies.comp_type, ...
            handles.jvx_struct.properties.technology.cat_output_file_32bit, ...
            handles.jvx_struct.properties.technology.id_output_file_32bit);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        selection = b.SUBFIELD;
        if(toggle)
            selection(fileId) = int16(1);
        else
            selection(fileId) = int16(0);
        end
        [a b]= handles.hostcall('set_property_uniqueid',handles.jvx_struct.technologies.comp_type, ...
            handles.jvx_struct.properties.technology.cat_output_file_32bit, ...
            handles.jvx_struct.properties.technology.id_output_file_32bit, selection);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        % ==============================================
        % Entry point for update is the parental window
        % ==============================================
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    end
     guidata(hObject_parent, handles_parent);
end

% --- Executes on selection change in popupmenu_audiodev_formats.
function popupmenu_audiodev_formats_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_audiodev_formats (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_audiodev_formats contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_audiodev_formats
idSelect = get(hObject, 'Value');
if(handles.jvx_struct.properties.device.id_aud_dev_formats >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    % ==============================================
    % Set new selection processing format
    % ==============================================
    [a b]= handles.hostcall('get_property_uniqueid',handles.jvx_struct.devices.comp_type, ...
        handles.jvx_struct.properties.device.cat_aud_dev_formats, ...
        handles.jvx_struct.properties.device.id_aud_dev_formats);
    if(~a)
        error(['Unexpected error: '  b.DESCRIPTION_STRING]);
    end
    
    jvxJvxHost.jvx_set_property_single_selection_mat_id(handles_parent,...
        handles.jvx_struct.devices.comp_type, ...
        handles.jvx_struct.properties.device.cat_aud_dev_formats, ...
        handles.jvx_struct.properties.device.id_aud_dev_formats, idSelect);
    if(~a)
        error(['Unexpected error: '  b.DESCRIPTION_STRING]);
    end
    
    % ==============================================
    % Entry point for update is the parental window
    % ==============================================
    [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
    [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    guidata(hObject_parent, handles_parent);
end


% --- Executes during object creation, after setting all properties.
function popupmenu_audiodev_formats_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_audiodev_formats (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in checkbox_audiodev_auto.
function checkbox_audiodev_auto_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox_audiodev_auto (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox_audiodev_auto
toggle = get(hObject, 'Value');
if(handles.jvx_struct.properties.device.id_auto_hw >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    % ==============================================
    
    % ==============================================
    [a b]= handles.hostcall('get_property_uniqueid',handles.jvx_struct.devices.comp_type, ...
        handles.jvx_struct.properties.device.cat_auto_hw, ...
        handles.jvx_struct.properties.device.id_auto_hw);
    if(~a)
        error(['Unexpected error: '  b.DESCRIPTION_STRING]);
    end
    
    selection = b.SUBFIELD;
    if(toggle)
        selection = int16(1);
    else
        selection = int16(0);
    end
    [a b]= handles.hostcall('set_property_uniqueid',handles.jvx_struct.devices.comp_type, ...
        handles.jvx_struct.properties.device.cat_auto_hw, ...
        handles.jvx_struct.properties.device.id_auto_hw, selection);
    if(~a)
        error(['Unexpected error: '  b.DESCRIPTION_STRING]);
    end
    
    % ==============================================
    % Entry point for update is the parental window
    % ==============================================
    [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
    [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    guidata(hObject_parent, handles_parent);
end

% --- Executes on selection change in popupmenu_qresampler_in.
function popupmenu_qresampler_in_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_qresampler_in (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_qresampler_in contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_qresampler_in
idSelect = get(hObject, 'Value');
if(handles.jvx_struct.properties.device.id_qresampler_in >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    % ==============================================
    % Set new selection processing format
    % ==============================================
    [a b]= handles.hostcall('get_property_uniqueid',handles.jvx_struct.devices.comp_type, ...
        handles.jvx_struct.properties.device.cat_qresampler_in, ...
        handles.jvx_struct.properties.device.id_qresampler_in);
    if(~a)
        error(['Unexpected error: '  b.DESCRIPTION_STRING]);
    end
    
    jvxJvxHost.jvx_set_property_single_selection_mat_id(handles_parent,...
        handles.jvx_struct.devices.comp_type, ...
        handles.jvx_struct.properties.device.cat_qresampler_in, ...
        handles.jvx_struct.properties.device.id_qresampler_in, idSelect);
    if(~a)
        error(['Unexpected error: '  b.DESCRIPTION_STRING]);
    end
    
    % ==============================================
    % Entry point for update is the parental window
    % ==============================================
    [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
    [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    guidata(hObject_parent, handles_parent);
end


% --- Executes during object creation, after setting all properties.
function popupmenu_qresampler_in_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_qresampler_in (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in popupmenu_qresampler_out.
function popupmenu_qresampler_out_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_qresampler_out (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_qresampler_out contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_qresampler_out
idSelect = get(hObject, 'Value');
if(handles.jvx_struct.properties.device.id_qresampler_out >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    % ==============================================
    % Set new selection processing format
    % ==============================================
    [a b]= handles.hostcall('get_property_uniqueid',handles.jvx_struct.devices.comp_type, ...
        handles.jvx_struct.properties.device.cat_qresampler_out, ...
        handles.jvx_struct.properties.device.id_qresampler_out);
    if(~a)
        error(['Unexpected error: '  b.DESCRIPTION_STRING]);
    end
    
    jvxJvxHost.jvx_set_property_single_selection_mat_id(handles_parent, ...
        handles.jvx_struct.devices.comp_type, ...
        handles.jvx_struct.properties.device.cat_qresampler_out, ...
        handles.jvx_struct.properties.device.id_qresampler_out, idSelect);
    if(~a)
        error(['Unexpected error: '  b.DESCRIPTION_STRING]);
    end
    
    % ==============================================
    % Entry point for update is the parental window
    % ==============================================
    [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
    [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    guidata(hObject_parent, handles_parent);
end


% --- Executes during object creation, after setting all properties.
function popupmenu_qresampler_out_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_qresampler_out (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton_asio_open.
function pushbutton_asio_open_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_asio_open (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

if(handles.jvx_struct.properties.device.id_ctrl_panel >= 1)
     
     hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    jvxJvxHost.jvx_set_property_single_selection_mat_id(handles_parent,...
        handles.jvx_struct.devices.comp_type, ...
        handles.jvx_struct.properties.device.cat_ctrl_panel, ...
        handles.jvx_struct.properties.device.id_ctrl_panel, 1);  
    
end



% --- Executes on selection change in popupmenu_alsa_access.
function popupmenu_alsa_access_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_alsa_access (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_alsa_access contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_alsa_access


% --- Executes during object creation, after setting all properties.
function popupmenu_alsa_access_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_alsa_access (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in popupmenu_alsa_int_datatype.
function popupmenu_alsa_int_datatype_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_alsa_int_datatype (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_alsa_int_datatype contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_alsa_int_datatype


% --- Executes during object creation, after setting all properties.
function popupmenu_alsa_int_datatype_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_alsa_int_datatype (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit_alsa_num_per_in_Callback(hObject, eventdata, handles)
% hObject    handle to edit_alsa_num_per_in (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_alsa_num_per_in as text
%        str2double(get(hObject,'String')) returns contents of edit_alsa_num_per_in as a double


% --- Executes during object creation, after setting all properties.
function edit_alsa_num_per_in_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_alsa_num_per_in (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit_alsa_num_per_out_Callback(hObject, eventdata, handles)
% hObject    handle to edit_alsa_num_per_out (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_alsa_num_per_out as text
%        str2double(get(hObject,'String')) returns contents of edit_alsa_num_per_out as a double


% --- Executes during object creation, after setting all properties.
function edit_alsa_num_per_out_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_alsa_num_per_out (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes when user attempts to close figure1.
function figure1_CloseRequestFcn(hObject, eventdata, handles)
% hObject    handle to figure1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: delete(hObject) closes the figure

% We trigger close of parent data but we need to remove link before to
% avoid infinite recursions.
handles = guidata(hObject);
if(isfield(handles, 'parent'))
    if(~isempty(handles.parent))
        if(ishandle(handles.parent))
            handles_parent = guidata(handles.parent);
            handles_parent.subui.hdl = [];
            guidata(handles.parent, handles_parent);
            close(handles.parent);
        end
    end
end

delete(hObject);



function edit_nrt_Callback(hObject, eventdata, handles)
% hObject    handle to edit_nrt (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_nrt as text
%        str2double(get(hObject,'String')) returns contents of edit_nrt as a double


% --- Executes during object creation, after setting all properties.
function edit_nrt_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_nrt (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in popupmenu_outmode.
function popupmenu_outmode_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_outmode (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_outmode contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_outmode
selection = get(hObject, 'Value');
if(handles.jvx_struct.properties.technology.id_output_file_mode >= 1)
    
    hObject_parent = handles.parent;
    handles_parent = guidata(hObject_parent);
    
    if(handles_parent.jvx_struct.properties.output_files.output.id_user >= 0)
        
        % ==============================================
        % Set new selection processing format
        % ==============================================
        selection = selection -1;
        txt = jvxBitField.jvx_create(selection, false);
        [a b]= handles.hostcall('set_property_uniqueid_o',handles.jvx_struct.technologies.comp_type, ...
            handles.jvx_struct.properties.technology.cat_output_file_mode, ...
            handles.jvx_struct.properties.technology.id_output_file_mode, ...
            handles_parent.jvx_struct.properties.output_files.output.id_user, ...
            txt);
        if(~a)
            error(['Unexpected error: '  b.DESCRIPTION_STRING]);
        end
        
        % ==============================================
        % Entry point for update is the parental window
        % ==============================================
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_refresh_props(hObject_parent, handles_parent);
        [hObject_parent, handles_parent] = jvxJvxHostRt.jvx_update_ui(hObject_parent, handles_parent);
    end
     guidata(hObject_parent, handles_parent);
end





% --- Executes during object creation, after setting all properties.
function popupmenu_outmode_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_outmode (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
