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
pub extern "C" fn ayf_starter_init(hdl: *mut ayf_starter, bsize: JvxSize) -> JvxDspBaseErrorType
{
    let hdl_ = unsafe { &mut *hdl };
    if hdl_.prv.is_none()
    {
        hdl_.prm_init.bsize = bsize;
        hdl_.prm_async.volume = 1.0;
        hdl_.prm_async.runorc = C_TRUE;

        hdl_.prm_async.orc_token_backend_ip = std::ptr::null();
        hdl_.prm_async.orc_token_backend_op = std::ptr::null();
        hdl_.prm_async.orc_token_debug = std::ptr::null();

        return JvxDspBaseErrorType::JVX_NO_ERROR;
    }
    JvxDspBaseErrorType::JVX_ERROR_WRONG_STATE
}

#[no_mangle]
pub extern "C" fn ayf_starter_prepare(hdl: *mut ayf_starter) -> JvxDspBaseErrorType
{
    let hdl_ = unsafe { &mut *hdl };
    if hdl_.prv.is_none()
    {
        hdl_.prv = Some(Box::new(ayf_starter_prv{ prm_init_copy: hdl_.prm_init.clone(), prm_async_copy: hdl_.prm_async.clone()}));
        return JvxDspBaseErrorType::JVX_NO_ERROR;
    }
    JvxDspBaseErrorType::JVX_ERROR_WRONG_STATE
}

#[no_mangle]
pub extern "C" fn ayf_starter_process(hdl: *mut ayf_starter, inputs: *const *const JvxData, outputs: *const *mut JvxData, num_channels_in: JvxSize, num_channels_out: JvxSize, buffer_size: JvxSize) -> JvxDspBaseErrorType
{
    let hdl_ = unsafe { &mut *hdl };
    if let Some(ref mut prv) = hdl_.prv
    {
        let inputs_: &[*const JvxData] = unsafe { std::slice::from_raw_parts(inputs, num_channels_in) };
        let outputs_: &[*mut JvxData] = unsafe { std::slice::from_raw_parts(outputs, num_channels_out) };
        if num_channels_in > 0 && num_channels_out > 0
        {
            for channel_idx_out in 0..num_channels_out
            {
                let channel_idx_in = channel_idx_out % num_channels_in;
                let input_: &[JvxData] = unsafe { std::slice::from_raw_parts(inputs_[channel_idx_in], buffer_size) };
                let output_: &mut[JvxData] = unsafe { std::slice::from_raw_parts_mut(outputs_[channel_idx_out], buffer_size) };
                for i in 0..buffer_size
                {
                    // We move from start to end within the buffer - it is all fine even if we run in-place
                    output_[i] = input_[i] * prv.prm_async_copy.volume;
                    //if channel_idx_out == 0    {
                    //    // Add profiling data
                    //    JVX_DATA_OUT_DBG_TP_STR(hdl->develop.dbg_handler, prv, JVX_STARTER_DATA_DEBUG_TP0_SHIFT,
                    //        TP0, outputs[channel_idx_out], prv.prm_init_copy.bsize);

                    //}
                }
            }
        }
        return JvxDspBaseErrorType::JVX_NO_ERROR;

    }
    JvxDspBaseErrorType::JVX_ERROR_WRONG_STATE
}

#[no_mangle]
pub extern "C" fn ayf_starter_postprocess(hdl: *mut ayf_starter) -> JvxDspBaseErrorType
{
    let hdl_ = unsafe { &mut *hdl };
    if hdl_.prv.is_some()
    {
        hdl_.prv = None;
        return JvxDspBaseErrorType::JVX_NO_ERROR;
    }
    JvxDspBaseErrorType::JVX_ERROR_WRONG_STATE
}

#[no_mangle]
pub extern "C" fn ayf_starter_update(hdl: *mut ayf_starter, operation: u16, want_to_set: bool) -> JvxDspBaseErrorType
{
    let hdl_ = unsafe { &mut *hdl };
    if let Some(ref mut prv) = hdl_.prv {
        if operation & JVX_DSP_UPDATE_ASYNC != 0
        {
            if want_to_set
            {
                prv.prm_async_copy = hdl_.prm_async.clone();
            }
            else
            {
                hdl_.prm_async = prv.prm_async_copy.clone();
            }
        }

        return JvxDspBaseErrorType::JVX_NO_ERROR;
    }
    JvxDspBaseErrorType::JVX_ERROR_WRONG_STATE
}
