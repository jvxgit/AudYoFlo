function [coeffsNum, coeffsDen] = jvx_design_iir(oversampling, rippleDB, attenuationDB)

    edgeFreq = 1/oversampling;
    deltaFreq = 1/10 * edgeFreq;
    edgeFreq_low = edgeFreq - deltaFreq/2;
    edgeFreq_high = edgeFreq + deltaFreq/2;
    [Ne, Wne] = ellipord(edgeFreq_low, edgeFreq_high, rippleDB, attenuationDB);
    [coeffsNum,coeffsDen] = ellip(Ne, rippleDB, attenuationDB, Wne);
    
    % Generate coefficients for use in C/C++
    % TODO