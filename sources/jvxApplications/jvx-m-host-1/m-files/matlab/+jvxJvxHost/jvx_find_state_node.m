function [idState] = jvx_find_state_node(info, handles)
    
id_check_none = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_NONE');
idState = id_check_none;
tt = struct2cell(info.COMPONENTS.AUDIO_NODES);;
for(ind = 1:size(tt,2))
    if(jvxBitField.jvx_compare(tt{ind}.STATE_BITFIELD, id_check_none) == false)
        idState = tt{ind}.STATE_INT32;
        break;
    end
end