% run example processing for overlap-save frequency domain mult
FFTSIZE = 1024;
FSHIFT = 256;
OVLP = 0;
WEIGHTS_FREQ_DOMAIN = true;

% Specify processing gains
if(WEIGHTS_FREQ_DOMAIN)
    
    % ==============================================
    % Define the weights for processing IN THE FREQUENCY DOMAIN
    % ==============================================

    %cWeights = ones(1, FFTSIZE/2+1);
    fft_idxs = [0:FFTSIZE/2];
    om = pi * fft_idxs/(FFTSIZE/2);
    %cWeights = exp(-1i * om *0.2345);
    cWeights = [ones(1,10) zeros(1, FFTSIZE/2+1-10)];

    % Do the antialias compensation
    [hdl_aa] = jvx_dsp.fast_conv_antialias.init(FFTSIZE, FFTSIZE-FSHIFT, 'hamming');
    [hdl_aa, cWeights, ir_shift, eq_delay] = jvx_dsp.fast_conv_antialias.convert_cplx_weights_ifft_fft(hdl_aa, cWeights);
else
    
    % ==============================================
    % Define the weights for processing IN THE TIME DOMAIN
    % ==============================================

    ir = randn(1,FFTSIZE-FSHIFT);
    ir = ir .* (1 - [1:FFTSIZE-FSHIFT]/(FFTSIZE-FSHIFT));
    irfft = zeros(1, FFTSIZE);
    irfft(1:FFTSIZE-FSHIFT) = ir(1:FFTSIZE-FSHIFT);
    cWeights = jvx_dsp_base.fft_ifft.jvx_fft(irfft);
    ir_shift = ir;
    eq_delay = 0;    
end

% ==============================================
% Input signal
% ==============================================
%IN = [1 zeros(1, 10000)];
IN = randn(1, 10000);

% ==============================================
% Some plotting options
% ==============================================
ploto.plot_ir = true; 
ploto.plot_out = true; 

% ==============================================
% Time domain processing equivalent
% ==============================================
out1 = filter(ir_shift, 1, IN);


% ==============================================
% Frequency domain processing
% ==============================================
[out2, delay] = jvx_dsp.fast_conv_os.process_signal(IN, FFTSIZE, FSHIFT, cWeights, eq_delay, OVLP, ploto);

% Align output signal lengths
align_length = min(size(out1, 2), size(out2, 2));

% Plot the two outputs and the difference
figure; 
subplot(2,1,1);
plot(out1);
hold on;
plot(out2, 'r:');
subplot(2,1,2);
plot(out1(1:align_length)-out2(1:align_length), 'g:');
