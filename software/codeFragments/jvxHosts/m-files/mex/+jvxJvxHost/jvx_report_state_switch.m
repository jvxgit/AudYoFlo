function [] = jvx_report_state_switch(stateSwitchTxt, componentIdentificationTxt, prePostHookTxt)
    
    global jvx_host_callback_stateswitch;
    if(~isempty(jvx_host_callback_stateswitch))
        jvx_host_callback_stateswitch(stateSwitchTxt, componentIdentificationTxt, prePostHookTxt);
    end
    
