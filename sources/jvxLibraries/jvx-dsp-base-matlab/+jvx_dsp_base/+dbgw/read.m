function [out] = read_dbg(fName, M, prec)
    if(nargin <3)
        prec = 'double';
    end
    
    if(nargin <2)
        M = 1;
    end
    
    fid = fopen(fName, 'r');
    out = fread(fid, inf, prec);
    NM = length(out);
    N = floor(NM / M);
    out = out(1:(N*M));
    out = reshape(out, [M N]);
    fclose(fid);
