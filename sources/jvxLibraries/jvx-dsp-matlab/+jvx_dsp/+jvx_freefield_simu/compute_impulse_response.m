function [hdl, irs] = compute_impulse_response(hdl, iscart, v0,v1,v2, fs, fftsize, fshift, l_ir)
    
if(iscart)
    x = v0;
    y = v1;
    z = v2;
else
    [x, y, z] = jvx_dsp_base.math.jvx_sperical_to_cart(v0, v1, v2);
end

num_mics = size(hdl.mics,2);
min_dist = 0;
for(ind=1:num_mics)
     tmp1 = (x - hdl.mics{ind}.x);
     tmp1 = tmp1 * tmp1;
     
     tmp2 = (x - hdl.mics{ind}.x);
     tmp2 = tmp2 * tmp2;

     tmp3 = (z - hdl.mics{ind}.z);
     tmp3 = tmp3 * tmp3;

     dist(ind) = sqrt(tmp3 + tmp2 + tmp1);
     if(dist(ind) < min_dist)
         min_dist = dist(ind);
     end
end

freqs = [0:fftsize/2]/(fftsize) * 2 * pi;
ir = [1 zeros(1, l_ir)];
    
% Do the antialias compensation
[hdl_aa] = jvx_dsp_base.fast_conv_antialias.init(FFTSIZE, fftsize-fshift, 'hamming');
[hdl_aa, cWeights, ir_shift, eq_delay] = jvx_dsp_base.fast_conv_antialias.convert_cplx_weights_ifft_fft(hdl_aa, cWeights);
    
for(ind=1:num_mics)
    cplx_weights = exp(-1i * freqs * fs * (dist(ind)/min_dist));
    
    
    
