#ifndef __CJVXGENERICRS232DEVICE_H__
#define __CJVXGENERICRS232DEVICE_H__

#include "jvxGenericConnectionTechnologies/CjvxGenericConnectionDevice.h"

#include "pcg_CjvxGenericRS232Device_pcg.h"

class CjvxGenericRS232Device : public CjvxGenericConnectionDevice,
	public CjvxGenericRs232Device_pcg
{
public:
	JVX_CALLINGCONVENTION CjvxGenericRS232Device(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxGenericRS232Device();

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename = "", jvxInt32 lineno = -1)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	void select_connection_specific() override;
	void unselect_connection_specific() override;
	jvxErrorType activate_connection_port() override;
	jvxErrorType deactivate_connection_port() override;
};

#endif
