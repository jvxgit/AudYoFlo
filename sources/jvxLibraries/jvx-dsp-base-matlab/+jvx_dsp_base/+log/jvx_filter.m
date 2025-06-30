function [str] = jvx_filter(fName, option, specified)
    
    if(nargin < 1)
        fName = '.tlog';
    end
    
    if(nargin < 2)
        option = 'module';
    end
    
    if(nargin < 3)
        specified = '';
    end
    
    txt = fileread(fName);
    
    str = jsondecode(txt);
    
    % Options: 
    % 'module -> entries for component withname
    % 'field' -> Entries for field name
    if(~isempty(option))
        strFull = str;        
        
        cc_str = struct2cell(strFull);
        fn_str = fieldnames(strFull);
        
        cnt = 0;
        for(jj = 1: size(cc_str))
            obj = cc_str{jj};           
            if(check_filter_match(obj, option, specified))
                cnt = cnt + 1;% str
            end
        end
        
        cc_filtered = cell(cnt,1);
        fn_filtered = cell(cnt,1);
        
        cnt = 1;
        for(jj = 1:size(cc_str))
            obj = cc_str{jj};
            if(check_filter_match(obj, option, specified))
                cc_filtered{cnt} = obj;
                fn_filtered{cnt} = fn_str{jj};
                cnt = cnt + 1;% str
            end
        end
        
        str = cell2struct(cc_filtered, fn_filtered, 1);
    end
end

 function [matches] = check_filter_match(obj, entry, specified)
     matches = false;
     if(isfield(obj, entry))
         cmd = ['value = obj.' entry ';'];
         eval(cmd);
         % if(strcmp(value, specified) == 1)
         sim = regexp(value, specified, 'ONCE');
         if(~isempty(sim))            
              matches = true;
         end
     end
 end