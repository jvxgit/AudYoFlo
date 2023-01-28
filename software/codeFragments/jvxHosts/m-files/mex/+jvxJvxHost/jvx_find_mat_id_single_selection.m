function [int_id_mat] = jvx_find_mat_id_single_selection(selection)
   
   int_id_mat = 0;
    selection =  jvxBitField.jvx_expand(selection);
    llValues = size(selection, 2);
    
    for(ind = 1:llValues-2)
        oneChar = selection(llValues - ind + 1:llValues - ind + 1);
        if(strcmp(oneChar, '1') == 1)
            int_id_mat = ind;
            break;
        end
    end
