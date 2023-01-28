% Writes values to the circbuffer.
% write_start: Index of the first value
% 
% Index counting starts with 0.

function [hdl] = write_at_index_update(hdl, in, write_start)
    
    szN = size(in,1);
    szM = size(in,2);
       
    if(szN ~= hdl.N)
        error('Circular buffer does not match dimensions of signal fragment to add');
    end
    
    write = mod(write_start, hdl.M);
    ll1 = min(szM, hdl.M - write);
    ll2 = szM - ll1;
    
    hdl.memory(:, write+1:write+ll1) = in(:,1:ll1);
    hdl.memory(:, 1:ll2) = in(:,ll1+1:ll1+ll2);
    
    hdl.fillheight = mod(hdl.fillheight + szM, hdl.M);
end