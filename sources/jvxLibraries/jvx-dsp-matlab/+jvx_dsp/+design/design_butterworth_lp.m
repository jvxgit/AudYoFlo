
function [num den] = design_butterworth_lp(order, fc, fs)
   
	[num, den, H0, b_rootszc, b_rootszs, b_poleszc, b_poleszs] = vx_dsp.design.design_butterworth_lp_core(order, fc, fs);
   
	
	
	
