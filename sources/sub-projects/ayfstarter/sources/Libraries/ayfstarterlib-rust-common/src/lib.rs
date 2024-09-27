#![allow(non_camel_case_types)]
use jvx_dsp::*;

pub const JVX_STARTER_DATA_DEBUG_TP0_SHIFT: u8 = 0;

// Declare sructs in C style data layout
#[repr(C)]
#[derive(Clone)]
pub struct ayf_starter_init {
    pub bsize: JvxSize,
}

#[repr(C)]
#[derive(Clone)]
pub struct ayf_starter_async 
{
    pub volume: JvxData,
    // the unused ones below are still required for ABI compatibility
    pub runorc: JvxCBool,
    pub orc_token_backend_ip: *const std::os::raw::c_char,
    pub orc_token_backend_op: *const std::os::raw::c_char,
    pub orc_token_debug: *const std::os::raw::c_char,
}

#[repr(C)]
struct develop 
{
    //dbg_handler: *mut std::ffi::c_void,
	dbg_handler: *mut ayf_starter_data_debug,
}

#[repr(C)]
pub struct ayf_starter
{
    pub prm_async: ayf_starter_async,
    pub prm_init: ayf_starter_init,

    develop: develop,
    pub prv: Option<Box<ayf_starter_prv>>, // NOTE should have the same memory layout as `*mut JvxHandle`
}

#[repr(C)]
pub struct ayf_starter_prv 
{
    pub prm_init_copy: ayf_starter_init,
    pub prm_async_copy: ayf_starter_async,
}

// Declare sructs in C style data layout
#[repr(C)]
#[derive(Clone)]
pub struct ayf_starter_data_debug {
    pub spec_data: JvxCBitField,
    pub tp0: jvx_profiler_data_entry
}

#[no_mangle]
pub extern "C" fn ayf_generate(_hdl: *mut ayf_starter) -> JvxDspBaseErrorType 
{
	return JvxDspBaseErrorType::JVX_NO_ERROR;
}
