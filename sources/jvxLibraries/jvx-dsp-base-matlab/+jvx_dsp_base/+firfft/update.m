function [hdl] = update(hdl, whatToUpdate, do_set)

switch (whatToUpdate)
    case 2
        if (do_set)
            tt = hdl.init.bsize + size(hdl.init.fir,2) - 1;
            tt = ceil(log2(tt));
            tt = 2^tt;
            hdl.derived.szFft = tt;
            hdl.derived.delay = 0;
            if(hdl.init.type == 1)
                hdl.derived.delay = floor(size(hdl.init.fir,2)/2);
            end
            
            if(~isempty(hdl.prv))
                hdl.prv.init_cpy = hdl.init;
                hdl.prv.derived_cpy = hdl.derived;
            end
        else
            if(~isempty(hdl.prv))
                hdl.init = hdl.prv.init_cpy;
                hdl.derived = hdl.prv.derived_cpy;
            end
        end
        
    otherwise
        error('Case is unsupported.');
end