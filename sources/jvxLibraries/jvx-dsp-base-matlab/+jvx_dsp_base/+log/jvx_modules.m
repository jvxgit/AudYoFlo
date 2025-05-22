function [entries] = jvx_modules(fName)
    
    cnt = 1;
    entries = {};
    if(nargin < 1)
        fName = '.tlog';
    end
    
    txt = fileread(fName);  
    str = jsondecode(txt);
    cc_str = struct2cell(str);
    
    for(jj = 1: size(cc_str))
         obj = cc_str{jj};    
         if(isfield(obj, 'module'))
             mod = obj.module;
             if(~find_in_entries(entries, mod)) 
                 entries{cnt} = mod;
                 cnt = cnt +1;
             end
         end
    end    
end

 function [isthere] = find_in_entries(entries, mod)
     isthere = false;
     for(jj =1:size(entries,1))
         if(strcmp(entries{jj}, mod) == 1)
             isthere = true;
             break;
         end
     end
 end