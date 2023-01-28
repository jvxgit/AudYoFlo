function [idState] = jvx_find_state_technology(info, lst)
    
id_check_none = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_NONE');
idState = id_check_none;
tt = struct2cell(lst);
for(ind = 1:size(tt,2))
    if(jvxBitField.jvx_compare(tt{ind}.STATE_BITFIELD, id_check_none) == false)
        idState = tt{ind}.STATE_BITFIELD;
        break;
    end
end
     