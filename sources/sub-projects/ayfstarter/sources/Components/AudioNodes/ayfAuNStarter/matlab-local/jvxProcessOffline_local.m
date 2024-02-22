function [jvx_handle, jvx_out_frame] =jvxProcessOffline_local(jvx_handle, jvx_in_frame)
    
% Obtain processing handle
global inProcessing; % <- access to the current processing data struct, frame counter etc.
global jvx_host_call_global; % <- access to the underlying AudYoFlo host

% Realize talkthrough
jvx_out_frame = jvx_in_frame;
