function     [hObject, handles] = jvx_core_processing(hObject, handles)

global inProcessing;

% Obtain references to all relevant properties
[hObject, handles] = jvxJvxHostOff.jvx_refresh_props(hObject, handles);

% Show current status
[hObject, handles] = jvxJvxHostOff.jvx_update_ui(hObject, handles);

inProcessing.progress = 0;
inProcessing.progress_inc = 1/8;
inProcessing.progress_next = 0;
inProcessing.in_rate = handles.jvx_struct.data.input.rate;
inProcessing.in_channels = size(handles.jvx_struct.data.input.data,1);
inProcessing.in_length = size(handles.jvx_struct.data.input.data,2);

[a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_bsize, handles.jvx_struct.properties.device.id_bsize);
if(~a)
    error('Unexpected');
end
inProcessing.bsize = double(b.SUBFIELD);

[a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_bsize, handles.jvx_struct.properties.device.id_srate);
if(~a)
    error('Unexpected');
end
inProcessing.srate = double(b.SUBFIELD);

inProcessing.in_frames = ceil(inProcessing.in_length/inProcessing.bsize);
inProcessing.sig_length = (inProcessing.bsize*inProcessing.in_frames);

[a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type,handles.jvx_struct.properties.device.cat_formats, handles.jvx_struct.properties.device.id_formats);
if(~a)
    error('Unexpected');
end
selection = jvxJvxHost.jvx_find_mat_id_single_selection(b.SUBFIELD.SELECTION_BITFIELD);
inProcessing.format_id = jvxJvxHost.jvx_format_option_2_idx(b.SUBFIELD.OPTIONS(selection), handles.jvx_struct.configuredForDouble);
if(inProcessing.format_id  == -1)
    error('Unexpected');
end

% =================================
% Manage input channels
% =================================
[a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_in_channels, handles.jvx_struct.properties.device.id_in_channels);
if(~a)
    error('Unexpected');
end
txtField = b.SUBFIELD.OPTIONS;
selection =b.SUBFIELD.SELECTION_BITFIELD;
if(size(txtField,1) ~= inProcessing.in_channels)
    error('Unexpected');
end

inProcessing.in_data =  jvxJvxHost.jvx_data_format_field_set([],  inProcessing.format_id, true, handles.jvx_struct.configuredForDouble);
if(inProcessing.srate ~= inProcessing.in_rate)
    disp(['Warning: Samperate mismatch in processing peperty and input, ' num2str(inProcessing.srate) ' vs. ' num2str(inProcessing.in_rate)]);
end

cnt = 1;
for(ind=1:inProcessing.in_channels)
    if(jvxBitField.jvx_test(selection, ind-1))
        inProcessing.in_data(cnt,:) = jvxJvxHost.jvx_data_format_field_set(zeros(1,inProcessing.sig_length), inProcessing.format_id, true, handles.jvx_struct.configuredForDouble);
        inProcessing.in_data(cnt,1:inProcessing.in_length) = jvxJvxHost.jvx_data_format_field_set(handles.jvx_struct.data.input.data(ind,:), inProcessing.format_id, true, handles.jvx_struct.configuredForDouble);
        cnt = cnt + 1;
    end
end
inProcessing.in_channels_use = size(inProcessing.in_data,1);

% =================================
% Manage output channels
% =================================
[a b]= handles.hostcall('get_property_uniqueid', handles.jvx_struct.devices.comp_type, handles.jvx_struct.properties.device.cat_out_channels, handles.jvx_struct.properties.device.id_out_channels);
if(~a)
    error('Unexpected');
end
txtField = b.SUBFIELD.OPTIONS;
selection =b.SUBFIELD.SELECTION_BITFIELD;
inProcessing.out_channels = size(txtField,1);
inProcessing.out_channels = min(inProcessing.out_channels, jvxBitField.jvx_num_selections(selection));

mask = 1;
cnt = 1;
for(ind=1:inProcessing.out_channels)
    if(jvxBitField.jvx_test(selection, ind-1))
        cnt = cnt + 1;
    end
    mask = bitshift(mask, 1);
end
inProcessing.out_channels_use = cnt - 1;
inProcessing.out_data =  jvxJvxHost.jvx_data_format_field_set(zeros(inProcessing.out_channels_use, inProcessing.sig_length), inProcessing.format_id, true, handles.jvx_struct.configuredForDouble);

inProcessing.frameCnt = 1;
inProcessing.idxStart = 1;
inProcessing.isRunning = 1;
inProcessing.out_complete = false;

set(handles.text_progress_0, 'BackgroundColor', [0 0.1 0]);
set(handles.text_progress_1, 'BackgroundColor', [0 0.1 0]);
set(handles.text_progress_2, 'BackgroundColor', [0 0.1 0]);
set(handles.text_progress_3, 'BackgroundColor', [0 0.1 0]);
set(handles.text_progress_4, 'BackgroundColor', [0 0.1 0]);
set(handles.text_progress_5, 'BackgroundColor', [0 0.1 0]);
set(handles.text_progress_6, 'BackgroundColor', [0 0.1 0]);
set(handles.text_progress_7, 'BackgroundColor', [0 0.1 0]);
set(handles.text_percent_progress, 'String', [num2str(inProcessing.progress) ' %']);
 
while(inProcessing.isRunning)
    if(inProcessing.triggerStop)
        inProcessing.isRunning = false;
        break;
    end
    if(inProcessing.frameCnt <=  inProcessing.in_frames)
        oneFrameIn = inProcessing.in_data(:,(inProcessing.idxStart:inProcessing.idxStart + inProcessing.bsize-1));
        
        % Run the minLevel function
        oneFrameIn = jvxMinLevel(oneFrameIn);
        
        % Process One Input frame in C subsystem
        [res, oneFrameOut] = handles.hostcall('external_call',  'CjvxOfflineAudioDevice', 'processOneBatch', oneFrameIn);
        if(res)
            
            inProcessing.out_data(:,(inProcessing.idxStart:inProcessing.idxStart + inProcessing.bsize-1)) = oneFrameOut;
            inProcessing.idxStart = inProcessing.idxStart + inProcessing.bsize;
            inProcessing.frameCnt = inProcessing.frameCnt + 1;
            inProcessing.progress = inProcessing.frameCnt /  inProcessing.in_frames;
            if(inProcessing.progress > inProcessing.progress_next)
                inProcessing.progress_next = inProcessing.progress_next + inProcessing.progress_inc;
                cnt = ceil(inProcessing.progress/inProcessing.progress_inc);
                if(cnt >= 1)
                    set(handles.text_progress_0, 'BackgroundColor', [0 1 0]);
                end
                if(cnt >= 2)
                    set(handles.text_progress_1, 'BackgroundColor', [0 1 0]);
                end
                if(cnt >= 3)
                    set(handles.text_progress_2, 'BackgroundColor', [0 1 0]);
                end
                if(cnt >= 4)
                    set(handles.text_progress_3, 'BackgroundColor', [0 1 0]);
                end
                if(cnt >= 5)
                    set(handles.text_progress_4, 'BackgroundColor', [0 1 0]);
                end
                if(cnt >= 6)
                    set(handles.text_progress_5, 'BackgroundColor', [0 1 0]);
                end
                if(cnt >= 7)
                    set(handles.text_progress_6, 'BackgroundColor', [0 1 0]);
                end
                if(cnt >= 8)
                    set(handles.text_progress_7, 'BackgroundColor', [0 1 0]);
                end
            end
            set(handles.text_percent_progress, 'String', [num2str(inProcessing.progress*100,  '%.1f') ' %']);
        else
            disp(['External call failed, reason: ' oneFrameOut.DESCRIPTION_STRING]);
            inProcessing.isRunning = false;
        end
    else
        inProcessing.out_complete = true;
        inProcessing.isRunning = false;
    end
    drawnow;
end

