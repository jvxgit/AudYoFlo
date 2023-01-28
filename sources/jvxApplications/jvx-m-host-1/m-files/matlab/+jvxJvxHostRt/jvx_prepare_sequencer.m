function [sequences, steps, active_sequence] = jvx_prepare_sequencer()
    sequences{1} = 'default sequence audio processing';

    steps = {};
    
    %% C CODE
    %{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE, JVX_COMPONENT_AUDIO_NODE, 0, 0, "Prepare Audio Node", "PrepANode", 0, 1000, "no label", "no label"},
	%{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE, JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "Prepare Audio Device", "PrepADevice", 0, 1000, "no label", "no label"},
	%{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_PROCESS_PREPARE, JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "Prepare link connections", "ConnAudio", 0, 1000, "*", "no label"},
	%{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_CALLBACK, JVX_COMPONENT_UNKNOWN, 0, 0, "Callback after all prepared", "AfterAllPrep", 0, 1000, "no label", "no label"},
	%{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_START, JVX_COMPONENT_AUDIO_NODE, 0, 0, "Start Audio Node", "StartANode", 0, 1000, "no label", "no label"},
	%{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_START, JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "Start Audio Device", "StartADevice", 0, 1000, "no label", "no label"},
	%{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_PROCESS_START, JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "Start link connections", "StartAudio", 0, 1000, "*", "no label" },
	%{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_CALLBACK, JVX_COMPONENT_UNKNOWN, 0, 0, "Callback after all started", "AfterAllStarted", 1, 1000, "no label", "no label"},
	%{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER, JVX_COMPONENT_UNKNOWN, 0, 0, "Update Viewer", "UpdateViewer", 0, 1000, "no label", "no label"},
	%{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_BREAK, JVX_COMPONENT_UNKNOWN, 0, 0, "Break sequencer steps", "Break", 0, -1, "no label", "no label"}
    
    step.seq_id = 0;
    step.seq_queue_type = 'JVX_SEQUENCER_QUEUE_TYPE_RUN';
 
    %% Prepare
    
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE';
    step.seq_cp_ident = 'audio node<0,0>';
    step.seq_step_descr = 'Prepare Audio Node';
    step.seq_step_fuc_com_id = -1;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'PrepANode';
    step.seq_step_label_cond1 = 'no label';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];
    
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE';
    step.seq_cp_ident = 'audio device<0,0>';
    step.seq_step_descr = 'Prepare Audio Device';
    step.seq_step_fuc_com_id = -1;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'PrepADevice';
    step.seq_step_label_cond1 = 'no label';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];   
    
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_COMMAND_PROCESS_PREPARE';
    step.seq_cp_ident = 'audio device<0,0>';
    step.seq_step_descr = 'Prepare Audio Process';
    step.seq_step_fuc_com_id = -1;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'PrepAProc';
    step.seq_step_label_cond1 = '*';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];  
    
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_CALLBACK';
    step.seq_cp_ident = 'unknown<0,0>';
    step.seq_step_descr = 'Callback after all prepared';
    step.seq_step_fuc_com_id = 0;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'AfterAllPrep';
    step.seq_step_label_cond1 = 'no label';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];   

    %% Start
    
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_START';
    step.seq_cp_ident = 'audio node<0,0>';
    step.seq_step_descr = 'Start Audio Node';
    step.seq_step_fuc_com_id = -1;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'StartANode';
    step.seq_step_label_cond1 = 'no label';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];
         
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_START';
    step.seq_cp_ident = 'audio device<0,0>';
    step.seq_step_descr = 'Start Audio Device';
    step.seq_step_fuc_com_id = -1;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'StartADevice';
    step.seq_step_label_cond1 = 'no label';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];
         
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_COMMAND_PROCESS_START';
    step.seq_cp_ident = 'audio device<0,0>';
    step.seq_step_descr = 'Start Audio Process';
    step.seq_step_fuc_com_id = -1;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'StartAProc';
    step.seq_step_label_cond1 = '*';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];
         
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_CALLBACK';
    step.seq_cp_ident = 'unknown<0,0>';
    step.seq_step_descr = 'Callback after all started';
    step.seq_step_fuc_com_id = 1;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'AfterAllStart';
    step.seq_step_label_cond1 = 'no label';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];   
      
    %% Misc
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_REQUEST_UPDATE_VIEWER';
    step.seq_cp_ident = 'unknown<0,0>';
    step.seq_step_descr = 'Update Viewer';
    step.seq_step_fuc_com_id = 1;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'UpdateViewer';
    step.seq_step_label_cond1 = 'no label';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];   
         
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_BREAK';
    step.seq_cp_ident = 'unknown<0,0>';
    step.seq_step_descr = 'Break sequencer steps';
    step.seq_step_fuc_com_id = 1;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'Break';
    step.seq_step_label_cond1 = 'no label';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];   
             
    % -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
    % -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
    % -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
    
    %{JVX_SEQUENCER_QUEUE_TYPE_LEAVE, , JVX_COMPONENT_UNKNOWN, 0, 0, "", "", 16, 1000, "no label", "no label"},
	%{JVX_SEQUENCER_QUEUE_TYPE_RUN, , JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "Stop link connections", "StopAudio", 0, 1000, "*", "no label" },
	%{JVX_SEQUENCER_QUEUE_TYPE_LEAVE, , JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "", "StopADevice", 0, 1000, "no label", "no label"},
	%{JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_STOP, JVX_COMPONENT_AUDIO_NODE, 0, 0, "Stop Audio Node", "StopANode", 0, 1000, "no label", "no label"},
	%{JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_CALLBACK, JVX_COMPONENT_UNKNOWN, 0, 0, "Callback before all postprocess", "BeforeAllPost", 17, 1000, "no label", "no label"},
	%{JVX_SEQUENCER_QUEUE_TYPE_RUN, JVX_SEQUENCER_TYPE_COMMAND_PROCESS_POSTPROCESS, JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "Postprocess link connections", "PostProcAudio", 0, 1000, "*", "no label" },
	%{JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_POSTPROCESS, JVX_COMPONENT_AUDIO_DEVICE, 0, 0, "Postprocess Audio Device", "PostADevice", 0, 1000, "no label", "no label"},
	%{JVX_SEQUENCER_QUEUE_TYPE_LEAVE, JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_POSTPROCESS, JVX_COMPONENT_AUDIO_NODE, 0, 0, "Postprocess Audio Node", "PostANode", 0, 1000, "no label", "no label"}

    step.seq_id = 0;
    step.seq_queue_type = 'JVX_SEQUENCER_QUEUE_TYPE_LEAVE';
 
    %% Stop
    
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_CALLBACK';
    step.seq_cp_ident = 'unknown<0,0>';
    step.seq_step_descr = 'Callback before all stop';
    step.seq_step_fuc_com_id = 16;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'BeforeAllStop';
    step.seq_step_label_cond1 = 'no label';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];
    
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_COMMAND_PROCESS_STOP';
    step.seq_cp_ident = 'audio device<0,0>';
    step.seq_step_descr = 'Stop Audio Process';
    step.seq_step_fuc_com_id = 16;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'StopAudioProc';
    step.seq_step_label_cond1 = '*';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];
    
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_STOP';
    step.seq_cp_ident = 'audio device<0,0>';
    step.seq_step_descr = 'Stop Audio Device';
    step.seq_step_fuc_com_id = -1;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'StopADevice';
    step.seq_step_label_cond1 = 'no label';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];
    
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_STOP';
    step.seq_cp_ident = 'audio node<0,0>';
    step.seq_step_descr = 'Stop Audio Node';
    step.seq_step_fuc_com_id = -1;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'StopANode';
    step.seq_step_label_cond1 = 'no label';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];
    
    % POSTPROCESS
        
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_CALLBACK';
    step.seq_cp_ident = 'unknown<0,0>';
    step.seq_step_descr = 'Callback before all stop';
    step.seq_step_fuc_com_id = 17;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'BeforeAllPost';
    step.seq_step_label_cond1 = 'no label';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];
    
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_COMMAND_PROCESS_POSTPROCESS';
    step.seq_cp_ident = 'audio device<0,0>';
    step.seq_step_descr = 'Postprocess Audio Process';
    step.seq_step_fuc_com_id = -1;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'PostProcAudioProc';
    step.seq_step_label_cond1 = '*';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];
    
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_POSTPROCESS';
    step.seq_cp_ident = 'audio device<0,0>';
    step.seq_step_descr = 'Postproc Audio Device';
    step.seq_step_fuc_com_id = -1;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'PostProcADevice';
    step.seq_step_label_cond1 = 'no label';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];
    
    step.seq_elm_type = 'JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_POSTPROCESS';
    step.seq_cp_ident = 'audio node<0,0>';
    step.seq_step_descr = 'Post process Audio Node';
    step.seq_step_fuc_com_id = -1;
    step.seq_step_timeount = -1;
    step.seq_step_label = 'PostProcANode';
    step.seq_step_label_cond1 = 'no label';
    step.seq_step_label_cond2 = 'no label';
    step.seq_step_insert = -1;
    steps = [steps step];
         
    active_sequence = 0;
 
