function [handle] = jvx_cic_dec_init(R, M, N, runfull)
    
    if(nargin < 4)
        runfull = false;
    end
    handle.mem_i = zeros(1, N);
    handle.mem_c = zeros(1, M*N);
    handle.R = R;
    handle.M = M;
    handle.N = N;
    handle.NM = N*M;
    handle.RM = R*M;
    handle.RNPM = (R*M)^N;
    handle.runfull = runfull;
    
    if(runfull)
        handle.mem_c = zeros(1, M*N*R);
        handle.NMR = N*M*R;
    else
        handle.mem_c = zeros(1, M*N);
    end
    

    % In-circ buffer counter
    handle.cp = 0;