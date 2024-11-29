// jvx_profiler_data_entry.h
use crate::*;

#[repr(C)]
#[derive(Clone)]
pub struct jvx_profiler_data_entry {
    pub sz_elm: JvxSize,
    pub fld: *mut core::ffi::c_void,
    pub cplx: JvxCBool,
    pub c_to_matlab: JvxCBool,
    pub valid_content: JvxCBool,
}

extern "C" {
    pub fn jvx_profiler_allocate_single_entry(
        entry: *mut jvx_profiler_data_entry,
        szFld: JvxSize,
        cplxFld: JvxCBool,
    );

    pub fn jvx_profiler_deallocate_single_entry(entry: *mut jvx_profiler_data_entry);
}

pub type jvx_register_entry_profiling_data_c = extern "C" fn(
    dat: *mut jvx_profiler_data_entry,
    name: *const core::ffi::c_char,
    inst: *mut core::ffi::c_void,
);
pub type RegisterCallbackFunction = Option<jvx_register_entry_profiling_data_c>;

pub type jvx_unregister_entry_profiling_data_c =
    extern "C" fn(name: *const core::ffi::c_char, inst: *mut core::ffi::c_void);
pub type UnregisterCallbackFunction = Option<jvx_unregister_entry_profiling_data_c>;

#[macro_export]
macro_rules! jvx_data_out_dbg_tp_direct {
    ($hdl:expr, $spec: expr, $entryTo:ident, $ptrFrom: expr, $szFrom: expr) => {
        if let Some(dbg) = unsafe { $hdl.as_mut() } {
            if (dbg.specData & ((1 as JvxCBitField) << ($spec as u8))) != 0 && !dbg.$entryTo.fld.is_null() {
                assert_eq!(dbg.$entryTo.sz_elm, $szFrom);
                if dbg.$entryTo.cplx != 0 {
                    unsafe { core::ptr::copy($ptrFrom as *const JvxDataCplx, dbg.$entryTo.fld as *mut JvxDataCplx, $szFrom); }
                } else {
                    unsafe { core::ptr::copy($ptrFrom as *const JvxData, dbg.$entryTo.fld as *mut JvxData, $szFrom); }
                }
            }
        }
    }
}

#[macro_export]
macro_rules! jvx_data_out_dbg_tp_str_check {
    ($hdl:expr, $spec: expr, $entryTo:ident) => {
        if let Some(dbg) = unsafe { $hdl.as_mut() } {
            (dbg.specData & ((1 as JvxCBitField) << ($spec as u8))) != 0 && !dbg.$entryTo.fld.is_null()
        } else {
            false
        }
    }
}

#[macro_export]
macro_rules! jvx_data_out_dbg_tp_str_val {
    ($var:ident, $hdl:expr, $entryTo:ident, $val: expr, $tp: ident, $idx: expr) => {
	    if $var {
            if let Some(dbg) = unsafe { $hdl.as_mut() } {
                if !dbg.$entryTo.fld.is_null() {
		            assert!($idx < dbg.$entryTo.sz_elm);
		            let fldOut = dbg.$entryTo.fld as *mut $tp;
		            unsafe { *fldOut.add($idx) = $val as $tp; }
                }
            }
        }
    }
}
