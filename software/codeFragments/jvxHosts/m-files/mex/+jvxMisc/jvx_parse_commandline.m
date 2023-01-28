function [out, suc] = jvx_parse_commandline(in, varargin)
    cnt = 1;
    inc = struct2cell(in);
    incnames = fieldnames(in);
    suc = true;
    
    % Read input and convert as desired
    while(cnt <= size(varargin,2))
        token = varargin{cnt};
        cnt = cnt + 1;
        foundit = false;
        for(ind2 = 1:size(inc,1))
            tokenSearch = inc{ind2}.token;
            numSearch = inc{ind2}.numSearch;
            if(strcmp(token, tokenSearch))
                foundit = true;
                if(numSearch == 0)
                    inc{ind2}.value = true;
                elseif(numSearch == 1)
                    if(ischar(inc{ind2}.value))
                        inc{ind2}.value = varargin{cnt};
                    elseif(isnumeric(inc{ind2}.value))
                            inc{ind2}.value = str2double(varargin{cnt});
                    elseif(islogical(inc{ind2}.value))
                            inc{ind2}.value = (strcmp(varargin{cnt}, 'false') == 1);
                    else
                        error('Unknown conversion format in command line parser');
                    end
                    cnt = cnt + 1;
                else
                    for(ind3=1:numSearch)
                        if(ischar(inc{ind2}.value))
                            inc{ind2}.value = varargin{cnt};
                        elseif(isnumeric(inc{ind2}.value))
                            inc{ind2}.value = str2double(varargin{cnt});
                         elseif(islogical(inc{ind2}.value))
                            inc{ind2}.value = (strcmp(varargin{cnt}, 'false') == 1);
                        else
                            error('Unknown conversion format in command line parser');
                        end
                        cnt = cnt + 1;
                    end;
                end
                break;
            end
        end
        if(foundit == false)
            disp(['Token ' token ' unmatched.']);
            suc = false;
        end
    end
    
    out = cell2struct(inc, incnames);
                    
        
        
        