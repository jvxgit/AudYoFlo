function [jvx_handle, jvx_out_frame_m] =jvxProcessOffline_local(jvx_handle, jvx_in_frame)
    
% Access to the host
global jvx_host_call_global; 

% Realize talkthrough
% Version I: C function version
[a, jvx_out_frame_c] = jvx_host_call_global('external_call', 'CayfAuNStarter', 'ayf_starter_lib_process', jvx_in_frame);

% Version II: Matlab version
jvx_out_frame_m = jvx_in_frame * jvx_handle.volume;

% Optional: compare both outputs!
