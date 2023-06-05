#ifndef _IJVXPROPERTYEXTENDERSPATIALDIRECTIONDIRECT_H__
#define _IJVXPROPERTYEXTENDERSPATIALDIRECTIONDIRECT_H__

JVX_INTERFACE IjvxPropertyExtenderSpatialDirectionDirect
{
public:
	virtual ~IjvxPropertyExtenderSpatialDirectionDirect(){};
	
	virtual jvxErrorType set_spatial_position(jvxData valueAzimuth, jvxData valueInclination) = 0;
};

#endif
