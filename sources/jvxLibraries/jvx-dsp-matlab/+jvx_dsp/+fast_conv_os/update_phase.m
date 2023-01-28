function [hdl] = update_phase(hdl)

    hdl.phase_start = mod(hdl.phase_start + hdl.fshift, hdl.fft_size);