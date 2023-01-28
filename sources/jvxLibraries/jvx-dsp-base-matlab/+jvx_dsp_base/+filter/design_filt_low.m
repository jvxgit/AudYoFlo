function [str] = design_filt_low(fs, fglow, ftrans, rdb, adb, type, doPlot)
    
    % Store the parameters
    str.fs = fs;
    str.fglow = fglow;
    str.ftrans = ftrans;
    str.rdb = rdb;
    str.adb = adb;
    
    % Compute the frequencies
    str.Wlow = str.fglow/str.fs*2;
    str.Whigh = (str.fglow+ str.ftrans)/str.fs*2;
    
    % Derive the filter
    if(strcmp(type, 'cheby'))
        [str.n, str.Wc] = cheb2ord(str.Wlow, str.Whigh, str.rdb, str.adb);
        [str.tf.num,str.tf.den] = cheby2(str.n, str.adb, str.Wc, 'low');
    else
        [str.n, str.Wc] = buttord(str.Wlow, str.Whigh, str.rdb, str.adb);
        [str.tf.num,str.tf.den] = butter(str.n, str.Wc, 'low');
    end
    
    str.sos = jvx_dsp_base.filter.convert_tf_sos(str.tf.num,str.tf.den);
    
    if(doPlot)
        [H, F] = freqz(str.tf.num,str.tf.den, 1024, str.fs);
        figure;
        plot(F, 20 * log10(abs(H)));
    end