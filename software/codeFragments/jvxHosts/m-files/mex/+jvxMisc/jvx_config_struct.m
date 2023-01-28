function [str] = jvx_config_struct(rawStruct)
    
    if(nargin == 1)
        printAssoc = false;
    end
    
    str = [];
    if(~isempty(rawStruct))

        cc = struct2cell(rawStruct);
        for(ind=1:size(cc,1))
            
            %[res,transl] = jvx_host_call_global('lookup_type_name__value', 'jvxConfigSectionTypes', cc{ind}.TYPE);
            
            switch(cc{ind}.TYPE)
                case 1
                    ct = jvxMisc.jvx_config_struct(cc{ind}.CONTENT);
                case 2
                    % Assignment string
                    ct = cc{ind}.CONTENT;
                case 3
                    % Assignment value
                    ct = cc{ind}.CONTENT;
                case 4
                    % Assignment value list
                    ct = cc{ind}.CONTENT;
                    if(iscell(ct))
                        if(prod(size(ct)) == 1)
                            ct = ct{:};
                        end
                    end
                case 5
                    % Assignment string list
                    ct = cc{ind}.CONTENT;
                case 6
                    % Reference
                    ct = [];
                case 7
                    
                    % Hex String
                    ct = cc{ind}.CONTENT;
            end
            
            if(isempty(ct))
                display(['Empty entry in field ' cc{ind}.DESCRIPTOR]);
            end
            str = createSubStruct(cc{ind}.DESCRIPTOR, ct, str);
        end
    end
end

function [str] = createSubStruct(descror, ct, str)

    lst = {};
    %descror
    
    while(1)
        idxs = strfind(descror, '/');
        if(isempty(idxs))
            if(~isempty(descror))
                lst = [lst descror];
            end
            break;
        else
            token = descror(1:idxs(1)-1);
            descror = descror(idxs(1)+1:end);
            %if(strcmp(token, '/') == 0)
            if(~isempty(token))
                lst = [lst token];
            end
        end
    end
    
    if(size(lst,2) > 0)
        evalExpr = lst{1};
        
        for(ind=2:size(lst,2))
            evalExpr = [evalExpr '.' lst{ind}];
        end
        
        evalExpr = ['str.' evalExpr '=ct;'];
        eval(evalExpr);
    end
end            