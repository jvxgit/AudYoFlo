function [numSelections] = jvx_num_selections(selection)
    
    numSelections = 0;
    if(size(selection,2) > 2)
        selection = selection(3:end);
    else
        error(['Invalid Hex expression ' selection]);
    end
       
    llValue = size(selection,2);
    for(ind=1:llValue)
        token = selection(end-ind+1:end-ind+1);
        vv = hex2dec(token);
        txt = dec2bin(vv);
        for(tnd=1:size(txt, 2))
            if(txt(tnd) == '1')
                numSelections = numSelections + 1;
            end
        end
    end
 
