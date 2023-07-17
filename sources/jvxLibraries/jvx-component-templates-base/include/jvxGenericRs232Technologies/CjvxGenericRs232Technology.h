#ifndef _CJVXGENERICRS232TECHNOLOGY_H__
#define _CJVXGENERICRS232TECHNOLOGY_H__

#include "jvxGenericConnectionTechnologies/CjvxGenericConnectionTechnology.h"

#include "pcg_CjvxGenericRs232Technology_pcg.h"

class CjvxGenericRs232Technology : public CjvxGenericConnectionTechnology, public CjvxGenericRs232Technology_pcg
{
protected:
	jvxSize numPortsPolled = JVX_SIZE_UNSELECTED;

public:
	JVX_CALLINGCONVENTION CjvxGenericRs232Technology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxGenericRs232Technology();

	jvxErrorType activate() override;
	jvxErrorType deactivate() override;

	jvxErrorType initializeConnectionCoreModule(IjvxConnection* conn) override;
};

#endif