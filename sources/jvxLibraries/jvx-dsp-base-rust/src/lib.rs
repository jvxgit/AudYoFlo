#![allow(non_camel_case_types)]
#![no_std]

extern crate alloc;

pub mod circbuffer;
pub mod fft;

pub type Error = JvxErrorType;
pub type Result<T> = core::result::Result<T, Error>;

// > jvx_system_dataformats.h
#[repr(C)]
#[derive(Clone, Copy)]
pub enum JvxDataFormat {
    JVX_DATAFORMAT_NONE = 0, // Undefined (invalid)
    JVX_DATAFORMAT_DATA,     // double or float datatype
    JVX_DATAFORMAT_16BIT_LE, // 16 Bit signed
    JVX_DATAFORMAT_32BIT_LE, // 32 Bit signed
    JVX_DATAFORMAT_64BIT_LE, // 64 Bit signed
    JVX_DATAFORMAT_8BIT,     // 8 Bit
    JVX_DATAFORMAT_SIZE,

    JVX_DATAFORMAT_STRING,      // String type (character buffers)
    JVX_DATAFORMAT_STRING_LIST, // String type (character buffers)
    JVX_DATAFORMAT_SELECTION_LIST,
    JVX_DATAFORMAT_VALUE_IN_RANGE,

    JVX_DATAFORMAT_U16BIT_LE, // 16 Bit unsigned
    JVX_DATAFORMAT_U32BIT_LE, // 32 Bit unsigned
    JVX_DATAFORMAT_U64BIT_LE, // 64 Bit unsigned
    JVX_DATAFORMAT_U8BIT,     // 8 Bit unsigned

    JVX_DATAFORMAT_FLOAT,  // explicit float for external access
    JVX_DATAFORMAT_DOUBLE, // explicit double for external access
    JVX_DATAFORMAT_BYTE,   // Void: Gateway for other datatypes

    /*
    * The external buffer datatype has a valid value type to identify
    * if the associated dataformat is correct.
    * The other "installable" formats are of a dedicated type
    JVX_DATAFORMAT_EXTERNAL_BUFFER,
    */
    JVX_DATAFORMAT_HANDLE,   // Void: Gateway for other datatypes
    JVX_DATAFORMAT_POINTER,  // Void: Gateway for other datatypes
    JVX_DATAFORMAT_CALLBACK, // Pointer for callbacks
    JVX_DATAFORMAT_INTERFACE,
    JVX_DATAFORMAT_LIMIT,
}
// < jvx_system_dataformats.h

// > jvx_system_common.h
const JVX_LIB_UPDATE_ASYNC_SHIFT: u8 = 0;
const JVX_LIB_UPDATE_SYNC_SHIFT: u8 = 1;
const JVX_LIB_UPDATE_INIT_SHIFT: u8 = 2;
const JVX_LIB_UPDATE_FEEDBACK_SHIFT: u8 = 3;

const JVX_LIB_UPDATE_USER_OFFSET_SHIFT: u8 = 5;

#[repr(C)]
#[derive(Clone, Copy)]
pub enum JvxParameterUpdateType {
    JVX_LIB_UPDATE_ASYNC = (1 << JVX_LIB_UPDATE_ASYNC_SHIFT), //0x1, // shift 0
    JVX_LIB_UPDATE_SYNC = (1 << JVX_LIB_UPDATE_SYNC_SHIFT),   //  0x2, // shift 1
    JVX_LIB_UPDATE_INIT = (1 << JVX_LIB_UPDATE_INIT_SHIFT),   //0x4, // shift 2
    JVX_LIB_UPDATE_FEEDBACK = (1 << JVX_LIB_UPDATE_FEEDBACK_SHIFT), //0x8, // shift 3
    JVX_LIB_UPDATE_USER_OFFSET = (1 << JVX_LIB_UPDATE_USER_OFFSET_SHIFT),
}
// < jvx_system_common.h

// > jvx_dsp_base.h
pub const JVX_DSP_UPDATE_ASYNC: u16 = JvxParameterUpdateType::JVX_LIB_UPDATE_ASYNC as u16;
pub const JVX_DSP_UPDATE_SYNC: u16 = JvxParameterUpdateType::JVX_LIB_UPDATE_SYNC as u16;
pub const JVX_DSP_UPDATE_INIT: u16 = JvxParameterUpdateType::JVX_LIB_UPDATE_INIT as u16;
pub const JVX_DSP_UPDATE_FEEDBACK: u16 = JvxParameterUpdateType::JVX_LIB_UPDATE_FEEDBACK as u16;
pub const JVX_DSP_UPDATE_USER_OFFSET: u16 =
    JvxParameterUpdateType::JVX_LIB_UPDATE_USER_OFFSET as u16;

#[repr(C)]
#[derive(Clone, Copy, Default)]
pub struct JvxDataCplx {
    pub re: JvxData,
    pub im: JvxData,
}
// < jvx_dsp_base.h

// jvx_system_error_types.h
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub enum JvxErrorType {
    JVX_NO_ERROR = 0,
    JVX_ERROR_UNKNOWN,
    JVX_ERROR_INVALID_ARGUMENT,
    JVX_ERROR_WRONG_STATE,
    JVX_ERROR_ID_OUT_OF_BOUNDS,
    JVX_ERROR_UNSUPPORTED,
    JVX_ERROR_ELEMENT_NOT_FOUND,
    JVX_ERROR_DUPLICATE_ENTRY,
    JVX_ERROR_OPEN_FILE_FAILED,
    JVX_ERROR_UNEXPECTED,
    JVX_ERROR_NOT_READY,
    JVX_ERROR_COMPONENT_BUSY,
    JVX_ERROR_BUFFER_OVERFLOW,
    JVX_ERROR_BUFFER_UNDERRUN,
    JVX_ERROR_WRONG_STATE_SUBMODULE,
    JVX_ERROR_MESSAGE_QUEUE_FULL,
    JVX_ERROR_PARSE_ERROR,
    JVX_ERROR_WRONG_SECTION_TYPE,
    JVX_ERROR_INTERNAL,
    JVX_ERROR_EMPTY_LIST,
    JVX_ERROR_CALL_SUB_COMPONENT_FAILED,
    JVX_ERROR_THREADING_MISMATCH,
    JVX_ERROR_SYSTEMCALL_FAILED,
    JVX_ERROR_SIZE_MISMATCH,
    JVX_ERROR_NO_ACCESS,
    JVX_ERROR_INVALID_SETTING,
    JVX_ERROR_TIMEOUT,
    JVX_ERROR_ABORT,
    JVX_ERROR_POSTPONE,
    JVX_ERROR_END_OF_FILE,
    JVX_ERROR_STANDARD_EXCEPTION,
    JVX_ERROR_SEQUENCER_TOOK_CONTROL,
    JVX_ERROR_THREAD_ONCE_MORE,
    JVX_ERROR_PROCESS_COMPLETE,
    JVX_ERROR_DSP,
    JVX_ERROR_ALREADY_IN_USE,
    JVX_ERROR_PROTOCOL_ERROR,
    JVX_ERROR_NOT_ENOUGH_INPUT_DATA,
    JVX_ERROR_NOT_ENOUGH_OUTPUT_SPACE,
    JVX_ERROR_COMPROMISE,
    JVX_ERROR_REMOTE,
    JVX_ERROR_ALLOCATION,
    JVX_ERROR_INVALID_FORMAT,
    JVX_ERROR_REQUEST_CALL_AGAIN,
    JVX_ERROR_NOT_IMPLEMENTED,
    JVX_ERROR_MORE_DATA,
    JVX_ERROR_LIMIT,
}
// < jvx_system_error_types.h

// > jvx_system_dataformats.h
#[cfg(feature = "jvx-system-use-data-format-float")]
pub type JvxData = f32;
#[cfg(not(feature = "jvx-system-use-data-format-float"))]
pub type JvxData = f64;

pub type JvxHandle = core::ffi::c_void;
// < jvx_system_dataformats.h

pub type JvxDspBaseErrorType = JvxErrorType;
pub type JvxSize = usize;
pub type JvxCBool = u16;
pub type JvxCBitField = u64;

pub const JVX_SIZE_UNSELECTED: usize = usize::MAX;

pub const C_TRUE: u16 = 1 as u16;
pub const C_FALSE: u16 = 0 as u16;

#[cfg(feature = "jvx-system-use-data-format-float")]
pub const MAT_EPS_MIN: JvxData = 1.192092895507812500000000000000e-007;
#[cfg(feature = "jvx-system-use-data-format-float")]
pub const MAT_EPS_MAX: JvxData = 3.402823466385288600000000000000e+038;
#[cfg(not(feature = "jvx-system-use-data-format-float"))]
pub const MAT_EPS_MIN: JvxData = 2.220446049250313100000000000000e-016;
#[cfg(not(feature = "jvx-system-use-data-format-float"))]
pub const MAT_EPS_MAX: JvxData = 1.797693134862315700000000000000e+308;

// > jvx_profiler_data_entry.h
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
        if !$hdl.is_null() {
            let dbg = unsafe { &*$hdl };
            if (dbg.specData & ((JvxCBitField)1 << $spec as u8)) && !dbg.$entryTo.fld.is_null() {
                assert_eq!(dbg.$entryTo.sz_elm, $szFrom);
                if dbg.$entryTo.cplx {
                    core::ptr::copy($ptrFrom as *JvxDataCplx, dbg.$entryTo.fld, $szFrom);
                } else {
                    core::ptr::copy($ptrFrom as JvxData, dbg.$entryTo.fld, $szFrom);
                }
            }
        }
    }
}
// < jvx_profiler_data_entry.h
