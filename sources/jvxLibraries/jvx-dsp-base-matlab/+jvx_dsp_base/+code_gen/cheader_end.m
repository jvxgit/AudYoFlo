function cheader_end(CH)


fprintf(CH.fid, [
    '#endif // %s\n' ...
                   ], CH.token ...
);
fclose(CH.fid);
