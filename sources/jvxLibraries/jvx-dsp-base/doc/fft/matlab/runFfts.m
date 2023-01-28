function [xk, Xn_fft_orig] = runFfts(xk)

disp(' ################## runFfts #########################');

if(nargin == 0)
    FFTS1 = 1024;
    xk = randn(FFTS1,1);
else
    FFTS1 = size(xk,1);
end

%% =======================================================================
NFFT = log2(FFTS1);

% Original FFT
Xn_fft_orig = fft(xk);

%% =======================================================================
disp('%% FFT variant No 1:');
disp('%% - Full FFT (matrix multiplication) vs FFT');

%% FFT variant No 1: Full computation
fftMat = zeros(FFTS1, FFTS1);
WN = exp(-i*2*pi/FFTS1);
for(indx = 1:FFTS1)
    for(indy = 1:FFTS1)
        fftMat(indx, indy) = WN ^( (indx-1)*(indy-1));
    end
end

Xn_dft = fftMat * xk;

diff = Xn_dft - Xn_fft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);

%% =======================================================================
disp('%% FFT variant No 2: ');
disp('%% - Butterfly algorithm, ');
disp('%% - full length, ');
disp('%% - complex part put to zero on init');
%% =======================================================================
inout_re = zeros(FFTS1, 1);
inout_im = zeros(FFTS1, 1);

for(idx = 0:FFTS1-1)
    idxWrite = idx;
    idxRead = bitreverse(idx, NFFT);
    inout_re(idxWrite+1) = xk(idxRead + 1);
end

tw_facs = exp(-i*2*pi/FFTS1 * [0:FFTS1/2-1]);
tw_facs_real = real(tw_facs);
tw_facs_imag = imag(tw_facs);

num_bfly = FFTS1/2;
num_ops_per_bfly = 1;
num_shift_bfly2bfly = 1;
num_shift_tw = num_bfly;

for(idx_stage = 0:NFFT-1)

    idx_in_x0 = 0;
    idx_in_x1 = num_ops_per_bfly;

    %disp('=== New Butterfly Loop ===');
    for(idx_bfly = 0:num_bfly - 1)
        
        idx_in_tw = 0;
        
        %disp('=== In-Butterfly-Loop ===');
        for(idx_inbfly=0:num_ops_per_bfly-1)
            %disp(['x0[' num2str(idx_in_x0) ']']);
            %disp(['x1[' num2str(idx_in_x1) ']']);
            %disp(['tw[' num2str(idx_in_tw) ']']);
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            x1_real = inout_re(idx_in_x1+1);
            x1_imag = inout_im(idx_in_x1+1);
            
            tw_fac_real = tw_facs_real(idx_in_tw+1);
            tw_fac_imag = tw_facs_imag(idx_in_tw+1);
            
            x0_real_p = x0_real + tw_fac_real * x1_real - tw_fac_imag * x1_imag ;
            x0_imag_p = x0_imag + tw_fac_real * x1_imag + tw_fac_imag * x1_real ;
            
            x1_real_p = x0_real - tw_fac_real * x1_real + tw_fac_imag * x1_imag;
            x1_imag_p = x0_imag - tw_fac_real * x1_imag - tw_fac_imag * x1_real;
            
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            inout_re(idx_in_x1+1) = x1_real_p;
            inout_im(idx_in_x1+1) = x1_imag_p;
            
            idx_in_tw = idx_in_tw + num_shift_tw;
            idx_in_x0 = idx_in_x0 + 1;
            idx_in_x1 = idx_in_x1 + 1;
        end
        
        idx_in_x0 = idx_in_x0 + num_shift_bfly2bfly;
        idx_in_x1 = idx_in_x1 + num_shift_bfly2bfly;
        
    end
    
    num_bfly = num_bfly/2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly * 2;
    num_shift_bfly2bfly = num_shift_bfly2bfly * 2;
    
end

Xn_fft1 = inout_re + i * inout_im;

diff = Xn_fft1 - Xn_fft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);

%% =======================================================================
disp('%% FFT variant No 3: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Sorting of input into real and cpomplex and bit reversal addr in two steps');
%% =======================================================================
inout_re = zeros(FFTS1/2, 1);
inout_im = zeros(FFTS1/2, 1);

tmp1 = xk(1:2:end);
tmp2 = xk(2:2:end);
FFTS1P = FFTS1/2;
NFFTP = NFFT-1;

for(idx = 0:FFTS1P-1)
    idxWrite = idx;
    idxRead = bitreverse(idx, NFFTP);
    inout_re(idxWrite+1) = tmp1(idxRead + 1);
    inout_im(idxWrite+1) = tmp2(idxRead + 1);
end

tw_facs = exp(-i*2*pi/FFTS1P * [0:FFTS1P/2-1]);
tw_facs_real = real(tw_facs);
tw_facs_imag = imag(tw_facs);

num_bfly = FFTS1P/2;
num_ops_per_bfly = 1;
num_shift_bfly2bfly = 1;
num_shift_tw = num_bfly;

for(idx_stage = 0:NFFTP-1)

    idx_in_x0 = 0;
    idx_in_x1 = num_ops_per_bfly;

    %disp('=== New Butterfly Loop ===');
    for(idx_bfly = 0:num_bfly - 1)
        
        idx_in_tw = 0;
        
        %disp('=== In-Butterfly-Loop ===');
        for(idx_inbfly=0:num_ops_per_bfly-1)
            %disp(['x0[' num2str(idx_in_x0) ']']);
            %disp(['x1[' num2str(idx_in_x1) ']']);
            %disp(['tw[' num2str(idx_in_tw) ']']);
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            x1_real = inout_re(idx_in_x1+1);
            x1_imag = inout_im(idx_in_x1+1);
            
            tw_fac_real = tw_facs_real(idx_in_tw+1);
            tw_fac_imag = tw_facs_imag(idx_in_tw+1);
            
            x0_real_p = x0_real + tw_fac_real * x1_real - tw_fac_imag * x1_imag ;
            x0_imag_p = x0_imag + tw_fac_real * x1_imag + tw_fac_imag * x1_real ;
            
            x1_real_p = x0_real - tw_fac_real * x1_real + tw_fac_imag * x1_imag;
            x1_imag_p = x0_imag - tw_fac_real * x1_imag - tw_fac_imag * x1_real;
            
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            inout_re(idx_in_x1+1) = x1_real_p;
            inout_im(idx_in_x1+1) = x1_imag_p;
            
            idx_in_tw = idx_in_tw + num_shift_tw;
            idx_in_x0 = idx_in_x0 + 1;
            idx_in_x1 = idx_in_x1 + 1;
        end
        
        idx_in_x0 = idx_in_x0 + num_shift_bfly2bfly;
        idx_in_x1 = idx_in_x1 + num_shift_bfly2bfly;
        
    end
    
    num_bfly = num_bfly/2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly * 2;
    num_shift_bfly2bfly = num_shift_bfly2bfly * 2;
    
end

Xn_fft3P = inout_re + i * inout_im;

Xn_fft3P_real = zeros(FFTS1P,1);
Xn_fft3P_imag = zeros(FFTS1P,1);

% Get back real part of output FFT:
Xn_fft3P_real(1) = (Xn_fft3P(1) +  conj(Xn_fft3P(1)))/2;
Xn_fft3P_imag(1) = -i * (Xn_fft3P(1) -  conj(Xn_fft3P(1)))/2;

for(idx = 1:FFTS1P-1)
    Xn_fft3P_real(idx + 1) = (Xn_fft3P(1+idx) +  conj(Xn_fft3P(FFTS1P-idx+1)))/2;
    Xn_fft3P_imag(idx + 1) = -i*(Xn_fft3P(1+idx) -  conj(Xn_fft3P(FFTS1P-idx+1)))/2;
end



tw_facs_p = exp(-i*2*pi/FFTS1 * [0:FFTS1/2-1]);
Xn_fft3 = zeros(FFTS1P + 1,1);
for(idx = 1:FFTS1P-1)
    tt1 =  (Xn_fft3P(idx+1) + conj(Xn_fft3P(FFTS1P-idx+1)))/2;
    tt2 =  -i*(Xn_fft3P(idx+1) - conj(Xn_fft3P(FFTS1P-idx+1)))/2;
    
    Xn_fft3(idx+1) = tt1 + tw_facs_p(idx+1) * tt2;
end

tt1 =  (Xn_fft3P(1) + conj(Xn_fft3P(1)))/2;
tt2 =  -i*(Xn_fft3P(1) - conj(Xn_fft3P(1)))/2;


Xn_fft3(1) = tt1 + tt2;
Xn_fft3(FFTS1P+1) = tt1 - tt2;

Xn_fft3c = [Xn_fft3 ; conj(Xn_fft3(FFTS1P:-1:2))];
diff = Xn_fft3c - Xn_fft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);

%% =======================================================================
disp('%% FFT variant No 4: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Sorting of input into real and cpomplex and bit reversal addr in one step');
%% =======================================================================
inout_re = zeros(FFTS1/2, 1);
inout_im = zeros(FFTS1/2, 1);

FFTS1P = FFTS1/2;
NFFTP = NFFT-1;


tw_facs = exp(-i*2*pi/FFTS1 * [0:FFTS1/2-1]);
tw_facs_real = real(tw_facs);
tw_facs_imag = imag(tw_facs);

num_bfly = FFTS1P/2;
%num_ops_per_bfly = 1;
%num_shift_bfly2bfly = 1;
%num_shift_tw = num_bfly * 2;

idx_in_x0 = 0;
idx_in_x1 = 1;

Xn_fft3_real = zeros(FFTS1P + 1,1);
Xn_fft3_imag = zeros(FFTS1P + 1,1);

%% First stage of FFT butterflies.
%% This stage takes into account:
%% - the bit reverse addressing mode of the input data, 
%% - transforms the real input into a complex FFT entry
%% - exploits that the butterfly twiddle factor is 1
for(idx_bfly = 0:FFTS1/4 - 1)
        
    idxRead0 = bitreverse(idx_in_x0, NFFT);
    idxRead1 = bitreverse(idx_in_x1, NFFT);
    
    x0_real = xk(idxRead0 + 1);
    x0_imag = xk(idxRead0 + 1 + 1);
    x1_real = xk(idxRead1 + 1);
    x1_imag = xk(idxRead1 + 1 + 1);
        
    x0_real_p = x0_real + x1_real; % Tw real is 1.0, Tw imag is 0
    x0_imag_p = x0_imag + x1_imag; % Tw real is 1.0, Tw imag is 0
    
    x1_real_p = x0_real - x1_real; % Tw real is 1.0, Tw imag is 0
    x1_imag_p = x0_imag - x1_imag; % Tw real is 1.0, Tw imag is 0
    
    inout_re(idx_in_x0+1) = x0_real_p;
    inout_im(idx_in_x0+1) = x0_imag_p;
    inout_re(idx_in_x1+1) = x1_real_p;
    inout_im(idx_in_x1+1) = x1_imag_p;
    
    idx_in_x0 = idx_in_x0 + 2;
    idx_in_x1 = idx_in_x1 + 2;

end

%% Second and following stages: FFT input buffers are all well prepared
%% Compute tw factor multiplications, butterfly processing
num_bfly = FFTS1/8;
num_shift_tw = FFTS1/4;
num_ops_per_bfly = 2;

% All other stages
for(idx_stage = 1:NFFTP-1)

    % Starting points for the butterfly
    idx_in_x0 = 0;
    idx_in_x1 = num_ops_per_bfly;

    % Butterfly Group Loop
    for(idx_bfly = 0:num_bfly - 1)
        
        % Start tw factors from 0-index
        idx_in_tw = 0;
        
        % Butterfly loop
        for(idx_inbfly=0:num_ops_per_bfly-1)
            
            % Read
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            x1_real = inout_re(idx_in_x1+1);
            x1_imag = inout_im(idx_in_x1+1);
            
            % Get twiddle factors
            tw_fac_real = tw_facs_real(idx_in_tw+1);
            tw_fac_imag = tw_facs_imag(idx_in_tw+1);
            
            % Compute complex multiplications
            x0_real_p = x0_real + tw_fac_real * x1_real - tw_fac_imag * x1_imag ;
            x0_imag_p = x0_imag + tw_fac_real * x1_imag + tw_fac_imag * x1_real ;            
            x1_real_p = x0_real - tw_fac_real * x1_real + tw_fac_imag * x1_imag;
            x1_imag_p = x0_imag - tw_fac_real * x1_imag - tw_fac_imag * x1_real;
            
            % Write results
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            inout_re(idx_in_x1+1) = x1_real_p;
            inout_im(idx_in_x1+1) = x1_imag_p;
            
            % Update indexing
            idx_in_tw = idx_in_tw + num_shift_tw;
            idx_in_x0 = idx_in_x0 + 1;
            idx_in_x1 = idx_in_x1 + 1;
        end
        
        % Update indexing: Shift to next butterfly groups
        idx_in_x0 = idx_in_x0 + num_ops_per_bfly;
        idx_in_x1 = idx_in_x1 + num_ops_per_bfly;
        
    end
    
    % Update from stage to stage
    num_bfly = num_bfly/2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly * 2;    
end

% Output stage: From complex N/2 FFT to N FFT
for(idx = 1:FFTS1P-1)
    
    % This is in fact the last stage of the N FFT butterfly BUT only the
    % upper half
    tw_fac_real = tw_facs_real(idx+1);
    tw_fac_imag = tw_facs_imag(idx+1);

    % Combine result from complex FFT to determine FFT for real part
    tt1_real = inout_re(idx+1) + inout_re(FFTS1P-idx+1);
    tt1_real = tt1_real/2;
    tt1_imag = inout_im(idx+1) - inout_im(FFTS1P-idx+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = inout_im(idx+1) + inout_im(FFTS1P-idx+1);
    tt2_real = tt2_real/2;
    tt2_imag = -inout_re(idx+1) + inout_re(FFTS1P-idx+1);
    tt2_imag = tt2_imag/2;
    
    % Final twiddle factor
    tt_real = tt1_real + tw_fac_real * tt2_real - tw_fac_imag * tt2_imag;
    tt_imag = tt1_imag + tw_fac_real * tt2_imag + tw_fac_imag * tt2_real;

    % Write to output buffer
    Xn_fft3_real(idx+1) = tt_real;
    Xn_fft3_imag(idx+1) = tt_imag;
end

% Special case for first and last element in FFT buffer (real valued)
tt1 =  inout_re(1);
tt2 =  inout_im(1);

Xn_fft3_real(1) = tt1 + tt2;
Xn_fft3_imag(1) = 0;
Xn_fft3_real(FFTS1P+1) = tt1 - tt2;
Xn_fft3_imag(FFTS1P+1) = 0;

% Combine to form output fft
Xn_fft3 = Xn_fft3_real + i * Xn_fft3_imag;

Xn_fft3c = [Xn_fft3 ; conj(Xn_fft3(FFTS1P:-1:2))];
diff = Xn_fft3c - Xn_fft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);

%% =======================================================================
disp('%% FFT variant No 5: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Sorting of input into real and cpomplex and bit reversal addr in two steps');
disp('%% - lifting scheme');
%% =======================================================================
inout_re = zeros(FFTS1/2, 1);
inout_im = zeros(FFTS1/2, 1);

FFTS1P = FFTS1/2;
NFFTP = NFFT-1;


tw_facs = exp(-i*2*pi/FFTS1 * [0:FFTS1/2-1]);
tw_facs_real = real(tw_facs);
tw_facs_imag = imag(tw_facs);

ls_facs_c0 = (tw_facs_real(1:FFTS1/4)-1)./tw_facs_imag(1:FFTS1/4);
ls_facs_s0 = tw_facs_imag(1:FFTS1/4);

ls_facs_c1 = (tw_facs_real(FFTS1/4+1:FFTS1/2)+1)./tw_facs_imag(FFTS1/4+1:FFTS1/2);
ls_facs_s1 = tw_facs_imag(FFTS1/4+1:FFTS1/2);

num_bfly = FFTS1P/2;
%num_ops_per_bfly = 1;
%num_shift_bfly2bfly = 1;
%num_shift_tw = num_bfly * 2;

idx_in_x0 = 0;
idx_in_x1 = 1;

Xn_fft3_real = zeros(FFTS1P + 1,1);
Xn_fft3_imag = zeros(FFTS1P + 1,1);

%% First stage of FFT butterflies.
%% This stage takes into account:
%% - the bit reverse addressing mode of the input data, 
%% - transforms the real input into a complex FFT entry
%% - exploits that the butterfly twiddle factor is 1
for(idx_bfly = 0:FFTS1/4 - 1)
        
    idxRead0 = bitreverse(idx_in_x0, NFFT);
    idxRead1 = bitreverse(idx_in_x1, NFFT);
    
    x0_real = xk(idxRead0 + 1);
    x0_imag = xk(idxRead0 + 1 + 1);
    x1_real = xk(idxRead1 + 1);
    x1_imag = xk(idxRead1 + 1 + 1);
        
    x0_real_p = x0_real + x1_real; % Tw real is 1.0, Tw imag is 0
    x0_imag_p = x0_imag + x1_imag; % Tw real is 1.0, Tw imag is 0
    
    x1_real_p = x0_real - x1_real; % Tw real is 1.0, Tw imag is 0
    x1_imag_p = x0_imag - x1_imag; % Tw real is 1.0, Tw imag is 0
    
    inout_re(idx_in_x0+1) = x0_real_p;
    inout_im(idx_in_x0+1) = x0_imag_p;
    inout_re(idx_in_x1+1) = x1_real_p;
    inout_im(idx_in_x1+1) = x1_imag_p;
    
    idx_in_x0 = idx_in_x0 + 2;
    idx_in_x1 = idx_in_x1 + 2;

end

%% Second and following stages: FFT input buffers are all well prepared
%% Compute tw factor multiplications, butterfly processing
num_bfly = FFTS1/8;
num_shift_tw = FFTS1/4;
num_ops_per_bfly = 2;

% All other stages
for(idx_stage = 1:NFFTP-1)

    % Starting points for the butterfly
    idx_in_x0 = 0;
    idx_in_x1 = num_ops_per_bfly;

    % Butterfly Group Loop
    for(idx_bfly = 0:num_bfly - 1)
                
         x0_real = inout_re(idx_in_x0+1);
         x0_imag = inout_im(idx_in_x0+1);
         x1_real = inout_re(idx_in_x1+1);
         x1_imag = inout_im(idx_in_x1+1);
            
         % Compute complex multiplications, only realpart is 1.0 here
         x0_real_p = x0_real + x1_real;
         x0_imag_p = x0_imag + x1_imag;            
         x1_real_p = x0_real - x1_real;
         x1_imag_p = x0_imag - x1_imag;
            
         % Write results
         inout_re(idx_in_x0+1) = x0_real_p;
         inout_im(idx_in_x0+1) = x0_imag_p;
         inout_re(idx_in_x1+1) = x1_real_p;
         inout_im(idx_in_x1+1) = x1_imag_p;
            
         % Update indexing
         idx_in_tw = num_shift_tw;
         idx_in_x0 = idx_in_x0 + 1;
         idx_in_x1 = idx_in_x1 + 1;

         % Butterfly loop
         for(idx_inbfly=0:num_ops_per_bfly/2-2)
             
             % Read
             x0_real = inout_re(idx_in_x0+1);
             x0_imag = inout_im(idx_in_x0+1);
             x1_real = inout_re(idx_in_x1+1);
             x1_imag = inout_im(idx_in_x1+1);
             
             % Get twiddle factors - lifting scheme             
             tw_lift_cm1 = ls_facs_c0(idx_in_tw+1);
             tw_lift_s = ls_facs_s0(idx_in_tw+1);
             
             % Complex multiplications using lifting scheme
             x_real_pp = x1_real + tw_lift_cm1 * x1_imag;
             x_imag_pp = x1_imag + tw_lift_s * x_real_pp;
             x_real_pp = x_real_pp + tw_lift_cm1 * x_imag_pp;

             x0_real_p = x0_real + x_real_pp;
             x0_imag_p = x0_imag + x_imag_pp;
             x1_real_p = x0_real - x_real_pp;
             x1_imag_p = x0_imag - x_imag_pp;
             
             % Write results
             inout_re(idx_in_x0+1) = x0_real_p;
             inout_im(idx_in_x0+1) = x0_imag_p;
             inout_re(idx_in_x1+1) = x1_real_p;
             inout_im(idx_in_x1+1) = x1_imag_p;
             
             % Update indexing
             idx_in_tw = idx_in_tw + num_shift_tw;
             idx_in_x0 = idx_in_x0 + 1;
             idx_in_x1 = idx_in_x1 + 1;
         end
        
        x0_real = inout_re(idx_in_x0+1);
         x0_imag = inout_im(idx_in_x0+1);
         x1_real = inout_re(idx_in_x1+1);
         x1_imag = inout_im(idx_in_x1+1);
                  
         % Compute complex multiplications
         x0_real_p = x0_real + x1_imag ;
         x0_imag_p = x0_imag - x1_real ;
         x1_real_p = x0_real - x1_imag;
         x1_imag_p = x0_imag + x1_real;
         
         % Write results
         inout_re(idx_in_x0+1) = x0_real_p;
         inout_im(idx_in_x0+1) = x0_imag_p;
         inout_re(idx_in_x1+1) = x1_real_p;
         inout_im(idx_in_x1+1) = x1_imag_p;
         
         % Update indexing
         idx_in_tw = num_shift_tw;
         idx_in_x0 = idx_in_x0 + 1;
         idx_in_x1 = idx_in_x1 + 1;
         
         for(idx_inbfly=0:num_ops_per_bfly/2-2)
             
             % Read
             x0_real = inout_re(idx_in_x0+1);
             x0_imag = inout_im(idx_in_x0+1);
             x1_real = inout_re(idx_in_x1+1);
             x1_imag = inout_im(idx_in_x1+1);
                          
             % Get twiddle factors - lifting scheme             
             tw_lift_cm1 = ls_facs_c1(idx_in_tw+1);
             tw_lift_s = ls_facs_s1(idx_in_tw+1);
             
             % Complex multiplications using lifting scheme
             x_real_pp = x1_real + tw_lift_cm1 * x1_imag;
             x_imag_pp = x1_imag - tw_lift_s * x_real_pp;
             x_real_pp = x_real_pp + tw_lift_cm1 * x_imag_pp;

             x0_real_p = x0_real - x_real_pp;
             x0_imag_p = x0_imag - x_imag_pp;
             x1_real_p = x0_real + x_real_pp;
             x1_imag_p = x0_imag + x_imag_pp;
             
             % Write results
             inout_re(idx_in_x0+1) = x0_real_p;
             inout_im(idx_in_x0+1) = x0_imag_p;
             inout_re(idx_in_x1+1) = x1_real_p;
             inout_im(idx_in_x1+1) = x1_imag_p;
             
             % Update indexing
             idx_in_tw = idx_in_tw + num_shift_tw;
             idx_in_x0 = idx_in_x0 + 1;
             idx_in_x1 = idx_in_x1 + 1;
         end
         
        % Update indexing: Shift to next butterfly groups
        idx_in_x0 = idx_in_x0 + num_ops_per_bfly;
        idx_in_x1 = idx_in_x1 + num_ops_per_bfly;
        
    end
    
    % Update from stage to stage
    num_bfly = num_bfly/2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly * 2;    
end

% This is in fact the last stage of the N FFT butterfly BUT only the
% upper half

% Combine result from complex FFT to determine FFT for real part
tt1_real = inout_re(1);

% Combine result from complex FFT to determine FFT for imag part
tt2_real = inout_im(1);

% Final twiddle factor
tt_real = tt1_real + tt2_real;

% Write to output buffer
Xn_fft3_real(1) = tt_real;
Xn_fft3_imag(1) = 0;

% Output stage: From complex N/2 FFT to N FFT
for(idx = 1:FFTS1P/2-1)
    
    % Get twiddle factors - lifting scheme             
    tw_lift_cm1 = ls_facs_c0(idx+1);
    tw_lift_s = ls_facs_s0(idx+1);
             
    % Combine result from complex FFT to determine FFT for real part
    tt1_real = inout_re(idx+1) + inout_re(FFTS1P-idx+1);
    tt1_real = tt1_real/2;
    tt1_imag = inout_im(idx+1) - inout_im(FFTS1P-idx+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = inout_im(idx+1) + inout_im(FFTS1P-idx+1);
    tt2_real = tt2_real/2;
    tt2_imag = -inout_re(idx+1) + inout_re(FFTS1P-idx+1);
    tt2_imag = tt2_imag/2;
    
    % Complex multiplications using lifting scheme
    x_real_pp = tt2_real + tw_lift_cm1 * tt2_imag;
    x_imag_pp = tt2_imag + tw_lift_s * x_real_pp;
    x_real_pp = x_real_pp + tw_lift_cm1 * x_imag_pp;
    
    tt_real = tt1_real + x_real_pp;
    tt_imag = tt1_imag + x_imag_pp;
             
    % Write to output buffer
    Xn_fft3_real(idx+1) = tt_real;
    Xn_fft3_imag(idx+1) = tt_imag;
end

% This is in fact the last stage of the N FFT butterfly BUT only the
% upper half
tw_fac_imag = tw_facs_imag(FFTS1P/2+1);

% Combine result from complex FFT to determine FFT for real part
tt1_real = inout_re(FFTS1P/2+1);
tt2_real = inout_im(FFTS1P/2+1);

% Final twiddle factor
tt_real = tt1_real;
tt_imag = tw_fac_imag * tt2_real;

% Write to output buffer
Xn_fft3_real(FFTS1P/2+1) = tt_real;
Xn_fft3_imag(FFTS1P/2+1) = tt_imag;
    
for(idx = 1:FFTS1P/2-1)
    
    % Get twiddle factors - lifting scheme
    tw_lift_cm1 = ls_facs_c1(idx+1);
    tw_lift_s = ls_facs_s1(idx+1);
                           
    % Combine result from complex FFT to determine FFT for real part
    tt1_real = inout_re(FFTS1P/2 + idx+1) + inout_re(FFTS1P/2 - idx+1);
    tt1_real = tt1_real/2;
    tt1_imag = inout_im(FFTS1P/2 + idx+1) - inout_im(FFTS1P/2 - idx+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = inout_im(FFTS1P/2 + idx+1) + inout_im(FFTS1P/2 - idx+1);
    tt2_real = tt2_real/2;
    tt2_imag = -inout_re(FFTS1P/2 + idx+1) + inout_re(FFTS1P/2 - idx+1);
    tt2_imag = tt2_imag/2;
       
    % Complex multiplications using lifting scheme
    x_real_pp = tt2_real + tw_lift_cm1 * tt2_imag;
    x_imag_pp = tt2_imag - tw_lift_s * x_real_pp;
    x_real_pp = x_real_pp + tw_lift_cm1 * x_imag_pp;
    
    tt_real = tt1_real - x_real_pp;
    tt_imag = tt1_imag - x_imag_pp;
    
    % Write to output buffer
    Xn_fft3_real(FFTS1P/2 + idx+1) = tt_real;
    Xn_fft3_imag(FFTS1P/2 + idx+1) = tt_imag;
end

% Combine result from complex FFT to determine FFT for real part
tt1_real = inout_re(1);

% Combine result from complex FFT to determine FFT for imag part
tt2_real = inout_im(1);

Xn_fft3_real(FFTS1P+1) = tt1_real - tt2_real;
Xn_fft3_imag(FFTS1P+1) = 0;

% Combine to form output fft
Xn_fft3 = Xn_fft3_real + i * Xn_fft3_imag;

Xn_fft3c = [Xn_fft3 ; conj(Xn_fft3(FFTS1P:-1:2))];
diff = Xn_fft3c - Xn_fft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);



%% =======================================================================
disp('%% FFT variant No 6: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Sorting of input into real and cpomplex and bit reversal addr in two steps');
disp('%% - lifting scheme');
disp('%% - in-place reconstruction');
%% =======================================================================
inout_re = zeros(FFTS1/2, 1);
inout_im = zeros(FFTS1/2, 1);

FFTS1P = FFTS1/2;
NFFTP = NFFT-1;


tw_facs = exp(-i*2*pi/FFTS1 * [0:FFTS1/2-1]);
tw_facs_real = real(tw_facs);
tw_facs_imag = imag(tw_facs);

ls_facs_c0 = (tw_facs_real(1:FFTS1/4)-1)./tw_facs_imag(1:FFTS1/4);
ls_facs_s0 = tw_facs_imag(1:FFTS1/4);

num_bfly = FFTS1P/2;
%num_ops_per_bfly = 1;
%num_shift_bfly2bfly = 1;
%num_shift_tw = num_bfly * 2;

idx_in_x0 = 0;
idx_in_x1 = 1;

Xn_fft3_real = zeros(FFTS1P + 1,1);
Xn_fft3_imag = zeros(FFTS1P + 1,1);

%% First stage of FFT butterflies.
%% This stage takes into account:
%% - the bit reverse addressing mode of the input data, 
%% - transforms the real input into a complex FFT entry
%% - exploits that the butterfly twiddle factor is 1
for(idx_bfly = 0:FFTS1/4 - 1)
        
    idxRead0 = bitreverse(idx_in_x0, NFFT);
    idxRead1 = bitreverse(idx_in_x1, NFFT);
    
    x0_real = xk(idxRead0 + 1);
    x0_imag = xk(idxRead0 + 1 + 1);
    x1_real = xk(idxRead1 + 1);
    x1_imag = xk(idxRead1 + 1 + 1);
        
    x0_real_p = x0_real + x1_real; % Tw real is 1.0, Tw imag is 0
    x0_imag_p = x0_imag + x1_imag; % Tw real is 1.0, Tw imag is 0
    
    x1_real_p = x0_real - x1_real; % Tw real is 1.0, Tw imag is 0
    x1_imag_p = x0_imag - x1_imag; % Tw real is 1.0, Tw imag is 0
    
    inout_re(idx_in_x0+1) = x0_real_p;
    inout_im(idx_in_x0+1) = x0_imag_p;
    inout_re(idx_in_x1+1) = x1_real_p;
    inout_im(idx_in_x1+1) = x1_imag_p;
    
    idx_in_x0 = idx_in_x0 + 2;
    idx_in_x1 = idx_in_x1 + 2;

end

%% Second and following stages: FFT input buffers are all well prepared
%% Compute tw factor multiplications, butterfly processing
num_bfly = FFTS1/8;
num_shift_tw = FFTS1/4;
num_ops_per_bfly = 2;

% All other stages
for(idx_stage = 1:NFFTP-1)

    % Starting points for the butterfly
    idx_in_x0 = 0;
    idx_in_x1 = num_ops_per_bfly;

    % Butterfly Group Loop
    for(idx_bfly = 0:num_bfly - 1)
                
         x0_real = inout_re(idx_in_x0+1);
         x0_imag = inout_im(idx_in_x0+1);
         x1_real = inout_re(idx_in_x1+1);
         x1_imag = inout_im(idx_in_x1+1);
            
         % Compute complex multiplications, only realpart is 1.0 here
         x0_real_p = x0_real + x1_real;
         x0_imag_p = x0_imag + x1_imag;            
         x1_real_p = x0_real - x1_real;
         x1_imag_p = x0_imag - x1_imag;
            
         % Write results
         inout_re(idx_in_x0+1) = x0_real_p;
         inout_im(idx_in_x0+1) = x0_imag_p;
         inout_re(idx_in_x1+1) = x1_real_p;
         inout_im(idx_in_x1+1) = x1_imag_p;
            
         % Update indexing
         idx_in_tw = num_shift_tw;
         idx_in_x0 = idx_in_x0 + 1;
         idx_in_x1 = idx_in_x1 + 1;

         % Butterfly loop
         for(idx_inbfly=0:num_ops_per_bfly/2-2)
             
             % Read
             x0_real = inout_re(idx_in_x0+1);
             x0_imag = inout_im(idx_in_x0+1);
             x1_real = inout_re(idx_in_x1+1);
             x1_imag = inout_im(idx_in_x1+1);
             
             % Get twiddle factors - lifting scheme             
             tw_lift_cm1 = ls_facs_c0(idx_in_tw+1);
             tw_lift_s = ls_facs_s0(idx_in_tw+1);
             
             % Complex multiplications using lifting scheme
             x_real_pp = x1_real + tw_lift_cm1 * x1_imag;
             x_imag_pp = x1_imag + tw_lift_s * x_real_pp;
             x_real_pp = x_real_pp + tw_lift_cm1 * x_imag_pp;

             x0_real_p = x0_real + x_real_pp;
             x0_imag_p = x0_imag + x_imag_pp;
             x1_real_p = x0_real - x_real_pp;
             x1_imag_p = x0_imag - x_imag_pp;
             
             % Write results
             inout_re(idx_in_x0+1) = x0_real_p;
             inout_im(idx_in_x0+1) = x0_imag_p;
             inout_re(idx_in_x1+1) = x1_real_p;
             inout_im(idx_in_x1+1) = x1_imag_p;
             
             % Update indexing
             idx_in_tw = idx_in_tw + num_shift_tw;
             idx_in_x0 = idx_in_x0 + 1;
             idx_in_x1 = idx_in_x1 + 1;
         end
        
        x0_real = inout_re(idx_in_x0+1);
         x0_imag = inout_im(idx_in_x0+1);
         x1_real = inout_re(idx_in_x1+1);
         x1_imag = inout_im(idx_in_x1+1);
                  
         % Compute complex multiplications
         x0_real_p = x0_real + x1_imag ;
         x0_imag_p = x0_imag - x1_real ;
         x1_real_p = x0_real - x1_imag;
         x1_imag_p = x0_imag + x1_real;
         
         % Write results
         inout_re(idx_in_x0+1) = x0_real_p;
         inout_im(idx_in_x0+1) = x0_imag_p;
         inout_re(idx_in_x1+1) = x1_real_p;
         inout_im(idx_in_x1+1) = x1_imag_p;
         
         % Update indexing
         idx_in_tw = FFTS1/4 - num_shift_tw;
         idx_in_x0 = idx_in_x0 + 1;
         idx_in_x1 = idx_in_x1 + 1;
         
         for(idx_inbfly=0:num_ops_per_bfly/2-2)
             
             % Read
             x0_real = inout_re(idx_in_x0+1);
             x0_imag = inout_im(idx_in_x0+1);
             x1_real = inout_re(idx_in_x1+1);
             x1_imag = inout_im(idx_in_x1+1);
                          
             % Get twiddle factors - lifting scheme     
             % We can save some memory here by reusing the tw facs from
             % first half
             tw_lift_cm1 = ls_facs_c0(idx_in_tw+1);
             tw_lift_s = ls_facs_s0(idx_in_tw+1);
             
             % Complex multiplications using lifting scheme
             x_real_pp = x1_real - tw_lift_cm1 * x1_imag;
             x_imag_pp = x1_imag - tw_lift_s * x_real_pp;
             x_real_pp = x_real_pp - tw_lift_cm1 * x_imag_pp;

             x0_real_p = x0_real - x_real_pp;
             x0_imag_p = x0_imag - x_imag_pp;
             x1_real_p = x0_real + x_real_pp;
             x1_imag_p = x0_imag + x_imag_pp;
             
             % Write results
             inout_re(idx_in_x0+1) = x0_real_p;
             inout_im(idx_in_x0+1) = x0_imag_p;
             inout_re(idx_in_x1+1) = x1_real_p;
             inout_im(idx_in_x1+1) = x1_imag_p;
             
             % Update indexing
             idx_in_tw = idx_in_tw - num_shift_tw;
             idx_in_x0 = idx_in_x0 + 1;
             idx_in_x1 = idx_in_x1 + 1;
         end
         
        % Update indexing: Shift to next butterfly groups
        idx_in_x0 = idx_in_x0 + num_ops_per_bfly;
        idx_in_x1 = idx_in_x1 + num_ops_per_bfly;
        
    end
    
    % Update from stage to stage
    num_bfly = num_bfly/2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly * 2;    
end

% This is in fact the last stage of the N FFT butterfly BUT only the
% upper half

% Optimization from version 5 to 6:
% The problem I had: In case of in-place operation, the two loops at the
% end of version 5 did not really work, hence, I started to bring those two
% together by index transformation. And then, I found that due to symmetrie
% constraints and similar little observations, I can handle all this in a
% straight forward loop with low complexity.

% Combine result from complex FFT to determine FFT for real part
tt1_real = inout_re(1);

% Combine result from complex FFT to determine FFT for imag part
tt2_real = inout_im(1);

% Write to output buffer
Xn_fft3_real(1) = tt1_real + tt2_real;
Xn_fft3_imag(1) = 0;

Xn_fft3_real(FFTS1P+1) = tt1_real - tt2_real;
Xn_fft3_imag(FFTS1P+1) = 0;

% Output stage: From complex N/2 FFT to N FFT
for(idx = 1:FFTS1P/2-1)
    
    % Get twiddle factors - lifting scheme             
    tw_lift_cm1 = ls_facs_c0(idx+1);
    tw_lift_s = ls_facs_s0(idx+1);          
    
    % Combine result from complex FFT to determine FFT for real part
    tt1_real = inout_re(idx+1) + inout_re(FFTS1P-idx+1);
    tt1_real = tt1_real/2;
    tt1_imag = inout_im(idx+1) - inout_im(FFTS1P-idx+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = inout_im(idx+1) + inout_im(FFTS1P-idx+1);
    tt2_real = tt2_real/2;
    tt2_imag = -inout_re(idx+1) + inout_re(FFTS1P-idx+1);
    tt2_imag = tt2_imag/2;
    
    % Complex multiplications using lifting scheme
    x_real_pp = tt2_real + tw_lift_cm1 * tt2_imag;
    x_imag_pp = tt2_imag + tw_lift_s * x_real_pp;
    x_real_pp = x_real_pp + tw_lift_cm1 * x_imag_pp;
    
    tt_real_l = tt1_real + x_real_pp;
    tt_imag_l = tt1_imag + x_imag_pp;
    tt_real_h = tt1_real - x_real_pp;
    tt_imag_h = x_imag_pp - tt1_imag;
             
    % Write to output buffer
    Xn_fft3_real(idx+1) = tt_real_l;
    Xn_fft3_imag(idx+1) = tt_imag_l;
        
    % Write to output buffer
    Xn_fft3_real(FFTS1P - idx +1) = tt_real_h;
    Xn_fft3_imag(FFTS1P - idx +1) = tt_imag_h;
end

% This is in fact the last stage of the N FFT butterfly BUT only the
% upper half

% Combine result from complex FFT to determine FFT for real part
tt1_real = inout_re(FFTS1P/2+1);
tt2_real = -inout_im(FFTS1P/2+1);

% Final twiddle factor
tt_real = tt1_real;
tt_imag = tt2_real;

% Write to output buffer
Xn_fft3_real(FFTS1P/2+1) = tt_real;
Xn_fft3_imag(FFTS1P/2+1) = tt_imag;

% Combine to form output fft
Xn_fft3 = Xn_fft3_real + i * Xn_fft3_imag;

Xn_fft3c = [Xn_fft3 ; conj(Xn_fft3(FFTS1P:-1:2))];
diff = Xn_fft3c - Xn_fft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);

%% =======================================================================
disp('%% FFT variant No 7: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Sorting of input into real and cpomplex and bit reversal addr in two steps');
disp('%% - lifting scheme');
disp('%% - in-place reconstruction');
disp('%% - load twiddle factor only once and use within butterfly groups');
%% =======================================================================
inout_re = zeros(FFTS1/2, 1);
inout_im = zeros(FFTS1/2, 1);

FFTS1P = FFTS1/2;
NFFTP = NFFT-1;


tw_facs = exp(-i*2*pi/FFTS1 * [0:FFTS1/2-1]);
tw_facs_real = real(tw_facs);
tw_facs_imag = imag(tw_facs);

ls_facs_c0 = (tw_facs_real(1:FFTS1/4)-1)./tw_facs_imag(1:FFTS1/4);
ls_facs_s0 = tw_facs_imag(1:FFTS1/4);

ls_facs_c1 = (tw_facs_real(FFTS1/4+1:FFTS1/2)+1)./tw_facs_imag(FFTS1/4+1:FFTS1/2);
ls_facs_s1 = tw_facs_imag(FFTS1/4+1:FFTS1/2);

Xn_fft3_real = zeros(FFTS1P + 1,1);
Xn_fft3_imag = zeros(FFTS1P + 1,1);

%% First stage of FFT butterflies.
%% This stage takes into account:
%% - the bit reverse addressing mode of the input data, 
%% - transforms the real input into a complex FFT entry
%% - exploits that the butterfly twiddle factor is 1
idx_in_x0 = 0;
idx_in_x1 = 1;
for(idx_bfly = 0:FFTS1/4 - 1)
        
    idxRead0 = bitreverse(idx_in_x0, NFFT);
    idxRead1 = bitreverse(idx_in_x1, NFFT);
    
    x0_real = xk(idxRead0 + 1);
    x0_imag = xk(idxRead0 + 1 + 1);
    x1_real = xk(idxRead1 + 1);
    x1_imag = xk(idxRead1 + 1 + 1);
        
    x0_real_p = x0_real + x1_real; % Tw real is 1.0, Tw imag is 0
    x0_imag_p = x0_imag + x1_imag; % Tw real is 1.0, Tw imag is 0
    
    x1_real_p = x0_real - x1_real; % Tw real is 1.0, Tw imag is 0
    x1_imag_p = x0_imag - x1_imag; % Tw real is 1.0, Tw imag is 0
    
    inout_re(idx_in_x0+1) = x0_real_p;
    inout_im(idx_in_x0+1) = x0_imag_p;
    inout_re(idx_in_x1+1) = x1_real_p;
    inout_im(idx_in_x1+1) = x1_imag_p;
    
    idx_in_x0 = idx_in_x0 + 2;
    idx_in_x1 = idx_in_x1 + 2;

end

%% Second and following stages: FFT input buffers are all well prepared
%% Compute tw factor multiplications, butterfly processing
num_bfly = FFTS1/8;
num_shift_tw = FFTS1/4;
num_ops_per_bfly = 2;
num_shifts_per_bfly = 2 * num_ops_per_bfly;

% All other stages
for(idx_stage = 1:NFFTP-1)
    
    idx_offset = 0;

    % Starting points for the butterfly
    idx_in_x0 = idx_offset;
    idx_in_x1 = idx_offset + num_ops_per_bfly;
    idx_in_tw = 0;
    
    % Butterfly Group Loop for weights equal to 1
    for(idx_bfly = 0:num_bfly - 1)
        
        x0_real = inout_re(idx_in_x0+1);
        x0_imag = inout_im(idx_in_x0+1);
        x1_real = inout_re(idx_in_x1+1);
        x1_imag = inout_im(idx_in_x1+1);
        
        % Compute complex multiplications, only realpart is 1.0 here
        x0_real_p = x0_real + x1_real;
        x0_imag_p = x0_imag + x1_imag;
        x1_real_p = x0_real - x1_real;
        x1_imag_p = x0_imag - x1_imag;
        
        % Write results
        inout_re(idx_in_x0+1) = x0_real_p;
        inout_im(idx_in_x0+1) = x0_imag_p;
        inout_re(idx_in_x1+1) = x1_real_p;
        inout_im(idx_in_x1+1) = x1_imag_p;
        
        % Update indexing
        idx_in_x0 = idx_in_x0 + num_shifts_per_bfly;
        idx_in_x1 = idx_in_x1 + num_shifts_per_bfly;
    end
    
    idx_offset = idx_offset + 1;
    
    % Butterfly loop
    for(idx_inbfly=0:num_ops_per_bfly/2-2)
        
        idx_in_x0 = idx_offset;
        idx_in_x1 = idx_offset + num_ops_per_bfly;
        idx_in_tw = idx_in_tw + num_shift_tw;
        
        % Get twiddle factors - lifting scheme
        tw_lift_cm1 = ls_facs_c0(idx_in_tw+1);
        tw_lift_s = ls_facs_s0(idx_in_tw+1);
        
        for(idx_bfly = 0:num_bfly - 1)
            
            % Read
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            x1_real = inout_re(idx_in_x1+1);
            x1_imag = inout_im(idx_in_x1+1);
            
            % Complex multiplications using lifting scheme
            x_real_pp = x1_real + tw_lift_cm1 * x1_imag;
            x_imag_pp = x1_imag + tw_lift_s * x_real_pp;
            x_real_pp = x_real_pp + tw_lift_cm1 * x_imag_pp;
            
            x0_real_p = x0_real + x_real_pp;
            x0_imag_p = x0_imag + x_imag_pp;
            x1_real_p = x0_real - x_real_pp;
            x1_imag_p = x0_imag - x_imag_pp;
            
            % Write results
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            inout_re(idx_in_x1+1) = x1_real_p;
            inout_im(idx_in_x1+1) = x1_imag_p;
            
            % Update indexing
            idx_in_x0 = idx_in_x0 + num_shifts_per_bfly;
            idx_in_x1 = idx_in_x1 + num_shifts_per_bfly;
        end
        idx_offset = idx_offset + 1;
    end
    
    % Starting points for the butterfly
    idx_in_x0 = idx_offset;
    idx_in_x1 = idx_offset + num_ops_per_bfly;
    idx_in_tw = 0;
    
    % Butterfly Group Loop for weights equal to 1
    for(idx_bfly = 0:num_bfly - 1)
        
        x0_real = inout_re(idx_in_x0+1);
        x0_imag = inout_im(idx_in_x0+1);
        x1_real = inout_re(idx_in_x1+1);
        x1_imag = inout_im(idx_in_x1+1);
        
        % Compute complex multiplications
        x0_real_p = x0_real + x1_imag ;
        x0_imag_p = x0_imag - x1_real ;
        x1_real_p = x0_real - x1_imag;
        x1_imag_p = x0_imag + x1_real;
        
        % Write results
        inout_re(idx_in_x0+1) = x0_real_p;
        inout_im(idx_in_x0+1) = x0_imag_p;
        inout_re(idx_in_x1+1) = x1_real_p;
        inout_im(idx_in_x1+1) = x1_imag_p;
        
        % Update indexing
        idx_in_x0 = idx_in_x0 + num_shifts_per_bfly;
        idx_in_x1 = idx_in_x1 + num_shifts_per_bfly;
    end
    
    idx_offset = idx_offset + 1;
    
    for(idx_inbfly=0:num_ops_per_bfly/2-2)
        
        idx_in_x0 = idx_offset;
        idx_in_x1 = idx_offset + num_ops_per_bfly;
        idx_in_tw = idx_in_tw + num_shift_tw;
        
        % Get twiddle factors - lifting scheme
        tw_lift_cm1 = ls_facs_c0(FFTS1/4-idx_in_tw+1);
        tw_lift_s = ls_facs_s0(FFTS1/4-idx_in_tw+1);
        
        for(idx_bfly = 0:num_bfly - 1)
            % Read
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            x1_real = inout_re(idx_in_x1+1);
            x1_imag = inout_im(idx_in_x1+1);
            
            % Complex multiplications using lifting scheme
            x_real_pp = x1_real - tw_lift_cm1 * x1_imag; % + -> - due toreuse of ls_facs_c0
            x_imag_pp = x1_imag - tw_lift_s * x_real_pp;
            x_real_pp = x_real_pp - tw_lift_cm1 * x_imag_pp; % + -> - due toreuse of ls_facs_c0
            
            x0_real_p = x0_real - x_real_pp;
            x0_imag_p = x0_imag - x_imag_pp;
            x1_real_p = x0_real + x_real_pp;
            x1_imag_p = x0_imag + x_imag_pp;
            
            % Write results
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            inout_re(idx_in_x1+1) = x1_real_p;
            inout_im(idx_in_x1+1) = x1_imag_p;
            
            % Update indexing
            idx_in_x0 = idx_in_x0 + num_shifts_per_bfly;
            idx_in_x1 = idx_in_x1 + num_shifts_per_bfly;
        end
        
        idx_offset = idx_offset + 1;        
    end
    
    % Update from stage to stage
    num_bfly = num_bfly/2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly * 2;
    num_shifts_per_bfly = num_shifts_per_bfly * 2;
end

% This is in fact the last stage of the N FFT butterfly BUT only the
% upper half

% Optimization from version 5 to 6:
% The problem I had: In case of in-place operation, the two loops at the
% end of version 5 did not really work, hence, I started to bring those two
% together by index transformation. And then, I found that due to symmetrie
% constraints and similar little observations, I can handle all this in a
% straight forward loop with low complexity.

% Combine result from complex FFT to determine FFT for real part
tt1_real = inout_re(1);

% Combine result from complex FFT to determine FFT for imag part
tt2_real = inout_im(1);

% Write to output buffer
Xn_fft3_real(1) = tt1_real + tt2_real;
Xn_fft3_imag(1) = 0;

Xn_fft3_real(FFTS1P+1) = tt1_real - tt2_real;
Xn_fft3_imag(FFTS1P+1) = 0;

% Output stage: From complex N/2 FFT to N FFT
for(idx = 1:FFTS1P/2-1)
    
    % Get twiddle factors - lifting scheme
    tw_lift_cm1 = ls_facs_c0(idx+1);
    tw_lift_s = ls_facs_s0(idx+1);
    
    % Combine result from complex FFT to determine FFT for real part
    tt1_real = inout_re(idx+1) + inout_re(FFTS1P-idx+1);
    tt1_real = tt1_real/2;
    tt1_imag = inout_im(idx+1) - inout_im(FFTS1P-idx+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = inout_im(idx+1) + inout_im(FFTS1P-idx+1);
    tt2_real = tt2_real/2;
    tt2_imag = -inout_re(idx+1) + inout_re(FFTS1P-idx+1);
    tt2_imag = tt2_imag/2;
    
    % Complex multiplications using lifting scheme
    x_real_pp = tt2_real + tw_lift_cm1 * tt2_imag;
    x_imag_pp = tt2_imag + tw_lift_s * x_real_pp;
    x_real_pp = x_real_pp + tw_lift_cm1 * x_imag_pp;
    
    tt_real_l = tt1_real + x_real_pp;
    tt_imag_l = tt1_imag + x_imag_pp;
    tt_real_h = tt1_real - x_real_pp;
    tt_imag_h = x_imag_pp - tt1_imag;
    
    % Write to output buffer
    Xn_fft3_real(idx+1) = tt_real_l;
    Xn_fft3_imag(idx+1) = tt_imag_l;
    
    % Write to output buffer
    Xn_fft3_real(FFTS1P - idx +1) = tt_real_h;
    Xn_fft3_imag(FFTS1P - idx +1) = tt_imag_h;
end

% This is in fact the last stage of the N FFT butterfly BUT only the
% upper half

% Combine result from complex FFT to determine FFT for real part
tt1_real = inout_re(FFTS1P/2+1);
tt2_real = -inout_im(FFTS1P/2+1);

% Final twiddle factor
tt_real = tt1_real;
tt_imag = tt2_real;

% Write to output buffer
Xn_fft3_real(FFTS1P/2+1) = tt_real;
Xn_fft3_imag(FFTS1P/2+1) = tt_imag;

% Combine to form output fft
Xn_fft3 = Xn_fft3_real + i * Xn_fft3_imag;

Xn_fft3c = [Xn_fft3 ; conj(Xn_fft3(FFTS1P:-1:2))];
diff = Xn_fft3c - Xn_fft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);





%% =======================================================================
disp('%% FFT variant No 8: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Sorting of input into real and complex and bit reversal addr in one step');
disp('%% - Original straight forward implementation with conventional complex multiplication');
%% =======================================================================
inout_re = zeros(FFTS1/2, 1);
inout_im = zeros(FFTS1/2, 1);

FFTS1P = FFTS1/2;
NFFTP = NFFT-1;


tw_facs = exp(-i*2*pi/FFTS1 * [0:FFTS1/2-1]);
tw_facs_real = real(tw_facs);
tw_facs_imag = imag(tw_facs);

idx_in_x0 = 0;
idx_in_x1 = 1;

Xn_fft3_real = zeros(FFTS1P + 1,1);
Xn_fft3_imag = zeros(FFTS1P + 1,1);

%% First stage of FFT butterflies.
%% This stage takes into account:
%% - the bit reverse addressing mode of the input data, 
%% - transforms the real input into a complex FFT entry
%% - exploits that the butterfly twiddle factor is 1
for(idx_bfly = 0:FFTS1/4 - 1)
        
    idxRead0 = bitreverse(idx_in_x0, NFFT);
    idxRead1 = bitreverse(idx_in_x1, NFFT);
    
    x0_real = xk(idxRead0 + 1);
    x0_imag = xk(idxRead0 + 1 + 1);
    x1_real = xk(idxRead1 + 1);
    x1_imag = xk(idxRead1 + 1 + 1);
        
    x0_real_p = x0_real + x1_real; % Tw real is 1.0, Tw imag is 0
    x0_imag_p = x0_imag + x1_imag; % Tw real is 1.0, Tw imag is 0
    
    x1_real_p = x0_real - x1_real; % Tw real is 1.0, Tw imag is 0
    x1_imag_p = x0_imag - x1_imag; % Tw real is 1.0, Tw imag is 0
    
    inout_re(idx_in_x0+1) = x0_real_p;
    inout_im(idx_in_x0+1) = x0_imag_p;
    inout_re(idx_in_x1+1) = x1_real_p;
    inout_im(idx_in_x1+1) = x1_imag_p;
    
    idx_in_x0 = idx_in_x0 + 2;
    idx_in_x1 = idx_in_x1 + 2;

end





num_bfly = 2;
num_shift_tw = FFTS1/4;
num_ops_per_bfly = FFTS1/8;

% All other stages
for(idx_stage = 1:NFFTP-1)

    % Start tw factors from 0-index
    idx_in_tw = 0;

    % Butterfly Group Loop
    for(idx_bfly = 0:num_bfly - 1)
        
        idx_in_x0 = idx_bfly;
        
        % Get twiddle factors
        tw_fac_real = tw_facs_real(idx_in_tw+1);
        tw_fac_imag = tw_facs_imag(idx_in_tw+1);

        % Butterfly loop
        for(idx_inbfly=0:num_ops_per_bfly-1)
            
            % Read
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + num_bfly;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 - num_bfly;
            
            % Compute complex multiplications
            out_real = tw_fac_real * x1_real - tw_fac_imag * x1_imag ;
            out_imag = tw_fac_real * x1_imag + tw_fac_imag * x1_real ;
            
            x0_real_p = x0_real + out_real;
            x0_imag_p = x0_imag + out_imag;            
            x1_real_p = x0_real - out_real;
            x1_imag_p = x0_imag - out_imag;
            
            % Write results
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + num_bfly;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + num_bfly;
            
        end
        
        % Update indexing
        idx_in_tw = idx_in_tw + num_shift_tw;
        
    end
    
    % Update from stage to stage
    num_bfly = num_bfly*2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly / 2;    
end


% Output stage: From complex N/2 FFT to N FFT
for(idx = 1:FFTS1P-1)
    
    % This is in fact the last stage of the N FFT butterfly BUT only the
    % upper half
    tw_fac_real = tw_facs_real(idx+1);
    tw_fac_imag = tw_facs_imag(idx+1);

    % Combine result from complex FFT to determine FFT for real part
    tt1_real = inout_re(idx+1) + inout_re(FFTS1P-idx+1);
    tt1_real = tt1_real/2;
    tt1_imag = inout_im(idx+1) - inout_im(FFTS1P-idx+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = inout_im(idx+1) + inout_im(FFTS1P-idx+1);
    tt2_real = tt2_real/2;
    tt2_imag = -inout_re(idx+1) + inout_re(FFTS1P-idx+1);
    tt2_imag = tt2_imag/2;
    
    % Final twiddle factor
    tt_real = tt1_real + tw_fac_real * tt2_real - tw_fac_imag * tt2_imag;
    tt_imag = tt1_imag + tw_fac_real * tt2_imag + tw_fac_imag * tt2_real;

    % Write to output buffer
    Xn_fft3_real(idx+1) = tt_real;
    Xn_fft3_imag(idx+1) = tt_imag;
end

% Special case for first and last element in FFT buffer (real valued)
tt1 =  inout_re(1);
tt2 =  inout_im(1);

Xn_fft3_real(1) = tt1 + tt2;
Xn_fft3_imag(1) = 0;
Xn_fft3_real(FFTS1P+1) = tt1 - tt2;
Xn_fft3_imag(FFTS1P+1) = 0;

% Combine to form output fft
Xn_fft3 = Xn_fft3_real + i * Xn_fft3_imag;

Xn_fft3c = [Xn_fft3 ; conj(Xn_fft3(FFTS1P:-1:2))];
diff = Xn_fft3c - Xn_fft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);





%% =======================================================================
disp('%% FFT variant No 9: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Sorting of input into real and complex and bit reversal addr in one step');
disp('%% - Come back to original algorithm with complex multiplication');
disp('%% - Reduced the number of coefficients to be stored as twiddle factors to LFFT/4+1');

%% =======================================================================
inout_re = zeros(FFTS1/2, 1);
inout_im = zeros(FFTS1/2, 1);

FFTS1P = FFTS1/2;
NFFTP = NFFT-1;


tw_facs = exp(-i*2*pi/FFTS1 * [0:FFTS1/2-1]);
tw_facs_real = real(tw_facs);
%tw_facs_imag = imag(tw_facs);

idx_in_x0 = 0;
idx_in_x1 = 1;

Xn_fft3_real = zeros(FFTS1P + 1,1);
Xn_fft3_imag = zeros(FFTS1P + 1,1);

%% First stage of FFT butterflies.
%% This stage takes into account:
%% - the bit reverse addressing mode of the input data, 
%% - transforms the real input into a complex FFT entry
%% - exploits that the butterfly twiddle factor is 1
for(idx_bfly = 0:FFTS1/4 - 1)
        
    idxRead0 = bitreverse(idx_in_x0, NFFT);
    idxRead1 = bitreverse(idx_in_x1, NFFT);
    
    x0_real = xk(idxRead0 + 1);
    x0_imag = xk(idxRead0 + 1 + 1);
    x1_real = xk(idxRead1 + 1);
    x1_imag = xk(idxRead1 + 1 + 1);
        
    x0_real_p = x0_real + x1_real; % Tw real is 1.0, Tw imag is 0
    x0_imag_p = x0_imag + x1_imag; % Tw real is 1.0, Tw imag is 0
    
    x1_real_p = x0_real - x1_real; % Tw real is 1.0, Tw imag is 0
    x1_imag_p = x0_imag - x1_imag; % Tw real is 1.0, Tw imag is 0
    
    inout_re(idx_in_x0+1) = x0_real_p;
    inout_im(idx_in_x0+1) = x0_imag_p;
    inout_re(idx_in_x1+1) = x1_real_p;
    inout_im(idx_in_x1+1) = x1_imag_p;
    
    idx_in_x0 = idx_in_x0 + 2;
    idx_in_x1 = idx_in_x1 + 2;

end





num_bfly = 1;
num_bfly_shift = 2;
num_shift_tw = FFTS1/4;
num_ops_per_bfly = FFTS1/8;

% All other stages
for(idx_stage = 1:NFFTP-1)

    % Start tw factors from 0-index
    idx_in_tw = 0;

    idx_in_x0_start = 0;
    
    % Butterfly Group Loop
    for(idx_bfly = 0:num_bfly - 1)
        
        idx_in_x0 = idx_in_x0_start;
        
        % Get twiddle factors
        tw_fac_real = tw_facs_real(idx_in_tw+1);
        tw_fac_imag = tw_facs_real((FFTS1/4-idx_in_tw)+1);
        
        % Butterfly loop
        for(idx_inbfly=0:num_ops_per_bfly-1)
            
            % Read
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + num_bfly_shift;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 - num_bfly_shift;
            
            % Compute complex multiplications
            out_real = tw_fac_real * x1_real + tw_fac_imag * x1_imag ;
            out_imag = tw_fac_real * x1_imag - tw_fac_imag * x1_real ;
            
            x0_real_p = x0_real + out_real;
            x0_imag_p = x0_imag + out_imag;            
            x1_real_p = x0_real - out_real;
            x1_imag_p = x0_imag - out_imag;
            
            % Write results
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + num_bfly_shift;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + num_bfly_shift;
            
        end
        
        % Update indexing
        idx_in_tw = idx_in_tw + num_shift_tw;
        idx_in_x0_start = idx_in_x0_start + 1;
        
    end
    
    % Butterfly Group Loop
    for(idx_bfly = 0:num_bfly - 1)
         
        idx_in_x0 = idx_in_x0_start;
        
        % Get twiddle factors
        
        %tw_fac_real = -tw_facs_real((idx_in_tw)+1);
        tw_fac_real = tw_facs_real((idx_in_tw)+1);
        %tw_fac_imag = tw_facs_imag((idx_in_tw)+1);
        tw_fac_imag = tw_facs_real((FFTS1/4-idx_in_tw)+1);

        % Butterfly loop
        for(idx_inbfly=0:num_ops_per_bfly-1)
            
            % Read
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + num_bfly_shift;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 - num_bfly_shift;
            
            % Compute complex multiplications
            out_real = tw_fac_real * x1_real - tw_fac_imag * x1_imag ;
            out_imag = tw_fac_real * x1_imag + tw_fac_imag * x1_real ;
            
            x0_real_p = x0_real - out_real;
            x0_imag_p = x0_imag - out_imag;            
            x1_real_p = x0_real + out_real;
            x1_imag_p = x0_imag + out_imag;
            
            % Write results
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + num_bfly_shift;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + num_bfly_shift;
            
        end
        
        % Update indexing
        idx_in_tw = idx_in_tw - num_shift_tw;
        idx_in_x0_start = idx_in_x0_start + 1;
    end
    
    % Update from stage to stage
    num_bfly = num_bfly*2;
    num_bfly_shift = num_bfly_shift * 2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly / 2;    
end

% Output stage: From complex N/2 FFT to N FFT
for(idx = 1:FFTS1P/2)
    
    % This is in fact the last stage of the N FFT butterfly BUT only the
    % upper half
    tw_fac_real = tw_facs_real(idx+1);
    %tw_fac_imag = tw_facs_imag(idx+1);
    tw_fac_imag = tw_facs_real((FFTS1P/2 - idx)+1);

    % Combine result from complex FFT to determine FFT for real part
    tt1_real = inout_re(idx+1) + inout_re(FFTS1P-idx+1);
    tt1_real = tt1_real/2;
    tt1_imag = inout_im(idx+1) - inout_im(FFTS1P-idx+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = inout_im(idx+1) + inout_im(FFTS1P-idx+1);
    tt2_real = tt2_real/2;
    tt2_imag = -inout_re(idx+1) + inout_re(FFTS1P-idx+1);
    tt2_imag = tt2_imag/2;
    
    out_real = tw_fac_real * tt2_real + tw_fac_imag * tt2_imag;
    out_imag = tw_fac_real * tt2_imag - tw_fac_imag * tt2_real;
    
    % Final twiddle factor
    tt_real = tt1_real + out_real;
    tt_imag = tt1_imag + out_imag;

    % Write to output buffer
    Xn_fft3_real(idx+1) = tt_real;
    Xn_fft3_imag(idx+1) = tt_imag;
end

for(idx = FFTS1P/2+1:FFTS1P-1)
    
    % This is in fact the last stage of the N FFT butterfly BUT only the
    % upper half
    tw_fac_real = tw_facs_real((FFTS1P - idx)+1);
    %tw_fac_imag = tw_facs_imag(idx+1);
    tw_fac_imag = tw_facs_real((idx-FFTS1P/2)+1);


    % Combine result from complex FFT to determine FFT for real part
    tt1_real = inout_re(idx+1) + inout_re(FFTS1P-idx+1);
    tt1_real = tt1_real/2;
    tt1_imag = inout_im(idx+1) - inout_im(FFTS1P-idx+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = inout_im(idx+1) + inout_im(FFTS1P-idx+1);
    tt2_real = tt2_real/2;
    tt2_imag = -inout_re(idx+1) + inout_re(FFTS1P-idx+1);
    tt2_imag = tt2_imag/2;
    
    out_real = tw_fac_real * tt2_real - tw_fac_imag * tt2_imag;
    out_imag = tw_fac_real * tt2_imag + tw_fac_imag * tt2_real;
    
    % Final twiddle factor
    tt_real = tt1_real - out_real;
    tt_imag = tt1_imag - out_imag;

    % Write to output buffer
    Xn_fft3_real(idx+1) = tt_real;
    Xn_fft3_imag(idx+1) = tt_imag;
end

% Special case for first and last element in FFT buffer (real valued)
tt1 =  inout_re(1);
tt2 =  inout_im(1);

Xn_fft3_real(1) = tt1 + tt2;
Xn_fft3_imag(1) = 0;
Xn_fft3_real(FFTS1P+1) = tt1 - tt2;
Xn_fft3_imag(FFTS1P+1) = 0;

% Combine to form output fft
Xn_fft3 = Xn_fft3_real + i * Xn_fft3_imag;

Xn_fft3c = [Xn_fft3 ; conj(Xn_fft3(FFTS1P:-1:2))];
diff = Xn_fft3c - Xn_fft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);

    
%% =======================================================================
disp('%% FFT variant No 10: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Sorting of input into real and complex and bit reversal addr in one step');
disp('%% - Come back to original algorithm with complex multiplication');
disp('%% - Reduced the number of coefficients to be stored as twiddle factors to LFFT/4+1');
disp('%% - Converged pairs of loops - hope that this will be better for pipeline and requires only one load of twiddle factors');
%% =======================================================================

inout_re = zeros(FFTS1/2, 1);
inout_im = zeros(FFTS1/2, 1);

FFTS1P = FFTS1/2;
NFFTP = NFFT-1;


tw_facs = exp(-i*2*pi/FFTS1 * [0:FFTS1/2-1]);
tw_facs_real = real(tw_facs);
%tw_facs_imag = imag(tw_facs);

idx_in_x0 = 0;
idx_in_x1 = 1;

Xn_fft3_real = zeros(FFTS1P + 1,1);
Xn_fft3_imag = zeros(FFTS1P + 1,1);

%% First stage of FFT butterflies.
%% This stage takes into account:
%% - the bit reverse addressing mode of the input data,
%% - transforms the real input into a complex FFT entry
%% - exploits that the butterfly twiddle factor is 1
for(idx_bfly = 0:FFTS1/4 - 1)
    
    idxRead0 = bitreverse(idx_in_x0, NFFT);
    idxRead1 = bitreverse(idx_in_x1, NFFT);
    
    x0_real = xk(idxRead0 + 1);
    x0_imag = xk(idxRead0 + 1 + 1);
    x1_real = xk(idxRead1 + 1);
    x1_imag = xk(idxRead1 + 1 + 1);
    
    x0_real_p = x0_real + x1_real; % Tw real is 1.0, Tw imag is 0
    x0_imag_p = x0_imag + x1_imag; % Tw real is 1.0, Tw imag is 0
    
    x1_real_p = x0_real - x1_real; % Tw real is 1.0, Tw imag is 0
    x1_imag_p = x0_imag - x1_imag; % Tw real is 1.0, Tw imag is 0
    
    inout_re(idx_in_x0+1) = x0_real_p;
    inout_im(idx_in_x0+1) = x0_imag_p;
    inout_re(idx_in_x1+1) = x1_real_p;
    inout_im(idx_in_x1+1) = x1_imag_p;
    
    idx_in_x0 = idx_in_x0 + 2;
    idx_in_x1 = idx_in_x1 + 2;
    
end

num_bfly = 1;
num_bfly_shift = 2;
num_shift_tw = FFTS1/4;
num_ops_per_bfly = FFTS1/8;

% All other stages
for(idx_stage = 1:NFFTP-1)
    
    % Start tw factors from 0-index
    idx_in_tw = 0;
    
    idx_in_x0_start = 0;

    inc1 = num_bfly_shift;
    inc2 = - num_bfly_shift;
    inc3 = - num_bfly;
    inc4 = + num_bfly;
    inc5 = num_shift_tw;
    
    % Butterfly Group Loop
    for(idx_bfly = 0:num_bfly - 1)
        
        idx_in_x0 = idx_in_x0_start;
        
        % Get twiddle factors
        tw_fac_real = tw_facs_real(idx_in_tw+1);
        tw_fac_imag = tw_facs_real((FFTS1/4-idx_in_tw)+1);
        
        % Butterfly loop
        for(idx_inbfly=0:num_ops_per_bfly-1)
            
            % #####################
            % First half of loop
            % #####################
            
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc2;
            
            % Compute complex multiplications
            out_real = tw_fac_real * x1_real;
            out_real = out_real + tw_fac_imag * x1_imag ;
            out_imag = tw_fac_real * x1_imag;
            out_imag = out_imag - tw_fac_imag * x1_real ;
            
            x0_real_p = x0_real + out_real;
            x0_imag_p = x0_imag + out_imag;
            x1_real_p = x0_real - out_real;
            x1_imag_p = x0_imag - out_imag;
            
            % Write results
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc1;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc3;
            
            % #####################
            % Second half of loop
            % #####################

            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc2;
            
            % Compute complex multiplications
            % Swap real and imaginary part due to everything in one loop
            out_real = tw_fac_imag * x1_real;
            out_real = out_real - tw_fac_real * x1_imag ;
            out_imag = tw_fac_imag * x1_imag;
            out_imag = out_imag + tw_fac_real * x1_real ;
            
            x0_real_p = x0_real - out_real;
            x0_imag_p = x0_imag - out_imag;
            x1_real_p = x0_real + out_real;
            x1_imag_p = x0_imag + out_imag;
            
            % Write results
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc1;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc4;
            
        end
        
        % Update indexing, maybe, we can arrange together with ringing of 
        % processing pipeline
        idx_in_tw = idx_in_tw + inc5;
        idx_in_x0_start = idx_in_x0_start + 1;
        
    end  
    
    % Update from stage to stage
    num_bfly = num_bfly*2;
    num_bfly_shift = num_bfly_shift * 2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly / 2;
end

idx_in_out_0 = 1;
idx_in_out_1 = FFTS1P - 1;

inc1 = FFTS1P/2;
inc2 = -FFTS1P/2 + 1;
inc3 = -FFTS1P/2;
inc4 = +FFTS1P/2 - 1;

% Output stage: From complex N/2 FFT to N FFT
for(idx = 1:FFTS1P/2-1)
    
    % This is in fact the last stage of the N FFT butterfly BUT only the
    % upper half
    tw_fac_real = tw_facs_real(idx+1);
    tw_fac_imag = tw_facs_real((FFTS1P/2 - idx)+1);
    
    % Combine result from complex FFT to determine FFT for real part
    tt1_real = inout_re(idx_in_out_0+1) + inout_re(idx_in_out_1+1);
    tt1_real = tt1_real/2;
    tt1_imag = inout_im(idx_in_out_0+1) - inout_im(idx_in_out_1+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = inout_im(idx_in_out_0+1) + inout_im(idx_in_out_1+1);
    tt2_real = tt2_real/2;
    tt2_imag = -inout_re(idx_in_out_0+1) + inout_re(idx_in_out_1+1);
    tt2_imag = tt2_imag/2;
    idx_in_out_1 = idx_in_out_1 + inc3;
    
    out_real = tw_fac_real * tt2_real;
    out_real = out_real + tw_fac_imag * tt2_imag;
    out_imag = tw_fac_real * tt2_imag;
    out_imag = out_imag - tw_fac_imag * tt2_real;
    
    % Final twiddle factor
    tt_real = tt1_real + out_real;
    tt_imag = tt1_imag + out_imag;
    
    % Write to output buffer
    Xn_fft3_real(idx_in_out_0+1) = tt_real;
    Xn_fft3_imag(idx_in_out_0+1) = tt_imag;
    
    idx_in_out_0 = idx_in_out_0 + inc1;
    
   
    % Combine result from complex FFT to determine FFT for real part
    tt1_real = inout_re(idx_in_out_0+1) + inout_re(idx_in_out_1+1);
    tt1_real = tt1_real/2;
    tt1_imag = inout_im(idx_in_out_0+1) - inout_im(idx_in_out_1+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = inout_im(idx_in_out_0+1) + inout_im(idx_in_out_1+1);
    tt2_real = tt2_real/2;
    tt2_imag = -inout_re(idx_in_out_0+1) + inout_re(idx_in_out_1+1);
    tt2_imag = tt2_imag/2;
    idx_in_out_1 = idx_in_out_1 + inc4;
    
    % Mix real and imag due to symmetry in load..
    out_real = tw_fac_imag * tt2_real;
    out_real = out_real - tw_fac_real * tt2_imag;
    out_imag = tw_fac_imag * tt2_imag;
    out_imag = out_imag + tw_fac_real * tt2_real;
    
    % Final twiddle factor
    tt_real = tt1_real - out_real;
    tt_imag = tt1_imag - out_imag;
    
    % Write to output buffer
    Xn_fft3_real(idx_in_out_0+1) = tt_real;
    Xn_fft3_imag(idx_in_out_0+1) = tt_imag;
    
    idx_in_out_0 = idx_in_out_0 + inc2;
end

% Special case for first and last element in FFT buffer (real valued)
tt1 =  inout_re(1);
tt2 =  inout_im(1);

Xn_fft3_real(1) = tt1 + tt2;
Xn_fft3_imag(1) = 0;

Xn_fft3_real(FFTS1P+1) = tt1 - tt2;
Xn_fft3_imag(FFTS1P+1) = 0;

% Combine result from complex FFT to determine FFT for real part
tt1_real = inout_re(FFTS1P/2+1);

% Combine result from complex FFT to determine FFT for imag part
tt2_real = inout_im(FFTS1P/2+1);

% Write to output buffer
Xn_fft3_real(FFTS1P/2+1) = tt1_real;
Xn_fft3_imag(FFTS1P/2+1) = -tt2_real;
    
% Combine to form output fft
Xn_fft3 = Xn_fft3_real + i * Xn_fft3_imag;

Xn_fft3c = [Xn_fft3 ; conj(Xn_fft3(FFTS1P:-1:2))];
diff = Xn_fft3c - Xn_fft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);

%% =======================================================================
disp('%% FFT variant No 11: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Sorting of input into real and complex and bit reversal addr in one step');
disp('%% - Come back to original algorithm with complex multiplication');
disp('%% - Reduced the number of coefficients to be stored as twiddle factors to LFFT/4+1');
disp('%% - Converged pairs of loops - hope that this will be better for pipeline and requires only one load of twiddle factors');
disp('%% - Split up first main loop into halfes for better M register usage');
%% =======================================================================

inout_re = zeros(FFTS1/2, 1);
inout_im = zeros(FFTS1/2, 1);

FFTS1P = FFTS1/2;
NFFTP = NFFT-1;


tw_facs = exp(-i*2*pi/FFTS1 * [0:FFTS1/2-1]);
tw_facs_real = real(tw_facs);
%tw_facs_imag = imag(tw_facs);

idx_in_x0 = 0;
idx_in_x1 = 1;

Xn_fft3_real = zeros(FFTS1P + 1,1);
Xn_fft3_imag = zeros(FFTS1P + 1,1);

%% First stage of FFT butterflies.
%% This stage takes into account:
%% - the bit reverse addressing mode of the input data,
%% - transforms the real input into a complex FFT entry
%% - exploits that the butterfly twiddle factor is 1
for(idx_bfly = 0:FFTS1/4 - 1)
    
    idxRead0 = bitreverse(idx_in_x0, NFFT);
    idxRead1 = bitreverse(idx_in_x1, NFFT);
    
    x0_real = xk(idxRead0 + 1);
    x0_imag = xk(idxRead0 + 1 + 1);
    x1_real = xk(idxRead1 + 1);
    x1_imag = xk(idxRead1 + 1 + 1);
    
    x0_real_p = x0_real + x1_real; % Tw real is 1.0, Tw imag is 0
    x0_imag_p = x0_imag + x1_imag; % Tw real is 1.0, Tw imag is 0
    
    x1_real_p = x0_real - x1_real; % Tw real is 1.0, Tw imag is 0
    x1_imag_p = x0_imag - x1_imag; % Tw real is 1.0, Tw imag is 0
    
    inout_re(idx_in_x0+1) = x0_real_p;
    inout_im(idx_in_x0+1) = x0_imag_p;
    inout_re(idx_in_x1+1) = x1_real_p;
    inout_im(idx_in_x1+1) = x1_imag_p;
    
    idx_in_x0 = idx_in_x0 + 2;
    idx_in_x1 = idx_in_x1 + 2;
    
end

num_bfly = 1;
num_bfly_shift = 2;
num_shift_tw = FFTS1/4;
num_ops_per_bfly = FFTS1/8;

% All other stages
for(idx_stage = 1:NFFTP-1)
    
    % Start tw factors from 0-index
    idx_in_tw = 0;
    
    idx_in_x0_start = 0;

    inc1 = num_bfly_shift;
    inc2 = - num_bfly_shift;
    inc3 = num_shift_tw;
    inc4 = -num_shift_tw;
    
    idx_tw_0 = 0;
    idx_tw_1 = FFTS1/4;
    
    % Butterfly Group Loop
    for(idx_bfly = 0:num_bfly - 1)
        
        idx_in_x0 = idx_in_x0_start;
        
        % Get twiddle factors
        tw_fac_real = tw_facs_real(idx_tw_0+1);
        idx_tw_0 = idx_tw_0 + inc3;
        tw_fac_imag = tw_facs_real((idx_tw_1)+1);
        idx_tw_1 = idx_tw_1 + inc4;
        
        % Butterfly loop
        for(idx_inbfly=0:num_ops_per_bfly-1)
            
            % #####################
            % First half of loop
            % #####################
            
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc2;
            
            % Compute complex multiplications
            out_real = tw_fac_real * x1_real;
            out_real = out_real + tw_fac_imag * x1_imag ;
            out_imag = tw_fac_real * x1_imag;
            out_imag = out_imag - tw_fac_imag * x1_real ;
            
            x0_real_p = x0_real + out_real;
            x0_imag_p = x0_imag + out_imag;
            x1_real_p = x0_real - out_real;
            x1_imag_p = x0_imag - out_imag;
            
            % Write results
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc1;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc1;
        end
        
        % I hope I can put this in parallel to the pipeline load/store ops
        idx_in_x0 = idx_in_x0_start + num_bfly;
        
        for(idx_inbfly=0:num_ops_per_bfly-1)
            % #####################
            % Second half of loop
            % #####################

            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc2;
            
            % Compute complex multiplications
            % Swap real and imaginary part due to everything in one loop
            out_real = tw_fac_imag * x1_real;
            out_real = out_real - tw_fac_real * x1_imag ;
            out_imag = tw_fac_imag * x1_imag;
            out_imag = out_imag + tw_fac_real * x1_real ;
            
            x0_real_p = x0_real - out_real;
            x0_imag_p = x0_imag - out_imag;
            x1_real_p = x0_real + out_real;
            x1_imag_p = x0_imag + out_imag;
            
            % Write results
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc1;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc1;
            
        end
        
        % Update indexing, maybe, we can arrange together with ringing of 
        % processing pipeline
        idx_in_x0_start = idx_in_x0_start + 1;
        
    end  
    
    % Update from stage to stage
    num_bfly = num_bfly*2;
    num_bfly_shift = num_bfly_shift * 2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly / 2;
end

idx_in_out_0 = 1;
idx_in_out_1 = FFTS1P - 1;

inc1 = FFTS1P/2;
inc2 = -FFTS1P/2 + 1;
inc3 = -FFTS1P/2;
inc4 = +FFTS1P/2 - 1;

% Output stage: From complex N/2 FFT to N FFT
for(idx = 1:FFTS1P/2-1)
    
    % This is in fact the last stage of the N FFT butterfly BUT only the
    % upper half
    tw_fac_real = tw_facs_real(idx+1);
    tw_fac_imag = tw_facs_real((FFTS1P/2 - idx)+1);
    
    % Combine result from complex FFT to determine FFT for real part
    tt1_real = inout_re(idx_in_out_0+1) + inout_re(idx_in_out_1+1);
    tt1_real = tt1_real/2;
    tt1_imag = inout_im(idx_in_out_0+1) - inout_im(idx_in_out_1+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = inout_im(idx_in_out_0+1) + inout_im(idx_in_out_1+1);
    tt2_real = tt2_real/2;
    tt2_imag = -inout_re(idx_in_out_0+1) + inout_re(idx_in_out_1+1);
    tt2_imag = tt2_imag/2;
    idx_in_out_1 = idx_in_out_1 + inc3;
    
    out_real = tw_fac_real * tt2_real;
    out_real = out_real + tw_fac_imag * tt2_imag;
    out_imag = tw_fac_imag * tt2_real;
    out_imag = out_imag - tw_fac_real * tt2_imag;
    
    % Final twiddle factor
    tt_real = tt1_real + out_real;
    tt_imag = tt1_imag - out_imag;
    
    % Write to output buffer
    Xn_fft3_real(idx_in_out_0+1) = tt_real;
    Xn_fft3_imag(idx_in_out_0+1) = tt_imag;
    
    idx_in_out_0 = idx_in_out_0 + inc1;
    
   
    % Combine result from complex FFT to determine FFT for real part
    tt1_real = inout_re(idx_in_out_0+1) + inout_re(idx_in_out_1+1);
    tt1_real = tt1_real/2;
    tt1_imag = inout_im(idx_in_out_0+1) - inout_im(idx_in_out_1+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = inout_im(idx_in_out_0+1) + inout_im(idx_in_out_1+1);
    tt2_real = tt2_real/2;
    tt2_imag = -inout_re(idx_in_out_0+1) + inout_re(idx_in_out_1+1);
    tt2_imag = tt2_imag/2;
    idx_in_out_1 = idx_in_out_1 + inc4;
    
    % Mix real and imag due to symmetry in load..
    out_real = tw_fac_imag * tt2_real;
    out_real = out_real - tw_fac_real * tt2_imag;
    out_imag = tw_fac_real * tt2_real;
    out_imag = out_imag + tw_fac_imag * tt2_imag;
    
    % Final twiddle factor
    tt_real = tt1_real - out_real;
    tt_imag = tt1_imag - out_imag;
    
    % Write to output buffer
    Xn_fft3_real(idx_in_out_0+1) = tt_real;
    Xn_fft3_imag(idx_in_out_0+1) = tt_imag;
    
    idx_in_out_0 = idx_in_out_0 + inc2;
end

% Special case for first and last element in FFT buffer (real valued)
tt1 =  inout_re(1);
tt2 =  inout_im(1);

Xn_fft3_real(1) = tt1 + tt2;
Xn_fft3_imag(1) = 0;

Xn_fft3_real(FFTS1P+1) = tt1 - tt2;
Xn_fft3_imag(FFTS1P+1) = 0;

% Combine result from complex FFT to determine FFT for real part
tt1_real = inout_re(FFTS1P/2+1);

% Combine result from complex FFT to determine FFT for imag part
tt2_real = inout_im(FFTS1P/2+1);

% Write to output buffer
Xn_fft3_real(FFTS1P/2+1) = tt1_real;
Xn_fft3_imag(FFTS1P/2+1) = -tt2_real;
    
% Combine to form output fft
Xn_fft3 = Xn_fft3_real + i * Xn_fft3_imag;

Xn_fft3c = [Xn_fft3 ; conj(Xn_fft3(FFTS1P:-1:2))];
diff = Xn_fft3c - Xn_fft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);





%% =======================================================================
disp('%% FFT variant No 12: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Sorting of input into real and complex and bit reversal addr in one step');
disp('%% - Come back to original algorithm with complex multiplication');
disp('%% - Reduced the number of coefficients to be stored as twiddle factors to LFFT/4+1');
disp('%% - Converged pairs of loops - hope that this will be better for pipeline and requires only one load of twiddle factors');
disp('%% - Split up first main loop into halfes for better M register usage');
disp('%% - Final loop optimized to loop FFTD/4 times only with some specific cases following');
%% =======================================================================

inout_re = zeros(FFTS1/2+1, 1);
inout_im = zeros(FFTS1/2+1, 1);

FFTS1P = FFTS1/2;
NFFTP = NFFT-1;


tw_facs = exp(-i*2*pi/FFTS1 * [0:FFTS1/4]);
tw_facs_real = real(tw_facs);
%tw_facs_imag = imag(tw_facs);

idx_in_x0 = 0;
idx_in_x1 = 1;

Xn_fft3_real = zeros(FFTS1P + 1,1);
Xn_fft3_imag = zeros(FFTS1P + 1,1);

%% First stage of FFT butterflies.
%% This stage takes into account:
%% - the bit reverse addressing mode of the input data,
%% - transforms the real input into a complex FFT entry
%% - exploits that the butterfly twiddle factor is 1
for(idx_bfly = 0:FFTS1/4 - 1)
        
    idxRead0 = bitreverse(idx_in_x0, NFFT);
    idxRead1 = bitreverse(idx_in_x1, NFFT);
    
    x0_real = xk(idxRead0 + 1);
    x0_imag = xk(idxRead0 + 1 + 1);
    x1_real = xk(idxRead1 + 1);
    x1_imag = xk(idxRead1 + 1 + 1);
    
    x0_real_p = x0_real + x1_real; % Tw real is 1.0, Tw imag is 0
    x0_imag_p = x0_imag + x1_imag; % Tw real is 1.0, Tw imag is 0
    
    x1_real_p = x0_real - x1_real; % Tw real is 1.0, Tw imag is 0
    x1_imag_p = x0_imag - x1_imag; % Tw real is 1.0, Tw imag is 0
    
    inout_re(idx_in_x0+1) = x0_real_p;
    inout_im(idx_in_x0+1) = x0_imag_p;
    inout_re(idx_in_x1+1) = x1_real_p;
    inout_im(idx_in_x1+1) = x1_imag_p;
    
    idx_in_x0 = idx_in_x0 + 2;
    idx_in_x1 = idx_in_x1 + 2;
    
end

num_bfly = 1;
num_bfly_shift = 2;
num_shift_tw = FFTS1/4;
num_ops_per_bfly = FFTS1/8;

% All other stages
for(idx_stage = 1:NFFTP-1)
    
    % Start tw factors from 0-index
    idx_in_tw = 0;
    
    idx_in_x0_start = 0;

    inc1 = num_bfly_shift;
    inc2 = - num_bfly_shift;
    inc3 = num_shift_tw;
    inc4 = -num_shift_tw;
    
    idx_tw_0 = 0;
    idx_tw_1 = FFTS1/4;
    
    % Butterfly Group Loop
    for(idx_bfly = 0:num_bfly - 1)
        
        idx_in_x0 = idx_in_x0_start;
        
        % Get twiddle factors
        tw_fac_real = tw_facs_real(idx_tw_0+1);
        idx_tw_0 = idx_tw_0 + inc3;
        tw_fac_imag = tw_facs_real((idx_tw_1)+1);
        idx_tw_1 = idx_tw_1 + inc4;
        
        % Butterfly loop
        for(idx_inbfly=0:num_ops_per_bfly-1)
            
            % #####################
            % First half of loop
            % #####################
            
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc2;
            
            % Compute complex multiplications
            out_real = tw_fac_real * x1_real;
            out_real = out_real + tw_fac_imag * x1_imag ;
            out_imag = tw_fac_real * x1_imag;
            out_imag = out_imag - tw_fac_imag * x1_real ;
            
            x0_real_p = x0_real + out_real;
            x0_imag_p = x0_imag + out_imag;
            x1_real_p = x0_real - out_real;
            x1_imag_p = x0_imag - out_imag;
            
            % Write results
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc1;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc1;
        end
        
        % I hope I can put this in parallel to the pipeline load/store ops
        idx_in_x0 = idx_in_x0_start + num_bfly;
        
        for(idx_inbfly=0:num_ops_per_bfly-1)
            % #####################
            % Second half of loop
            % #####################

            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc2;
            
            % Compute complex multiplications
            % Swap real and imaginary part due to everything in one loop
            out_real = tw_fac_imag * x1_real;
            out_real = out_real - tw_fac_real * x1_imag ;
            out_imag = tw_fac_imag * x1_imag;
            out_imag = out_imag + tw_fac_real * x1_real ;
            
            x0_real_p = x0_real - out_real;
            x0_imag_p = x0_imag - out_imag;
            x1_real_p = x0_real + out_real;
            x1_imag_p = x0_imag + out_imag;
            
            % Write results
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc1;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc1;
            
        end
        
        % Update indexing, maybe, we can arrange together with ringing of 
        % processing pipeline
        idx_in_x0_start = idx_in_x0_start + 1;
        
    end  
    
    % Update from stage to stage
    num_bfly = num_bfly*2;
    num_bfly_shift = num_bfly_shift * 2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly / 2;
end

idx_in_out_0 = 0;
idx_in_out_1 = FFTS1P - 0;

inc1 = FFTS1P/2;
inc2 = -FFTS1P/2 + 1;
inc3 = -FFTS1P/2;
inc4 = +FFTS1P/2 - 1;

%inout_re(FFTS1P + 1) = inout_re(1);
%inout_im(FFTS1P + 1) = inout_im(1);

% Special case for first and last element in FFT buffer (real valued)
tt1 =  inout_re(idx_in_out_0 + 1);
tt2 =  inout_im(idx_in_out_0 + 1);

inout_re(idx_in_out_0 + 1) = tt1 + tt2;
inout_im(idx_in_out_0 + 1) = 0;

inout_re(idx_in_out_1+1) = tt1 - tt2;
inout_im(idx_in_out_1+1) = 0;

idx_in_out_0 = idx_in_out_0 + inc1;

% Combine result from complex FFT to determine FFT for real part
tt1_real = inout_re(idx_in_out_0+1);

% Combine result from complex FFT to determine FFT for imag part
tt2_real = inout_im(idx_in_out_0+1);

% Write to output buffer
inout_re(idx_in_out_0+1) = tt1_real;
inout_im(idx_in_out_0+1) = -tt2_real;

idx_in_out_0 = idx_in_out_0 + inc2;

idx_in_out_1 = idx_in_out_1 - 1;

% Output stage: From complex N/2 FFT to N FFT
for(idx = 1:FFTS1P/4-1)
    
    % This is in fact the last stage of the N FFT butterfly BUT only the
    % upper half
    tw_fac_real = tw_facs_real(idx+1);
    tw_fac_imag = tw_facs_real((FFTS1P/2 - idx)+1);
 
    % Combine result from complex FFT to determine FFT for real part
    tt1_real = inout_re(idx_in_out_0+1) + inout_re(idx_in_out_1+1);
    tt1_real = tt1_real/2;
    tt1_imag = inout_im(idx_in_out_0+1) - inout_im(idx_in_out_1+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = inout_im(idx_in_out_0+1) + inout_im(idx_in_out_1+1);
    tt2_real = tt2_real/2;
    tt2_imag = -inout_re(idx_in_out_0+1) + inout_re(idx_in_out_1+1);
    tt2_imag = tt2_imag/2;
    
    out_real = tw_fac_real * tt2_real;
    out_real = out_real + tw_fac_imag * tt2_imag;
    out_imag = tw_fac_imag * tt2_real;
    out_imag = out_imag - tw_fac_real * tt2_imag;
    
    % Final twiddle factor
    tt_real = tt1_real + out_real;
    tt_imag = tt1_imag - out_imag;
    
    % Write to output buffer
    inout_re(idx_in_out_0+1) = tt_real;
    inout_im(idx_in_out_0+1) = tt_imag;
      
    idx_in_out_0 = idx_in_out_0 + inc1;
        
    % Final twiddle factor
    tt_real = tt1_real - out_real;
    tt_imag = -tt1_imag - out_imag;
    
    inout_re(idx_in_out_1+1) = tt_real;
    inout_im(idx_in_out_1+1) = tt_imag;
    idx_in_out_1 = idx_in_out_1 + inc3;
    
    
    
    % Combine result from complex FFT to determine FFT for real part
    tt1_real = inout_re(idx_in_out_0+1) + inout_re(idx_in_out_1+1);
    tt1_real = tt1_real/2;
    tt1_imag = inout_im(idx_in_out_0+1) - inout_im(idx_in_out_1+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = inout_im(idx_in_out_0+1) + inout_im(idx_in_out_1+1);
    tt2_real = tt2_real/2;
    tt2_imag = -inout_re(idx_in_out_0+1) + inout_re(idx_in_out_1+1);
    tt2_imag = tt2_imag/2;
    
    % Mix real and imag due to symmetry in load..
    out_real = tw_fac_imag * tt2_real;
    out_real = out_real - tw_fac_real * tt2_imag;
    out_imag = tw_fac_real * tt2_real;
    out_imag = out_imag + tw_fac_imag * tt2_imag;
    
    % Final twiddle factor
    tt_real = tt1_real - out_real;
    tt_imag = tt1_imag - out_imag;
    
    % Write to output buffer
    inout_re(idx_in_out_0+1) = tt_real;
    inout_im(idx_in_out_0+1) = tt_imag;
    
    idx_in_out_0 = idx_in_out_0 + inc2;
    
    % Final twiddle factor
    tt_real = tt1_real + out_real;
    tt_imag = -tt1_imag - out_imag;
    
    inout_re(idx_in_out_1+1) = tt_real;
    inout_im(idx_in_out_1+1) = tt_imag;
    idx_in_out_1 = idx_in_out_1 + inc4;
    
    %disp('----------------------------');

end

idx = idx + 1;

% This is in fact the last stage of the N FFT butterfly BUT only the
% upper half
tw_fac_real = tw_facs_real(idx+1);
tw_fac_imag = tw_facs_real((FFTS1P/2 - idx)+1);

% Combine result from complex FFT to determine FFT for real part
tt1_real = inout_re(idx_in_out_0+1) + inout_re(idx_in_out_1+1);
tt1_real = tt1_real/2;
tt1_imag = inout_im(idx_in_out_0+1) - inout_im(idx_in_out_1+1);
tt1_imag = tt1_imag/2;

% Combine result from complex FFT to determine FFT for imag part
tt2_real = inout_im(idx_in_out_0+1) + inout_im(idx_in_out_1+1);
tt2_real = tt2_real/2;
tt2_imag = -inout_re(idx_in_out_0+1) + inout_re(idx_in_out_1+1);
tt2_imag = tt2_imag/2;

out_real = tw_fac_real * tt2_real;
out_real = out_real + tw_fac_imag * tt2_imag;
out_imag = tw_fac_imag * tt2_real;
out_imag = out_imag - tw_fac_real * tt2_imag;

% Final twiddle factor
tt_real = tt1_real + out_real;
tt_imag = tt1_imag - out_imag;

% Write to output buffer
inout_re(idx_in_out_0+1) = tt_real;
inout_im(idx_in_out_0+1) = tt_imag;

% Final twiddle factor
tt_real = tt1_real - out_real;
tt_imag = -tt1_imag - out_imag;

inout_re(idx_in_out_1+1) = tt_real;
inout_im(idx_in_out_1+1) = tt_imag;
    

% Combine to form output fft
Xn_fft3 = inout_re + i * inout_im;

Xn_fft3c = [Xn_fft3 ; conj(Xn_fft3(FFTS1P:-1:2))];
diff = Xn_fft3c - Xn_fft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);






%% =======================================================================
disp('%% FFT variant No 13: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Sorting of input into real and complex and bit reversal addr in one step');
disp('%% - Come back to original algorithm with complex multiplication');
disp('%% - Reduced the number of coefficients to be stored as twiddle factors to LFFT/4+1');
disp('%% - Converged pairs of loops - hope that this will be better for pipeline and requires only one load of twiddle factors');
disp('%% - Split up first main loop into halfes for better M register usage');
disp('%% - Final loop optimized to loop FFTD/4 times only with some specific cases following');
%% =======================================================================

inout_re = zeros(FFTS1/2+1, 1);
inout_im = zeros(FFTS1/2+1, 1);
work_buf_re = zeros(FFTS1/2, 1);
work_buf_im = zeros(FFTS1/2, 1);

FFTS1P = FFTS1/2;
NFFTP = NFFT-1;

idx_in_0 = 0;
inc0 = 2;
inc1 = -1;

% Do bit reversal addressing
for(idx_bfly = 0:FFTS1/2 - 1)
    x0_real = xk(idx_in_0 + 1);
    idxWrite = bitreverse(idx_bfly, NFFT-1);

    idx_in_0 = idx_in_0 + 1;
    
    x0_imag = xk(idx_in_0 + 1);
    idx_in_0 = idx_in_0 + 1;
    
    
    % Let us do the input normalization here..
    work_buf_re(idxWrite + 1) = x0_real;
    work_buf_im(idxWrite + 1) = x0_imag;
end

tw_facs = exp(-i*2*pi/FFTS1 * [0:FFTS1/4]);
tw_facs_real = real(tw_facs);
%tw_facs_imag = imag(tw_facs);

idx_in_x0 = 0;

Xn_fft3_real = zeros(FFTS1P + 1,1);
Xn_fft3_imag = zeros(FFTS1P + 1,1);

%% First stage of FFT butterflies.
%% This stage takes into account:
%% - the bit reverse addressing mode of the input data,
%% - transforms the real input into a complex FFT entry
%% - exploits that the butterfly twiddle factor is 1
for(idx_bfly = 0:FFTS1/4 - 1)
    
    x0_real = work_buf_re(idx_in_x0 + 1);
    x0_imag = work_buf_im(idx_in_x0 + 1);
    idx_in_x0 = idx_in_x0 + 1;
    x1_real = work_buf_re(idx_in_x0 + 1);
    x1_imag = work_buf_im(idx_in_x0 + 1);
    idx_in_x0 = idx_in_x0 - 1;
    
    x0_real_p = x0_real + x1_real; % Tw real is 1.0, Tw imag is 0
    x0_imag_p = x0_imag + x1_imag; % Tw real is 1.0, Tw imag is 0
    
    x1_real_p = x0_real - x1_real; % Tw real is 1.0, Tw imag is 0
    x1_imag_p = x0_imag - x1_imag; % Tw real is 1.0, Tw imag is 0
    
    work_buf_re(idx_in_x0+1) = x0_real_p;
    work_buf_im(idx_in_x0+1) = x0_imag_p;
    idx_in_x0 = idx_in_x0 + 1;

    work_buf_re(idx_in_x0+1) = x1_real_p;
    work_buf_im(idx_in_x0+1) = x1_imag_p;
    idx_in_x0 = idx_in_x0 + 1;   
end

num_bfly = 1;
num_bfly_shift = 2;
num_shift_tw = FFTS1/4;
num_ops_per_bfly = FFTS1/8;

% All other stages
for(idx_stage = 1:NFFTP-1)
    
    % Start tw factors from 0-index
    idx_in_tw = 0;
    
    idx_in_x0_start = 0;

    inc1 = num_bfly_shift;
    inc2 = - num_bfly_shift;
    inc3 = num_shift_tw;
    inc4 = -num_shift_tw;
    
    idx_tw_0 = 0;
    idx_tw_1 = FFTS1/4;
    
    % Butterfly Group Loop
    for(idx_bfly = 0:num_bfly - 1)
        
        idx_in_x0 = idx_in_x0_start;
        
        % Get twiddle factors
        tw_fac_real = tw_facs_real(idx_tw_0+1);
        idx_tw_0 = idx_tw_0 + inc3;
        tw_fac_imag = tw_facs_real((idx_tw_1)+1);
        idx_tw_1 = idx_tw_1 + inc4;
        
        % Butterfly loop
        for(idx_inbfly=0:num_ops_per_bfly-1)
            
            % #####################
            % First half of loop
            % #####################
            
            x0_real = work_buf_re(idx_in_x0+1);
            x0_imag = work_buf_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            x1_real = work_buf_re(idx_in_x0+1);
            x1_imag = work_buf_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc2;
            
            % Compute complex multiplications
            out_real = tw_fac_real * x1_real;
            out_real = out_real + tw_fac_imag * x1_imag ;
            out_imag = tw_fac_real * x1_imag;
            out_imag = out_imag - tw_fac_imag * x1_real ;
            
            x0_real_p = x0_real + out_real;
            x0_imag_p = x0_imag + out_imag;
            x1_real_p = x0_real - out_real;
            x1_imag_p = x0_imag - out_imag;
            
            % Write results
            work_buf_re(idx_in_x0+1) = x0_real_p;
            work_buf_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc1;
            
            work_buf_re(idx_in_x0+1) = x1_real_p;
            work_buf_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc1;
        end
        
        % I hope I can put this in parallel to the pipeline load/store ops
        idx_in_x0 = idx_in_x0_start + num_bfly;
        
        for(idx_inbfly=0:num_ops_per_bfly-1)
            % #####################
            % Second half of loop
            % #####################

            x0_real = work_buf_re(idx_in_x0+1);
            x0_imag = work_buf_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            x1_real = work_buf_re(idx_in_x0+1);
            x1_imag = work_buf_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc2;
            
            % Compute complex multiplications
            % Swap real and imaginary part due to everything in one loop
            out_real = tw_fac_imag * x1_real;
            out_real = out_real - tw_fac_real * x1_imag ;
            out_imag = tw_fac_imag * x1_imag;
            out_imag = out_imag + tw_fac_real * x1_real ;
            
            x0_real_p = x0_real - out_real;
            x0_imag_p = x0_imag - out_imag;
            x1_real_p = x0_real + out_real;
            x1_imag_p = x0_imag + out_imag;
            
            % Write results
            work_buf_re(idx_in_x0+1) = x0_real_p;
            work_buf_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc1;
            
            work_buf_re(idx_in_x0+1) = x1_real_p;
            work_buf_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc1;
            
        end
        
        % Update indexing, maybe, we can arrange together with ringing of 
        % processing pipeline
        idx_in_x0_start = idx_in_x0_start + 1;
        
    end  
    
    % Update from stage to stage
    num_bfly = num_bfly*2;
    num_bfly_shift = num_bfly_shift * 2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly / 2;
end

idx_in_out_0 = 0;
idx_in_out_1 = FFTS1P - 0;

inc1 = FFTS1P/2;
inc2 = -FFTS1P/2 + 1;
inc3 = -FFTS1P/2;
inc4 = +FFTS1P/2 - 1;

%inout_re(FFTS1P + 1) = inout_re(1);
%inout_im(FFTS1P + 1) = inout_im(1);

% Special case for first and last element in FFT buffer (real valued)
tt1 =  work_buf_re(idx_in_out_0 + 1);
tt2 =  work_buf_im(idx_in_out_0 + 1);

inout_re(idx_in_out_0 + 1) = tt1 + tt2;
inout_im(idx_in_out_0 + 1) = 0;

inout_re(idx_in_out_1+1) = tt1 - tt2;
inout_im(idx_in_out_1+1) = 0;

idx_in_out_0 = idx_in_out_0 + inc1;

% Combine result from complex FFT to determine FFT for real part
tt1_real = work_buf_re(idx_in_out_0+1);

% Combine result from complex FFT to determine FFT for imag part
tt2_real = work_buf_im(idx_in_out_0+1);

% Write to output buffer
inout_re(idx_in_out_0+1) = tt1_real;
inout_im(idx_in_out_0+1) = -tt2_real;

idx_in_out_0 = idx_in_out_0 + inc2;

idx_in_out_1 = idx_in_out_1 - 1;

% Output stage: From complex N/2 FFT to N FFT
for(idx = 1:FFTS1P/4-1)
    
    % This is in fact the last stage of the N FFT butterfly BUT only the
    % upper half
    tw_fac_real = tw_facs_real(idx+1);
    tw_fac_imag = tw_facs_real((FFTS1P/2 - idx)+1);
 
    % Combine result from complex FFT to determine FFT for real part
    tt1_real = work_buf_re(idx_in_out_0+1) + work_buf_re(idx_in_out_1+1);
    tt1_real = tt1_real/2;
    tt1_imag = work_buf_im(idx_in_out_0+1) - work_buf_im(idx_in_out_1+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = work_buf_im(idx_in_out_0+1) + work_buf_im(idx_in_out_1+1);
    tt2_real = tt2_real/2;
    tt2_imag = -work_buf_re(idx_in_out_0+1) + work_buf_re(idx_in_out_1+1);
    tt2_imag = tt2_imag/2;
    
    out_real = tw_fac_real * tt2_real;
    out_real = out_real + tw_fac_imag * tt2_imag;
    out_imag = tw_fac_imag * tt2_real;
    out_imag = out_imag - tw_fac_real * tt2_imag;
    
    % Final twiddle factor
    tt_real = tt1_real + out_real;
    tt_imag = tt1_imag - out_imag;
    
    % Write to output buffer
    inout_re(idx_in_out_0+1) = tt_real;
    inout_im(idx_in_out_0+1) = tt_imag;
      
    idx_in_out_0 = idx_in_out_0 + inc1;
        
    % Final twiddle factor
    tt_real = tt1_real - out_real;
    tt_imag = -tt1_imag - out_imag;
    
    inout_re(idx_in_out_1+1) = tt_real;
    inout_im(idx_in_out_1+1) = tt_imag;
    idx_in_out_1 = idx_in_out_1 + inc3;
    
    
    
    % Combine result from complex FFT to determine FFT for real part
    tt1_real = work_buf_re(idx_in_out_0+1) + work_buf_re(idx_in_out_1+1);
    tt1_real = tt1_real/2;
    tt1_imag = work_buf_im(idx_in_out_0+1) - work_buf_im(idx_in_out_1+1);
    tt1_imag = tt1_imag/2;
    
    % Combine result from complex FFT to determine FFT for imag part
    tt2_real = work_buf_im(idx_in_out_0+1) + work_buf_im(idx_in_out_1+1);
    tt2_real = tt2_real/2;
    tt2_imag = -work_buf_re(idx_in_out_0+1) + work_buf_re(idx_in_out_1+1);
    tt2_imag = tt2_imag/2;
    
    % Mix real and imag due to symmetry in load..
    out_real = tw_fac_imag * tt2_real;
    out_real = out_real - tw_fac_real * tt2_imag;
    out_imag = tw_fac_real * tt2_real;
    out_imag = out_imag + tw_fac_imag * tt2_imag;
    
    % Final twiddle factor
    tt_real = tt1_real - out_real;
    tt_imag = tt1_imag - out_imag;
    
    % Write to output buffer
    inout_re(idx_in_out_0+1) = tt_real;
    inout_im(idx_in_out_0+1) = tt_imag;
    
    idx_in_out_0 = idx_in_out_0 + inc2;
    
    % Final twiddle factor
    tt_real = tt1_real + out_real;
    tt_imag = -tt1_imag - out_imag;
    
    inout_re(idx_in_out_1+1) = tt_real;
    inout_im(idx_in_out_1+1) = tt_imag;
    idx_in_out_1 = idx_in_out_1 + inc4;
    
    %disp('----------------------------');

end

idx = idx + 1;

% This is in fact the last stage of the N FFT butterfly BUT only the
% upper half
tw_fac_real = tw_facs_real(idx+1);
tw_fac_imag = tw_facs_real((FFTS1P/2 - idx)+1);

% Combine result from complex FFT to determine FFT for real part
tt1_real = work_buf_re(idx_in_out_0+1) + work_buf_re(idx_in_out_1+1);
tt1_real = tt1_real/2;
tt1_imag = work_buf_im(idx_in_out_0+1) - work_buf_im(idx_in_out_1+1);
tt1_imag = tt1_imag/2;

% Combine result from complex FFT to determine FFT for imag part
tt2_real = work_buf_im(idx_in_out_0+1) + work_buf_im(idx_in_out_1+1);
tt2_real = tt2_real/2;
tt2_imag = -work_buf_re(idx_in_out_0+1) + work_buf_re(idx_in_out_1+1);
tt2_imag = tt2_imag/2;

out_real = tw_fac_real * tt2_real;
out_real = out_real + tw_fac_imag * tt2_imag;
out_imag = tw_fac_imag * tt2_real;
out_imag = out_imag - tw_fac_real * tt2_imag;

% Final twiddle factor
tt_real = tt1_real + out_real;
tt_imag = tt1_imag - out_imag;

% Write to output buffer
inout_re(idx_in_out_0+1) = tt_real;
inout_im(idx_in_out_0+1) = tt_imag;

% Final twiddle factor
tt_real = tt1_real - out_real;
tt_imag = -tt1_imag - out_imag;

inout_re(idx_in_out_1+1) = tt_real;
inout_im(idx_in_out_1+1) = tt_imag;
    

% Combine to form output fft
Xn_fft3 = inout_re + i * inout_im;

Xn_fft3c = [Xn_fft3 ; conj(Xn_fft3(FFTS1P:-1:2))];
diff = Xn_fft3c - Xn_fft_orig;

disp(['--> Result deviation from original: ' num2str(diff' * diff)]);



function [idx] = bitreverse(idx, numDigs)
idx = bin2dec(fliplr(dec2bin(idx, numDigs)));