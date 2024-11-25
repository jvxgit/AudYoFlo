use crate::*;

pub mod ffi;

pub struct LevinsonDurbin {
    hdl: *mut JvxHandle,
}

impl LevinsonDurbin {
    pub fn new(lpc_order_max: JvxSize) -> Result<Self> {
        let mut hdl: *mut JvxHandle = core::ptr::null_mut();
        let res = unsafe { ffi::jvx_levinson_durbin_initialize(&mut hdl, lpc_order_max) };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(Self { hdl }),
            err => Err(err),
        }
    }

    /// returns (e_start, e_stop)
    pub fn compute(
        &mut self,
        r_xx: &[JvxData],
        alpha: &mut [JvxData],
    ) -> Result<(JvxData, JvxData)> {
        let mut e_start: JvxData = 0.0;
        let mut e_stop: JvxData = 0.0;
        let res = unsafe {
            ffi::jvx_levinson_durbin_compute(
                self.hdl,
                r_xx.as_ptr(),
                alpha.as_mut_ptr(),
                &mut e_start,
                &mut e_stop,
            )
        };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok((e_start, e_stop)),
            err => Err(err),
        }
    }
}

impl Drop for LevinsonDurbin {
    fn drop(&mut self) {
        unsafe {
            ffi::jvx_levinson_durbin_terminate(self.hdl);
        }
    }
}
