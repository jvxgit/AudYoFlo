#include "jvx.h"
#include "interfaces/all-hosts/configHostFeatures_common.h"

extern "C"
{
jvxErrorType jvx_configure_factoryhost_features(configureFactoryHost_features* features)
{
	return JVX_NO_ERROR;
}

jvxErrorType jvx_invalidate_factoryhost_features(configureFactoryHost_features* features)
{
	return(JVX_NO_ERROR);
}

}
	
