function [out] = jvx_to_common_path_format(in)
    out = in;
    for(ind = 1:size(out,2))
        if(out(ind) == '\')
            out(ind) = '/';
        end
    end