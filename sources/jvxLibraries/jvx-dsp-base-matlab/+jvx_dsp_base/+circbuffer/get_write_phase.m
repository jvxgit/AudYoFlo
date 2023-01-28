function [ret] = get_write_phase(hdl)
    ret = mod(hdl.read + hdl.fillheight, hdl.M);
    