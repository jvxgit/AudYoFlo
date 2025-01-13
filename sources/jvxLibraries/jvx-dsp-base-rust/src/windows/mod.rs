use crate::*;

pub mod ffi;

#[derive(Clone, Copy)]
pub enum WindowType {
    Rect,
    Hamming,
    Blackman,
    FlattopHann,
    SqrtFlattopHann,
    Hann,
    SqrtHann,
    HalfHann,
    HalfLinear,
    HalfHamming,
    HammingAsym,
    Kaiser,
    Limit,
}

impl From<WindowType> for ffi::JvxWindowType {
    fn from(item: WindowType) -> Self {
        match item {
            WindowType::Rect => Self::JVX_WINDOW_RECT,
            WindowType::Hamming => Self::JVX_WINDOW_HAMMING,
            WindowType::Blackman => Self::JVX_WINDOW_BLACKMAN,
            WindowType::FlattopHann => Self::JVX_WINDOW_FLATTOP_HANN,
            WindowType::SqrtFlattopHann => Self::JVX_WINDOW_SQRT_FLATTOP_HANN,
            WindowType::Hann => Self::JVX_WINDOW_HANN,
            WindowType::SqrtHann => Self::JVX_WINDOW_SQRT_HANN,
            WindowType::HalfHann => Self::JVX_WINDOW_HALF_HANN,
            WindowType::HalfLinear => Self::JVX_WINDOW_HALF_LINEAR,
            WindowType::HalfHamming => Self::JVX_WINDOW_HALF_HAMMING,
            WindowType::HammingAsym => Self::JVX_WINDOW_HAMMING_ASYM,
            WindowType::Kaiser => Self::JVX_WINDOW_KAISER,
            WindowType::Limit => Self::JVX_WINDOW_LIMIT,
        }
    }
}

pub fn compute(ptr_field: &mut [JvxData], field_size: i32, second_arg: JvxData, third_arg: JvxData, win_type: WindowType) -> Result<JvxData> {
    let mut normalization = 1.0;
    match unsafe { ffi::jvx_compute_window(ptr_field.as_mut_ptr(), field_size, second_arg, third_arg, win_type.into(), &mut normalization) } {
        JvxDspBaseErrorType::JVX_NO_ERROR => Ok(normalization),
        err => Err(err),
    }
}
