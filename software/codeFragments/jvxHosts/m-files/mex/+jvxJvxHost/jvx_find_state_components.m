% Function to find the state of a component which is not JVX_STATE_NONE.
% If the first component has been found, the state and the component id
% is returned. If desired, the component feature class property is evaluated
% and returned.
function [idState, idCompMat, foundSpecific] = jvx_find_state_components(handles, lst, feature_class_bitset)
    
id_check_none = jvxJvxHost.jvx_lookup_type_id__name('jvxState', 'JVX_STATE_NONE');
idState = id_check_none;
idCompMat = -1;
if(nargout == 3)
    foundSpecific = false;
end

% If no components are available, catch this error
if(isstruct(lst))
    tt = struct2cell(lst);
else
    tt = {};
end

for(ind = 1:size(tt,1))
    if(jvxBitField.jvx_compare(tt{ind}.STATE_BITFIELD, id_check_none) == false)
        idState = tt{ind}.STATE_BITFIELD;
        idCompMat = ind;
        if((nargin == 3) & (nargout == 3))
            if(jvxBitField.jvx_evaluate(jvxBitField.jvx_and(tt{ind}.FEATURE_CLASS_BITFIELD, feature_class_bitset)))
                foundSpecific = true;
            end      
        end
        break;
    end
end
     