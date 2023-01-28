function [jvx_handle, jvx_out_frame] =jvxProcessOffline_local(jvx_handle, jvx_in_frame)
    
% Obtain processing handle
global inProcessing;

% Separate input signals
signal = jvx_in_frame(1,:);

jvx_out_frame = [signal; signal];

