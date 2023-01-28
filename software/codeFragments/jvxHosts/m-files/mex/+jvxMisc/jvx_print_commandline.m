function [] = jvx_print_commandline(in, mfilename)
    inc = struct2cell(in);

    disp(['M-file ' mfilename ': Usage:']);
    for(ind2 = 1:size(inc,1))
        disp(inc{ind2}.description);
    end