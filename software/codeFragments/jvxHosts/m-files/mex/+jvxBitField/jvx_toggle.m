function [txt] = jvx_toggle(selection, position)
 [exprsel llV]= jvxBitField.jvx_expand(selection);
 llValue = size(exprsel,2);
 txt = [];
 oneChar = exprsel(llValue - position);
 if(strcmp(oneChar, '1'))
     oneChar = '0';
 else
     oneChar = '1';
 end
 exprsel(llValue - position) = oneChar;
 for(ind=1:llV-2)
     idxStart = (ind)*4-1;
     idxStop = (ind-1)*4;
     fldTxt = exprsel(llValue - idxStart:llValue - idxStop);
     oneHex = dec2hex(bin2dec(fldTxt));
     txt = [oneHex txt];
 end
 txt = ['0x' txt];
