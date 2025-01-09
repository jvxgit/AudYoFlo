% Function to replace a separator character by another
function[out] = replaceSeparatorPath(path, from, to)
    out = '';
    cc = strsplit(path, from);
    lcc = length(cc);
    if(lcc > 0)
        out = cc{1};
    end
    
    for(idx = 2:length(cc))
        out = [out to cc{idx}];
    end    