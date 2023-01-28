function [hdl] = init(fft_size, l_ir, window_type)
    hdl.fft_size = fft_size;

    if(mod(l_ir,2) == 0)
        l_ir = l_ir + 1;
    end
    hdl.l_ir = l_ir;

    hdl.win_pos = zeros(1, ceil(l_ir/2));
    hdl.win_neg = zeros(1, floor(l_ir/2));

    if(strcmp(window_type, 'rect'))
        hdl.win_pos = ones(size(hdl.win_pos));
        hdl.win_neg = ones(size(hdl.win_neg));
    else
        h = hamming(l_ir)';
        hdl.win_pos = h(ceil(l_ir/2):end);
        hdl.win_neg = h(1:ceil(l_ir/2)-1);
    end