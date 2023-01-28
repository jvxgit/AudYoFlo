function [selection] = jvx_and(selection1, selection2)
    selection = '';
    if(size(selection1,2) > 2)
        selection1 = selection1(3:end);
    else
        error(['Invalid Hex expression ' selection1]);
    end
    
    if(size(selection2,2) > 2)
        selection2 = selection2(3:end);
    else
        error(['Invalid Hex expression ' selection2]);
    end
    
    llValue1 = size(selection1,2);
    llValue2 = size(selection2,2);
    llValue = max(llValue1, llValue2);
 for(ind=1:llValue)
     token1 = '0';
     token2 = '0';
     if(ind <= llValue1)
         token1 = selection1(end-ind+1:end-ind+1);
     end
     if(ind <= llValue2)
         token2 = selection2(end-ind+1:end-ind+1);
     end
     
     oneCompare = bitand(hex2dec(token1),hex2dec(token2));
     selection = [dec2hex(oneCompare, 1) selection];
 end
 selection = ['0x' selection];
