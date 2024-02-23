function [jvx_handle, jvx_out_frame_m] =jvxProcessOffline_local(jvx_handle, jvx_in_frame)
    
% Access to the host
global jvx_host_call_global; 

% Realize talkthrough
% Version I: C function version
[a, jvx_out_frame_c] = jvx_host_call_global('external_call', 'CayfAuNStarter', 'ayf_starter_lib_process', jvx_in_frame);
       
% Version II: Matlab version
jvx_out_frame_m = jvx_in_frame * jvx_handle.volume;

% Obtain profiling handle
global hdl_profile_data;
if(~isempty(hdl_profile_data) && isfield(hdl_profile_data, 'tp'))
    if(jvxBitField.jvx_test(hdl_profile_data.tp.tokenTp, 0))
        hdl_profile_data.tp.starter_tp0_m = jvx_out_frame_m(1,:);
    end
end

