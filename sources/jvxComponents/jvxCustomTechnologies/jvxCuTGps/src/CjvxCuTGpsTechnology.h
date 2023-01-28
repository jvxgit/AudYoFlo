#ifndef __CjvxCuTGpsTechnology_H__
#define __CjvxCuTGpsTechnology_H__

#include "jvxGenericRS232Technologies/CjvxGenericRS232Technology.h"

class CjvxCuTGpsTechnology : public CjvxGenericRS232Technology
{

public:
	JVX_CALLINGCONVENTION CjvxCuTGpsTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxCuTGpsTechnology();

	virtual CjvxGenericRS232Device* allocate_device(const std::string& pName, jvxSize id) override;

};

#endif