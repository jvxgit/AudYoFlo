function [xk] = runIffts(Xk_full)

disp(' ################## runIffts #########################');

%% =======================================================================
disp('%% IFFT variant No 1: ');
disp('%% - Full FFT (matrix multiplication) vs FFT');
%% =======================================================================
FFTS1 = length(Xk_full);
NFFT = log2(FFTS1);
FFTS1P = FFTS1/2;
NFFTP = NFFT-1;

% Let us prepare the twiddle facs from FFT here
tw_facs_fft = exp(-i*2*pi/FFTS1 * [0:FFTS1/2-1]);
tw_facs_fft_real = real(tw_facs_fft);
tw_facs_fft_imag = imag(tw_facs_fft);

%% FFT variant No 1: Full computation
ifftMat = zeros(FFTS1, FFTS1);
WN = exp(i*2*pi/FFTS1);
for(indx = 1:FFTS1)
    for(indy = 1:FFTS1)
        ifftMat(indx, indy) = WN ^( (indx-1)*(indy-1));
    end
end

xk_idft = ifftMat * Xk_full;
xk_ifft_orig = ifft(Xk_full) * FFTS1; % <- Matlab does this already

diff = xk_idft - xk_ifft_orig;
disp(['--> Result deviation: ' num2str(diff' * diff)]);

% All following IFFTs work on N/2+1 buffers as input
Xk = Xk_full(1:FFTS1/2+1);

%% =======================================================================
disp('%% IFFT variant No 2: ');
disp('%% - (N/2 + 1)-> complex N/2 IFFT reconstruction, ');
disp('%% - ifft function call, ');
disp('%% - reconstruction by de-interleaving');
%% =======================================================================

% Reconstruction algorithm, refer to http://www.engineeringproductivitytools.com/stuff/T0001/PT10.HTM
Xk_fft_even = zeros(FFTS1/2,1);
for(idx = 1:FFTS1/2-1)
    Xk_fft_even(idx + 1) = 0.5 * (Xk(idx + 1) + conj (Xk(FFTS1/2-idx+1)));
end
Xk_fft_even(1) = 0.5 * (Xk(1) + Xk(FFTS1/2 + 1));

tw = exp(i*2*pi*[0:FFTS1-1]/FFTS1);

Xk_fft_odd = zeros(FFTS1/2,1);
for(idx = 1:FFTS1/2-1)
    Xk_fft_odd(idx + 1) = 0.5 * (Xk(idx + 1) - conj (Xk(FFTS1/2-idx+1))) * tw(idx+1);
end
Xk_fft_odd(1) = 0.5 * (Xk(1) - Xk(FFTS1/2 + 1));

Xk_fft_all = Xk_fft_even + i * Xk_fft_odd;

% Speed up IFFT by using Matlab function
xk_cmplx2 = ifft(Xk_fft_all)* FFTS1;% <- Matlab does this already
xk_ifft2 = zeros(FFTS1,1);
for(idx = 0:FFTS1/2-1)
    xk_ifft2(2*idx + 1) = real(xk_cmplx2(idx+1));
    xk_ifft2(2*idx + 2) = imag(xk_cmplx2(idx+1));
end
diff = xk_ifft2 - xk_ifft_orig;
disp(['--> Result deviation: ' num2str(diff' * diff)]);
    
%% =======================================================================
disp('%% IFFT variant No 2: ');
disp('%% - (N/2 + 1)-> complex N/2 IFFT reconstruction, ');
disp('%% - Butterfly IFFT, ');
disp('%% - reconstruction by de-interleaving and bit reverse addr in one operation');
%% =======================================================================

% Initial stage: Recinstruction of complex N/2 IFFT input
tw = exp(i*2*pi*[0:FFTS1/2-1]/FFTS1);
tw_real = real(tw);
tw_imag = imag(tw);

Xk_fft_N2_real = zeros(FFTS1/2,1);
Xk_fft_N2_imag = zeros(FFTS1/2,1);

for(idx = 1:FFTS1/2-1)

    tt_real1 = (real(Xk(idx + 1)) + real(Xk(FFTS1/2-idx+1)))/2;
    tt_imag1 = (imag(Xk(idx + 1)) - imag(Xk(FFTS1/2-idx+1)))/2;
    
    t_real = -(imag(Xk(idx + 1)) + imag(Xk(FFTS1/2-idx+1)))/2;
    t_imag = (real(Xk(idx + 1)) - real(Xk(FFTS1/2-idx+1)))/2;
    
    tt_real2 = t_real * tw_real(idx + 1) - t_imag * tw_imag(idx+1);
    tt_imag2 = t_real * tw_imag(idx + 1) + t_imag * tw_real(idx+1);
    
    Xk_fft_N2_real(idx + 1) = tt_real1 + tt_real2;
    Xk_fft_N2_imag(idx + 1) = tt_imag1 + tt_imag2;
end
Xk_fft_N2_real(1) = (Xk(1) + Xk(FFTS1/2 + 1))/2;
Xk_fft_N2_imag(1) = (Xk(1) - Xk(FFTS1/2 + 1))/2;

inout_re = Xk_fft_N2_real;
inout_im = Xk_fft_N2_imag;

% Butterflies
num_bfly = 1;
num_shift_tw = 2;
num_ops_per_bfly = FFTS1P/2;
num_shift_bfly2bfly = FFTS1P/2;

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
            
            tw_fac_real = tw_real(idx_in_tw+1);
            tw_fac_imag = tw_imag(idx_in_tw+1);
            
            x0_real_p = x0_real + x1_real;
            x0_imag_p = x0_imag + x1_imag;
            
            x1_real_p = x0_real - x1_real;
            x1_imag_p = x0_imag - x1_imag;
            
            x1_real = x1_real_p * tw_fac_real - x1_imag_p * tw_fac_imag;
            x1_imag = x1_real_p * tw_fac_imag + x1_imag_p * tw_fac_real;
            
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            inout_re(idx_in_x1+1) = x1_real;
            inout_im(idx_in_x1+1) = x1_imag;
            
            idx_in_tw = idx_in_tw + num_shift_tw;
            idx_in_x0 = idx_in_x0 + 1;
            idx_in_x1 = idx_in_x1 + 1;
        end
        
        idx_in_x0 = idx_in_x0 + num_shift_bfly2bfly;
        idx_in_x1 = idx_in_x1 + num_shift_bfly2bfly;
        
    end
    
    num_bfly = num_bfly*2;
    num_shift_tw = num_shift_tw*2;
    num_ops_per_bfly = num_ops_per_bfly / 2;
    num_shift_bfly2bfly = num_shift_bfly2bfly / 2;
    
end

xk_ifft3 = zeros(FFTS1,1);

for(idx_bfly = 0:FFTS1/2 - 1)
        
    idxWrite = bitreverse(idx_bfly, NFFT);
    
    xk_ifft3(idxWrite + 1) = inout_re(idx_bfly + 1) * 2; % Factor 2 due to N/2-fft-scaling vs N-ifft-scaling
    xk_ifft3(idxWrite + 1 + 1) = inout_im(idx_bfly + 1) * 2; % Factor 2 due to N/2-fft-scaling vs N-ifft-scaling
    
end

xk = xk_ifft_orig;

diff = xk_ifft3 - xk_ifft_orig;
disp(['--> Result deviation: ' num2str(diff' * diff)]);
    
    
%% =======================================================================
disp('%% IFFT variant No 3: ');
disp('%% - (N/2 + 1)-> complex N/2 IFFT reconstruction, ');
disp('%% - Butterfly IFFT, ');
disp('%% - Lifting scheme with FFT twiddle facs, ');
disp('%% - reconstruction by de-interleaving and bit reverse addr in one operation');
%% =======================================================================

% Initial stage: Recinstruction of complex N/2 IFFT input
tw = exp(i*2*pi*[0:FFTS1/2-1]/FFTS1);
tw_real = real(tw);
tw_imag = imag(tw);

ls_facs_c0 = (tw_facs_fft_real(1:FFTS1/4)-1)./tw_facs_fft_imag(1:FFTS1/4);
ls_facs_s0 = tw_facs_fft_imag(1:FFTS1/4);

ls_facs_c1 = (tw_facs_fft_real(FFTS1/4+1:FFTS1/2)+1)./tw_facs_fft_imag(FFTS1/4+1:FFTS1/2);
ls_facs_s1 = tw_facs_fft_imag(FFTS1/4+1:FFTS1/2);


inout_re = zeros(FFTS1/2,1);
inout_im = zeros(FFTS1/2,1);

% idx = 0
inout_re(1) = (Xk(1) + Xk(FFTS1/2 + 1))/2;
inout_im(1) = (Xk(1) - Xk(FFTS1/2 + 1))/2;

for(idx = 1:FFTS1/4-1)

    tt_real1 = (real(Xk(idx + 1)) + real(Xk(FFTS1/2-idx+1)))/2;
    tt_imag1 = (imag(Xk(idx + 1)) - imag(Xk(FFTS1/2-idx+1)))/2;
    
    t_real = -(imag(Xk(idx + 1)) + imag(Xk(FFTS1/2-idx+1)))/2;
    t_imag = (real(Xk(idx + 1)) - real(Xk(FFTS1/2-idx+1)))/2;
        
    tw_c0 = ls_facs_c0(idx + 1);
    tw_s0 = ls_facs_s0(idx + 1);
    tt_real2 = t_real - tw_c0 * t_imag;
    tt_imag2 = t_imag - tw_s0 * tt_real2;
    tt_real2 = tt_real2 - tw_c0 * tt_imag2;
    
    inout_re(idx + 1) = tt_real1 + tt_real2;
    inout_im(idx + 1) = tt_imag1 + tt_imag2;
end
    
inout_re(FFTS1/4 + 1) = real(Xk(FFTS1/4 + 1));
inout_im(FFTS1/4 + 1) = -(imag(Xk(FFTS1/4 + 1)));

for(idx = 1:FFTS1/4-1)

    tt_real1 = (real(Xk(FFTS1/4 + idx + 1)) + real(Xk(FFTS1/4-idx+1)))/2;
    tt_imag1 = (imag(Xk(FFTS1/4 + idx + 1)) - imag(Xk(FFTS1/4-idx+1)))/2;
    
    t_real = -(imag(Xk(FFTS1/4 + idx + 1)) + imag(Xk(FFTS1/4-idx+1)))/2;
    t_imag = (real(Xk(FFTS1/4 + idx + 1)) - real(Xk(FFTS1/4-idx+1)))/2;
            
    tw_c1 = ls_facs_c1(idx + 1);
    tw_s1 = ls_facs_s1(idx + 1);
    tt_real2 = t_real - tw_c1 * t_imag;
    tt_imag2 = t_imag + tw_s1 * tt_real2;
    tt_real2 = tt_real2 - tw_c1 * tt_imag2;
    
    inout_re(FFTS1/4 + idx + 1) = tt_real1 - tt_real2; % - due to negative output from lifting scheme
    inout_im(FFTS1/4 + idx + 1) = tt_imag1 - tt_imag2; % - due to negative output from lifting scheme
end

% Butterflies
num_bfly = 1;
num_shift_tw = 2;
num_ops_per_bfly = FFTS1P/2;
num_shift_bfly2bfly = FFTS1P/2;

for(idx_stage = 0:NFFTP-2)

    idx_in_x0 = 0;
    idx_in_x1 = num_ops_per_bfly;

    %disp('=== New Butterfly Loop ===');
    for(idx_bfly = 0:num_bfly - 1)
        
        idx_in_tw = 0;
        
        x0_real = inout_re(idx_in_x0+1);
        x0_imag = inout_im(idx_in_x0+1);
        x1_real = inout_re(idx_in_x1+1);
        x1_imag = inout_im(idx_in_x1+1);
        
        tw_fac_real = tw_real(idx_in_tw+1);
        
        x0_real_p = x0_real + x1_real;
        x0_imag_p = x0_imag + x1_imag;
        
        x1_real_p = x0_real - x1_real;
        x1_imag_p = x0_imag - x1_imag;
        
        x1_real = x1_real_p * tw_fac_real;
        x1_imag = x1_imag_p * tw_fac_real;
        
        inout_re(idx_in_x0+1) = x0_real_p;
        inout_im(idx_in_x0+1) = x0_imag_p;
        inout_re(idx_in_x1+1) = x1_real;
        inout_im(idx_in_x1+1) = x1_imag;
        
        idx_in_tw = idx_in_tw + num_shift_tw;
        idx_in_x0 = idx_in_x0 + 1;
        idx_in_x1 = idx_in_x1 + 1;
        
        %disp('=== In-Butterfly-Loop ===');
        for(idx_inbfly=1:num_ops_per_bfly/2-1)
            %disp(['x0[' num2str(idx_in_x0) ']']);
            %disp(['x1[' num2str(idx_in_x1) ']']);
            %disp(['tw[' num2str(idx_in_tw) ']']);
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            x1_real = inout_re(idx_in_x1+1);
            x1_imag = inout_im(idx_in_x1+1);
                       
            x0_real_p = x0_real + x1_real;
            x0_imag_p = x0_imag + x1_imag;
            
            x1_real_p = x0_real - x1_real;
            x1_imag_p = x0_imag - x1_imag;
                       
            tw_c0 = ls_facs_c0(idx_in_tw + 1);
            tw_s0 = ls_facs_s0(idx_in_tw + 1);
            x1_real = x1_real_p - tw_c0 * x1_imag_p;
            x1_imag = x1_imag_p - tw_s0 * x1_real;
            x1_real = x1_real - tw_c0 * x1_imag;
            
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            inout_re(idx_in_x1+1) = x1_real;
            inout_im(idx_in_x1+1) = x1_imag;
            
            idx_in_tw = idx_in_tw + num_shift_tw;
            idx_in_x0 = idx_in_x0 + 1;
            idx_in_x1 = idx_in_x1 + 1;
        end
        
        idx_in_tw = 0;
        
        % First butterfly operation in second quadrant of tw
        x0_real = inout_re(idx_in_x0+1);
        x0_imag = inout_im(idx_in_x0+1);
        x1_real = inout_re(idx_in_x1+1);
        x1_imag = inout_im(idx_in_x1+1);
                
        x0_real_p = x0_real + x1_real;
        x0_imag_p = x0_imag + x1_imag;
        
        x1_real_p = x0_real - x1_real;
        x1_imag_p = x0_imag - x1_imag;
        
        x1_real = - x1_imag_p;
        x1_imag = x1_real_p;
        
        inout_re(idx_in_x0+1) = x0_real_p;
        inout_im(idx_in_x0+1) = x0_imag_p;
        inout_re(idx_in_x1+1) = x1_real;
        inout_im(idx_in_x1+1) = x1_imag;
        
        idx_in_tw = idx_in_tw + num_shift_tw;
        idx_in_x0 = idx_in_x0 + 1;
        idx_in_x1 = idx_in_x1 + 1;
        
        for(idx_inbfly=num_ops_per_bfly/2+1:num_ops_per_bfly-1)
            %disp(['x0[' num2str(idx_in_x0) ']']);
            %disp(['x1[' num2str(idx_in_x1) ']']);
            %disp(['tw[' num2str(idx_in_tw) ']']);
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            x1_real = inout_re(idx_in_x1+1);
            x1_imag = inout_im(idx_in_x1+1);
            
           
            x0_real_p = x0_real + x1_real;
            x0_imag_p = x0_imag + x1_imag;
            
            x1_real_p = -(x0_real - x1_real);
            x1_imag_p = -(x0_imag - x1_imag);
                        
            % Output inversion and computations all put together here..
            tw_c1 = ls_facs_c1(idx_in_tw + 1);
            tw_s1 = ls_facs_s1(idx_in_tw + 1);
            x1_real = x1_real_p - tw_c1 * x1_imag_p;
            x1_imag = x1_imag_p + tw_s1 * x1_real;
            x1_real = x1_real - tw_c1 * x1_imag;
 
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            inout_re(idx_in_x1+1) = x1_real;
            inout_im(idx_in_x1+1) = x1_imag;
            
            idx_in_tw = idx_in_tw + num_shift_tw;
            idx_in_x0 = idx_in_x0 + 1;
            idx_in_x1 = idx_in_x1 + 1;
        end
        
        idx_in_x0 = idx_in_x0 + num_shift_bfly2bfly;
        idx_in_x1 = idx_in_x1 + num_shift_bfly2bfly;
        
    end
    
    num_bfly = num_bfly*2;
    num_shift_tw = num_shift_tw*2;
    num_ops_per_bfly = num_ops_per_bfly / 2;
    num_shift_bfly2bfly = num_shift_bfly2bfly / 2;
    
end

xk_ifft3 = zeros(FFTS1,1);
idx_in_x0 = 0;
idx_in_x1 = num_ops_per_bfly;

% Last stage: Do single butterfly stage and bit reversal readout in 
% parallel
for(idx_bfly = 0:num_bfly - 1)
    
    idxWrite0 = bitreverse(idx_in_x0, NFFT);
    idxWrite1 = bitreverse(idx_in_x1, NFFT);
    
    x0_real = inout_re(idx_in_x0+1);
    x0_imag = inout_im(idx_in_x0+1);
    x1_real = inout_re(idx_in_x1+1);
    x1_imag = inout_im(idx_in_x1+1);
        
    x0_real_p = x0_real + x1_real;
    x0_imag_p = x0_imag + x1_imag;
    
    x1_real_p = x0_real - x1_real;
    x1_imag_p = x0_imag - x1_imag;
    
    x1_real = x1_real_p;
    x1_imag = x1_imag_p;
    
    xk_ifft3(idxWrite0 + 1) = x0_real_p * 2;
    xk_ifft3(idxWrite0 + 1 + 1) = x0_imag_p * 2;
    xk_ifft3(idxWrite1 + 1) = x1_real * 2;
    xk_ifft3(idxWrite1 + 1 + 1) = x1_imag * 2;
  
    idx_in_x0 = idx_in_x0 + 1;
    idx_in_x1 = idx_in_x1 + 1;
    idx_in_x0 = idx_in_x0 + num_shift_bfly2bfly;
    idx_in_x1 = idx_in_x1 + num_shift_bfly2bfly;
    
end

diff = xk_ifft3 - xk_ifft_orig;
disp(['--> Result deviation: ' num2str(diff' * diff)]);
xk = xk_ifft_orig;


%% =======================================================================
disp('%% IFFT variant No 4: ');
disp('%% - (N/2 + 1)-> complex N/2 IFFT reconstruction, ');
disp('%% - Butterfly IFFT, ');
disp('%% - Lifting scheme with FFT twiddle facs, ');
disp('%% - reconstruction by de-interleaving and bit reverse addr in one operation');
disp('%% - load twiddle factor only once and use within butterfly groups');
%% =======================================================================

% Initial stage: Recinstruction of complex N/2 IFFT input
ls_facs_c0 = (tw_facs_fft_real(1:FFTS1/4)-1)./tw_facs_fft_imag(1:FFTS1/4);
ls_facs_s0 = tw_facs_fft_imag(1:FFTS1/4);

inout_re = zeros(FFTS1/2,1);
inout_im = zeros(FFTS1/2,1);

% idx = 0
inout_re(1) = (Xk(1) + Xk(FFTS1/2 + 1))/2;
inout_im(1) = (Xk(1) - Xk(FFTS1/2 + 1))/2;

for(idx = 1:FFTS1/4-1)

    tt_real1 = (real(Xk(idx + 1)) + real(Xk(FFTS1/2-idx+1)))/2;
    tt_imag1 = (imag(Xk(idx + 1)) - imag(Xk(FFTS1/2-idx+1)))/2;
    
    t_real = -(imag(Xk(idx + 1)) + imag(Xk(FFTS1/2-idx+1)))/2;
    t_imag = (real(Xk(idx + 1)) - real(Xk(FFTS1/2-idx+1)))/2;
        
    tw_c0 = ls_facs_c0(idx + 1);
    tw_s0 = ls_facs_s0(idx + 1);
    
    tt_real2 = t_real - tw_c0 * t_imag;
    tt_imag2 = t_imag - tw_s0 * tt_real2;
    tt_real2 = tt_real2 - tw_c0 * tt_imag2;
    
    inout_re(idx + 1) = tt_real1 + tt_real2;
    inout_im(idx + 1) = tt_imag1 + tt_imag2;
    inout_re(FFTS1/2-idx + 1) = tt_real1 - tt_real2; % - due to negative output from lifting scheme
    inout_im(FFTS1/2 - idx + 1) = tt_imag2 - tt_imag1; % - due to negative output from lifting scheme
end
    
inout_re(FFTS1/4 + 1) = real(Xk(FFTS1/4 + 1));
inout_im(FFTS1/4 + 1) = -(imag(Xk(FFTS1/4 + 1)));


% Butterflies
num_bfly = 1;
num_shift_tw = 2;
num_ops_per_bfly = FFTS1P/2;
num_shifts_per_bfly = num_ops_per_bfly*2;

for(idx_stage = 0:NFFTP-2)

    idx_offset = 0;

    % Starting points for the butterfly
    idx_in_x0 = idx_offset;
    idx_in_x1 = idx_offset + num_ops_per_bfly;
    idx_in_tw = 0;

    %disp('=== New Butterfly Loop ===');
    for(idx_bfly = 0:num_bfly - 1)
                
        x0_real = inout_re(idx_in_x0+1);
        x0_imag = inout_im(idx_in_x0+1);
        x1_real = inout_re(idx_in_x1+1);
        x1_imag = inout_im(idx_in_x1+1);
        
       
        x0_real_p = x0_real + x1_real;
        x0_imag_p = x0_imag + x1_imag;
        
        x1_real_p = x0_real - x1_real;
        x1_imag_p = x0_imag - x1_imag;
               
        inout_re(idx_in_x0+1) = x0_real_p;
        inout_im(idx_in_x0+1) = x0_imag_p;
        inout_re(idx_in_x1+1) = x1_real_p;
        inout_im(idx_in_x1+1) = x1_imag_p;
        
        idx_in_x0 = idx_in_x0 + num_shifts_per_bfly;
        idx_in_x1 = idx_in_x1 + num_shifts_per_bfly;

    end
    
    idx_offset = idx_offset + 1;

    %disp('=== In-Butterfly-Loop ===');
    for(idx_inbfly=1:num_ops_per_bfly/2-1)
                
        idx_in_x0 = idx_offset;
        idx_in_x1 = idx_offset + num_ops_per_bfly;
        idx_in_tw = idx_in_tw + num_shift_tw;
        
        % Get twiddle factors - lifting scheme
        tw_c0 = ls_facs_c0(idx_in_tw+1);
        tw_s0 = ls_facs_s0(idx_in_tw+1);
        
        for(idx_bfly = 0:num_bfly - 1)
            %disp(['x0[' num2str(idx_in_x0) ']']);
            %disp(['x1[' num2str(idx_in_x1) ']']);
            %disp(['tw[' num2str(idx_in_tw) ']']);
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            x1_real = inout_re(idx_in_x1+1);
            x1_imag = inout_im(idx_in_x1+1);
                       
            x0_real_p = x0_real + x1_real;
            x0_imag_p = x0_imag + x1_imag;
            
            x1_real_p = x0_real - x1_real;
            x1_imag_p = x0_imag - x1_imag;
                       
            x1_real = x1_real_p - tw_c0 * x1_imag_p;
            x1_imag = x1_imag_p - tw_s0 * x1_real;
            x1_real = x1_real - tw_c0 * x1_imag;
            
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            inout_re(idx_in_x1+1) = x1_real;
            inout_im(idx_in_x1+1) = x1_imag;
            
            idx_in_x0 = idx_in_x0 + num_shifts_per_bfly;
            idx_in_x1 = idx_in_x1 + num_shifts_per_bfly;
        end
        idx_offset = idx_offset + 1;
    end
    
    idx_in_x0 = idx_offset;
    idx_in_x1 = idx_offset + num_ops_per_bfly;
    idx_in_tw = 0;
    
    for(idx_bfly = 0:num_bfly - 1)
        
        % First butterfly operation in second quadrant of tw
        x0_real = inout_re(idx_in_x0+1);
        x0_imag = inout_im(idx_in_x0+1);
        x1_real = inout_re(idx_in_x1+1);
        x1_imag = inout_im(idx_in_x1+1);
                
        x0_real_p = x0_real + x1_real;
        x0_imag_p = x0_imag + x1_imag;
        
        x1_real_p = x0_real - x1_real;
        x1_imag_p = x0_imag - x1_imag;
        
        x1_real = - x1_imag_p;
        x1_imag = x1_real_p;
        
        inout_re(idx_in_x0+1) = x0_real_p;
        inout_im(idx_in_x0+1) = x0_imag_p;
        inout_re(idx_in_x1+1) = x1_real;
        inout_im(idx_in_x1+1) = x1_imag;
        
        idx_in_x0 = idx_in_x0 + num_shifts_per_bfly;
        idx_in_x1 = idx_in_x1 + num_shifts_per_bfly;
    end
    
    idx_offset = idx_offset + 1;
    
    for(idx_inbfly=num_ops_per_bfly/2+1:num_ops_per_bfly-1)
       idx_in_x0 = idx_offset;
        idx_in_x1 = idx_offset + num_ops_per_bfly;
        idx_in_tw = idx_in_tw + num_shift_tw;
        
        % Get twiddle factors - lifting scheme
        tw_c1 = ls_facs_c0(FFTS1/4-idx_in_tw+1);
        tw_s1 = ls_facs_s0(FFTS1/4-idx_in_tw+1);
        
        for(idx_bfly = 0:num_bfly - 1)
            %disp(['x0[' num2str(idx_in_x0) ']']);
            %disp(['x1[' num2str(idx_in_x1) ']']);
            %disp(['tw[' num2str(idx_in_tw) ']']);
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            x1_real = inout_re(idx_in_x1+1);
            x1_imag = inout_im(idx_in_x1+1);
            
           
            x0_real_p = x0_real + x1_real;
            x0_imag_p = x0_imag + x1_imag;
            
            x1_real_p = -(x0_real - x1_real);
            x1_imag_p = -(x0_imag - x1_imag);
                        
            % Output inversion and computations all put together here..
            x1_real = x1_real_p + tw_c1 * x1_imag_p;
            x1_imag = x1_imag_p + tw_s1 * x1_real;
            x1_real = x1_real + tw_c1 * x1_imag;
 
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            inout_re(idx_in_x1+1) = x1_real;
            inout_im(idx_in_x1+1) = x1_imag;
            
            idx_in_x0 = idx_in_x0 + num_shifts_per_bfly;
            idx_in_x1 = idx_in_x1 + num_shifts_per_bfly;
        end
        idx_offset = idx_offset + 1;
        
    end
        
    num_bfly = num_bfly*2;
    num_shift_tw = num_shift_tw*2;
    num_ops_per_bfly = num_ops_per_bfly / 2;
    num_shifts_per_bfly = num_shifts_per_bfly / 2;
    
end

xk_ifft3 = zeros(FFTS1,1);
idx_in_x0 = 0;
idx_in_x1 = 1;

% Last stage: Do single butterfly stage and bit reversal readout in 
% parallel
for(idx_bfly = 0:FFTS1/4 - 1)
    
    idxWrite0 = bitreverse(idx_in_x0, NFFT);
    idxWrite1 = bitreverse(idx_in_x1, NFFT);
    
    x0_real = inout_re(idx_in_x0+1);
    x0_imag = inout_im(idx_in_x0+1);
    x1_real = inout_re(idx_in_x1+1);
    x1_imag = inout_im(idx_in_x1+1);
        
    x0_real_p = x0_real + x1_real;
    x0_imag_p = x0_imag + x1_imag;
    
    x1_real_p = x0_real - x1_real;
    x1_imag_p = x0_imag - x1_imag;
    
    x1_real = x1_real_p;
    x1_imag = x1_imag_p;
    
    xk_ifft3(idxWrite0 + 1) = x0_real_p * 2;
    xk_ifft3(idxWrite0 + 1 + 1) = x0_imag_p * 2;
    xk_ifft3(idxWrite1 + 1) = x1_real * 2;
    xk_ifft3(idxWrite1 + 1 + 1) = x1_imag * 2;
  
    idx_in_x0 = idx_in_x0 + 2;
    idx_in_x1 = idx_in_x1 + 2;
    
end

diff = xk_ifft3 - xk_ifft_orig;
disp(['--> Result deviation: ' num2str(diff' * diff)]);








%% =======================================================================
disp('%% IFFT variant No 5: ');
disp('%% - (N/2 + 1)-> complex N/2 IFFT reconstruction, ');
disp('%% - Butterfly IFFT, ');
disp('%% - reconstruction by de-interleaving and bit reverse addr in one operation');
%% =======================================================================

% Initial stage: Recinstruction of complex N/2 IFFT input
tw = exp(i*2*pi*[0:FFTS1/2-1]/FFTS1);
tw_real = real(tw);
tw_imag = imag(tw);

Xk_fft_N2_real = zeros(FFTS1/2,1);
Xk_fft_N2_imag = zeros(FFTS1/2,1);

for(idx = 1:FFTS1/2-1)

%    disp(['1:' num2str(idx) '::' num2str(FFTS1/2-idx)]);

    tt_real1 = (real(Xk(idx + 1)) + real(Xk(FFTS1/2-idx+1)))/2;
    tt_imag1 = (imag(Xk(idx + 1)) - imag(Xk(FFTS1/2-idx+1)))/2;
    
    t_real = -(imag(Xk(idx + 1)) + imag(Xk(FFTS1/2-idx+1)))/2;
    t_imag = (real(Xk(idx + 1)) - real(Xk(FFTS1/2-idx+1)))/2;
    
    tt_real2 = t_real * tw_real(idx + 1) - t_imag * tw_imag(idx+1);
    tt_imag2 = t_real * tw_imag(idx + 1) + t_imag * tw_real(idx+1);
    
    Xk_fft_N2_real(idx + 1) = tt_real1 + tt_real2;
    Xk_fft_N2_imag(idx + 1) = tt_imag1 + tt_imag2;
end
Xk_fft_N2_real(1) = (Xk(1) + Xk(FFTS1/2 + 1))/2;
Xk_fft_N2_imag(1) = (Xk(1) - Xk(FFTS1/2 + 1))/2;

inout_re = Xk_fft_N2_real;
inout_im = Xk_fft_N2_imag;

% Butterflies
num_bfly = 1;
num_shift_tw = 2;
num_ops_per_bfly = FFTS1P/2;
num_shift_bfly2bfly = FFTS1P/2;

for(idx_stage = 0:NFFTP-2)

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
            
            tw_fac_real = tw_real(idx_in_tw+1);
            tw_fac_imag = tw_imag(idx_in_tw+1);
            
            x0_real_p = x0_real + x1_real;
            x0_imag_p = x0_imag + x1_imag;
            
            x1_real_p = x0_real - x1_real;
            x1_imag_p = x0_imag - x1_imag;
            
            x1_real = x1_real_p * tw_fac_real - x1_imag_p * tw_fac_imag;
            x1_imag = x1_real_p * tw_fac_imag + x1_imag_p * tw_fac_real;
            
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            inout_re(idx_in_x1+1) = x1_real;
            inout_im(idx_in_x1+1) = x1_imag;
            
            idx_in_tw = idx_in_tw + num_shift_tw;
            idx_in_x0 = idx_in_x0 + 1;
            idx_in_x1 = idx_in_x1 + 1;
        end
        
        idx_in_x0 = idx_in_x0 + num_shift_bfly2bfly;
        idx_in_x1 = idx_in_x1 + num_shift_bfly2bfly;
        
    end
    
    num_bfly = num_bfly*2;
    num_shift_tw = num_shift_tw*2;
    num_ops_per_bfly = num_ops_per_bfly / 2;
    num_shift_bfly2bfly = num_shift_bfly2bfly / 2;
    
end

xk_ifft3 = zeros(FFTS1,1);
idx_in_x0 = 0;
idx_in_x1 = num_ops_per_bfly;

% Last stage: Do single butterfly stage and bit reversal readout in 
% parallel
for(idx_bfly = 0:FFTS1/4 - 1)
    
    idxWrite0 = bitreverse(idx_in_x0, NFFT);
    idxWrite1 = bitreverse(idx_in_x1, NFFT);
    
    x0_real = inout_re(idx_in_x0+1);
    x0_imag = inout_im(idx_in_x0+1);
    x1_real = inout_re(idx_in_x1+1);
    x1_imag = inout_im(idx_in_x1+1);
        
    x0_real_p = x0_real + x1_real;
    x0_imag_p = x0_imag + x1_imag;
    
    x1_real_p = x0_real - x1_real;
    x1_imag_p = x0_imag - x1_imag;
    
    x1_real = x1_real_p;
    x1_imag = x1_imag_p;
    
    xk_ifft3(idxWrite0 + 1) = x0_real_p * 2;
    xk_ifft3(idxWrite0 + 1 + 1) = x0_imag_p * 2;
    xk_ifft3(idxWrite1 + 1) = x1_real * 2;
    xk_ifft3(idxWrite1 + 1 + 1) = x1_imag * 2;
  
    idx_in_x0 = idx_in_x0 + 2;
    idx_in_x1 = idx_in_x1 + 2;
    
end

xk = xk_ifft_orig;

diff = xk_ifft3 - xk_ifft_orig;
disp(['--> Result deviation: ' num2str(diff' * diff)]);
    









%% =======================================================================
disp('%% IFFT variant No 6: ');
disp('%% - (N/2 + 1)-> complex N/2 IFFT reconstruction, ');
disp('%% - Butterfly IFFT, ');
disp('%% - reconstruction by de-interleaving and bit reverse addr in one operation');
disp('%% - First release variant IFFT with 5 cycles inner most loop');
%% =======================================================================

% Initial stage: Reconstruction of complex N/2 IFFT input

%% How did I find the overall solution for this loop?
% Imagine we have a 16-IFFT
% We start with out-of-place loop (all index specs in matlab notation with
% C style index)
% 1) in_re(1+1) with in_re(7+1) -> in_re(1+1) : in_im(1+1) with in_im(7+1) -> in_im(1+1)
% 2) in_re(2+1) with in_re(6+1) -> in_re(2+1) : in_im(2+1) with in_im(6+1) -> in_im(2+1)
% 3) in_re(3+1) with in_re(5+1) -> in_re(3+1) : in_im(3+1) with in_im(5+1) -> in_im(3+1)
% 4) in_re(4+1) with in_re(4+1) -> in_re(4+1) : in_im(4+1) with in_im(4+1) -> in_im(4+1)
% 5) in_re(5+1) with in_re(3+1) -> in_re(5+1) : in_im(5+1) with in_im(3+1) -> in_im(5+1)
% 6) in_re(6+1) with in_re(2+1) -> in_re(6+1) : in_im(6+1) with in_im(2+1) -> in_im(6+1)
% 7) in_re(7+1) with in_re(1+1) -> in_re(7+1) : in_im(7+1) with in_im(1+1) -> in_im(7+1)
% Why this does not work in-place? Well we write in_re(1+1) in 1) and use it 
% in 7); hence, we need to do two things in one loop:
% 1) in_re(1+1) with in_re(7+1) -> in_re(1+1) : in_im(1+1) with in_im(7+1) -> in_im(1+1)
%    in_re(7+1) with in_re(1+1) -> in_re(7+1) : in_im(7+1) with in_im(1+1) -> in_im(7+1)
% 2) in_re(2+1) with in_re(6+1) -> in_re(2+1) : in_im(2+1) with in_im(6+1) -> in_im(2+1)
%    in_re(6+1) with in_re(2+1) -> in_re(6+1) : in_im(6+1) with in_im(2+1) -> in_im(6+1)
% 3) in_re(3+1) with in_re(5+1) -> in_re(3+1) : in_im(3+1) with in_im(5+1) -> in_im(3+1)
%    in_re(5+1) with in_re(3+1) -> in_re(5+1) : in_im(5+1) with in_im(3+1) -> in_im(5+1)
% 4) in_re(4+1) with in_re(4+1) -> in_re(4+1) : in_im(4+1) with in_im(4+1) -> in_im(4)
% This works in-place but requires the last line to be a specific case out
% of the loop. 
% Also, we need to modify the sign of the real twiddle factor since 
% tw_real(idx + 1) = - tw_real(FFTS1/2-idx + 1) BUT
% This is compensated by 
% 0.5*(im(idx + 1) - im(FFTS1/2-idx + 1)) = -0.5*(im(idx + 1) - im(FFTS1/2-idx + 1))
% So, the we can save the second complex multiplication and others.
% Finally, we can exploit that
% tw_real(idx + 1) = tw_imag(FFTS1/4 + idx + 1) and tw_imag(idx + 1) = tw_real(FFTS1/4 + idx + 1)
% Hence, we can use the twiddle factors twice in one loop step by swapping real and imaginary
% twiddle factors and end up in the shown structure.
% We end up with a one step loop:
% 1) in_re(1+1) with in_re(7+1) -> in_re(1+1) : in_im(1+1) with in_im(7+1) -> in_im(1+1)
%    in_re(7+1) with in_re(1+1) -> in_re(7+1) : in_im(7+1) with in_im(1+1) -> in_im(7+1)
%    in_re(3+1) with in_re(5+1) -> in_re(3+1) : in_im(3+1) with in_im(5+1) -> in_im(3+1)
%    in_re(5+1) with in_re(3+1) -> in_re(5+1) : in_im(5+1) with in_im(3+1) -> in_im(5+1)
% 2) in_re(2+1) with in_re(6+1) -> in_re(2+1) : in_im(2+1) with in_im(6+1) -> in_im(2+1)
%    in_re(6+1) with in_re(2+1) -> in_re(6+1) : in_im(6+1) with in_im(2+1) -> in_im(6+1)
% 4) in_re(4+1) with in_re(4+1) -> in_re(4+1) : in_im(4+1) with in_im(4+1) -> in_im(4)
% Note: we need a special rule for 2) which is the first half of 1) indeed,
% and we need a special rule for 4).
% With pointers running from the beginning to the end and back in forth
% while jumping by FFTS1/4 inbetween and reading tw_real from beginning and
% from end, we have a rather efficient implementation.

tw = exp(i*2*pi*[0:FFTS1/2-1]/FFTS1);
tw_real = real(tw);
tw_imag = imag(tw);

inout_re = real(Xk);
inout_im = imag(Xk);

i0 = 0;
i2 = FFTS1/2;
i4 = 1;
i5 = FFTS1/4 - 1;

inout_im(i0 + 1) = (inout_re(i0 + 1) - inout_re(i2 + 1))/2;
inout_re(i0 + 1) = (inout_re(i0 + 1) + inout_re(i2 + 1))/2;

inc0 = FFTS1/4;
inc1 = -FFTS1/4 + 1;
inc2 = -FFTS1/4;
inc3 = FFTS1/4 -1;

i0 = i0 + inc0;

inout_im(i0+1) = - inout_im(i0+1);

i0 = i0 + inc1;
i2 = i2 -1;

for(idx = 1:FFTS1/8-1)

    tw_fac_real = tw_real(i4 + 1);
    tw_fac_imag = tw_real(i5 + 1);
    i4 = i4 + 1;
    i5 = i5 - 1;
    
    tt_real11 = (inout_re(i0 + 1) + inout_re(i2+1))/2;
    tt_imag11 = (inout_im(i0 + 1) - inout_im(i2+1))/2;
    
    t_real1 = -(inout_im(i0 + 1) + inout_im(i2+1))/2;
    t_imag1 = (inout_re(i0 + 1) - inout_re(i2+1))/2;
    
    tt_real21 = t_real1 * tw_fac_real - t_imag1 * tw_fac_imag;
    tt_imag21 = t_real1 * tw_fac_imag + t_imag1 * tw_fac_real;
              
    inout_re(i0+1) = tt_real11 + tt_real21;
    inout_im(i0+1) = tt_imag11 + tt_imag21;

    inout_re(i2+1) = tt_real11 - tt_real21;
    inout_im(i2+1) = -tt_imag11 + tt_imag21;
    
    i0 = i0 + inc0;
    i2 = i2 + inc2;
    
    tt_real11 = (inout_re(i2 + 1) + inout_re(i0+1))/2;
    tt_imag11 = (inout_im(i2 + 1) - inout_im(i0+1))/2;
    
    t_real1 = -(inout_im(i2 + 1) + inout_im(i0+1))/2;
    t_imag1 = (inout_re(i2 + 1) - inout_re(i0+1))/2;
    
    tt_real21 = t_real1 * tw_fac_imag - t_imag1 * tw_fac_real;
    tt_imag21 = t_real1 * tw_fac_real + t_imag1 * tw_fac_imag;
       
    inout_re(i2+1) = tt_real11 + tt_real21;
    inout_im(i2+1) = tt_imag11 + tt_imag21;

    inout_re(i0+1) = tt_real11 - tt_real21;
    inout_im(i0+1) = -tt_imag11 + tt_imag21;

    i0 = i0 + inc1;
    i2 = i2 + inc3;

end

% Final special rule
tw_fac_real = tw_real(i0 + 1);
tw_fac_imag = tw_imag(i0+1);

tt_real11 = (inout_re(i0 + 1) + inout_re(i2+1))/2;
tt_imag11 = (inout_im(i0 + 1) - inout_im(i2+1))/2;

t_real1 = -(inout_im(i0 + 1) + inout_im(i2+1))/2;
t_imag1 = (inout_re(i0 + 1) - inout_re(i2+1))/2;

tt_real21 = t_real1 * tw_fac_real - t_imag1 * tw_fac_imag;
tt_imag21 = t_real1 * tw_fac_imag + t_imag1 * tw_fac_real;

inout_re(i0+1) = tt_real11 + tt_real21;
inout_im(i0+1) = tt_imag11 + tt_imag21;

inout_re(i2+1) = tt_real11 - tt_real21;
inout_im(i2+1) = -tt_imag11 + tt_imag21;

%% ================================================================
% What is the trick here:
% 1) Let us propagate through the butterflies such that each twiddle factor
% is only loaded once
% 2) Let us split up the loop into the first and the second half
% Number of iterations in both loops: num_ops_per_bfly/2
% 3) Let us exploit that 
% tw_real(FFTS1P/2-idx_in_tw+1) = tw_imag(idx_in_tw+1)
% -> no more tw_imag required
% 4) Exploit symmetrie for idx_in_tw > FFTS1P/2:
% tw_real(FFTS1P - idx_in_tw+1) = - tw_real(idx_in_tw+1)
% 5) Do the same for second loop as in 3)
% Reset idx_in_tw before second loop yielding the offset -FFTS1P/2 in
% twiddle factor addressing. The twiddle facs in the second loop almost are
% identical to those in the first loop
% 6) Modify the second loop idx in for statement to be identical to that in 
% first loop
% 7) In second loop rename tw_fac_real to tw_fac_imag and move sign from
% line where twiddle factor tw_fac_imag is negative into the equations
% -> line with twiddle factors read are identical in both loops
% Insecond loop, move signs around to yield a real MAC operation 
% (a= b*c% +/- d*e). A form of a = -b*c +/- d*e would be a problem for
% Kalimba.
% 8) Start to use one idx for the inner most loops with inc0 and inc1 to
% move pointer back and forth
% 9) Finally, put the loops together. Note that between first and second
% loop, there is a pointer shift.
% Butterflies
num_bfly = 1;
num_shift_tw = 2;
num_ops_per_bfly = FFTS1P/2;
num_shift_bfly2bfly = FFTS1P/2;

for(idx_stage = 0:NFFTP-2)

    idx_in_x0_start = 0;
    
    idx_in_tw = 0;
    idx_in_tw_bwd = FFTS1P/2;
    
    inc0 = num_ops_per_bfly;
    inc1 = -num_ops_per_bfly;
    inc2 = num_shift_tw;
    inc3 = -num_shift_tw;
    
    %disp('=== New Butterfly Loop ===');
    for(idx_inbfly=0:num_ops_per_bfly/2-1)

        idx_in_x0 = idx_in_x0_start;
        
        tw_fac_real = tw_real(idx_in_tw+1);
        tw_fac_imag = tw_real(idx_in_tw_bwd+1);
        %tw_fac_imag2 = tw_real(FFTS1P/2-idx_in_tw+1);%
        idx_in_tw = idx_in_tw + inc2;
        idx_in_tw_bwd = idx_in_tw_bwd + inc3;
        
        for(idx_bfly = 0:num_bfly - 1)
            %disp('=== In-Butterfly-Loop ===');
            %disp(['x0[' num2str(idx_in_x0) ']']);
            %disp(['x1[' num2str(idx_in_x1) ']']);
            %disp(['tw[' num2str(idx_in_tw) ']']);
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc0;

            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
                        
            x0_real_p = x0_real + x1_real;
            x0_imag_p = x0_imag + x1_imag;
            
            x1_real_p = x0_real - x1_real;
            x1_imag_p = x0_imag - x1_imag;
            
            x1_real = x1_real_p * tw_fac_real - x1_imag_p * tw_fac_imag;
            x1_imag = x1_real_p * tw_fac_imag + x1_imag_p * tw_fac_real;
            
            inout_re(idx_in_x0+1) = x0_real_p;            
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;

            inout_re(idx_in_x0+1) = x1_real;
            inout_im(idx_in_x0+1) = x1_imag;
            idx_in_x0 = idx_in_x0 + inc0;                        
            
        end
        
        idx_in_x0 = idx_in_x0_start + num_ops_per_bfly/2;
        
        for(idx_bfly = 0:num_bfly - 1)
            %disp('=== In-Butterfly-Loop ===');
            %disp(['x0[' num2str(idx_in_x0) ']']);
            %disp(['x1[' num2str(idx_in_x1) ']']);
            %disp(['tw[' num2str(idx_in_tw) ']']);
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc0;
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            x0_real_p = x0_real + x1_real;
            x0_imag_p = x0_imag + x1_imag;
            
            x1_real_p = x1_real - x0_real;
            x1_imag_p = x1_imag - x0_imag;
            
            x1_real = x1_real_p * tw_fac_imag + x1_imag_p * tw_fac_real;
            x1_imag = -x1_real_p * tw_fac_real + x1_imag_p * tw_fac_imag;
            
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
            
            inout_re(idx_in_x0+1) = x1_real;
            inout_im(idx_in_x0+1) = x1_imag;
            idx_in_x0 = idx_in_x0 + inc0;

        end
        
        idx_in_x0_start = idx_in_x0_start + 1;
        
    end
    
    num_bfly = num_bfly*2;
    num_shift_tw = num_shift_tw*2;
    num_ops_per_bfly = num_ops_per_bfly / 2;
    num_shift_bfly2bfly = num_shift_bfly2bfly / 2;
    
end

% Last stage, final operations and the bit reversal output
xk_ifft3 = zeros(FFTS1,1);
idx_in_x0 = 0;
idx_in_x1 = num_ops_per_bfly;

% Last stage: Do single butterfly stage and bit reversal readout in 
% parallel
for(idx_bfly = 0:FFTS1/4 - 1)
    
    idxWrite0 = bitreverse(idx_in_x0, NFFT);
    idxWrite1 = bitreverse(idx_in_x1, NFFT);
    
    x0_real = inout_re(idx_in_x0+1);
    x0_imag = inout_im(idx_in_x0+1);
    x1_real = inout_re(idx_in_x1+1);
    x1_imag = inout_im(idx_in_x1+1);
        
    x0_real_p = x0_real + x1_real;
    x0_imag_p = x0_imag + x1_imag;
    
    x1_real_p = x0_real - x1_real;
    x1_imag_p = x0_imag - x1_imag;
    
    x1_real = x1_real_p;
    x1_imag = x1_imag_p;
    
    xk_ifft3(idxWrite0 + 1) = x0_real_p * 2;
    xk_ifft3(idxWrite0 + 1 + 1) = x0_imag_p * 2;
    xk_ifft3(idxWrite1 + 1) = x1_real * 2;
    xk_ifft3(idxWrite1 + 1 + 1) = x1_imag * 2;
  
    idx_in_x0 = idx_in_x0 + 2;
    idx_in_x1 = idx_in_x1 + 2;
    
end

xk = xk_ifft_orig;

diff = xk_ifft3 - xk_ifft_orig;
disp(['--> Result deviation: ' num2str(diff' * diff)]);
















%% =======================================================================
disp('%% FFT variant No 7: ');
disp('%% - Butterfly algorithm, ');
disp('%% - full length, ');
disp('%% - Bit reversal addressing on input side!!');
%% =======================================================================
inout_re = zeros(FFTS1, 1);
inout_im = zeros(FFTS1, 1);

for(idx = 0:FFTS1-1)
    idxWrite = idx;
    idxRead = bitreverse(idx, NFFT);
    inout_re(idxWrite+1) = real(Xk_full(idxRead + 1));
    inout_im(idxWrite+1) = imag(Xk_full(idxRead + 1));
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
            tw_fac_imag = -tw_facs_imag(idx_in_tw+1);
            
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

xk_new = inout_re;

diff = xk_new - xk_ifft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);


%% =======================================================================
disp('%% FFT variant No 8: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Bit reversal addressing on input side!!');
%% =======================================================================
out_re = zeros(FFTS1P, 1);
out_im = zeros(FFTS1P, 1);




tw = exp(i*2*pi*[0:FFTS1/2-1]/FFTS1);
tw_real = real(tw);
tw_imag = imag(tw);

inout_re = real(Xk);
inout_im = imag(Xk);

i0 = 0;
i2 = FFTS1/2;
i4 = 1;
i5 = FFTS1/4 - 1;

out_im(i0 + 1) = (inout_re(i0 + 1) - inout_re(i2 + 1))/2;
out_re(i0 + 1) = (inout_re(i0 + 1) + inout_re(i2 + 1))/2;

inc0 = FFTS1/4;
inc1 = -FFTS1/4 + 1;
inc2 = -FFTS1/4;
inc3 = FFTS1/4 -1;

i0 = i0 + inc0;

out_re(i0+1) = inout_re(i0+1);
out_im(i0+1) = - inout_im(i0+1);

i0 = i0 + inc1;
i2 = i2 -1;

for(idx = 1:FFTS1/8-1)

    tw_fac_real = tw_real(i4 + 1);
    tw_fac_imag = tw_real(i5 + 1);
    i4 = i4 + 1;
    i5 = i5 - 1;
    
    tt_real11 = (inout_re(i0 + 1) + inout_re(i2+1))/2;
    tt_imag11 = (inout_im(i0 + 1) - inout_im(i2+1))/2;
    
    t_real1 = -(inout_im(i0 + 1) + inout_im(i2+1))/2;
    t_imag1 = (inout_re(i0 + 1) - inout_re(i2+1))/2;
    
    tt_real21 = t_real1 * tw_fac_real - t_imag1 * tw_fac_imag;
    tt_imag21 = t_real1 * tw_fac_imag + t_imag1 * tw_fac_real;
              
    out_re(i0+1) = tt_real11 + tt_real21;
    out_im(i0+1) = tt_imag11 + tt_imag21;

    out_re(i2+1) = tt_real11 - tt_real21;
    out_im(i2+1) = -tt_imag11 + tt_imag21;
    
    i0 = i0 + inc0;
    i2 = i2 + inc2;
    
    tt_real11 = (inout_re(i2 + 1) + inout_re(i0+1))/2;
    tt_imag11 = (inout_im(i2 + 1) - inout_im(i0+1))/2;
    
    t_real1 = -(inout_im(i2 + 1) + inout_im(i0+1))/2;
    t_imag1 = (inout_re(i2 + 1) - inout_re(i0+1))/2;
    
    tt_real21 = t_real1 * tw_fac_imag - t_imag1 * tw_fac_real;
    tt_imag21 = t_real1 * tw_fac_real + t_imag1 * tw_fac_imag;
       
    out_re(i2+1) = tt_real11 + tt_real21;
    out_im(i2+1) = tt_imag11 + tt_imag21;

    out_re(i0+1) = tt_real11 - tt_real21;
    out_im(i0+1) = -tt_imag11 + tt_imag21;

    i0 = i0 + inc1;
    i2 = i2 + inc3;

end

% Final special rule
tw_fac_real = tw_real(i0 + 1);
tw_fac_imag = tw_imag(i0+1);

tt_real11 = (inout_re(i0 + 1) + inout_re(i2+1))/2;
tt_imag11 = (inout_im(i0 + 1) - inout_im(i2+1))/2;

t_real1 = -(inout_im(i0 + 1) + inout_im(i2+1))/2;
t_imag1 = (inout_re(i0 + 1) - inout_re(i2+1))/2;

tt_real21 = t_real1 * tw_fac_real - t_imag1 * tw_fac_imag;
tt_imag21 = t_real1 * tw_fac_imag + t_imag1 * tw_fac_real;

out_re(i0+1) = tt_real11 + tt_real21;
out_im(i0+1) = tt_imag11 + tt_imag21;

out_re(i2+1) = tt_real11 - tt_real21;
out_im(i2+1) = -tt_imag11 + tt_imag21;

fft_tmp = ifft(out_re + i*out_im) * FFTS1P * 2;

for(ind = 0:FFTS1P-1)
    xk_new(2*ind + 1) = real(fft_tmp(ind + 1));
    xk_new(2*ind + 1 + 1) = imag(fft_tmp(ind + 1));
end

diff = xk_new - xk_ifft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);











%% =======================================================================
disp('%% FFT variant No 9: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Bit reversal addressing on input side!!');
disp('%% - half length IFFT realized in code - non optimized');
%% =======================================================================
out_re = zeros(FFTS1P, 1);
out_im = zeros(FFTS1P, 1);




tw = exp(i*2*pi*[0:FFTS1/2-1]/FFTS1);
tw_real = real(tw);
tw_imag = imag(tw);

inout_re = real(Xk);
inout_im = imag(Xk);

i0 = 0;
i2 = FFTS1/2;
i4 = 1;
i5 = FFTS1/4 - 1;

out_im(i0 + 1) = (inout_re(i0 + 1) - inout_re(i2 + 1))/2;
out_re(i0 + 1) = (inout_re(i0 + 1) + inout_re(i2 + 1))/2;

inc0 = FFTS1/4;
inc1 = -FFTS1/4 + 1;
inc2 = -FFTS1/4;
inc3 = FFTS1/4 -1;

i0 = i0 + inc0;

out_re(i0+1) = inout_re(i0+1);
out_im(i0+1) = - inout_im(i0+1);

i0 = i0 + inc1;
i2 = i2 -1;

for(idx = 1:FFTS1/8-1)

    tw_fac_real = tw_real(i4 + 1);
    tw_fac_imag = tw_real(i5 + 1);
    i4 = i4 + 1;
    i5 = i5 - 1;
    
    tt_real11 = (inout_re(i0 + 1) + inout_re(i2+1))/2;
    tt_imag11 = (inout_im(i0 + 1) - inout_im(i2+1))/2;
    
    t_real1 = -(inout_im(i0 + 1) + inout_im(i2+1))/2;
    t_imag1 = (inout_re(i0 + 1) - inout_re(i2+1))/2;
    
    tt_real21 = t_real1 * tw_fac_real - t_imag1 * tw_fac_imag;
    tt_imag21 = t_real1 * tw_fac_imag + t_imag1 * tw_fac_real;
              
    out_re(i0+1) = tt_real11 + tt_real21;
    out_im(i0+1) = tt_imag11 + tt_imag21;

    out_re(i2+1) = tt_real11 - tt_real21;
    out_im(i2+1) = -tt_imag11 + tt_imag21;
    
    i0 = i0 + inc0;
    i2 = i2 + inc2;
    
    tt_real11 = (inout_re(i2 + 1) + inout_re(i0+1))/2;
    tt_imag11 = (inout_im(i2 + 1) - inout_im(i0+1))/2;
    
    t_real1 = -(inout_im(i2 + 1) + inout_im(i0+1))/2;
    t_imag1 = (inout_re(i2 + 1) - inout_re(i0+1))/2;
    
    tt_real21 = t_real1 * tw_fac_imag - t_imag1 * tw_fac_real;
    tt_imag21 = t_real1 * tw_fac_real + t_imag1 * tw_fac_imag;
       
    out_re(i2+1) = tt_real11 + tt_real21;
    out_im(i2+1) = tt_imag11 + tt_imag21;

    out_re(i0+1) = tt_real11 - tt_real21;
    out_im(i0+1) = -tt_imag11 + tt_imag21;

    i0 = i0 + inc1;
    i2 = i2 + inc3;

end

% Final special rule
tw_fac_real = tw_real(i0 + 1);
tw_fac_imag = tw_imag(i0+1);

tt_real11 = (inout_re(i0 + 1) + inout_re(i2+1))/2;
tt_imag11 = (inout_im(i0 + 1) - inout_im(i2+1))/2;

t_real1 = -(inout_im(i0 + 1) + inout_im(i2+1))/2;
t_imag1 = (inout_re(i0 + 1) - inout_re(i2+1))/2;

tt_real21 = t_real1 * tw_fac_real - t_imag1 * tw_fac_imag;
tt_imag21 = t_real1 * tw_fac_imag + t_imag1 * tw_fac_real;

out_re(i0+1) = tt_real11 + tt_real21;
out_im(i0+1) = tt_imag11 + tt_imag21;

out_re(i2+1) = tt_real11 - tt_real21;
out_im(i2+1) = -tt_imag11 + tt_imag21;

% ===========================================
% Bit reversal addressing on input side
% ===========================================
for(idx = 0:FFTS1/2-1)
    idxWrite = idx;
    idxRead = bitreverse(idx, NFFT-1);
    inout_re(idxWrite+1) = out_re(idxRead + 1);
    inout_im(idxWrite+1) = out_im(idxRead + 1);
end

% ===========================================
% Final "IFFT" in "FFT" layout 
% ===========================================
tw_facs = exp(i*2*pi/FFTS1P * [0:FFTS1P/2-1]);
tw_facs_real = real(tw_facs);
tw_facs_imag = imag(tw_facs);

num_bfly = FFTS1P/2;
num_ops_per_bfly = 1;
num_shift_bfly2bfly = 1;
num_shift_tw = num_bfly;




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
     
    
for(idx_stage = 1:NFFTP-1)

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

for(ind = 0:FFTS1P-1)
    xk_new(2*ind + 1) = inout_re(ind + 1) * 2;
    xk_new(2*ind + 1 + 1) = inout_im(ind + 1) * 2;
end

diff = xk_new - xk_ifft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);

%xk_new




% Version 9 -> 10: Accidentially, we had two sets of twiddle factors in
% version 9. Also, the twiddle factors were not identical to those in the
% fft. This has been fixed in version 10
%% =======================================================================
disp('%% IFFT variant No 10: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Bit reversal addressing on input side!!');
disp('%% - half length IFFT realized in code');
disp('%% - Second release variant IFFT with out-of-place bit reversal addressing');
%% =======================================================================
out_re = zeros(FFTS1P, 1);
out_im = zeros(FFTS1P, 1);

tw = exp(i*2*pi*[0:FFTS1/2-1]/FFTS1);
tw_real = real(tw);

inout_re = real(Xk);
inout_im = imag(Xk);

i0 = 0;
i2 = FFTS1/2;
i4 = 1;
i5 = FFTS1/4 - 1;

out_im(i0 + 1) = (inout_re(i0 + 1) - inout_re(i2 + 1))/2;
out_re(i0 + 1) = (inout_re(i0 + 1) + inout_re(i2 + 1))/2;

inc0 = FFTS1/4;
inc1 = -FFTS1/4 + 1;
inc2 = -FFTS1/4;
inc3 = FFTS1/4 -1;

i0 = i0 + inc0;

out_re(i0+1) = inout_re(i0+1);
out_im(i0+1) = - inout_im(i0+1);

i0 = i0 + inc1;
i2 = i2 -1;

for(idx = 1:FFTS1/8-1)

    tw_fac_real = tw_real(i4 + 1);
    tw_fac_imag = tw_real(i5 + 1);
    i4 = i4 + 1;
    i5 = i5 - 1;
    
    tt_real11 = (inout_re(i0 + 1) + inout_re(i2+1))/2;
    tt_imag11 = (inout_im(i0 + 1) - inout_im(i2+1))/2;
    
    t_real1 = -(inout_im(i0 + 1) + inout_im(i2+1))/2;
    t_imag1 = (inout_re(i0 + 1) - inout_re(i2+1))/2;
    
    tt_real21 = t_real1 * tw_fac_real - t_imag1 * tw_fac_imag;
    tt_imag21 = t_real1 * tw_fac_imag + t_imag1 * tw_fac_real;
              
    out_re(i0+1) = tt_real11 + tt_real21;
    out_im(i0+1) = tt_imag11 + tt_imag21;

    out_re(i2+1) = tt_real11 - tt_real21;
    out_im(i2+1) = -tt_imag11 + tt_imag21;
    
    i0 = i0 + inc0;
    i2 = i2 + inc2;
    
    tt_real11 = (inout_re(i2 + 1) + inout_re(i0+1))/2;
    tt_imag11 = (inout_im(i2 + 1) - inout_im(i0+1))/2;
    
    t_real1 = -(inout_im(i2 + 1) + inout_im(i0+1))/2;
    t_imag1 = (inout_re(i2 + 1) - inout_re(i0+1))/2;
    
    tt_real21 = t_real1 * tw_fac_imag - t_imag1 * tw_fac_real;
    tt_imag21 = t_real1 * tw_fac_real + t_imag1 * tw_fac_imag;
       
    out_re(i2+1) = tt_real11 + tt_real21;
    out_im(i2+1) = tt_imag11 + tt_imag21;

    out_re(i0+1) = tt_real11 - tt_real21;
    out_im(i0+1) = -tt_imag11 + tt_imag21;

    i0 = i0 + inc1;
    i2 = i2 + inc3;

end

% Final special rule
tw_fac_real = tw_real(i0 + 1);
%tw_fac_imag = tw_imag(i0+1); same as real value (sqrt(2)

tt_real11 = (inout_re(i0 + 1) + inout_re(i2+1))/2;
tt_imag11 = (inout_im(i0 + 1) - inout_im(i2+1))/2;

t_real1 = -(inout_im(i0 + 1) + inout_im(i2+1))/2;
t_imag1 = (inout_re(i0 + 1) - inout_re(i2+1))/2;

tt_real21 = t_real1 * tw_fac_real - t_imag1 * tw_fac_real;
tt_imag21 = t_real1 * tw_fac_real + t_imag1 * tw_fac_real;

out_re(i0+1) = tt_real11 + tt_real21;
out_im(i0+1) = tt_imag11 + tt_imag21;

out_re(i2+1) = tt_real11 - tt_real21;
out_im(i2+1) = -tt_imag11 + tt_imag21;

% ===========================================
% Final "IFFT" in "FFT" layout 
% ===========================================

tw_facs = exp(-i*2*pi/FFTS1P * [0:FFTS1P/2]);
tw_facs_real = real(tw_facs);
tw_facs_imag = [];
% ===========================================
% First stage of loop separately, otherwise following optimization will not work
% Exploits positively that there is only one twiddle factor which is 1 for
% real and 0 for imaginary part
% Bit reversal addressing on input side
% There is NO way to combine this with the
% butterfly, the index is just to unregular!!
% We should, however, use this loop to normalize the 
% FFT bin so that we end up in Q23
% On a DSP this requires out-of-place buffers!!!
% ===========================================
idx_in_x0 = 0;

for(idx_bfly = 0:FFTS1P/2 - 1)
    
    idxRead = bitreverse(idx_in_x0, NFFT-1);
    
    x0_real = out_re(idxRead +1);
    x0_imag = out_im(idxRead +1);
    idx_in_x0 = idx_in_x0 + 1;
    
    idxRead = bitreverse(idx_in_x0, NFFT-1);
    x1_real = out_re(idxRead+1);
    x1_imag = out_im(idxRead+1);
    idx_in_x0 = idx_in_x0 - 1;
    
    x0_real_p = x0_real + x1_real;
    x0_imag_p = x0_imag + x1_imag;
    
    x1_real_p = x0_real - x1_real;
    x1_imag_p = x0_imag - x1_imag;
    
    inout_re(idx_in_x0+1) = x0_real_p;
    inout_im(idx_in_x0+1) = x0_imag_p;
    idx_in_x0 = idx_in_x0 + 1;
    inout_re(idx_in_x0+1) = x1_real_p;
    inout_im(idx_in_x0+1) = x1_imag_p;
    idx_in_x0 = idx_in_x0 + 1;
    
end
      

% ===================================================================
% Rest of loops up to very final stage
% ===================================================================
num_bfly = FFTS1P/4;
num_ops_per_bfly = 2;
num_shift_bfly2bfly = 2;
num_shift_tw = num_bfly;

for(idx_stage = 1:NFFTP-2)

    idx_in_stage_start = 0;
    idx_in_tw = 0;
    inc0 = num_shift_bfly2bfly;
    inc1 = -num_shift_bfly2bfly;
    
    for(idx_inbfly=0:num_ops_per_bfly/2-1)

        idx_in_x0 = idx_in_stage_start;
        tw_fac_real = tw_facs_real(idx_in_tw+1);
        tw_fac_imag = tw_facs_real(FFTS1P/4 - idx_in_tw+1);

        for(idx_bfly = 0:num_bfly - 1)
        
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc0;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            compl_mult1 = tw_fac_real * x1_real - tw_fac_imag * x1_imag;
            compl_mult2 = tw_fac_real * x1_imag + tw_fac_imag * x1_real;
            
            x0_real_p = x0_real + compl_mult1;
            x0_imag_p = x0_imag + compl_mult2;
            
            x1_real_p = x0_real - compl_mult1;
            x1_imag_p = x0_imag - compl_mult2;
            
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
        end
                
        idx_in_x0 = idx_in_stage_start + num_ops_per_bfly/2;
               
        for(idx_bfly = 0:num_bfly - 1)
        
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc0;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            compl_mult1 = -tw_fac_imag * x1_real - tw_fac_real * x1_imag;
            compl_mult2 = -tw_fac_imag * x1_imag + tw_fac_real * x1_real;
            
            x0_real_p = x0_real + compl_mult1;
            x0_imag_p = x0_imag + compl_mult2;
            
            x1_real_p = x0_real - compl_mult1;
            x1_imag_p = x0_imag - compl_mult2;
            
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
        end
                
        idx_in_tw = idx_in_tw + num_shift_tw;
        idx_in_stage_start = idx_in_stage_start + 1;        
 
    end
    
    
    num_bfly = num_bfly/2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly * 2;
    num_shift_bfly2bfly = num_shift_bfly2bfly * 2;
    
end

% ===================================================================
% Last stage: Last twiddle factors plus Interleaved output
% ===================================================================
idx_in_x0 = 0;
idx_in_x1 = idx_in_x0 + FFTS1/4;
%idx_in_x0 = num_ops_per_bfly;
idx_out_0 = 0;
idx_out_1 = idx_out_0 + FFTS1/4;

inc4 = FFTS1/2 - 1;
inc5 = -FFTS1/2 + 1;
inc6 = FFTS1/8;
inc7 = -FFTS1/8 + 1;

idx_in_tw_fwd = 0;
idx_in_tw_bwd = FFTS1P/4;
%disp('=== In-Butterfly-Loop ===');
for(idx_inbfly=0:FFTS1/8-1)

    % Read in twiddle factors, update pointer
    tw_fac_real = tw_facs_real(idx_in_tw_fwd+1);
    tw_fac_imag = tw_facs_real(idx_in_tw_bwd + 1);    
    idx_in_tw_fwd = idx_in_tw_fwd + 1;
    idx_in_tw_bwd = idx_in_tw_bwd - 1;

    % Read in first input
    x0_real = inout_re(idx_in_x0+1);
    x0_imag = inout_im(idx_in_x0+1);
    idx_in_x0 = idx_in_x0 + inc6;
    
    % Read in second input
    x1_real = inout_re(idx_in_x1+1);
    x1_imag = inout_im(idx_in_x1+1);
    idx_in_x1 = idx_in_x1 + inc6;
    
    % Do complex multiplication
    tmp_1 = tw_fac_real * x1_real - tw_fac_imag * x1_imag;
    tmp_2 = tw_fac_real * x1_imag + tw_fac_imag * x1_real;
    
    % Use complex multiplication outputs
    x0_real_p = x0_real + tmp_1;
    x0_imag_p = x0_imag + tmp_2;
    
    % Use complex multiplication outputs
    x1_real_p = x0_real - tmp_1;
    x1_imag_p = x0_imag - tmp_2;
    
    % Write output to linear buffer
    xk_new(idx_out_0 + 1) = x0_real_p * 2;
    idx_out_0 = idx_out_0 + 1;
    xk_new(idx_out_0 + 1) = x0_imag_p * 2;
    idx_out_0 = idx_out_0 + inc4;
    xk_new(idx_out_0 + 1) = x1_real_p * 2;
    idx_out_0 = idx_out_0 + 1;
    xk_new(idx_out_0 + 1) = x1_imag_p * 2;
    idx_out_0 = idx_out_0 + inc5;
   
    % Read in third input
    x0_real = inout_re(idx_in_x0+1);
    x0_imag = inout_im(idx_in_x0+1);
    idx_in_x0 = idx_in_x0 + inc7;
    
    % Read in fourth input
    x1_real = inout_re(idx_in_x1+1);
    x1_imag = inout_im(idx_in_x1+1);
    idx_in_x1 = idx_in_x1 + inc7;
    
    % Do complex multiplication
    tmp_1 = tw_fac_imag * x1_real + tw_fac_real * x1_imag;
    tmp_2 = tw_fac_imag * x1_imag - tw_fac_real * x1_real;
    
    % Use complex multiplication outputs
    x0_real_p = x0_real - tmp_1;
    x0_imag_p = x0_imag - tmp_2;
    
    % Use complex multiplication outputs
    x1_real_p = x0_real + tmp_1;
    x1_imag_p = x0_imag + tmp_2;
    
    % Write output to linear buffer
    xk_new(idx_out_1 + 1) = x0_real_p * 2;
    idx_out_1 = idx_out_1 + 1;
    xk_new(idx_out_1 + 1) = x0_imag_p * 2;
    idx_out_1 = idx_out_1 + inc4;
    xk_new(idx_out_1 + 1) = x1_real_p * 2;
    idx_out_1 = idx_out_1 + 1;
    xk_new(idx_out_1 + 1) = x1_imag_p * 2;
    idx_out_1 = idx_out_1 + inc5;
   
end
%xk_new

diff = xk_new - xk_ifft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);




% Version 10 -> 11: Moved all /2 and *2 away from the corresponding loops
% and combines all of this in the bit reversal loop for noamlization 
%% =======================================================================
disp('%% IFFT variant No 11: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Bit reversal addressing on input side!!');
disp('%% - half length IFFT realized in code');
disp('%% - Second release variant IFFT with out-of-place bit reversal addressing');
%% =======================================================================
out_re = zeros(FFTS1P, 1);
out_im = zeros(FFTS1P, 1);
xk_new = zeros(FFTS1, 1);

tw = exp(-i*2*pi*[0:FFTS1/4]/FFTS1);
tw_real = real(tw);


inout_re = real(Xk);
inout_im = imag(Xk);

i0 = 0;
i2 = FFTS1/2;
i4 = 1;
i5 = FFTS1/4 - 1;

tmp1 = (inout_re(i0 + 1) - inout_re(i2 + 1));
tmp2 = (inout_re(i0 + 1) + inout_re(i2 + 1));

inout_im(i0 + 1) = tmp1;
inout_re(i0 + 1) = tmp2;

inc0 = FFTS1/4;
inc1 = -FFTS1/4 + 1;
inc2 = -FFTS1/4;
inc3 = FFTS1/4 -1;

i0 = i0 + inc0;

inout_re(i0+1) = inout_re(i0+1) + inout_re(i0+1);
inout_im(i0+1) = - (inout_im(i0+1) + inout_im(i0+1));

i0 = i0 + inc1;
i2 = i2 -1;

for(idx = 1:FFTS1/8-1)

    tw_fac_real = tw_real(i4 + 1);
    tw_fac_imag = tw_real(i5 + 1);
    i4 = i4 + 1;
    i5 = i5 - 1;
    
    tt_real11 = (inout_re(i0 + 1) + inout_re(i2+1));
    tt_imag11 = (inout_im(i0 + 1) - inout_im(i2+1));
    
    t_real1 = -(inout_im(i0 + 1) + inout_im(i2+1));
    t_imag1 = (inout_re(i0 + 1) - inout_re(i2+1));
    
    tt_real21 = t_real1 * tw_fac_real - t_imag1 * tw_fac_imag;
    tt_imag21 = t_real1 * tw_fac_imag + t_imag1 * tw_fac_real;
              
    inout_re(i0+1) = tt_real11 + tt_real21;
    inout_im(i0+1) = tt_imag11 + tt_imag21;

    inout_re(i2+1) = tt_real11 - tt_real21;
    inout_im(i2+1) = -tt_imag11 + tt_imag21;
    
    i0 = i0 + inc0;
    i2 = i2 + inc2;
    
    tt_real11 = (inout_re(i2 + 1) + inout_re(i0+1));
    tt_imag11 = (inout_im(i2 + 1) - inout_im(i0+1));
    
    t_real1 = -(inout_im(i2 + 1) + inout_im(i0+1));
    t_imag1 = (inout_re(i2 + 1) - inout_re(i0+1));
    
    tt_real21 = t_real1 * tw_fac_imag - t_imag1 * tw_fac_real;
    tt_imag21 = t_real1 * tw_fac_real + t_imag1 * tw_fac_imag;
       
    inout_re(i2+1) = tt_real11 + tt_real21;
    inout_im(i2+1) = tt_imag11 + tt_imag21;

    inout_re(i0+1) = tt_real11 - tt_real21;
    inout_im(i0+1) = -tt_imag11 + tt_imag21;

    i0 = i0 + inc1;
    i2 = i2 + inc3;

end

% Final special rule
tw_fac_real = tw_real(i0 + 1);
%tw_fac_imag = tw_imag(i0+1); same as real value (sqrt(2)

tt_real11 = (inout_re(i0 + 1) + inout_re(i2+1));
tt_imag11 = (inout_im(i0 + 1) - inout_im(i2+1));

t_real1 = -(inout_im(i0 + 1) + inout_im(i2+1));
t_imag1 = (inout_re(i0 + 1) - inout_re(i2+1));

tt_real21 = t_real1 * tw_fac_real - t_imag1 * tw_fac_real;
tt_imag21 = t_real1 * tw_fac_real + t_imag1 * tw_fac_real;

inout_re(i0+1) = tt_real11 + tt_real21;
inout_im(i0+1) = tt_imag11 + tt_imag21;

inout_re(i2+1) = tt_real11 - tt_real21;
inout_im(i2+1) = -tt_imag11 + tt_imag21;

for(idx = 0:FFTS1/2 - 1)
    
    idxWrite = bitreverse(idx, NFFT-1);
    out_re(idxWrite+1) = inout_re(idx+1)/2*2;    
    out_im(idxWrite+1) = inout_im(idx+1)/2*2;
end

% ===========================================
% Final "IFFT" in "FFT" layout 
% ===========================================

% ===========================================
% First stage of loop separately, otherwise following optimization will not work
% Exploits positively that there is only one twiddle factor which is 1 for
% real and 0 for imaginary part
% Bit reversal addressing on input side
% There is NO way to combine this with the
% butterfly, the index is just to unregular!!
% We should, however, use this loop to normalize the 
% FFT bin so that we end up in Q23
% On a DSP this requires out-of-place buffers!!!
% ===========================================
idx_in_x0 = 0;

for(idx_bfly = 0:FFTS1P/2 - 1)
    
    %idxRead = bitreverse(idx_in_x0, NFFT-1);
    idxRead = idx_in_x0;
    
    x0_real = out_re(idxRead +1);
    x0_imag = out_im(idxRead +1);
    idx_in_x0 = idx_in_x0 + 1;
    
    %idxRead = bitreverse(idx_in_x0, NFFT-1);
    idxRead = idx_in_x0;
    x1_real = out_re(idxRead+1);
    x1_imag = out_im(idxRead+1);
    idx_in_x0 = idx_in_x0 - 1;
    
    x0_real_p = x0_real + x1_real;
    x0_imag_p = x0_imag + x1_imag;
    
    x1_real_p = x0_real - x1_real;
    x1_imag_p = x0_imag - x1_imag;
    
    inout_re(idx_in_x0+1) = x0_real_p;
    inout_im(idx_in_x0+1) = x0_imag_p;
    idx_in_x0 = idx_in_x0 + 1;
    inout_re(idx_in_x0+1) = x1_real_p;
    inout_im(idx_in_x0+1) = x1_imag_p;
    idx_in_x0 = idx_in_x0 + 1;
    
end
      

% ===================================================================
% Rest of loops up to very final stage
% ===================================================================
num_bfly = FFTS1P/4;
num_ops_per_bfly = 2;
num_shift_bfly2bfly = 2;
num_shift_tw = num_bfly * 2;

for(idx_stage = 1:NFFTP-2)

    idx_in_stage_start = 0;
    idx_in_tw = 0;
    inc0 = num_shift_bfly2bfly;
    inc1 = -num_shift_bfly2bfly;
    
    for(idx_inbfly=0:num_ops_per_bfly/2-1)

        idx_in_x0 = idx_in_stage_start;
        tw_fac_real = tw_real(idx_in_tw+1);
        tw_fac_imag = tw_real(FFTS1/4 - idx_in_tw+1);

        %txc = tw_real(idx_in_tw+1);
        
        for(idx_bfly = 0:num_bfly - 1)
        
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc0;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            compl_mult1 = tw_fac_real * x1_real - tw_fac_imag * x1_imag;
            compl_mult2 = tw_fac_real * x1_imag + tw_fac_imag * x1_real;
            
            x0_real_p = x0_real + compl_mult1;
            x0_imag_p = x0_imag + compl_mult2;
            
            x1_real_p = x0_real - compl_mult1;
            x1_imag_p = x0_imag - compl_mult2;
            
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
        end
                
        idx_in_x0 = idx_in_stage_start + num_ops_per_bfly/2;
               
        for(idx_bfly = 0:num_bfly - 1)
        
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc0;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            compl_mult1 = -tw_fac_imag * x1_real - tw_fac_real * x1_imag;
            compl_mult2 = -tw_fac_imag * x1_imag + tw_fac_real * x1_real;
            
            x0_real_p = x0_real + compl_mult1;
            x0_imag_p = x0_imag + compl_mult2;
            
            x1_real_p = x0_real - compl_mult1;
            x1_imag_p = x0_imag - compl_mult2;
            
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
        end
                
        idx_in_tw = idx_in_tw + num_shift_tw;
        idx_in_stage_start = idx_in_stage_start + 1;        
 
    end
    
    
    num_bfly = num_bfly/2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly * 2;
    num_shift_bfly2bfly = num_shift_bfly2bfly * 2;
    
end

% ===================================================================
% Last stage: Last twiddle factors plus Interleaved output
% ===================================================================
idx_in_x0 = 0;
idx_in_x1 = idx_in_x0 + FFTS1/4;
%idx_in_x0 = num_ops_per_bfly;
idx_out_0 = 0;
idx_out_1 = idx_out_0 + FFTS1/4;

inc4 = FFTS1/2 - 1;
inc5 = -FFTS1/2 + 1;
inc6 = FFTS1/8;
inc7 = -FFTS1/8 + 1;

idx_in_tw_fwd = 0;
idx_in_tw_bwd = FFTS1/4;
%disp('=== In-Butterfly-Loop ===');
for(idx_inbfly=0:FFTS1/8-1)

    % Read in twiddle factors, update pointer
    tw_fac_real = tw_real(idx_in_tw_fwd+1);
    tw_fac_imag = tw_real(idx_in_tw_bwd + 1);       
    %tw_fac_real2 = tw_real(2*idx_in_tw_fwd+1);
    %tw_fac_imag2 = tw_real(2*idx_in_tw_bwd + 1);    
    idx_in_tw_fwd = idx_in_tw_fwd + 2; % This may turn out to be a problem: increment of -2 requires special register usage.
    idx_in_tw_bwd = idx_in_tw_bwd - 2;

    % Read in first input
    x0_real = inout_re(idx_in_x0+1);
    x0_imag = inout_im(idx_in_x0+1);
    idx_in_x0 = idx_in_x0 + inc6;
    
    % Read in second input
    x1_real = inout_re(idx_in_x1+1);
    x1_imag = inout_im(idx_in_x1+1);
    idx_in_x1 = idx_in_x1 + inc6;
    
    % Do complex multiplication
    tmp_1 = tw_fac_real * x1_real - tw_fac_imag * x1_imag;
    tmp_2 = tw_fac_real * x1_imag + tw_fac_imag * x1_real;
    
    % Use complex multiplication outputs
    x0_real_p = x0_real + tmp_1;
    x0_imag_p = x0_imag + tmp_2;
    
    % Use complex multiplication outputs
    x1_real_p = x0_real - tmp_1;
    x1_imag_p = x0_imag - tmp_2;
    
    % Write output to linear buffer
    xk_new(idx_out_0 + 1) = x0_real_p;
    idx_out_0 = idx_out_0 + 1;
    xk_new(idx_out_0 + 1) = x0_imag_p;
    idx_out_0 = idx_out_0 + inc4;
    xk_new(idx_out_0 + 1) = x1_real_p;
    idx_out_0 = idx_out_0 + 1;
    xk_new(idx_out_0 + 1) = x1_imag_p;
    idx_out_0 = idx_out_0 + inc5;
   
    % Read in third input
    x0_real = inout_re(idx_in_x0+1);
    x0_imag = inout_im(idx_in_x0+1);
    idx_in_x0 = idx_in_x0 + inc7;
    
    % Read in fourth input
    x1_real = inout_re(idx_in_x1+1);
    x1_imag = inout_im(idx_in_x1+1);
    idx_in_x1 = idx_in_x1 + inc7;
    
    % Do complex multiplication
    tmp_1 = tw_fac_imag * x1_real + tw_fac_real * x1_imag;
    tmp_2 = tw_fac_imag * x1_imag - tw_fac_real * x1_real;
    
    % Use complex multiplication outputs
    x0_real_p = x0_real - tmp_1;
    x0_imag_p = x0_imag - tmp_2;
    
    % Use complex multiplication outputs
    x1_real_p = x0_real + tmp_1;
    x1_imag_p = x0_imag + tmp_2;
    
    % Write output to linear buffer
    xk_new(idx_out_1 + 1) = x0_real_p;
    idx_out_1 = idx_out_1 + 1;
    xk_new(idx_out_1 + 1) = x0_imag_p;
    idx_out_1 = idx_out_1 + inc4;
    xk_new(idx_out_1 + 1) = x1_real_p;
    idx_out_1 = idx_out_1 + 1;
    xk_new(idx_out_1 + 1) = x1_imag_p;
    idx_out_1 = idx_out_1 + inc5;
   
end
%xk_new

diff = xk_new - xk_ifft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);








% Version 11-> 12: Moved the "-" in the imaginary output for the second
% half of the buffers in the input stage to the stage right behind the bit
% reversal addressing.
%% =======================================================================
disp('%% IFFT variant No 12: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Bit reversal addressing on input side!!');
disp('%% - half length IFFT realized in code');
disp('%% - Second release variant IFFT with out-of-place bit reversal addressing');
%% =======================================================================
out_re = zeros(FFTS1P, 1);
out_im = zeros(FFTS1P, 1);
xk_new = zeros(FFTS1, 1);

tw = exp(-i*2*pi*[0:FFTS1/4]/FFTS1);
tw_real = real(tw);

fac = 1;

inout_re = real(Xk);
inout_im = imag(Xk);

i0 = 0;
i2 = FFTS1/2;
i4 = 1;
i5 = FFTS1/4 - 1;

tmp1 = (inout_re(i0 + 1) - inout_re(i2 + 1));
tmp2 = (inout_re(i0 + 1) + inout_re(i2 + 1));

inout_im(i0 + 1) = tmp1;
inout_re(i0 + 1) = tmp2;

inc0 = FFTS1/4;
inc1 = -FFTS1/4 + 1;
inc2 = -FFTS1/4;
inc3 = FFTS1/4 -1;

i0 = i0 + inc0;

inout_re(i0+1) = inout_re(i0+1) + inout_re(i0+1);
inout_im(i0+1) = fac* (inout_im(i0+1) + inout_im(i0+1)); % (A)

i0 = i0 + inc1;
i2 = i2 -1;

for(idx = 1:FFTS1/8-1)

    tw_fac_real = tw_real(i4 + 1);
    tw_fac_imag = tw_real(i5 + 1);
    i4 = i4 + 1;
    i5 = i5 - 1;
    
    tt_real11 = (inout_re(i0 + 1) + inout_re(i2+1));
    tt_imag11 = (inout_im(i0 + 1) - inout_im(i2+1));
    
    t_real1 = (inout_im(i0 + 1) + inout_im(i2+1));
    t_imag1 = (inout_re(i0 + 1) - inout_re(i2+1));
    
    tt_real21 = t_real1 * tw_fac_real;
    tt_real21 = tt_real21 + t_imag1 * tw_fac_imag;
    
    tt_imag21 = t_real1 * tw_fac_imag;
    tt_imag21 = tt_imag21 - t_imag1 * tw_fac_real;
              
    inout_re(i0+1) = tt_real11 - tt_real21;
    inout_im(i0+1) = tt_imag11 - tt_imag21;

    inout_re(i2+1) = tt_real11 + tt_real21;
    inout_im(i2+1) = fac*(tt_imag11 + tt_imag21);  % (A)
    
    i0 = i0 + inc0;
    i2 = i2 + inc2;
    
    tt_real11 = (inout_re(i2 + 1) + inout_re(i0+1));
    tt_imag11 = (inout_im(i2 + 1) - inout_im(i0+1));
    
    t_real1 = (inout_im(i2 + 1) + inout_im(i0+1));
    t_imag1 = (inout_re(i2 + 1) - inout_re(i0+1));
    
    tt_real21 = t_real1 * tw_fac_imag + t_imag1 * tw_fac_real;
    tt_imag21 = t_real1 * tw_fac_real - t_imag1 * tw_fac_imag;
       
    inout_re(i2+1) = tt_real11 - tt_real21;
    inout_im(i2+1) = tt_imag11 - tt_imag21;

    inout_re(i0+1) = tt_real11 + tt_real21;
    inout_im(i0+1) = fac*(tt_imag11 + tt_imag21);  % (A)
    %inout_im(i0+1) = -tt_imag11 - tt_imag21; <- original, lert us move the
    %"-" into the next loop!!

    i0 = i0 + inc1;
    i2 = i2 + inc3;

end

% Final special rule
tw_fac_real = tw_real(i0 + 1);
%tw_fac_imag = tw_imag(i0+1); same as real value (sqrt(2)

tt_real11 = (inout_re(i0 + 1) + inout_re(i2+1));
tt_imag11 = (inout_im(i0 + 1) - inout_im(i2+1));

t_real1 = (inout_im(i0 + 1) + inout_im(i2+1));
t_imag1 = (inout_re(i0 + 1) - inout_re(i2+1));

tt_real21 = t_real1 * tw_fac_real + t_imag1 * tw_fac_real;
tt_imag21 = t_real1 * tw_fac_real - t_imag1 * tw_fac_real;

inout_re(i0+1) = tt_real11 - tt_real21;
inout_im(i0+1) = tt_imag11 - tt_imag21;

inout_re(i2+1) = tt_real11 + tt_real21;
inout_im(i2+1) = fac*(tt_imag11 + tt_imag21); % (A)

%inout_re
%inout_im
i0 = 0;
% Do bit reversal mapping here, here, we can place the normalization!
for(idx = 0:FFTS1/2 - 1)
    
    idxWrite = bitreverse(i0, NFFT-1);
    out_re(idxWrite+1) = inout_re(i0+1)/2*2;    
    out_im(idxWrite+1) = inout_im(i0+1)/2*2;
    i0 = i0 + 1;    
end

% ===========================================
% Final "IFFT" in "FFT" layout 
% ===========================================

% ===========================================
% First stage of loop separately, otherwise following optimization will not work
% Exploits positively that there is only one twiddle factor which is 1 for
% real and 0 for imaginary part
% Bit reversal addressing on input side
% There is NO way to combine this with the
% butterfly, the index is just to unregular!!
% We should, however, use this loop to normalize the 
% FFT bin so that we end up in Q23
% On a DSP this requires out-of-place buffers!!!
% ===========================================
idx_in_x0 = 0;

for(idx_bfly = 0:FFTS1P/2 - 1)
    
    %idxRead = bitreverse(idx_in_x0, NFFT-1);
    idxRead = idx_in_x0;
    
    x0_real = out_re(idxRead +1);
    x0_imag = out_im(idxRead +1);
    idx_in_x0 = idx_in_x0 + 1;
    
    %idxRead = bitreverse(idx_in_x0, NFFT-1);
    idxRead = idx_in_x0;
    x1_real = out_re(idxRead+1);
    x1_imag = out_im(idxRead+1);
    idx_in_x0 = idx_in_x0 - 1;
    
    x0_real_p = x0_real + x1_real;
    x0_imag_p = x0_imag - x1_imag;% <- negative sign from first loop, second half (A) moved to here!
    
    x1_real_p = x0_real - x1_real;
    x1_imag_p = x0_imag + x1_imag; % <- negative sign from first loop, second half (A) moved to here!
    
    inout_re(idx_in_x0+1) = x0_real_p;
    inout_im(idx_in_x0+1) = x0_imag_p;
    idx_in_x0 = idx_in_x0 + 1;
    inout_re(idx_in_x0+1) = x1_real_p;
    inout_im(idx_in_x0+1) = x1_imag_p;
    idx_in_x0 = idx_in_x0 + 1;
    
end


% ===================================================================
% Rest of loops up to very final stage
% ===================================================================
num_bfly = FFTS1P/4;
num_ops_per_bfly = 2;
num_shift_bfly2bfly = 2;
num_shift_tw = num_bfly * 2;

for(idx_stage = 1:NFFTP-2)

    idx_in_stage_start = 0;
    idx_in_tw = 0;
    inc0 = num_shift_bfly2bfly;
    inc1 = -num_shift_bfly2bfly;
    
    for(idx_inbfly=0:num_ops_per_bfly/2-1)

        idx_in_x0 = idx_in_stage_start;
        tw_fac_real = tw_real(idx_in_tw+1);
        tw_fac_imag = tw_real(FFTS1/4 - idx_in_tw+1);

        %txc = tw_real(idx_in_tw+1);
        
        for(idx_bfly = 0:num_bfly - 1)
        
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc0;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            compl_mult1 = tw_fac_real * x1_real - tw_fac_imag * x1_imag;
            compl_mult2 = tw_fac_real * x1_imag + tw_fac_imag * x1_real;
            
            x0_real_p = x0_real + compl_mult1;
            x0_imag_p = x0_imag + compl_mult2;
            
            x1_real_p = x0_real - compl_mult1;
            x1_imag_p = x0_imag - compl_mult2;
            
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
        end
                
        idx_in_x0 = idx_in_stage_start + num_ops_per_bfly/2;
               
        for(idx_bfly = 0:num_bfly - 1)
        
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc0;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            compl_mult1 = tw_fac_real * x1_imag;
            compl_mult1 = compl_mult1 + tw_fac_imag * x1_real;
            compl_mult2 = tw_fac_real * x1_real;
            compl_mult2 = compl_mult2 -tw_fac_imag * x1_imag;
            
            x0_real_p = x0_real - compl_mult1;
            x0_imag_p = x0_imag + compl_mult2;
            
            x1_real_p = x0_real + compl_mult1;
            x1_imag_p = x0_imag - compl_mult2;
            
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
        end
                
        idx_in_tw = idx_in_tw + num_shift_tw;
        idx_in_stage_start = idx_in_stage_start + 1;        
 
    end
    
    
    num_bfly = num_bfly/2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly * 2;
    num_shift_bfly2bfly = num_shift_bfly2bfly * 2;
    
end

%inout_re
%inout_im
% ===================================================================
% Last stage: Last twiddle factors plus Interleaved output
% ===================================================================
idx_in_x0 = 0;
idx_in_x1 = idx_in_x0 + FFTS1/4;
%idx_in_x0 = num_ops_per_bfly;
idx_out_0 = 0;
idx_out_1 = idx_out_0 + FFTS1/4;

inc4 = FFTS1/2 - 1;
inc5 = -FFTS1/2 + 1;
inc6 = FFTS1/8;
inc7 = -FFTS1/8 + 1;

idx_in_tw_fwd = 0;
idx_in_tw_bwd = FFTS1/4;
%disp('=== In-Butterfly-Loop ===');
for(idx_inbfly=0:FFTS1/8-1)

    % Read in twiddle factors, update pointer
    tw_fac_real = tw_real(idx_in_tw_fwd+1);
    tw_fac_imag = tw_real(idx_in_tw_bwd + 1);       
    %tw_fac_real2 = tw_real(2*idx_in_tw_fwd+1);
    %tw_fac_imag2 = tw_real(2*idx_in_tw_bwd + 1);    
    idx_in_tw_fwd = idx_in_tw_fwd + 2; % This may turn out to be a problem: increment of -2 requires special register usage.
    idx_in_tw_bwd = idx_in_tw_bwd - 2;

    % Read in first input
    x0_real = inout_re(idx_in_x0+1);
    x0_imag = inout_im(idx_in_x0+1);
    idx_in_x0 = idx_in_x0 + inc6;
    
    % Read in second input
    x1_real = inout_re(idx_in_x1+1);
    x1_imag = inout_im(idx_in_x1+1);
    idx_in_x1 = idx_in_x1 + inc6;
    
    % Do complex multiplication
    tmp_1 = tw_fac_real * x1_real - tw_fac_imag * x1_imag;
    tmp_2 = tw_fac_real * x1_imag + tw_fac_imag * x1_real;
    
    % Use complex multiplication outputs
    x0_real_p = x0_real + tmp_1;
    x0_imag_p = x0_imag + tmp_2;
    
    % Use complex multiplication outputs
    x1_real_p = x0_real - tmp_1;
    x1_imag_p = x0_imag - tmp_2;
    
    % Write output to linear buffer
    xk_new(idx_out_0 + 1) = x0_real_p;
    idx_out_0 = idx_out_0 + 1;
    xk_new(idx_out_0 + 1) = x0_imag_p;
    idx_out_0 = idx_out_0 + inc4;
    xk_new(idx_out_0 + 1) = x1_real_p;
    idx_out_0 = idx_out_0 + 1;
    xk_new(idx_out_0 + 1) = x1_imag_p;
    idx_out_0 = idx_out_0 + inc5;
   
    % Read in third input
    x0_real = inout_re(idx_in_x0+1);
    x0_imag = inout_im(idx_in_x0+1);
    idx_in_x0 = idx_in_x0 + inc7;
    
    % Read in fourth input
    x1_real = inout_re(idx_in_x1+1);
    x1_imag = inout_im(idx_in_x1+1);
    idx_in_x1 = idx_in_x1 + inc7;
    
    % Do complex multiplication
    tmp_1 = tw_fac_imag * x1_real + tw_fac_real * x1_imag;
    tmp_2 = tw_fac_imag * x1_imag - tw_fac_real * x1_real;
    
    % Use complex multiplication outputs
    x0_real_p = x0_real - tmp_1;
    x0_imag_p = x0_imag - tmp_2;
    
    % Use complex multiplication outputs
    x1_real_p = x0_real + tmp_1;
    x1_imag_p = x0_imag + tmp_2;
    
    % Write output to linear buffer
    xk_new(idx_out_1 + 1) = x0_real_p;
    idx_out_1 = idx_out_1 + 1;
    xk_new(idx_out_1 + 1) = x0_imag_p;
    idx_out_1 = idx_out_1 + inc4;
    xk_new(idx_out_1 + 1) = x1_real_p;
    idx_out_1 = idx_out_1 + 1;
    xk_new(idx_out_1 + 1) = x1_imag_p;
    idx_out_1 = idx_out_1 + inc5;
   
end
%xk_new

diff = xk_new - xk_ifft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);

%% =======================================================================
% version 12 -> 13: Mainly, output stage with four output pointers and four
% pointer increments in input addressing.
disp('%% IFFT variant No 13: ');
disp('%% - Butterfly algorithm, ');
disp('%% - half length, ');
disp('%% - Bit reversal addressing on input side!!');
disp('%% - half length IFFT realized in code');
disp('%% - Second release variant IFFT with out-of-place bit reversal addressing');
%% =======================================================================
out_re = zeros(FFTS1P, 1);
out_im = zeros(FFTS1P, 1);
xk_new = zeros(FFTS1, 1);

tw = exp(-i*2*pi*[0:FFTS1/4]/FFTS1);
tw_real = real(tw);

fac = 1;

inout_re = real(Xk);
inout_im = imag(Xk);

i0 = 0;
i2 = FFTS1/2;
i4 = 1;
i5 = FFTS1/4 - 1;

tmp1 = (inout_re(i0 + 1) - inout_re(i2 + 1));
tmp2 = (inout_re(i0 + 1) + inout_re(i2 + 1));

inout_im(i0 + 1) = tmp1;
inout_re(i0 + 1) = tmp2;

inc0 = FFTS1/4;
inc1 = -FFTS1/4 + 1;
inc2 = -FFTS1/4;
inc3 = FFTS1/4 -1;

i0 = i0 + inc0;

inout_re(i0+1) = inout_re(i0+1) + inout_re(i0+1);
inout_im(i0+1) = fac* (inout_im(i0+1) + inout_im(i0+1)); % (A)

i0 = i0 + inc1;
i2 = i2 -1;

for(idx = 1:FFTS1/8-1)

    tw_fac_real = tw_real(i4 + 1);
    tw_fac_imag = tw_real(i5 + 1);
    i4 = i4 + 1;
    i5 = i5 - 1;
    
    tt_real11 = (inout_re(i0 + 1) + inout_re(i2+1));
    tt_imag11 = (inout_im(i0 + 1) - inout_im(i2+1));
    
    t_real1 = (inout_im(i0 + 1) + inout_im(i2+1));
    t_imag1 = (inout_re(i0 + 1) - inout_re(i2+1));
    
    tt_real21 = t_real1 * tw_fac_real;
    tt_real21 = tt_real21 + t_imag1 * tw_fac_imag;
    
    tt_imag21 = t_real1 * tw_fac_imag;
    tt_imag21 = tt_imag21 - t_imag1 * tw_fac_real;
              
    inout_re(i0+1) = tt_real11 - tt_real21;
    inout_im(i0+1) = tt_imag11 - tt_imag21;

    inout_re(i2+1) = tt_real11 + tt_real21;
    inout_im(i2+1) = fac*(tt_imag11 + tt_imag21);  % (A)
    
    i0 = i0 + inc0;
    i2 = i2 + inc2;
    
    tt_real11 = (inout_re(i2 + 1) + inout_re(i0+1));
    tt_imag11 = (inout_im(i2 + 1) - inout_im(i0+1));
    
    t_real1 = (inout_im(i2 + 1) + inout_im(i0+1));
    t_imag1 = (inout_re(i2 + 1) - inout_re(i0+1));
    
    tt_real21 = t_real1 * tw_fac_imag + t_imag1 * tw_fac_real;
    tt_imag21 = t_real1 * tw_fac_real - t_imag1 * tw_fac_imag;
       
    inout_re(i2+1) = tt_real11 - tt_real21;
    inout_im(i2+1) = tt_imag11 - tt_imag21;

    inout_re(i0+1) = tt_real11 + tt_real21;
    inout_im(i0+1) = fac*(tt_imag11 + tt_imag21);  % (A)
    %inout_im(i0+1) = -tt_imag11 - tt_imag21; <- original, lert us move the
    %"-" into the next loop!!

    i0 = i0 + inc1;
    i2 = i2 + inc3;

end

% Final special rule
tw_fac_real = tw_real(i0 + 1);
%tw_fac_imag = tw_imag(i0+1); same as real value (sqrt(2)

tt_real11 = (inout_re(i0 + 1) + inout_re(i2+1));
tt_imag11 = (inout_im(i0 + 1) - inout_im(i2+1));

t_real1 = (inout_im(i0 + 1) + inout_im(i2+1));
t_imag1 = (inout_re(i0 + 1) - inout_re(i2+1));

tt_real21 = t_real1 * tw_fac_real + t_imag1 * tw_fac_real;
tt_imag21 = t_real1 * tw_fac_real - t_imag1 * tw_fac_real;

inout_re(i0+1) = tt_real11 - tt_real21;
inout_im(i0+1) = tt_imag11 - tt_imag21;

inout_re(i2+1) = tt_real11 + tt_real21;
inout_im(i2+1) = fac*(tt_imag11 + tt_imag21); % (A)

%inout_re
%inout_im
i0 = 0;
% Do bit reversal mapping here, here, we can place the normalization!
for(idx = 0:FFTS1/2 - 1)
    
    idxWrite = bitreverse(i0, NFFT-1);
    out_re(idxWrite+1) = inout_re(i0+1)/2*2;    
    out_im(idxWrite+1) = inout_im(i0+1)/2*2;
    i0 = i0 + 1;    
end

% ===========================================
% Final "IFFT" in "FFT" layout 
% ===========================================

% ===========================================
% First stage of loop separately, otherwise following optimization will not work
% Exploits positively that there is only one twiddle factor which is 1 for
% real and 0 for imaginary part
% Bit reversal addressing on input side
% There is NO way to combine this with the
% butterfly, the index is just to unregular!!
% We should, however, use this loop to normalize the 
% FFT bin so that we end up in Q23
% On a DSP this requires out-of-place buffers!!!
% ===========================================
idx_in_x0 = 0;

for(idx_bfly = 0:FFTS1P/2 - 1)
    
    %idxRead = bitreverse(idx_in_x0, NFFT-1);
    idxRead = idx_in_x0;
    
    x0_real = out_re(idxRead +1);
    x0_imag = out_im(idxRead +1);
    idx_in_x0 = idx_in_x0 + 1;
    
    %idxRead = bitreverse(idx_in_x0, NFFT-1);
    idxRead = idx_in_x0;
    x1_real = out_re(idxRead+1);
    x1_imag = out_im(idxRead+1);
    idx_in_x0 = idx_in_x0 - 1;
    
    x0_real_p = x0_real + x1_real;
    x0_imag_p = x0_imag - x1_imag;% <- negative sign from first loop, second half (A) moved to here!
    
    x1_real_p = x0_real - x1_real;
    x1_imag_p = x0_imag + x1_imag; % <- negative sign from first loop, second half (A) moved to here!
    
    inout_re(idx_in_x0+1) = x0_real_p;
    inout_im(idx_in_x0+1) = x0_imag_p;
    idx_in_x0 = idx_in_x0 + 1;
    inout_re(idx_in_x0+1) = x1_real_p;
    inout_im(idx_in_x0+1) = x1_imag_p;
    idx_in_x0 = idx_in_x0 + 1;
    
end


% ===================================================================
% Rest of loops up to very final stage
% ===================================================================
num_bfly = FFTS1P/4;
num_ops_per_bfly = 2;
num_shift_bfly2bfly = 2;
num_shift_tw = num_bfly * 2;

for(idx_stage = 1:NFFTP-2)

    idx_in_stage_start = 0;
    idx_in_tw = 0;
    inc0 = num_shift_bfly2bfly;
    inc1 = -num_shift_bfly2bfly;
    
    for(idx_inbfly=0:num_ops_per_bfly/2-1)

        idx_in_x0 = idx_in_stage_start;
        tw_fac_real = tw_real(idx_in_tw+1);
        tw_fac_imag = tw_real(FFTS1/4 - idx_in_tw+1);

        %txc = tw_real(idx_in_tw+1);
        
        for(idx_bfly = 0:num_bfly - 1)
        
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc0;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            compl_mult1 = tw_fac_real * x1_real - tw_fac_imag * x1_imag;
            compl_mult2 = tw_fac_real * x1_imag + tw_fac_imag * x1_real;
            
            x0_real_p = x0_real + compl_mult1;
            x0_imag_p = x0_imag + compl_mult2;
            
            x1_real_p = x0_real - compl_mult1;
            x1_imag_p = x0_imag - compl_mult2;
            
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
        end
                
        idx_in_x0 = idx_in_stage_start + num_ops_per_bfly/2;
               
        for(idx_bfly = 0:num_bfly - 1)
        
            x0_real = inout_re(idx_in_x0+1);
            x0_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc0;
            
            x1_real = inout_re(idx_in_x0+1);
            x1_imag = inout_im(idx_in_x0+1);
            idx_in_x0 = idx_in_x0 + inc1;
            
            compl_mult1 = tw_fac_real * x1_imag;
            compl_mult1 = compl_mult1 + tw_fac_imag * x1_real;
            compl_mult2 = tw_fac_real * x1_real;
            compl_mult2 = compl_mult2 -tw_fac_imag * x1_imag;
            
            x0_real_p = x0_real - compl_mult1;
            x0_imag_p = x0_imag + compl_mult2;
            
            x1_real_p = x0_real + compl_mult1;
            x1_imag_p = x0_imag - compl_mult2;
            
            inout_re(idx_in_x0+1) = x0_real_p;
            inout_im(idx_in_x0+1) = x0_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
            
            inout_re(idx_in_x0+1) = x1_real_p;
            inout_im(idx_in_x0+1) = x1_imag_p;
            idx_in_x0 = idx_in_x0 + inc0;
        end
                
        idx_in_tw = idx_in_tw + num_shift_tw;
        idx_in_stage_start = idx_in_stage_start + 1;        
 
    end
    
    
    num_bfly = num_bfly/2;
    num_shift_tw = num_shift_tw/2;
    num_ops_per_bfly = num_ops_per_bfly * 2;
    num_shift_bfly2bfly = num_shift_bfly2bfly * 2;
    
end

%inout_re
%inout_im
% ===================================================================
% Last stage: Last twiddle factors plus Interleaved output
% ===================================================================
idx_in = 0;

idx_out_a = 0;
idx_out_b = FFTS1/2;
idx_out_c = FFTS1/4;
idx_out_d = 3*FFTS1/4;

M0 = FFTS1/4;
M1 = - FFTS1/8;
M2 = FFTS1/8 + 1;
M3 = -2;

idx_in_tw_fwd = 0;
idx_in_tw_bwd = FFTS1/4;
%disp('=== In-Butterfly-Loop ===');
for(idx_inbfly=0:FFTS1/8-1)

    % Read in twiddle factors, update pointer
    tw_fac_real = tw_real(idx_in_tw_fwd+1);
    tw_fac_imag = tw_real(idx_in_tw_bwd + 1);       
    %tw_fac_real2 = tw_real(2*idx_in_tw_fwd+1);
    %tw_fac_imag2 = tw_real(2*idx_in_tw_bwd + 1);    
    idx_in_tw_fwd = idx_in_tw_fwd + 2; % This may turn out to be a problem: increment of -2 requires special register usage.
    idx_in_tw_bwd = idx_in_tw_bwd + M3;

    % Read in first input
    x0_real = inout_re(idx_in+1);
    x0_imag = inout_im(idx_in+1);
    idx_in = idx_in + M0;
    
    % Read in second input
    x1_real = inout_re(idx_in+1);
    x1_imag = inout_im(idx_in+1);
    idx_in = idx_in + M1;
    
    % Do complex multiplication
    tmp_1 = tw_fac_real * x1_real - tw_fac_imag * x1_imag;
    tmp_2 = tw_fac_real * x1_imag + tw_fac_imag * x1_real;
    
    % Use complex multiplication outputs
    x0_real_p = x0_real + tmp_1;
    x0_imag_p = x0_imag + tmp_2;
    
    % Use complex multiplication outputs
    x1_real_p = x0_real - tmp_1;
    x1_imag_p = x0_imag - tmp_2;
    
    % Write output to linear buffer
    xk_new(idx_out_a + 1) = x0_real_p;
    idx_out_a = idx_out_a + 1;

    xk_new(idx_out_a + 1) = x0_imag_p;
    idx_out_a = idx_out_a + 1;
    
    xk_new(idx_out_b + 1) = x1_real_p;
    idx_out_b = idx_out_b + 1;
    
    xk_new(idx_out_b + 1) = x1_imag_p;
    idx_out_b = idx_out_b + 1;
    
    % Read in third input
    x0_real = inout_re(idx_in+1);
    x0_imag = inout_im(idx_in+1);
    idx_in = idx_in + M0;
    
    % Read in fourth input
    x1_real = inout_re(idx_in+1);
    x1_imag = inout_im(idx_in+1);
    idx_in = idx_in + M2;
    idx_in = idx_in - FFTS1/2; % <- circular wrap around
    
    % Do complex multiplication
    tmp_1 = tw_fac_imag * x1_real + tw_fac_real * x1_imag;
    tmp_2 = tw_fac_imag * x1_imag - tw_fac_real * x1_real;
    
    % Use complex multiplication outputs
    x0_real_p = x0_real - tmp_1;
    x0_imag_p = x0_imag - tmp_2;
    
    % Use complex multiplication outputs
    x1_real_p = x0_real + tmp_1;
    x1_imag_p = x0_imag + tmp_2;
    
    % Write output to linear buffer
    xk_new(idx_out_c + 1) = x0_real_p;
    idx_out_c = idx_out_c + 1;
    
    xk_new(idx_out_c + 1) = x0_imag_p;
    idx_out_c = idx_out_c + 1;
    
    xk_new(idx_out_d + 1) = x1_real_p;
    idx_out_d = idx_out_d + 1;
    
    xk_new(idx_out_d + 1) = x1_imag_p;
    idx_out_d = idx_out_d + 1;
end
%xk_new

diff = xk_new - xk_ifft_orig;
disp(['--> Result deviation from original: ' num2str(diff' * diff)]);

function [idx] = bitreverse(idx, numDigs)
    idx = bin2dec(fliplr(dec2bin(idx, numDigs)));