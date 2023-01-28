function [result, oneFrameOut, handle] = rtp_IFFT_frame_transformComplex(oneFrameFFT, handle)

if strcmp(handle.METHOD,'OLS_CF')
    assert(size(oneFrameFFT,1)==2, 'Current and old frame needed for OLS_CF');
else
    assert(size(oneFrameFFT,1)==1);
end

result = true;

% Expand the spectrum buffer
if (handle.cut_symmetries == 1) && (strcmp(handle.METHOD, 'OLS_CF')==0)
    oneFrameFFT = [oneFrameFFT conj(fliplr(oneFrameFFT(2:end-1)))];
end

switch handle.METHOD
    case 'DW_OLA'
        [oneFrameOut, handle.HANDLEIFFT] = fftTools.rtp_IFFT_frame_transformComplex_dw_ola(oneFrameFFT, handle.HANDLEIFFT);
    case 'SW_OLA'
        [oneFrameOut, handle.HANDLEIFFT] = fftTools.rtp_IFFT_frame_transformComplex_sw_ola(oneFrameFFT, handle.HANDLEIFFT);
    case 'OLS'
        [oneFrameOut, handle.HANDLEIFFT] = fftTools.rtp_IFFT_frame_transformComplex_ols(oneFrameFFT, handle.HANDLEIFFT);
    case 'OLS_CF'
        [oneFrameOut, handle.HANDLEIFFT] = fftTools.rtp_IFFT_frame_transformComplex_ols_cf(oneFrameFFT(2,:), oneFrameFFT(1,:), handle.HANDLEIFFT);
    otherwise
        disp(['--> Error in processing method specification, ' dataProcessing.METHOD ' is unknown in rtpFXProcess']);
        result = false;
        return;
end
end