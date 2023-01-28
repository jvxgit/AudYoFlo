function [isSet] = jvx_test(selection, position)
 isSet = false;
 [exprsel llV]= jvxBitField.jvx_expand(selection);
 llValue = size(exprsel,2);
 txt = [];
 oneChar = exprsel(llValue - position);
if(strcmp(oneChar, '1'))
	isSet = true;
end