use crate::*;

/// Typedefs for window function
#[repr(i16)]
#[derive(Debug, Clone, Copy)]
pub enum JvxWindowType {
    JVX_WINDOW_RECT = 0,
    JVX_WINDOW_HAMMING = 1,
    JVX_WINDOW_BLACKMAN = 2,
    JVX_WINDOW_FLATTOP_HANN = 3,
    JVX_WINDOW_SQRT_FLATTOP_HANN = 4,
    JVX_WINDOW_HANN = 5,
    JVX_WINDOW_SQRT_HANN = 6,
    JVX_WINDOW_HALF_HANN = 7,
    JVX_WINDOW_HALF_LINEAR = 8,
    JVX_WINDOW_HALF_HAMMING = 9,
    JVX_WINDOW_HAMMING_ASYM = 10,
    JVX_WINDOW_KAISER = 11,
    JVX_WINDOW_LIMIT,
}

extern "C" {
    pub fn jvx_compute_window(
        ptrField: *mut JvxData,
        fieldSize: i32,
        secondArgDbl: JvxData,
        thirdArg: JvxData,
        winType: JvxWindowType,
        normalization: *mut JvxData,
    ) -> JvxDspBaseErrorType;
}
