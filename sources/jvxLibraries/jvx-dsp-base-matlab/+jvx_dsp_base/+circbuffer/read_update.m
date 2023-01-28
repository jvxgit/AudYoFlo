function [hdl, out] = read_update(hdl, num)
    
    if(num <= hdl.fillheight)
        out = zeros(hdl.N, num);             

        ll1 = min(num, hdl.M-hdl.read);
        ll2 = num - ll1;
        
        out(:,1:ll1) = hdl.memory(:, hdl.read+1:hdl.read+ll1);
        out(:,ll1+1:ll1+ll2) = hdl.memory(:, 1:ll2);
        
        hdl.read = mod(hdl.read+num, hdl.M);
        hdl.fillheight = hdl.fillheight - num;
    else
        error('Circular buffer does not provide required number of samples');
    end
end
