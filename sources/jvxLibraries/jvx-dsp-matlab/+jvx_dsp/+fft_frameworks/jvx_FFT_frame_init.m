function handle = rtp_FFT_frame_init(method, lFFT, lBUF, lFRAME, cut_symmetries)

handle.FFTSIZE = lFFT;
handle.BUFFERSIZE = lBUF;
handle.FRAMESHIFT = lFRAME;
handle.METHOD = method;
handle.cut_symmetries = cut_symmetries;

switch method
    
    case 'DW_OLA'
        
        disp('FFT Framework OVERLAP-ADD sqrt Hann (DW OLA)');
        
        % Struct for overlap add, v1 (the NR group solution which is not linear
        % convolution)
        
        handle.DELAY = handle.BUFFERSIZE - handle.FRAMESHIFT;
        handle.FILTER_DELAY = 0;
        
        % Compute flattop Hann window
        [handle.WINDOW, handle.normalization]= ...
            fftTools.rtp_ComputeWindow(handle.BUFFERSIZE, ...
            handle.FRAMESHIFT, 'Flattop-Hann');
        
        % Original version
        % Sqrt of window
        handle.WINDOW = sqrt(handle.WINDOW);
        handle.HANDLEFFT = fftTools.rtp_FFT_frame_init_dw_ola(handle.FFTSIZE, handle.BUFFERSIZE, handle.FRAMESHIFT, ...
            handle.WINDOW, 0, []);
        handle.HANDLEIFFT = fftTools.rtp_IFFT_frame_init_dw_ola(handle.FFTSIZE, handle.BUFFERSIZE, handle.FRAMESHIFT, ...
            handle.WINDOW, 0, handle.normalization);
        % Note that offset can also be zero, the output is identical
        
        handle.numBins = handle.FFTSIZE/2+1;
        
        % For RTProc framework identification
        handle.RTPROC.setup.idFrameworkType_c = 1;
        
    case 'SW_OLA'
        
        disp('FFT Framework OVERLAP-ADD Linear convolution (SW OLA)');
        
        % Struct for overlap add, v2 (linear convolution)
        
        handle.DELAY = handle.BUFFERSIZE - handle.FRAMESHIFT;
        handle.FILTER_DELAY = 0;
        
        % Compute flattop Hann window
        [handle.WINDOW, handle.normalization]= fftTools.rtp_ComputeWindow(handle.BUFFERSIZE, ...
            handle.FRAMESHIFT, 'Flattop-Hann');
        
        % Allocate handles
        handle.HANDLEFFT = fftTools.rtp_FFT_frame_init_sw_ola(handle.FFTSIZE, handle.BUFFERSIZE, handle.FRAMESHIFT, ...
            handle.WINDOW, []);
        handle.HANDLEIFFT = fftTools.rtp_IFFT_frame_init_sw_ola(handle.FFTSIZE, handle.FRAMESHIFT, handle.normalization);
        
        handle.numBins = handle.FFTSIZE/2+1;
        
        % For RTProc framework identification
        handle.RTPROC.setup.idFrameworkType_c = 0;
        
    case 'OLS'
        
        disp('FFT Framework OVERLAP-SAVE (OLS)');
        
        % Struct for overlap add, v2 (linear convolution)
        handle.DELAY = 0;
        handle.normalization = 1.0;
        
        % Allocate handles
        handle.HANDLEFFT = fftTools.rtp_FFT_frame_init_ols(handle.FFTSIZE, ...
            handle.FRAMESHIFT, []);
        handle.HANDLEIFFT = fftTools.rtp_IFFT_frame_init_ols(handle.FFTSIZE, ...
            handle.FRAMESHIFT, handle.normalization);
        
        handle.numBins = handle.FFTSIZE/2+1;
        
        % For RTProc framework identification
        handle.RTPROC.setup.idFrameworkType_c = 2;
        
    case 'OLS_CF'
        
        disp('FFT Framework OVERLAP-SAVE with output cross fading (OLS CF)');
        
        % Struct for overlap add, v2 (linear convolution)
        handle.DELAY = 0;
        handle.normalization = 1.0;
        
        [handle.WINDOW, handle.normalization]= fftTools.rtp_ComputeWindow(handle.FRAMESHIFT, handle.FRAMESHIFT, 'HalfHann');
        
        % Allocate handles
        handle.HANDLEFFT = fftTools.rtp_FFT_frame_init_ols_cf(handle.FFTSIZE, ...
            handle.FRAMESHIFT, []);
        handle.HANDLEIFFT = fftTools.rtp_IFFT_frame_init_ols_cf(handle.FFTSIZE, ...
            handle.FRAMESHIFT, handle.WINDOW, handle.normalization);
        
        handle.numBins = handle.FFTSIZE/2+1;
        
        % For RTProc framework identification
        handle.RTPROC.setup.idFrameworkType_c = 3;
        
    otherwise
        
        disp(['-->> Specification of FFT Framework is not known (' method ').']);
        handle = false;
        return;
end

