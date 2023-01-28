function [] = compare(token)
    fNameC = [token '.cdat'];
    fNameM = [token '.mdat'];
    
    sigC = jvx_dsp_base.dbgw.read(fNameC);
    sigM = jvx_dsp_base.dbgw.read(fNameM);
    
    figure; plot(sigC-sigM);