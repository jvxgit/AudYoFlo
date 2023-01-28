function [bfield foundit] = jvx_modify_settings_selection(str, sel_tag, onlyOne)

    foundit = false;
	ll = size(str.OPTIONS, 1);
	bfield = jvxBitField.jvx_create(-1);
	for(ind=1:ll)
		if(strcmp(str.OPTIONS{ind}, sel_tag) == 1)
            foundit = true;
			bfield = jvxBitField.jvx_set(bfield, ind-1);
			if(onlyOne)
				break;
			end
		end
	end
end
	
