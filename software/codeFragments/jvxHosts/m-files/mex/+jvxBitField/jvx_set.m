function [txt] = jvx_set(selection, position)
 [exprsel llV]= jvxBitField.jvx_expand(selection);
 llValue = size(exprsel,2);
 txt = [];
 oneChar = '1';
 exprsel(llValue - position) = oneChar;
 for(ind=1:llV-2)
     idxStart = (ind)*4-1;
     idxStop = (ind-1)*4;
     fldTxt = exprsel(llValue - idxStart:llValue - idxStop);
     oneHex = dec2hex(bin2dec(fldTxt));
     txt = [oneHex txt];
 end
 txt = ['0x' txt];
