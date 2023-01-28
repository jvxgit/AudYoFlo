function [jvx_handle, jvx_out_frame] =jvxProcessOffline_local(jvx_handle, jvx_in_frame)
    
% Obtain processing handle
global inProcessing;
global jvx_host_call_global;
global dbg;

if(bitand(jvx_handle.operation_mode, 2))
end

if(bitand(jvx_handle.operation_mode, 1))
end

if(jvx_handle.operation_mode == 3)
end

%sig_x = jvx_in_frame(1,:);
%sig_y = jvx_in_frame(2,:);

sig_x = jvx_in_frame(2,:);
sig_y = jvx_in_frame(2,:);

 
% Change format
jvx_out_frame  = [sig_x; sig_y];

