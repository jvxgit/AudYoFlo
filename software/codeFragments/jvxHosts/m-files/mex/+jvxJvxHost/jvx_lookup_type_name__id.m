function [name] = jvx_lookup_component_type_id(table, id)
    foundit = false;
    for(ind=1:size(table,2))
        entry = table{ind};
        if(entry{2} == id)
            name = entry{1};
            foundit = true;
            break;
        end
    end
    
    if(foundit == false)
        error(['Failed to lookup ' num2str(id) ' in lookup table']);
    end