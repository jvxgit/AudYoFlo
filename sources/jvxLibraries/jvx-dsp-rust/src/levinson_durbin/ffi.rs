#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
use crate::*;

extern "C" {
    pub fn jvx_levinson_durbin_initialize(
        handleOnReturn: *mut *mut JvxHandle,
        lpcOrderMax: JvxSize,
    ) -> JvxDspBaseErrorType;
    pub fn jvx_levinson_durbin_terminate(handle: *mut JvxHandle) -> JvxDspBaseErrorType;
    pub fn jvx_levinson_durbin_compute(
        handle: *mut JvxHandle,
        Rxx: *const JvxData,
        alpha: *const JvxData,
        eStart: *const JvxData,
        eStop: *const JvxData,
    ) -> JvxDspBaseErrorType;
}
