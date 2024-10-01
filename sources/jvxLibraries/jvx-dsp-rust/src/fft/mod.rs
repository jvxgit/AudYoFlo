// see sources/jvxLibraries/jvx-dsp-base/include/jvx_fft_tools/jvx_fft_core.h
// TODO ensure jvx-dsp is linked!

use crate::*;
use alloc::rc::Rc;

pub mod ffi;

pub enum FftSize {
    Size16,
    Size32,
    Size64,
    Size128,
    Size256,
    Size512,
    Size1024,
    Size2048,
    Size4096,
    Size8192,
    ArbitrarySize(JvxSize),
}

impl From<FftSize> for ffi::JvxFFTSize {
    fn from(item: FftSize) -> Self {
        match item {
            FftSize::Size16 => Self::JVX_FFT_TOOLS_FFT_SIZE_16,
            FftSize::Size32 => Self::JVX_FFT_TOOLS_FFT_SIZE_32,
            FftSize::Size64 => Self::JVX_FFT_TOOLS_FFT_SIZE_64,
            FftSize::Size128 => Self::JVX_FFT_TOOLS_FFT_SIZE_128,
            FftSize::Size256 => Self::JVX_FFT_TOOLS_FFT_SIZE_256,
            FftSize::Size512 => Self::JVX_FFT_TOOLS_FFT_SIZE_512,
            FftSize::Size1024 => Self::JVX_FFT_TOOLS_FFT_SIZE_1024,
            FftSize::Size2048 => Self::JVX_FFT_TOOLS_FFT_SIZE_2048,
            FftSize::Size4096 => Self::JVX_FFT_TOOLS_FFT_SIZE_4096,
            FftSize::Size8192 => Self::JVX_FFT_TOOLS_FFT_SIZE_8192,
            FftSize::ArbitrarySize(_) => Self::JVX_FFT_TOOLS_FFT_ARBITRARY_SIZE,
        }
    }
}

pub enum FftOperate {
    PreserveInput,
    Efficient,
}

impl From<FftOperate> for ffi::JvxFftIfftOperate {
    fn from(item: FftOperate) -> Self {
        match item {
            FftOperate::PreserveInput => Self::JVX_FFT_IFFT_PRESERVE_INPUT,
            FftOperate::Efficient => Self::JVX_FFT_IFFT_EFFICIENT,
        }
    }
}

pub struct FftBufferReal {
    buffer: *mut JvxData,
    num_elements: JvxSize,
}

impl FftBufferReal {
    pub fn new(num_elements: JvxSize) -> Self {
        Self {
            buffer: unsafe { ffi::jvx_allocate_fft_buffer_real(num_elements) },
            num_elements,
        }
    }

    pub fn as_slice(&self) -> &[JvxData] {
        unsafe { core::slice::from_raw_parts(self.buffer, self.num_elements) }
    }

    pub fn as_mut_slice(&self) -> &mut [JvxData] {
        unsafe { core::slice::from_raw_parts_mut(self.buffer, self.num_elements) }
    }
}

impl Drop for FftBufferReal {
    fn drop(&mut self) {
        unsafe {
            ffi::jvx_free_fft_buffer(self.buffer as *const core::ffi::c_void);
        }
    }
}

impl<Idx> core::ops::Index<Idx> for FftBufferReal
where
    Idx: core::slice::SliceIndex<[JvxData]>,
{
    type Output = Idx::Output;

    fn index(&self, index: Idx) -> &Self::Output {
        &self.as_slice()[index]
    }
}

impl<Idx> core::ops::IndexMut<Idx> for FftBufferReal
where
    Idx: core::slice::SliceIndex<[JvxData]>,
{
    fn index_mut(&mut self, index: Idx) -> &mut Self::Output {
        &mut self.as_mut_slice()[index]
    }
}

pub struct FftBufferCplx {
    buffer: *mut JvxDataCplx,
    num_elements: JvxSize,
}

impl FftBufferCplx {
    pub fn new(num_elements: JvxSize) -> Self {
        Self {
            buffer: unsafe { ffi::jvx_allocate_fft_buffer_complex(num_elements) },
            num_elements,
        }
    }

    pub fn as_slice(&self) -> &[JvxDataCplx] {
        unsafe { core::slice::from_raw_parts(self.buffer, self.num_elements) }
    }

    pub fn as_mut_slice(&self) -> &mut [JvxDataCplx] {
        unsafe { core::slice::from_raw_parts_mut(self.buffer, self.num_elements) }
    }
}

impl Drop for FftBufferCplx {
    fn drop(&mut self) {
        unsafe {
            ffi::jvx_free_fft_buffer(self.buffer as *const core::ffi::c_void);
        }
    }
}

impl<Idx> core::ops::Index<Idx> for FftBufferCplx
where
    Idx: core::slice::SliceIndex<[JvxDataCplx]>,
{
    type Output = Idx::Output;

    fn index(&self, index: Idx) -> &Self::Output {
        &self.as_slice()[index]
    }
}

impl<Idx> core::ops::IndexMut<Idx> for FftBufferCplx
where
    Idx: core::slice::SliceIndex<[JvxDataCplx]>,
{
    fn index_mut(&mut self, index: Idx) -> &mut Self::Output {
        &mut self.as_mut_slice()[index]
    }
}

pub struct FftFactory {
    pub hdl: *const ffi::JvxFFTGlobal,
}

impl FftFactory {
    pub fn new(size_max: FftSize) -> Result<Rc<Self>> {
        let mut hdl: *const ffi::JvxFFTGlobal = core::ptr::null();
        match unsafe { ffi::jvx_create_fft_ifft_global(&mut hdl, size_max.into()) } {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(Rc::new(Self { hdl })),
            err => Err(err),
        }
    }

    pub fn new_fft_real_2_complex<'a, 'b>(
        self: &Rc<Self>,
        size: FftSize,
        operate: FftOperate,
        input: Option<&'a mut FftBufferReal>,
        output: Option<&'b mut FftBufferCplx>,
    ) -> Result<FftReal2Cplx<'a, 'b>> {
        let mut hdl: *const ffi::JvxFFT = core::ptr::null();
        let mut input_n: *mut JvxData = core::ptr::null_mut();
        let mut output_2np1: *mut JvxDataCplx = core::ptr::null_mut();
        let mut n_fft_size: JvxSize = 0;
        let size_arbitrary = match size {
            FftSize::ArbitrarySize(x) => x,
            _ => 0,
        };
        let res = unsafe {
            ffi::jvx_create_fft_real_2_complex(
                &mut hdl,
                self.hdl,
                size.into(),
                &mut input_n,
                &mut output_2np1,
                &mut n_fft_size,
                operate.into(),
                input
                    .as_ref()
                    .map(|x| x.buffer)
                    .unwrap_or(core::ptr::null_mut()),
                output
                    .as_ref()
                    .map(|x| x.buffer)
                    .unwrap_or(core::ptr::null_mut()),
                size_arbitrary,
            )
        };
        let input_n = if let Some(buffer) = input {
            RealBufferRef::External(buffer)
        } else {
            RealBufferRef::Internal((input_n, n_fft_size))
        };
        let output_2np1 = if let Some(buffer) = output {
            CplxBufferRef::External(buffer)
        } else {
            CplxBufferRef::Internal((output_2np1, n_fft_size * 2 + 1))
        };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(FftReal2Cplx {
                _global: Rc::clone(self),
                hdl,
                n_fft_size,
                input_n,
                output_2np1,
            }),
            err => Err(err),
        }
    }

    pub fn new_fft_complex_2_complex<'a, 'b>(
        self: &Rc<Self>,
        size: FftSize,
        operate: FftOperate,
        input: Option<&'a mut FftBufferCplx>,
        output: Option<&'b mut FftBufferCplx>,
    ) -> Result<FftCplx2Cplx<'a, 'b>> {
        let mut hdl: *const ffi::JvxFFT = core::ptr::null();
        let mut input_n: *mut JvxDataCplx = core::ptr::null_mut();
        let mut output_2np1: *mut JvxDataCplx = core::ptr::null_mut();
        let mut n_fft_size: JvxSize = 0;
        let size_arbitrary = match size {
            FftSize::ArbitrarySize(x) => x,
            _ => 0,
        };
        let res = unsafe {
            ffi::jvx_create_fft_complex_2_complex(
                &mut hdl,
                self.hdl,
                size.into(),
                &mut input_n,
                &mut output_2np1,
                &mut n_fft_size,
                operate.into(),
                input
                    .as_ref()
                    .map(|x| x.buffer)
                    .unwrap_or(core::ptr::null_mut()),
                output
                    .as_ref()
                    .map(|x| x.buffer)
                    .unwrap_or(core::ptr::null_mut()),
                size_arbitrary,
            )
        };
        let input_n = if let Some(buffer) = input {
            CplxBufferRef::External(buffer)
        } else {
            CplxBufferRef::Internal((input_n, n_fft_size))
        };
        let output_2np1 = if let Some(buffer) = output {
            CplxBufferRef::External(buffer)
        } else {
            CplxBufferRef::Internal((output_2np1, n_fft_size * 2 + 1))
        };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(FftCplx2Cplx {
                _global: Rc::clone(self),
                hdl,
                n_fft_size,
                input_n,
                output_2np1,
            }),
            err => Err(err),
        }
    }

    pub fn new_ifft_complex_2_real<'a, 'b>(
        self: &Rc<Self>,
        size: FftSize,
        operate: FftOperate,
        input: Option<&'a mut FftBufferCplx>,
        output: Option<&'b mut FftBufferReal>,
    ) -> Result<IfftCplx2Real<'a, 'b>> {
        let mut hdl: *const ffi::JvxFFT = core::ptr::null();
        let mut input_2np1: *mut JvxDataCplx = core::ptr::null_mut();
        let mut output_n: *mut JvxData = core::ptr::null_mut();
        let mut n_fft_size: JvxSize = 0;
        let size_arbitrary = match size {
            FftSize::ArbitrarySize(x) => x,
            _ => 0,
        };
        let res = unsafe {
            ffi::jvx_create_ifft_complex_2_real(
                &mut hdl,
                self.hdl,
                size.into(),
                &mut input_2np1,
                &mut output_n,
                &mut n_fft_size,
                operate.into(),
                input
                    .as_ref()
                    .map(|x| x.buffer)
                    .unwrap_or(core::ptr::null_mut()),
                output
                    .as_ref()
                    .map(|x| x.buffer)
                    .unwrap_or(core::ptr::null_mut()),
                size_arbitrary,
            )
        };
        let input_2np1 = if let Some(buffer) = input {
            CplxBufferRef::External(buffer)
        } else {
            CplxBufferRef::Internal((input_2np1, n_fft_size))
        };
        let output_n = if let Some(buffer) = output {
            RealBufferRef::External(buffer)
        } else {
            RealBufferRef::Internal((output_n, n_fft_size * 2 + 1))
        };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(IfftCplx2Real {
                _global: Rc::clone(self),
                hdl,
                n_fft_size,
                input_2np1,
                output_n,
            }),
            err => Err(err),
        }
    }

    pub fn new_ifft_complex_2_complex<'a, 'b>(
        self: &Rc<Self>,
        size: FftSize,
        operate: FftOperate,
        input: Option<&'a mut FftBufferCplx>,
        output: Option<&'b mut FftBufferCplx>,
    ) -> Result<IfftCplx2Cplx<'a, 'b>> {
        let mut hdl: *const ffi::JvxFFT = core::ptr::null();
        let mut input_2np1: *mut JvxDataCplx = core::ptr::null_mut();
        let mut output_n: *mut JvxDataCplx = core::ptr::null_mut();
        let mut n_fft_size: JvxSize = 0;
        let size_arbitrary = match size {
            FftSize::ArbitrarySize(x) => x,
            _ => 0,
        };
        let res = unsafe {
            ffi::jvx_create_ifft_complex_2_complex(
                &mut hdl,
                self.hdl,
                size.into(),
                &mut input_2np1,
                &mut output_n,
                &mut n_fft_size,
                operate.into(),
                input
                    .as_ref()
                    .map(|x| x.buffer)
                    .unwrap_or(core::ptr::null_mut()),
                output
                    .as_ref()
                    .map(|x| x.buffer)
                    .unwrap_or(core::ptr::null_mut()),
                size_arbitrary,
            )
        };
        let input_2np1 = if let Some(buffer) = input {
            CplxBufferRef::External(buffer)
        } else {
            CplxBufferRef::Internal((input_2np1, n_fft_size))
        };
        let output_n = if let Some(buffer) = output {
            CplxBufferRef::External(buffer)
        } else {
            CplxBufferRef::Internal((output_n, n_fft_size * 2 + 1))
        };
        match res {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(IfftCplx2Cplx {
                _global: Rc::clone(self),
                hdl,
                n_fft_size,
                input_2np1,
                output_n,
            }),
            err => Err(err),
        }
    }
}

impl Drop for FftFactory {
    fn drop(&mut self) {
        unsafe {
            ffi::jvx_destroy_fft_ifft_global(self.hdl);
        }
    }
}

enum RealBufferRef<'a> {
    External(&'a mut FftBufferReal),
    Internal((*mut JvxData, JvxSize)),
}

impl RealBufferRef<'_> {
    pub fn as_slice(&self) -> &[JvxData] {
        match self {
            Self::External(x) => x.as_slice(),
            Self::Internal((ptr, size)) => unsafe { core::slice::from_raw_parts(*ptr, *size) },
        }
    }

    pub fn as_mut_slice(&self) -> &mut [JvxData] {
        match self {
            Self::External(ref x) => x.as_mut_slice(),
            Self::Internal((ptr, size)) => unsafe { core::slice::from_raw_parts_mut(*ptr, *size) },
        }
    }
}

enum CplxBufferRef<'a> {
    External(&'a mut FftBufferCplx),
    Internal((*mut JvxDataCplx, JvxSize)),
}

impl CplxBufferRef<'_> {
    pub fn as_slice(&self) -> &[JvxDataCplx] {
        match self {
            Self::External(x) => x.as_slice(),
            Self::Internal((ptr, size)) => unsafe { core::slice::from_raw_parts(*ptr, *size) },
        }
    }

    pub fn as_mut_slice(&self) -> &mut [JvxDataCplx] {
        match self {
            Self::External(ref x) => x.as_mut_slice(),
            Self::Internal((ptr, size)) => unsafe { core::slice::from_raw_parts_mut(*ptr, *size) },
        }
    }
}

pub struct FftReal2Cplx<'a, 'b> {
    _global: Rc<FftFactory>,
    hdl: *const ffi::JvxFFT,
    n_fft_size: JvxSize,
    input_n: RealBufferRef<'a>,
    output_2np1: CplxBufferRef<'b>,
}

impl FftReal2Cplx<'_, '_> {
    pub fn execute(&self) -> Result<()> {
        match unsafe { ffi::jvx_execute_fft(self.hdl) } {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(()),
            err => Err(err),
        }
    }

    pub fn input(&self) -> &[JvxData] {
        self.input_n.as_slice()
    }

    pub fn input_mut(&self) -> &mut [JvxData] {
        self.input_n.as_mut_slice()
    }

    pub fn output(&self) -> &[JvxDataCplx] {
        self.output_2np1.as_slice()
    }

    pub fn output_mut(&self) -> &mut [JvxDataCplx] {
        self.output_2np1.as_mut_slice()
    }

    pub fn n_fft_size(&self) -> JvxSize {
        self.n_fft_size
    }
}

impl Drop for FftReal2Cplx<'_, '_> {
    fn drop(&mut self) {
        unsafe {
            ffi::jvx_destroy_fft(self.hdl);
        }
    }
}

pub struct FftCplx2Cplx<'a, 'b> {
    _global: Rc<FftFactory>,
    hdl: *const ffi::JvxFFT,
    n_fft_size: JvxSize,
    input_n: CplxBufferRef<'a>,
    output_2np1: CplxBufferRef<'b>,
}

impl FftCplx2Cplx<'_, '_> {
    pub fn execute(&self) -> Result<()> {
        match unsafe { ffi::jvx_execute_fft(self.hdl) } {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(()),
            err => Err(err),
        }
    }

    pub fn input(&self) -> &[JvxDataCplx] {
        self.input_n.as_slice()
    }

    pub fn input_mut(&self) -> &mut [JvxDataCplx] {
        self.input_n.as_mut_slice()
    }

    pub fn output(&self) -> &[JvxDataCplx] {
        self.output_2np1.as_slice()
    }

    pub fn output_mut(&self) -> &mut [JvxDataCplx] {
        self.output_2np1.as_mut_slice()
    }

    pub fn n_fft_size(&self) -> JvxSize {
        self.n_fft_size
    }
}

impl Drop for FftCplx2Cplx<'_, '_> {
    fn drop(&mut self) {
        unsafe {
            ffi::jvx_destroy_fft(self.hdl);
        }
    }
}

pub struct IfftCplx2Real<'a, 'b> {
    _global: Rc<FftFactory>,
    hdl: *const ffi::JvxIFFT,
    n_fft_size: JvxSize,
    input_2np1: CplxBufferRef<'a>,
    output_n: RealBufferRef<'b>,
}

impl IfftCplx2Real<'_, '_> {
    pub fn execute(&self) -> Result<()> {
        match unsafe { ffi::jvx_execute_ifft(self.hdl) } {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(()),
            err => Err(err),
        }
    }

    pub fn input(&self) -> &[JvxDataCplx] {
        self.input_2np1.as_slice()
    }

    pub fn input_mut(&self) -> &mut [JvxDataCplx] {
        self.input_2np1.as_mut_slice()
    }

    pub fn output(&self) -> &[JvxData] {
        self.output_n.as_slice()
    }

    pub fn output_mut(&self) -> &mut [JvxData] {
        self.output_n.as_mut_slice()
    }

    pub fn n_fft_size(&self) -> JvxSize {
        self.n_fft_size
    }
}

impl Drop for IfftCplx2Real<'_, '_> {
    fn drop(&mut self) {
        unsafe {
            ffi::jvx_destroy_ifft(self.hdl);
        }
    }
}

pub struct IfftCplx2Cplx<'a, 'b> {
    _global: Rc<FftFactory>,
    hdl: *const ffi::JvxIFFT,
    n_fft_size: JvxSize,
    input_2np1: CplxBufferRef<'a>,
    output_n: CplxBufferRef<'b>,
}

impl IfftCplx2Cplx<'_, '_> {
    pub fn execute(&self) -> Result<()> {
        match unsafe { ffi::jvx_execute_ifft(self.hdl) } {
            JvxDspBaseErrorType::JVX_NO_ERROR => Ok(()),
            err => Err(err),
        }
    }

    pub fn input(&self) -> &[JvxDataCplx] {
        self.input_2np1.as_slice()
    }

    pub fn input_mut(&self) -> &mut [JvxDataCplx] {
        self.input_2np1.as_mut_slice()
    }

    pub fn output(&self) -> &[JvxDataCplx] {
        self.output_n.as_slice()
    }

    pub fn output_mut(&self) -> &mut [JvxDataCplx] {
        self.output_n.as_mut_slice()
    }

    pub fn n_fft_size(&self) -> JvxSize {
        self.n_fft_size
    }
}

impl Drop for IfftCplx2Cplx<'_, '_> {
    fn drop(&mut self) {
        unsafe {
            ffi::jvx_destroy_ifft(self.hdl);
        }
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn fft_real_to_complex_external_buffers() {
        let factory = FftFactory::new(FftSize::Size32).unwrap();
        let mut input = FftBufferReal::new(32);
        let mut output = FftBufferCplx::new(32 * 2 + 1);
        let fft = factory
            .new_fft_real_2_complex(
                FftSize::Size32,
                FftOperate::PreserveInput,
                Some(&mut input),
                Some(&mut output),
            )
            .unwrap();
        fft.execute().unwrap();
        // TODO supply input data and verify output data
    }

    #[test]
    fn fft_real_to_complex_internal_buffers() {
        let factory = FftFactory::new(FftSize::Size32).unwrap();
        let fft = factory
            .new_fft_real_2_complex(FftSize::Size32, FftOperate::PreserveInput, None, None)
            .unwrap();
        let mut _input = fft.input_mut();
        let _output = fft.output();
        fft.execute().unwrap();
        // TODO supply input data and verify output data
    }

    #[test]
    fn fft_complex_to_complex_external_buffers() {
        let factory = FftFactory::new(FftSize::Size32).unwrap();
        let mut input = FftBufferCplx::new(32);
        let mut output = FftBufferCplx::new(32 * 2 + 1);
        let fft = factory
            .new_fft_complex_2_complex(
                FftSize::Size32,
                FftOperate::PreserveInput,
                Some(&mut input),
                Some(&mut output),
            )
            .unwrap();
        fft.execute().unwrap();
        // TODO supply input data and verify output data
    }

    #[test]
    fn fft_complex_to_complex_internal_buffers() {
        let factory = FftFactory::new(FftSize::Size32).unwrap();
        let fft = factory
            .new_fft_complex_2_complex(FftSize::Size32, FftOperate::PreserveInput, None, None)
            .unwrap();
        let mut _input = fft.input_mut();
        let _output = fft.output();
        fft.execute().unwrap();
        // TODO supply input data and verify output data
    }
}
