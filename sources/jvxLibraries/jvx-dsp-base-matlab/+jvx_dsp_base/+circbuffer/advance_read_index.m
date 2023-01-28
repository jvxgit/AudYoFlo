% Advance read index as if num elements had been read, but 
% without actually reading them.

function [hdl] = advance_read_index(hdl, num)
    
    if(num <= hdl.fillheight)
        
        hdl.read = mod(hdl.read+num, hdl.M);
        hdl.fillheight = hdl.fillheight - num;
    else
        error('Circular buffer does not provide required number of samples');
    end
end
