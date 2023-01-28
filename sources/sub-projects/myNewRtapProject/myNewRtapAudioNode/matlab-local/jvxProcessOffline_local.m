function [jvx_handle, jvx_out_frame] =jvxProcessOffline_local(jvx_handle, jvx_in_frame)
    
% Obtain processing handle (if desired)
global inProcessing;

jvx_out_frame = jvx_handle.myNewProject.process(jvx_in_frame, jvx_handle.setup.in.num, jvx_handle.setup.out.num, jvx_handle.setup.out.bsize); 
