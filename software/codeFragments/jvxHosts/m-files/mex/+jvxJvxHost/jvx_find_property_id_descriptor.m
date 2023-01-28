function [id, cat,tp, access] = jvx_find_property_id_descriptor(str, descriptor)
id = -1;
cat = -1;
tp = -1;

tt = struct2cell(str);
            
for(ind=1:size(tt,1))
    % tt{ind}.DESCRIPTOR_STRING
    if(strcmp(tt{ind}.DESCRIPTOR_STRING, descriptor) == 1)
        id = tt{ind}.UNIQUE_ID;
        cat =  tt{ind}.CATEGORY_INT32;
        tp = tt{ind}.FORMAT_INT32;
        if(nargout == 4)
            access = tt{ind}.ACCESS_TYPE_INT32;
        end
        break;
    end
end
