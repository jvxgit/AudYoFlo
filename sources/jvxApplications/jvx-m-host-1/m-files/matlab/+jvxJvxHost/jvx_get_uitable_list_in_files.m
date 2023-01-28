function [lst] = jvx_get_uitable_list_in_files(txtField)
lst = {};

for(ind= 1:size(txtField, 1))
    oneLine = {txtField{ind}};
   
    lst = [lst; oneLine];
end
        
