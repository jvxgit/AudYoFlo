function [jvx_out_frame] =jvxProcessOffline(jvx_in_frame, jvx_xchg_cnt)
 
% Obtain processing handle
global inProcessing;
jvx_handle = inProcessing.jvx_handle;

jvx_out_frame = zeros(jvx_handle.setup.out.num, jvx_handle.setup.out.bsize);

% If single precision, convert output
if(isa(jvx_in_frame, 'single'))
    jvx_out_frame = single(jvx_out_frame);
end

for(ind = 1:jvx_handle.setup.out.num)
    indi = mod(ind-1, jvx_handle.setup.in.num);
    indi = indi + 1;
    jvx_out_frame(ind,:) = jvx_in_frame(indi,:) * inProcessing.processing.volume;
end

% Restore private data
inProcessing.jvx_handle = jvx_handle;

