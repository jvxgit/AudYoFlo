#ifndef _CJVXGENERICRS232TECHNOLOGY_H__
#define _CJVXGENERICRS232TECHNOLOGY_H__

#include "jvxGenericConnectionTechnologies/CjvxGenericConnectionTechnology.h"

#include "pcg_CjvxGenericSocketTechnology_pcg.h"

class CjvxGenericSocketTechnology : public CjvxGenericConnectionTechnology, 
	public IjvxConfiguration,
	public CjvxGenericSocketTechnology_pcg
{
protected:

	// Start client, UDP, passive (no connect), local port 65000
	// std::string txtConfigure = "udp";

public:
	JVX_CALLINGCONVENTION CjvxGenericSocketTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxGenericSocketTechnology();

	jvxErrorType select(IjvxObject* owner) override;
	jvxErrorType unselect() override;

	jvxErrorType activate() override;
	jvxErrorType deactivate() override;

	jvxErrorType initializeConnectionCoreModule(IjvxConnection* conn)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename = "", jvxInt32 lineno = -1) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override;

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

};

#endif