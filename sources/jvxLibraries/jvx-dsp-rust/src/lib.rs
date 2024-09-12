#![allow(non_camel_case_types)]
// > jvx_system_common.h
const JVX_LIB_UPDATE_ASYNC_SHIFT: u8 = 0;
const JVX_LIB_UPDATE_SYNC_SHIFT: u8 = 1;
const JVX_LIB_UPDATE_INIT_SHIFT: u8 = 2;
const JVX_LIB_UPDATE_FEEDBACK_SHIFT: u8 = 3;

const JVX_LIB_UPDATE_USER_OFFSET_SHIFT: u8 = 5;

#[repr(C)]
pub enum JvxParameterUpdateType {
        JVX_LIB_UPDATE_ASYNC = (1 << JVX_LIB_UPDATE_ASYNC_SHIFT), //0x1, // shift 0
        JVX_LIB_UPDATE_SYNC = (1 << JVX_LIB_UPDATE_SYNC_SHIFT), //  0x2, // shift 1
        JVX_LIB_UPDATE_INIT = (1 << JVX_LIB_UPDATE_INIT_SHIFT), //0x4, // shift 2
        JVX_LIB_UPDATE_FEEDBACK = (1 << JVX_LIB_UPDATE_FEEDBACK_SHIFT), //0x8, // shift 3
        JVX_LIB_UPDATE_USER_OFFSET = (1 << JVX_LIB_UPDATE_USER_OFFSET_SHIFT)
}
// < jvx_system_common.h

// jvx_dsp_base.h
pub const JVX_DSP_UPDATE_ASYNC: u16 = JvxParameterUpdateType::JVX_LIB_UPDATE_ASYNC as u16;
pub const JVX_DSP_UPDATE_SYNC: u16 = JvxParameterUpdateType::JVX_LIB_UPDATE_SYNC as u16;
pub const JVX_DSP_UPDATE_INIT: u16 = JvxParameterUpdateType::JVX_LIB_UPDATE_INIT as u16;
pub const JVX_DSP_UPDATE_FEEDBACK: u16 = JvxParameterUpdateType::JVX_LIB_UPDATE_FEEDBACK as u16;
pub const JVX_DSP_UPDATE_USER_OFFSET: u16 = JvxParameterUpdateType::JVX_LIB_UPDATE_USER_OFFSET as u16;
// > jvx_dsp_base.h

// jvx_system_error_types.h
#[repr(C)]
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
        JVX_ERROR_LIMIT
}
// < jvx_system_error_types.h

// > jvx_system_dataformats.h
#[cfg(feature = "jvx-system-use-data-format-float")]
pub type JvxData = f32;
#[cfg(not(feature = "jvx-system-use-data-format-float"))]
pub type JvxData = f64;
// < jvx_system_dataformats.h

pub type JvxDspBaseErrorType = JvxErrorType;
pub type JvxSize = usize;
