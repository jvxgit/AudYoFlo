% Reference implementtion of a complex division WITHOUT polar representtion
function [out] = jvx_cplx_div(in, divisor)
    
    a3 = real(in);
    b3 = imag(in);

    a2 = real(divisor);
    b2 = imag(divisor);
    if(b2 == 0)
        b2 = eps;
    end
    
    a1 = ((a3*a2) + (b2*b3))/(a2*a2 + b2*b2);
    b1 = (a1 * a2 - a3)/b2;
    
    out = a1 + j*b1;
    
    