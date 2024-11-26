#![allow(non_camel_case_types)]
use crate::*;

pub type JvxFFTGlobal = JvxHandle;
pub type JvxFFT = JvxHandle;
pub type JvxIFFT = JvxHandle;

#[repr(C)]
#[derive(Clone, Copy)]
pub enum JvxFftIfftOperate {
    JVX_FFT_IFFT_PRESERVE_INPUT,
    JVX_FFT_IFFT_EFFICIENT,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub enum JvxFftRoundType {
    JVX_FFT_ROUND_NEAREST = 0,
    JVX_FFT_ROUND_DOWN = 1,
    JVX_FFT_ROUND_UP = 2,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub enum JvxFftToolsCoreFftType {
    JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_REAL_2_COMPLEX = 0,
    JVX_FFT_TOOLS_FFT_CORE_TYPE_FFT_COMPLEX_2_COMPLEX,
    JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_REAL,
    JVX_FFT_TOOLS_FFT_CORE_TYPE_IFFT_COMPLEX_2_COMPLEX,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub enum JvxFFTSize {
    JVX_FFT_TOOLS_FFT_SIZE_16 = 0, // <- must be 2^JVX_OFFSET_FFT_TYPE_MIN
    JVX_FFT_TOOLS_FFT_SIZE_32,
    JVX_FFT_TOOLS_FFT_SIZE_64,
    JVX_FFT_TOOLS_FFT_SIZE_128,
    JVX_FFT_TOOLS_FFT_SIZE_256,
    JVX_FFT_TOOLS_FFT_SIZE_512,
    JVX_FFT_TOOLS_FFT_SIZE_1024,
    JVX_FFT_TOOLS_FFT_SIZE_2048,
    JVX_FFT_TOOLS_FFT_SIZE_4096,
    JVX_FFT_TOOLS_FFT_SIZE_8192,
    JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
}

extern "C" {
    pub fn jvx_create_fft_ifft_global(
        global_hdl: *mut *const JvxFFTGlobal,
        fftType_max: JvxFFTSize,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_create_fft_real_2_complex(
        hdl: *mut *const JvxFFT,
        global_hdl: *const JvxFFTGlobal,
        fftType: JvxFFTSize,
        in_ptr_fld_N: *mut *mut JvxData,
        out_ptr_fld_N2P1: *mut *mut JvxDataCplx,
        NfftSize: *mut JvxSize,
        operate: JvxFftIfftOperate,
        input: *mut JvxData,
        output: *mut JvxDataCplx,
        fftSizeArbitrary: JvxSize,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_create_fft_complex_2_complex(
        hdl: *mut *const JvxFFT,
        global_hdl: *const JvxFFTGlobal,
        fftType: JvxFFTSize,
        in_ptr_fld_N: *mut *mut JvxDataCplx,
        out_ptr_fld_N: *mut *mut JvxDataCplx,
        NfftSize: *mut JvxSize,
        operate: JvxFftIfftOperate,
        input: *mut JvxDataCplx,
        output: *mut JvxDataCplx,
        fftSizeArbitrary: JvxSize,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_create_ifft_complex_2_real(
        hdl: *mut *const JvxFFT,
        global_hdl: *const JvxFFTGlobal,
        fftType: JvxFFTSize,
        in_ptr_fld_N2P1: *mut *mut JvxDataCplx,
        out_ptr_fld_N: *mut *mut JvxData,
        NfftSize: *mut JvxSize,
        operate: JvxFftIfftOperate,
        input: *mut JvxDataCplx,
        output: *mut JvxData,
        fftSizeArbitrary: JvxSize,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_create_ifft_complex_2_complex(
        hdl: *mut *const JvxFFT,
        global_hdl: *const JvxFFTGlobal,
        fftType: JvxFFTSize,
        in_ptr_fld_N: *mut *mut JvxDataCplx,
        out_ptr_fld_N: *mut *mut JvxDataCplx,
        NfftSize: *mut JvxSize,
        operate: JvxFftIfftOperate,
        input: *mut JvxDataCplx,
        output: *mut JvxDataCplx,
        fftSizeArbitrary: JvxSize,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_execute_fft(hdl: *const JvxFFT) -> JvxDspBaseErrorType;
    pub fn jvx_execute_ifft(hdl: *const JvxIFFT) -> JvxDspBaseErrorType;

    pub fn jvx_destroy_fft(hdl: *const JvxFFT) -> JvxDspBaseErrorType;
    pub fn jvx_destroy_ifft(hdl: *const JvxIFFT) -> JvxDspBaseErrorType;

    pub fn jvx_destroy_fft_ifft_global(global_hdl: *const JvxFFTGlobal) -> JvxDspBaseErrorType;

    pub fn jvx_allocate_fft_buffer_real(num_elements: JvxSize) -> *mut JvxData;
    pub fn jvx_allocate_fft_buffer_complex(num_elements: JvxSize) -> *mut JvxDataCplx;
    pub fn jvx_free_fft_buffer(buffer: *const core::ffi::c_void);

    pub fn jvx_oneshot_ifft_complex_2_real(
        fft_size: JvxSize,
        input: *const JvxDataCplx,
        outout: *mut JvxData,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_get_nearest_size_fft(
        szTypeOnReturn: *mut JvxFFTSize,
        n: JvxSize,
        tp: JvxFftRoundType,
        szOnReturn: *mut JvxSize,
    ) -> JvxDspBaseErrorType;
    pub fn jvx_get_fft_size(fftSz: JvxFFTSize) -> JvxSize;
}
