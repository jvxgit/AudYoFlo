function [jvx_out_frame] =jvxProcessOffline(jvx_in_frame, jvx_xchg_cnt)
    
global inProcessing;
jvx_handle = inProcessing.jvx_handle;

jvx_out_frame = [];
for(ind = 1:jvx_handle.setup.num_out)
    indi = mod(ind-1, jvx_handle.setup.num_in);
    indi = indi + 1;
    jvx_out_frame(ind,:) = jvx_in_frame(indi,:) * inProcessing.processing.volume;
end

inProcessing.jvx_handle = jvx_handle;