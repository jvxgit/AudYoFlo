#ifndef CJVXAPPHOSTPRODUCT_H__
#define CJVXAPPHOSTPRODUCT_H__

#include "CjvxAppHostBase.h"

#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS
#define JVX_APPHOST_PRODUCT_CLASSNAME CjvxAppHostProduct_nd
#else
#define JVX_APPHOST_PRODUCT_CLASSNAME CjvxAppHostProduct
#endif

class JVX_APPHOST_PRODUCT_CLASSNAME : public JVX_APPHOST_CLASSNAME
{
protected:
	JVX_APPHOST_PRODUCT_CLASSNAME() {};
	~JVX_APPHOST_PRODUCT_CLASSNAME() {};

	virtual jvxErrorType boot_initialize_product() override;
	jvxErrorType shutdown_terminate_product() override;
};

#endif
