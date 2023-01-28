function [retVal] = jvx_value32(selection)
   retVal = false;
    if(size(selection,2) > 2)
        selection = selection(3:end);
    else
        error(['Invalid Hex expression ' selection1]);
    end
    
    retVal = int32(hex2dec(selection));
