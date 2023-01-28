function [hdl, res] = put_data(hdl, in_frame)
res = 0; % <- success
space = hdl.b_length - hdl.fill_height;
if(size(in_frame,1) == hdl.num_channels)
    num_samples_in = size(in_frame,2);
    if(space >= num_samples_in)
        idx_write = mod(hdl.idx_read + hdl.fill_height, hdl.b_length);
        ll1 = min(hdl.b_length - idx_write, num_samples_in);
        ll2 = num_samples_in - ll1;
        ll2 = max(ll2, 0);
        
        hdl.sig_buffer(:, idx_write + 1: idx_write + ll1) = in_frame(:, 1:ll1);
        hdl.sig_buffer(:, 1:ll2) = in_frame(:, ll1 + 1:ll1 + ll2);
        
        hdl.fill_height = hdl.fill_height + num_samples_in;
        
    else
        
        % Indicate buffer overflow
        res = 2;
    end
else
    res = 3;
end
