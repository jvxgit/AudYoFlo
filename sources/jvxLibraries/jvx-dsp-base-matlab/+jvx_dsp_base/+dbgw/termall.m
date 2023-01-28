function [] = termall(hdl)
    ccs = struct2cell(hdl);
    ccsn = fieldnames(hdl);
    for(ind = 1:size(ccs,1))
        ccsl = ccs{ind};
        if(isfield(ccsl, 'active'))
            if(ccsl.active)
                if(isfield(ccsl, 'fH'))
                    
                    disp(['--> Closing log file for token <' ccsn{ind} '>']);
                    fclose(ccsl.fH);
                end
            end
        end
    end