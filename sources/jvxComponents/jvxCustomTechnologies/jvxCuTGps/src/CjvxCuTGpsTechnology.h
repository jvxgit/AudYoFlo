#ifndef __CjvxCuTGpsTechnology_H__
#define __CjvxCuTGpsTechnology_H__

#include "jvxGenericConnectionTechnologies/CjvxGenericConnectionTechnology.h"

class CjvxCuTGpsTechnology : public CjvxGenericConnectionTechnology
{

public:
	JVX_CALLINGCONVENTION CjvxCuTGpsTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxCuTGpsTechnology();

	virtual CjvxGenericConnectionDevice* allocate_device(const std::string& pName, jvxSize id) override;

};

#endif