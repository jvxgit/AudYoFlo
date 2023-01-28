function [] = jvx_print_setting_selection(str, tag)

	disp(['==================-' tag '-================================']);
	ll = size(str.OPTIONS, 1);
	for(ind=1:ll)
		txt = ['--#' num2str(ind-1) ': ' str.OPTIONS{ind}];
		if(jvxBitField.jvx_test(str.SELECTION_BITFIELD, ind-1))
			 txt = [txt '*'];
		end
		disp(txt);
	end
	disp('==================================================');
			
end