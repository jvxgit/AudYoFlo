function [hdl] = write_update(hdl, in)
    
    szN = size(in,1);
    szM = size(in,2);
    
    avail = hdl.M - hdl.fillheight;
    
    if(szN ~= hdl.N)
        error('Circular buffer does not match dimensions of signal fragment to add');
    end
    
    if(szM <= avail)
        
        write = mod(hdl.read + hdl.fillheight, hdl.M);
        ll1 = min(szM, hdl.M - write);
        ll2 = szM - ll1;
        
        hdl.memory(:, write+1:write+ll1) = in(:,1:ll1);
        hdl.memory(:, 1:ll2) = in(:,ll1+1:ll1+ll2);
                
        hdl.fillheight = hdl.fillheight + szM;
    else
        error('Circular buffer is full');
    end
end
