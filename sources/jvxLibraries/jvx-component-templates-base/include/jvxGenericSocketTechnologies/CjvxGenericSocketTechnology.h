#ifndef _CJVXGENERICRS232TECHNOLOGY_H__
#define _CJVXGENERICRS232TECHNOLOGY_H__

#include "jvxGenericConnectionTechnologies/CjvxGenericConnectionTechnology.h"

#include "pcg_CjvxGenericSocketTechnology_pcg.h"

class CjvxGenericSocketTechnology : public CjvxGenericConnectionTechnology, public CjvxGenericSocketTechnology_pcg
{
protected:

	// Start client, UDP, passive (no connect), local port 65000
	std::string txtConfigure = "udp";

public:
	JVX_CALLINGCONVENTION CjvxGenericSocketTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxGenericSocketTechnology();

	jvxErrorType activate() override;
	jvxErrorType deactivate() override;
	jvxErrorType initializeConnectionCoreModule(IjvxConnection* conn)override;
};

#endif