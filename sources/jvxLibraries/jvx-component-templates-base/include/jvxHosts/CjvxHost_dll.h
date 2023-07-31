#ifndef __CJVXHOST_DLL_H__
#define __CJVXHOST_DLL_H__

#include "jvxHosts/CjvxHost.h"

class CjvxHost_dll: public CjvxHost
{
public:

	JVX_CALLINGCONVENTION CjvxHost_dll(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxHost_dll();


protected:

	//! Functions to load and unload the dlls - to be redefined from CjvxHost class
	void load_dynamic_objects() override;
	void unload_dynamic_objects() override;
};

#endif