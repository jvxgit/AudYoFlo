function [retVal] = jvx_selectionid(selection)
retVal = -1;
[exprsel llV]= jvxBitField.jvx_expand(selection);
llValue = size(exprsel,2);
txt = [];
oneChar = '1';
for(ind=0:size(exprsel, 2)-1)
    if(exprsel(end-ind) == oneChar)
        retVal = ind;
        break;
    end
end
