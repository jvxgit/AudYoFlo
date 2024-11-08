use crate::*;

pub mod ffi;
pub mod fft;

pub struct Circbuffer {
    hdl: *mut ffi::JvxCircbuffer,
}

impl Circbuffer {
    pub fn new(
        number_elements: JvxSize,
        number_sections: JvxSize,
        channels: JvxSize,
    ) -> Result<Self> {
        let mut hdl: *mut ffi::JvxCircbuffer = core::ptr::null_mut();
        let res = unsafe {
            ffi::jvx_circbuffer_allocate(&mut hdl, number_elements, number_sections, channels)
        };
        // TODO maybe fill with 0 to make sure the values are initialized
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(Self { hdl }),
            err => Err(err),
        }
    }

    pub fn fill(&mut self, to_fill_with: JvxData, number_values_fill: JvxSize) -> Result<()> {
        let res = unsafe { ffi::jvx_circbuffer_fill(self.hdl, to_fill_with, number_values_fill) };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(()),
            err => Err(err),
        }
    }

    pub fn write_update(&mut self, field_fill: &[JvxData]) -> Result<()> {
        let res = unsafe {
            ffi::jvx_circbuffer_write_update(self.hdl, field_fill.as_ptr(), field_fill.len())
        };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(()),
            err => Err(err),
        }
    }

    pub fn copy_update_buf2buf(
        copy_to: &mut Self,
        copy_from: &Self,
        number_values_fill: JvxSize,
    ) -> Result<()> {
        let res = unsafe {
            ffi::jvx_circbuffer_copy_update_buf2buf(copy_to.hdl, copy_from.hdl, number_values_fill)
        };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(()),
            err => Err(err),
        }
    }

    pub fn advance_read_index(&mut self, number_values_read: JvxSize) -> Result<()> {
        let res = unsafe { ffi::jvx_circbuffer_advance_read_index(self.hdl, number_values_read) };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(()),
            err => Err(err),
        }
    }

    pub fn get_write_phase(&self) -> Result<JvxSize> {
        let mut phase: JvxSize = 0;
        let res = unsafe { ffi::jvx_circbuffer_get_write_phase(self.hdl, &mut phase) };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(phase),
            err => Err(err),
        }
    }
}

impl Drop for Circbuffer {
    fn drop(&mut self) {
        unsafe {
            ffi::jvx_circbuffer_deallocate(self.hdl);
        }
    }
}
