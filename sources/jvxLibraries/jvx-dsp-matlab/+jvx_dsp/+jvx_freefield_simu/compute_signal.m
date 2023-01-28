function [hdl, out] = compute_signal(hdl, iscart, v0,v1,v2, fs, fftsize, fshift, signal)
    
if(iscart)
    x = v0;
    y = v1;
    z = v2;
else
    [x, y, z] = jvx_dsp_base.math.jvx_sperical_to_cart(v0, v1, v2);
end

num_mics = size(hdl.mics,2);
dist = zeros(1,num_mics);
min_dist = -1;
for(ind=1:num_mics)
     tmp1 = (x - hdl.mics{ind}.x);
     tmp1 = tmp1 * tmp1;
     
     tmp2 = (y - hdl.mics{ind}.y);
     tmp2 = tmp2 * tmp2;

     tmp3 = (z - hdl.mics{ind}.z);
     tmp3 = tmp3 * tmp3;

     dist(ind) = sqrt(tmp3 + tmp2 + tmp1);
     if(min_dist < 0)
         min_dist = dist(ind);
     end
     if(dist(ind) < min_dist)
         min_dist = dist(ind);
     end
end

freqs = [0:fftsize/2]/(fftsize);
    
[hdl_aa] = jvx_dsp.fast_conv_antialias.init(fftsize, fftsize-fshift, 'hamming');
    
ploto.plot_ir = false; 
ploto.plot_out = false; 
ovlp = 0;

for(ind=1:num_mics)
    
    % Determine complex weights
    f = freqs * fs;
    om = f * 2 * pi;
    dT =  (dist(ind)-min_dist) / hdl.c;
    cplx_weights = exp(-1i * om * dT);
       
    % Do the antialias compensation
    [hdl_aa, cplx_weights, ir_shift, eq_delay] = jvx_dsp.fast_conv_antialias.convert_cplx_weights_ifft_fft(hdl_aa, cplx_weights);
    
    % Process signal for output
    [out(ind,:)] = jvx_dsp.fast_conv_os.process_signal(signal, fftsize, fshift, cplx_weights, eq_delay, ovlp, ploto);
end

    
    
    
