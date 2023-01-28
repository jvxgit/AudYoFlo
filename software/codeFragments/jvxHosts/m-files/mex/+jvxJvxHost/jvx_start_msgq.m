function [] = jvx_start_msgq(userdata, callback_resp, period_ms, really_start)

global jvx_global_control;
jvx_global_control.tasks.msgq.hdl = timer('TimerFcn',callback_resp, 'Period', period_ms, 'ExecutionMode', 'fixedSpacing' );
jvx_global_control.tasks.msgq.context.userdata = userdata;
jvx_global_control.tasks.msgq.update_mode = 0; % Update mode 0: processing inactive 1: processing active


% This will instantaneously run it for the first time
if(really_start)
    jvx_global_control.tasks.msgq.started = true;
    start(jvx_global_control.tasks.msgq.hdl);
    disp('Global message queue started.');
else
    jvx_global_control.tasks.msgq.started = false;
    disp('Global message queue not started.');
end
    

