use jvx_dsp::*;
use ayfstarterlibrs_common::*;

#[no_mangle]
pub extern "C" fn ayf_starter_data_debug_prepare(_hdl_dbg: *mut ayf_starter_data_debug, _hdl_starter: *mut ayf_starter, _spec: JvxCBitField, _cb_register: jvx_register_entry_profiling_data_c, _inst: *mut std::ffi::c_void) -> JvxDspBaseErrorType 
{
    //let hdl_ = unsafe { &mut *hdl };
    //if hdl_.is_null()
    //{
    //    JvxDspBaseErrorType::JVX_ERROR_WRONG_STATE
        //hdl_.prm_init.bsize = bsize;
        //hdl_.prm_async.volume = 1.0;
        //hdl_.prm_async.runorc = C_TRUE;

        //hdl_.prm_async.orc_token_backend_ip = std::ptr::null();
        //hdl_.prm_async.orc_token_backend_op = std::ptr::null();
        //hdl_.prm_async.orc_token_debug = std::ptr::null();

    //}
    return JvxDspBaseErrorType::JVX_NO_ERROR;
}

#[no_mangle]
pub extern "C" fn ayf_starter_data_debug_postprocess(_hdl_dbg: *mut ayf_starter_data_debug, _cb_unregister: jvx_unregister_entry_profiling_data_c, _inst: *mut std::ffi::c_void) -> JvxDspBaseErrorType 
{
    //let hdl_ = unsafe { &mut *hdl };
    //if hdl_.is_null()
    //{
    //    JvxDspBaseErrorType::JVX_ERROR_WRONG_STATE
        //hdl_.prm_init.bsize = bsize;
        //hdl_.prm_async.volume = 1.0;
        //hdl_.prm_async.runorc = C_TRUE;

        //hdl_.prm_async.orc_token_backend_ip = std::ptr::null();
        //hdl_.prm_async.orc_token_backend_op = std::ptr::null();
        //hdl_.prm_async.orc_token_debug = std::ptr::null();

    //}
    return JvxDspBaseErrorType::JVX_NO_ERROR;
}

