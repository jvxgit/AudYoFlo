%lsfs = 3.0;
lsfs = [0: 0.1:pi];
lsfs = 0.114;
ap = 0.95;
EPS = 1e-8;

% Weg 1: 
lsfs_exp = exp(i*lsfs);
lsfs_out_w1 = (lsfs_exp + ap)./(1 + ap*lsfs_exp);
lsfs_out_w2 = [];

% Weg 2: 
for(ind=1:size(lsfs,2))
    real_lsf = cos(lsfs(ind));
    imag_lsf = sin(lsfs(ind));
    
    tmp1 = 1 + real_lsf * ap;
    tmp2 = imag_lsf * ap; 
	tmp3 = tmp1 * tmp1 + tmp2*tmp2;
	tmp3 = sqrt(tmp3);
    tmp4 = atan(tmp2/(abs(tmp1) + EPS));
    if(tmp1 < 0)
        tmp4 = pi - tmp4;
    end
	
    tmp1 = real_lsf + ap;
	tmp2 = imag_lsf;

    tmp5 = tmp1 * tmp1 + tmp2*tmp2;
	tmp5 = sqrt(tmp5);
    tmp6 = atan(tmp2/(abs(tmp1)+EPS));
     if(tmp1 < 0)
        tmp6 = pi - tmp6;
    end
    
    tmp1 = tmp5 / tmp3;
	tmp2 = tmp6 - tmp4;
	lsfs_out_w2(ind) = exp(i*tmp2);
end
    
figure; plot(lsfs_exp, 'k*'); hold on; plot(lsfs_out_w1, 'rs'); plot(0.95*lsfs_out_w2, 'gs');
