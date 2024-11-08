#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
use crate::fft::ffi::*;
use crate::*;
use circbuffer::ffi::*;

#[repr(C)]
pub struct Ram {
    field_cplx: *const *mut JvxDataCplx,
}

#[repr(C)]
pub struct JvxCircbufferFft {
    circBuffer: JvxCircbuffer,

    fftType: JvxFftToolsCoreFftType,
    fftSize: JvxFFTSize,
    fftifft: JvxHandle, //struct jvxFftIfftHandle*;

    length_cplx: JvxSize,
}

extern "C" {
    // jvx_circbuffer_allocate_global_fft_ifft is equivalent to jvx_create_fft_ifft_global
    // jvx_circbuffer_destroy_global_fft_ifft is equivalent to jvx_circbuffer_destroy_global_fft_ifft

    pub fn jvx_circbuffer_allocate_fft_ifft(
        hdlOnReturn: *mut *mut JvxCircbufferFft,
        hdl_global_fft: *const JvxFFTGlobal,
        fftType: JvxFftToolsCoreFftType,
        fftSize: JvxFFTSize,
        preserveInput: JvxCBool,
        nChannels: JvxSize,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_circbuffer_deallocate_fft_ifft(
        hdlReturn: *const JvxCircbufferFft,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_circbuffer_access_cplx_fft_ifft(
        hdl: *const JvxCircbufferFft,
        outPtr: *const *mut JvxDataCplx,
        outLen: *mut JvxSize,
        idx: JvxSize,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_circbuffer_get_write_phase_fft_ifft(
        hdl: *const JvxCircbufferFft,
        phase: *mut JvxSize,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_circbuffer_process_fft_ifft(hdl: *mut JvxCircbufferFft) -> JvxDspBaseErrorType;
}
