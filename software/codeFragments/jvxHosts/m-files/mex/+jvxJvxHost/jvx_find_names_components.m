function [lst, idLastState] = jvx_find_names_components(lstIn, stateFind)

lst = {};
idLastState = -1;

if(~isempty(lstIn))
    tt = struct2cell(lstIn);
    for(ind = 1:size(tt,1))
        lst{ind} = tt{ind}.DESCRIPTION_STRING;
        
        if(nargin == 2) && (nargout == 2)
            if(jvxBitField.jvx_value32(tt{ind}.STATE_BITFIELD) >= jvxBitField.jvx_value32(stateFind))
                idLastState = ind;
            end
        end
    end
end