function [] = jvx_diff_in_proc(fname, bsize, offset_stop)
f1 = fopen(['c/' fname], 'rb');
f2 = fopen(['mat/' fname], 'rb');

datac = fread(f1, Inf, 'single');
datamat = fread(f2, Inf, 'single');

dimX = bsize;
dimY = size(datac,1)/dimX;

datacr = reshape(datac, [dimX  dimY])';
datamatr = reshape(datamat, [dimX  dimY])';

collect = zeros(1, dimY);
for(ind=1:dimY-offset_stop)
    one = datacr(ind,:);
    two = datamatr(ind,:);
    ee = one - two;
    diff = ee * ee';
    collect(ind) = diff;
end

figure; 
plot(collect);
