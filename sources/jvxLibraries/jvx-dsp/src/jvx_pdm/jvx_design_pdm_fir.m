function [] = jvx_design_pdm_fir(os, ppfilterlength, FIRDESIGN_FAC, outfile)

    generateCHeader = true;
    isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;
    % Design filter IR
    delta_filter = 0.4*1/2/os;

    % Compue effective filter length
    efffilterlength = os*ppfilterlength;

    delta_filter = delta_filter * FIRDESIGN_FAC;
    f = [0 1/os - delta_filter/2 1/os 1];
    a = [1  1  0 0];
    dsigntoken = ['_d2_ll' num2str(efffilterlength) '_fac' num2str(FIRDESIGN_FAC) '_os' num2str(os)];
    if(isOctave)
        ir = [firls(efffilterlength-2,f,a)' 0];
    else
        ir = firls(efffilterlength-1,f,a);
    end
    [hdl] = jvx_dsp.pdm.jvx_pdm_dec_cpu_init_r16(efffilterlength, true, ir, generateCHeader, dsigntoken, outfile);
