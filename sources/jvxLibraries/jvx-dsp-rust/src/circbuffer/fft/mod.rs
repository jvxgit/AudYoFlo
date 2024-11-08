use crate::fft::*;
use crate::*;
use alloc::rc::Rc;
use alloc::vec::Vec;

pub mod ffi;

pub struct CircbufferFft {
    _global: Rc<FftFactory>,
    hdl: *mut ffi::JvxCircbufferFft,
}

impl CircbufferFft {
    pub fn access_cplx(&self, out: &mut [&mut [JvxDataCplx]], idx: JvxSize) -> Result<JvxSize> {
        let out_ptrs: Vec<*mut JvxDataCplx> = out.iter_mut().map(|x| x.as_mut_ptr()).collect();
        let mut out_len: JvxSize = 0;
        let res = unsafe {
            ffi::jvx_circbuffer_access_cplx_fft_ifft(self.hdl, out_ptrs.as_ptr(), &mut out_len, idx)
        };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(out_len),
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
}

impl FftFactory {
    // TODO implement external buffer version
    pub fn new_circbuffer_fft(
        self: &Rc<Self>,
        fft_type: JvxFftToolsCoreFftType,
        size: FftSize,
        preserve_input: JvxCBool,
        n_channels: JvxSize,
    ) -> Result<CircbufferFft> {
        let mut hdl: *mut ffi::JvxCircbufferFft = core::ptr::null_mut();
        let res = unsafe {
            ffi::jvx_circbuffer_allocate_fft_ifft(
                &mut hdl,
                self.hdl,
                fft_type,
                size.into(),
                preserve_input,
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
