% Reads the most recently written values without updating the read index.
% num: Number of values to read.

function [out] = read_recently_written(hdl, num)
    
    if num>hdl.M
        error('Buffer length too low');
    end
    
    out = zeros(hdl.N, num);             
    read_start = mod(hdl.fillheight-num, hdl.M);
    
    ll1 = min(num, hdl.M-read_start);
    ll2 = num - ll1;
    
    out(:,1:ll1) = hdl.memory(:, read_start+1:read_start+ll1);
    out(:,ll1+1:ll1+ll2) = hdl.memory(:, 1:ll2);
end
