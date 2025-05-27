#include "jvx_cart_polar/jvx_cart_polar.h"

jvxErrorType
jvx_polar_2_cartesion_deg(jvxData* in_azimIncl2, jvxData* out_xyz_vec3)
{ 
	// angleAzimuthDeg to the left positiv
    // angleElevationDeg 0-180 - 90° to the front!
    
    // https://stackoverflow.com/questions/20769011/converting-3d-polar-coordinates-to-cartesian-coordinates
    // x = sin(horizontal_angle) * cos(vertical_angle)
    // y = sin(horizontal_angle) * sin(vertical_angle)
    // z = cos(horizontal_angle)   
    	
    jvxData angleAzimuthRad = in_azimIncl2[0] / 180.0 * M_PI;
    jvxData angleInclinationRad = in_azimIncl2[1] /180.0 * M_PI;
    
	out_xyz_vec3[2] = cos(angleInclinationRad);
    jvxData rflat = sin(angleInclinationRad);
    
    out_xyz_vec3[0] = rflat * cos(angleAzimuthRad);
    out_xyz_vec3[1] = rflat * sin(angleAzimuthRad);
	return JVX_DSP_NO_ERROR;
}
   
jvxErrorType
jvx_cartesian_2_polar_deg(jvxData* in_xyz_vec3, jvxData* out_azimIncl2)
{
    // angleAzimuthDeg to the left positiv
    // angleElevationDeg 0-180 - 90° to the front!
    
    // https://stackoverflow.com/questions/20769011/converting-3d-polar-coordinates-to-cartesian-coordinates
    // x = sin(horizontal_angle) * cos(vertical_angle)
    // y = sin(horizontal_angle) * sin(vertical_angle)
    // z = cos(horizontal_angle)   
   // x = vec(1);
    //y = vec(2);
    //z = vec(3);
    
   jvxData angleInclinationRad = acos(in_xyz_vec3[2]);
   out_azimIncl2[1] = angleInclinationRad / M_PI * 180;
    
    jvxData rflat = sin(angleInclinationRad);
    jvxData angleAzimuthRad = 0;
    if (rflat > 0)
    {
        jvxData x = in_xyz_vec3[0];
        if (in_xyz_vec3[1] > 0)
        {
            angleAzimuthRad = acos(x / rflat);
        }
        else
        {
            angleAzimuthRad = -acos(x / rflat);
        }
    }
    out_azimIncl2[0] = angleAzimuthRad / M_PI * 180;
    return JVX_DSP_NO_ERROR;
  }
       
