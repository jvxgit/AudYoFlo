function[] = jvx_set_property_single_selection_mat_id(handles, tpC, catProp, idProp, newValMatId)
    selection = jvxBitField.jvx_create(newValMatId-1);
     
    % Simplified property setting: only selection
    [a b]= handles.hostcall('set_property_uniqueid', tpC, catProp, idProp, selection);
     if(~a)
         % ERROR
     end
