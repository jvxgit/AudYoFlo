function [result, oneFrameFFT, handle ] = rtp_FFT_frame_transformComplex(oneFrame, handle)

assert(size(oneFrame,1)==1);

result = true;
switch handle.METHOD
    case 'DW_OLA'
        [oneFrameFFT, handle.HANDLEFFT] = fftTools.rtp_FFT_frame_transformComplex_dw_ola(oneFrame, handle.HANDLEFFT);
    case 'SW_OLA'
        [oneFrameFFT, handle.HANDLEFFT] = fftTools.rtp_FFT_frame_transformComplex_sw_ola(oneFrame, handle.HANDLEFFT);
    case 'OLS'
        [oneFrameFFT, handle.HANDLEFFT] = fftTools.rtp_FFT_frame_transformComplex_ols(oneFrame, handle.HANDLEFFT);
    case 'OLS_CF'
        [oneFrameFFT, handle.HANDLEFFT] = fftTools.rtp_FFT_frame_transformComplex_ols_cf(oneFrame, handle.HANDLEFFT);
    otherwise
        disp(['--> Error in processing method specification, ' handle.METHOD ' is unknown in rtpFXProcess']);
        result = false;
        return;
end

% Reduce FFT size by exploiting symmetry
if handle.cut_symmetries
    assert(isreal(oneFrame),'no symmetries for complex input :(');
    oneFrameFFT = oneFrameFFT(1:handle.numBins);
end

end
