function [x, y, z] = jvx_spherical_to_cart(r, theta, phi)
    
    z = sin(theta) * r;
    rr = cos(theta) *r;
	x = sin(phi) * rr;
	y = cos(phi) * rr;
