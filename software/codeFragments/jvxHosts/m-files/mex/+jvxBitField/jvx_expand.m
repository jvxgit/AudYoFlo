function [txt, llValues] = jvx_expand_bits(selection)
 llValues = size(selection, 2);
 txt = '';   
    for(ind = 1:llValues-2)
        oneChar = selection(llValues - ind + 1:llValues - ind + 1);
        val = hex2dec(oneChar);
        txtApp = dec2bin(val, 4);
        txt = [txtApp txt];
    end
    