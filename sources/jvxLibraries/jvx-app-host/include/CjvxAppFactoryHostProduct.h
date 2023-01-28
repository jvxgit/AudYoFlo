#ifndef CJVXAPPFACTORYHOSTPRODUCT_H__
#define CJVXAPPFACTORYHOSTPRODUCT_H__

#include "CjvxAppHostBase.h"

#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS
#define JVX_APP_FACTORY_HOST_PRODUCT_CLASSNAME CjvxAppFactoryHostProduct_nd
#else
#define JVX_APP_FACTORY_HOST_PRODUCT_CLASSNAME CjvxAppFactoryHostProduct
#endif

class JVX_APP_FACTORY_HOST_PRODUCT_CLASSNAME : public JVX_APP_FACTORY_HOST_CLASSNAME
{
protected:
	JVX_APP_FACTORY_HOST_PRODUCT_CLASSNAME() {};
	~JVX_APP_FACTORY_HOST_PRODUCT_CLASSNAME() {};

	//virtual jvxErrorType boot_initialize_product() override;
};

#endif