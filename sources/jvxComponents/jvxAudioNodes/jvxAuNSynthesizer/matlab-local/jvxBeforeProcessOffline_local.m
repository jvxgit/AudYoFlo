function [jvx_handle] = jvxBeforeProcessOffline(jvx_handle, isOffline)
     
    global inProcessing;
    
	inProcessing.algorithm.dbg.m1_delayed = zeros(1, inProcessing.sig_length);
    inProcessing.algorithm.dbg.m2_delayed = zeros(1, inProcessing.sig_length);
    inProcessing.algorithm.dbg.m1_x00filtered = zeros(1, inProcessing.sig_length);
    inProcessing.algorithm.dbg.m2_x00filtered = zeros(1, inProcessing.sig_length);
    inProcessing.algorithm.dbg.m1_x0cfiltered = zeros(1, inProcessing.sig_length);
    inProcessing.algorithm.dbg.signal_fwd = zeros(1, inProcessing.sig_length);
    inProcessing.algorithm.dbg.signal_bwd = zeros(1, inProcessing.sig_length);
    inProcessing.algorithm.dbg.signal_gsc = zeros(1, inProcessing.sig_length);
    inProcessing.algorithm.dbg.signal_out = zeros(1, inProcessing.sig_length);
    inProcessing.algorithm.dbg.signal_constr = zeros(1, inProcessing.sig_length);
