function [magn_dB, freq] = derive_magnitude_from_ir(ir, fs, lfft)
    
    [magn_dB, freq] = freqz(ir, 1, lfft, fs);
    magn_dB = 10 * log10(magn_dB.*conj(magn_dB));

