function cheader_pragma(CH, comment, pragma)
    if(~isempty(comment))
		fprintf(CH.fid, '// %s\n', comment);
	end
	fprintf(CH.fid, '%s\n', pragma);
