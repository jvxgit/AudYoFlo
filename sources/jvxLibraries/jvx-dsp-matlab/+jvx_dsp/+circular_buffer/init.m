function [hdl] = init(bsize, max_delay, num_channels)
    hdl.b_length = bsize + max_delay;
    hdl.idx_read = 0;
    hdl.fill_height = 0;
    hdl.num_channels = num_channels;
    hdl.sig_buffer = zeros(hdl.num_channels, hdl.b_length);