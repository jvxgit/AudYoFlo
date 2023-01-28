function [lst] = jvx_get_uitable_list_multiple_selection(txtField, selection)
lst = {};
numEntries = size(txtField, 1);
selection =  jvxBitField.jvx_expand(selection);

llValues = size(selection, 2);
for(ind= 1:numEntries)
    oneChar = '0';
    if(ind <= (llValues-2))
        oneChar = selection(llValues - ind + 1:llValues - ind + 1);
    end
    if(strcmp(oneChar, '1') == 1)
           oneLine = {txtField{ind}, 'yes'};
    else
           oneLine = {txtField{ind}, 'no'};
    end
    
    lst = [lst;oneLine];
end
        
