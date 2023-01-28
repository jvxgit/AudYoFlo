% Read a memory dump and convert content back to struct
function [strCell, memdump] = jvx_read_mem_dump(fname, outformat)
    fid = fopen(fname);
    memdump = [];
    strCell = [];
    while(1)
        oneline = fgets(fid);
        
        if(~ischar(oneline))
            break;
        else
            
            oneline = filter_string(oneline);
            memdump = [memdump oneline];
        end
    end
    
    [strCell] = jvxMemDump(memdump, outformat);
    
% ===========================================================================

function [out] = filter_string(in)
    out = [];
    for(ind=1:length(in))
        if(...
                (double(in(ind)) == double(' ')) || ...
                (double(in(ind)) == 10))
                
            % ok
        elseif(double(in(ind) >= double('0'))&& (double(in(ind)) <= double('9')))
            out = [out in(ind)];
        elseif(double(in(ind)) >= double('A')&& (double(in(ind)) <= double('F')))
            out = [out in(ind)];
        elseif(double(in(ind) >= double('a'))&& (double(in(ind)) <= double('f')))
            out = [out in(ind)];
        else
            error(['Invalid memory dump character ' in(ind)]);
        end
    end