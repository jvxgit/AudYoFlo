#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
use crate::*;

#[repr(C)]
pub struct Ram {
    pub field: *const *mut JvxData,
}

#[repr(C)]
pub struct JvxCircbuffer {
    pub length: JvxSize,
    pub channels: JvxSize,
    pub idxRead: JvxSize,
    pub fHeight: JvxSize,
    pub nUnits: JvxSize, // for SOS IIR filters
    pub format: JvxDataFormat,
    pub szElement: JvxSize,
    pub ram: Ram,
}

extern "C" {
    pub fn jvx_circbuffer_allocate(
        hdlOnReturn: *mut *mut JvxCircbuffer,
        numberElements: JvxSize,
        nSections: JvxSize,
        channels: JvxSize,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_circbuffer_deallocate(hdlReturn: *mut JvxCircbuffer) -> JvxDspBaseErrorType;

    pub fn jvx_circbuffer_access(
        hdl: *const JvxCircbuffer,
        outPtr: *const *mut JvxData,
        outLen: *mut JvxSize,
        idx: JvxSize,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_circbuffer_fill(
        hdl: *mut JvxCircbuffer,
        toFillWith: JvxData,
        numberValuesFill: JvxSize,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_circbuffer_write_update(
        hdl: *mut JvxCircbuffer,
        fieldFill: *const JvxData,
        numberValuesFill: JvxSize,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_circbuffer_copy_update_buf2buf(
        hdl_copyTo: *mut JvxCircbuffer,
        hdl_copyFrom: *const JvxCircbuffer,
        numberValuesFill: JvxSize,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_circbuffer_advance_read_index(
        hdl: *mut JvxCircbuffer,
        numberValuesRead: JvxSize,
    ) -> JvxDspBaseErrorType;

    pub fn jvx_circbuffer_get_write_phase(
        hdl: *const JvxCircbuffer,
        phase: *mut JvxSize,
    ) -> JvxDspBaseErrorType;
}
