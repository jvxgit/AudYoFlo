use crate::fft::*;
use crate::*;
use alloc::rc::Rc;

pub mod ffi;
use crate::circbuffer;
use crate::fft::ffi::JvxFftToolsCoreFftType;

pub struct CircbufferFft {
    _global: Rc<_FftFactory>,
    pub hdl: *mut ffi::JvxCircbufferFft,
}

impl CircbufferFft {
    pub fn access_cplx(&self, idx: JvxSize) -> Result<&mut [JvxDataCplx]> {
        let mut out_ptr: *mut JvxDataCplx = core::ptr::null_mut();
        let mut out_len: JvxSize = 0;
        let res = unsafe {
            ffi::jvx_circbuffer_access_cplx_fft_ifft(self.hdl, &mut out_ptr, &mut out_len, idx)
        };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => {
                Ok(unsafe { core::slice::from_raw_parts_mut(out_ptr, out_len) })
            }
            err => Err(err),
        }
    }

    pub fn access(&self, idx: JvxSize) -> Result<&mut [JvxData]> {
        let mut out_ptr: *mut JvxData = core::ptr::null_mut();
        let mut out_len: JvxSize = 0;
        let res = unsafe {
            use crate::circbuffer::ffi::jvx_circbuffer_access;
            jvx_circbuffer_access(&(*(self.hdl)).circBuffer, &mut out_ptr, &mut out_len, idx)
        };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => {
                Ok(unsafe { core::slice::from_raw_parts_mut(out_ptr, out_len) })
            }
            err => Err(err),
        }
    }

    pub fn copy_update_from_circbuffer(
        &mut self,
        copy_from: &circbuffer::Circbuffer,
        number_values_fill: JvxSize,
    ) -> Result<()> {
        let res = unsafe {
            circbuffer::ffi::jvx_circbuffer_copy_update_buf2buf(
                &mut (*(self.hdl)).circBuffer,
                copy_from.hdl,
                number_values_fill,
            )
        };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(()),
            err => Err(err),
        }
    }

    pub fn get_write_phase(&self) -> Result<JvxSize> {
        let mut phase: JvxSize = 0;
        let res = unsafe { ffi::jvx_circbuffer_get_write_phase_fft_ifft(self.hdl, &mut phase) };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(phase),
            err => Err(err),
        }
    }

    pub fn process(&mut self) -> Result<()> {
        let res = unsafe { ffi::jvx_circbuffer_process_fft_ifft(self.hdl) };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(()),
            err => Err(err),
        }
    }

    pub fn length_cplx(&self) -> JvxSize {
        unsafe { (*(self.hdl)).length_cplx }
    }
}

impl _FftFactory {
    // TODO implement external buffer version
    pub fn new_circbuffer_fft(
        self: &Rc<Self>,
        fft_type: JvxFftToolsCoreFftType,
        size: FftSize,
        preserve_input: bool,
        n_channels: JvxSize,
    ) -> Result<CircbufferFft> {
        let mut hdl: *mut ffi::JvxCircbufferFft = core::ptr::null_mut();
        let res = unsafe {
            ffi::jvx_circbuffer_allocate_fft_ifft(
                &mut hdl,
                self.hdl,
                fft_type,
                size.into(),
                preserve_input as JvxCBool,
                n_channels,
            )
        };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(CircbufferFft {
                _global: Rc::clone(self),
                hdl,
            }),
            err => Err(err),
        }
    }
}
