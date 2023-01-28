function [hdl] = init(fft_size, fshift, read_shift, read_ovlp)
       
    hdl.sig_buffer = zeros(1, fft_size);
    hdl.phase_start = 0;
    hdl.read_shift = read_shift;
    hdl.read_ovlp = read_ovlp;
    
    hdl.fft_size = fft_size;
    hdl.fshift = fshift;
    hdl.ovlp_save = zeros(1, hdl.read_ovlp);
    hdl.win_ovlp = [0:hdl.read_ovlp-1]/hdl.read_ovlp;
