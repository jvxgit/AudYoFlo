function [jvx_handle, jvx_out_frame] =jvxProcessOffline_local(jvx_handle, jvx_in_frame)
    
% Obtain processing handle (if desired)
global inProcessing;

jvx_out_frame = zeros(jvx_handle.setup.out.num, jvx_handle.setup.out.bsize);

% If single precision, convert output
if(isa(jvx_in_frame, 'single'))
    jvx_out_frame = single(jvx_out_frame);
end

for(ind = 1:jvx_handle.setup.out.num)
    indi = mod(ind-1, jvx_handle.setup.in.num);
    indi = indi + 1;
    jvx_out_frame(ind,:) = jvx_in_frame(indi,:) * jvx_handle.algorithm.volume;
end

