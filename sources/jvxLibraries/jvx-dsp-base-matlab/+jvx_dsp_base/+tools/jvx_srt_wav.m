function [] = jvx_srt_wav(fsin_in, fsout_in, files_in)
  
% Argument handling
fsin = 16000;
fsout = 48000;
files = '*.out';

if(nargin > 1)
	fsin = fsin_in;
end
if(nargin > 2)
	fsou = fsout_in;
end
if(nargin > 3)
	files = files_in;
end

% ================================================
% Processing
% ================================================
	
str = dir(files);

for(ii = 1:size(str, 1))
    
    if(~str(ii).isdir)
        
        fname = str(ii).name;
        
        disp(['Processing file <' fname '>.']);
        [path name ext] = fileparts(fname);
        
		% Read in int16 samples and normalize
        ff = fopen(fname);
        sig = fread(ff, inf, 'int16');
        sign = sig ./ max(abs(sig)) * 0.95;
        
		if(fsin ~= fsout)
		
			% Resample
			gcd_val = gcd(fsin, fsout);
			Q = fsout/gcd_val;
			P = fsin /gcd_val;
			signQP = resample(sign, Q, P);
		else
			signQP = sign;
		end
        
		audiowrite([name '.wav'], signQP, fsout);
        fclose(ff);
    end
end
           
           
    