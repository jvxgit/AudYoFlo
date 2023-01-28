function [] = jvx_print_struct(str, prefix)
    
    if(nargin == 1)
        prefix = [];
    end
    
    cc = struct2cell(str);
    nn = fieldnames(str);
    for(ind=1:size(cc))
        
        if(isempty(prefix))
            structName = nn{ind};
        else
            structName = [prefix '.' nn{ind}];
        end
        
        if(isstruct(cc{ind}))
            jvxMisc.jvx_print_struct(cc{ind}, structName);
        elseif(iscell(cc{ind}))
                disp([structName '=' cell2str(cc{ind}) ';']);
        elseif(isnumeric(cc{ind}))
                disp([structName '=' num2str(cc{ind}) ';']);
        elseif (ischar(cc{ind}))
            disp([structName '=''' cc{ind} ''';']);
        else
            error('Unexpected.');
        end
    end
end             
     
function [out] = cell2str(cc)
    out = '{';
    for(ind = 1:size(cc,1))
        if(isnumeric(cc{ind}))
            out = [out ' ' num2str(cc{ind})];
        elseif(ischar(cc{ind}))
            out = [out ' ''' cc{ind} ''''];
        elseif(iscell(cc{ind}))
            out = [out cellstr(cc{ind})];
        end
    end
    out = [out '}'];
end
        