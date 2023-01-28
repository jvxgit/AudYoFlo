function [hdl] = write(hdl, fld)
    fwrite(hdl.fH, fld, 'double');