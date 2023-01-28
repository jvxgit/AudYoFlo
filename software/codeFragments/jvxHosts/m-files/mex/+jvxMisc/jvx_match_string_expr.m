function [result] = jvx_match_string_expr(compareme_wc, tome)

if(~ischar(compareme_wc) || ~ischar(tome) )
    result = false;
    return;
end

if(isempty(compareme_wc))
    result = isempty(tome);
    return;
end

theEntry.isWildcard = false;
theEntry.token = '';

lstWildcards = {};
wildcardToken = '';
idx = 0;
matches = true;

bslashb4 = false;

cnt = 0;
while(1)
    if(cnt < size(compareme_wc,2))
        if(bslashb4 == false)
            if(strcmp(compareme_wc(cnt+1), '\'))
                bslashb4 = true;
            elseif((strcmp(compareme_wc(cnt+1), '?')) || ...
                    (strcmp(compareme_wc(cnt+1), '*')) || ...
                    (strcmp(compareme_wc(cnt+1), '+')))
                
                if(~isempty(wildcardToken))
                    theEntry.token = wildcardToken;
                    theEntry.isWildcard = false;
                    lstWildcards = [lstWildcards theEntry];
                    wildcardToken= '';
                end
                theEntry.token = compareme_wc(cnt+1);
                theEntry.isWildcard = true;
                lstWildcards = [lstWildcards theEntry];
            else
                wildcardToken = [ wildcardToken compareme_wc(cnt+1)];
            end
        else
            wildcardToken = [wildcardToken compareme_wc(cnt+1)];
            bslashb4 = false;
        end
        cnt = cnt + 1;
    else
        break;
    end
end

if(~isempty(wildcardToken))
    theEntry.token = wildcardToken;
    theEntry.isWildcard = false;
    lstWildcards = [lstWildcards theEntry];
end


minVal = 1;
maxVal = 1;

for(ii = 1:size(lstWildcards,2))
    ctoken = lstWildcards{ii}.token;
    
    if(lstWildcards{ii}.isWildcard)
        if(strcmp(ctoken, '*'))
            minVal = minVal + 0;
            maxVal = maxVal + size(tome,2)-1;
        end
        if(strcmp(ctoken, '?'))
            
            minVal = minVal +0;
            maxVal = maxVal + 1;
        end
        if(ctoken == "+")
            
            minVal = minVal + 1;
            maxVal = maxVal + size(tome,2)-1;
        end
    else
        idx = strfind(tome, ctoken);
        if(isempty(idx))
            matches = false;
            break;
        else
            idx = idx(1); % First occurrence
            if(~((idx >= minVal ) && (idx <= maxVal)))
                matches = false;
                break;
            else
                % It is all good, pattern checked
                tome = tome(idx + size(ctoken,2):end);
                minVal = 1;
                maxVal = 1;
            end
        end
    end
end
if(~((size(tome,2) >= (minVal-1))))
    matches = false;
end
result = matches;
