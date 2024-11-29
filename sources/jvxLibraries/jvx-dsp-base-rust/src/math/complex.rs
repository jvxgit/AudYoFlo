use crate::*;

impl JvxDataCplx {
    /// complex multiplication
    pub fn multiply(&self, other: &Self) -> Self {
        Self {
    	    re: self.re * other.re - self.im * other.im,
    	    im: self.im * other.re + self.re * other.im,
        }
    }
    
    pub fn multiply_n(in1: &[Self], in2: &[Self], out: &mut [Self]) {
        assert_eq!(in1.len(), in2.len());
        assert_eq!(in1.len(), out.len());
        for ((in1_elem, in2_elem), out_elem) in in1.iter().zip(in2.iter()).zip(out.iter_mut()) {
            *out_elem = in1_elem.multiply(in2_elem);
        }
    }

    pub fn multiply_real_n(inout: &mut [Self], in2: &[JvxData]) {
        assert_eq!(inout.len(), in2.len());
        for (inout_elem, in2_elem) in inout.iter_mut().zip(in2.iter()) {
            *inout_elem = Self {
                re: inout_elem.re * in2_elem,
                im: inout_elem.im * in2_elem,
            }
        }
    }
    
    /// complex multiplication with conjugate first argument
    pub fn multiply_conj1(&self, other: &Self) -> Self {
        Self{
            re: self.re * other.re + self.im * other.im,
            im: self.re * other.im - self.im * other.re,
        }
    }
    
    pub fn multiply_conj1_n(in1: &[Self], in2: &[Self], out: &mut[Self]) {
        assert_eq!(in1.len(), in2.len());
        assert_eq!(in1.len(), out.len());
        for ((in1_elem, in2_elem), out_elem) in in1.iter().zip(in2.iter()).zip(out.iter_mut()) {
            *out_elem = in1_elem.multiply_conj1(in2_elem);
        }
    }

    /// square of magnitude
    pub fn square_of_magnitude(&self) -> JvxData {
        self.re * self.re + self.im * self.im
    }
    
    pub fn square_of_magnitude_n(input: &[Self], out: &mut [JvxData]) {
        assert_eq!(input.len(), out.len());
        for (in_elem, out_elem) in input.iter().zip(out.iter_mut()) {
            *out_elem = in_elem.square_of_magnitude()
        }
    }
    
    pub fn square_of_magnitude_add_n(input: &[Self], out: &mut [JvxData]) {
        for (in_elem, out_elem) in input.iter().zip(out.iter_mut()) {
            *out_elem += in_elem.square_of_magnitude();
        }
    }
}
