use jvx_dsp_base::*;
use ayfstarterlibrs::*;

#[cfg(feature = "jvx-allow-profiler-data-macros")]
const JVX_ALLOW_PROFILER_DATA_MACROS: u8 = 1;

#[cfg(not(feature = "jvx-allow-profiler-data-macros"))]
const JVX_ALLOW_PROFILER_DATA_MACROS: u8 = 0;

#[no_mangle]
pub extern "C" fn ayf_starter_data_debug_prepare(hdl_dbg: *mut ayf_starter_data_debug, hdl_starter: *mut ayf_starter, spec_data: JvxCBitField, 
                                            cb_register: RegisterCallbackFunction, inst: *mut std::ffi::c_void) -> JvxDspBaseErrorType 
{
    let hdl_dbg_ = unsafe { &mut *hdl_dbg };
    let hdl_starter_ = unsafe { &mut *hdl_starter };

	if JVX_ALLOW_PROFILER_DATA_MACROS==1
	{
		hdl_dbg_.spec_data = spec_data;

		// All pointers are initialized to 0 from outside (memset(0))	
		if (hdl_dbg_.spec_data & (1 << JVX_STARTER_DATA_DEBUG_TP0_SHIFT)) != 0
		{
			unsafe 
			{
				jvx_profiler_allocate_single_entry(&mut hdl_dbg_.tp0, hdl_starter_.prm_init.bsize, C_FALSE);        
			}

			let tag = std::ffi::CString::new("starter_tp0_c").expect("CString::new failed");
			
			if let Some(cb_register_f) = cb_register {
				cb_register_f(&mut hdl_dbg_.tp0, tag.into_raw(), inst);
			}
		}
	}
	
    return JvxDspBaseErrorType::JVX_NO_ERROR;   
}

#[no_mangle]
pub extern "C" fn ayf_starter_data_debug_postprocess(hdl_dbg: *mut ayf_starter_data_debug, 
	cb_unregister: UnregisterCallbackFunction, inst: *mut std::ffi::c_void) -> JvxDspBaseErrorType 
{
    let hdl_dbg_ = unsafe { &mut *hdl_dbg };

	if JVX_ALLOW_PROFILER_DATA_MACROS==1
	{
        let tag = std::ffi::CString::new("starter_tp0_c").expect("CString::new failed");
         
        if let Some(cb_unregister_f) = cb_unregister {
            cb_unregister_f(tag.into_raw(), inst);
        }	

		if !hdl_dbg.is_null()
        {
			unsafe 
			{
	            jvx_profiler_deallocate_single_entry(&mut hdl_dbg_.tp0);        
			}
		}
	}
    return JvxDspBaseErrorType::JVX_NO_ERROR;
}

