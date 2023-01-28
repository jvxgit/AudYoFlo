function [retVal] = jvx_evaluate(selection)
   retVal = false;
    if(size(selection,2) > 2)
        selection = selection(3:end);
    else
        error(['Invalid Hex expression ' selection1]);
    end
    
    llValue = size(selection,2);
    for(ind=1:llValue)
        token1 = selection(end-ind+1:end-ind+1);
        if(hex2dec(token1))
            retVal =  true;
            break;
        end
    end
