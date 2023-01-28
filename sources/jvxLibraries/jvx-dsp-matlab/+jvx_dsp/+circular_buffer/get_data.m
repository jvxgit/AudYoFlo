function [hdl, out_frame, res] = get_data(hdl, num_samples_out)
res = 0; % <- success

if(hdl.fill_height >= num_samples_out)
    out_frame = zeros(hdl.num_channels, num_samples_out);
    
    ll1 = min(hdl.b_length - hdl.idx_read, num_samples_out);
    ll2 = num_samples_out - ll1;
    ll2 = max(ll2, 0);
    
    out_frame(:, 1:ll1) = hdl.sig_buffer(:, hdl.idx_read + 1: hdl.idx_read + ll1);
    out_frame(:, ll1 + 1:ll1 + ll2) = hdl.sig_buffer(:, 1:ll2);
    hdl.fill_height = hdl.fill_height - num_samples_out;
    hdl.idx_read = mod(hdl.idx_read + num_samples_out, hdl.b_length);
else
    
    % Indicate buffer underflow
    res = 1;
end
