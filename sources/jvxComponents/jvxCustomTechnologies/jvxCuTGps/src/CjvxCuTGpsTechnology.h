#ifndef __CjvxCuTGpsTechnology_H__
#define __CjvxCuTGpsTechnology_H__

#include "jvxGenericRs232Technologies/CjvxGenericRs232Technology.h"

class CjvxCuTGpsTechnology : public CjvxGenericRs232Technology
{

public:
	JVX_CALLINGCONVENTION CjvxCuTGpsTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxCuTGpsTechnology();

	virtual CjvxGenericConnectionDevice* allocate_device(const std::string& pName, jvxSize id) override;

};

#endif