#ifndef __CJVXFULLMASTERDEVICE_H__
#define __CJVXFULLMASTERDEVICE_H__

#include "common/CjvxSimpleMasterDevice.h"
#include "common/CjvxProperties.h"
#include "pcg_CjvxFullMasterDevice_pcg.h"
#include "common/CjvxNegotiate.h"

class CjvxFullMasterDevice : public CjvxSimpleMasterDevice, 
	public IjvxProperties, public CjvxProperties,
	public IjvxConfiguration,
	public CjvxFullMasterDevice_genpcg
{
protected:

	CjvxNegotiate_input neg_input;
	CjvxNegotiate_output neg_output;
	jvxLinkDataDescriptor_con_params inputParams;
	jvxLinkDataDescriptor_con_params outputParams;

public:
	JVX_CALLINGCONVENTION CjvxFullMasterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxFullMasterDevice();

public:

	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType accept_output_parameters_start(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	virtual jvxErrorType accept_output_parameters_stop(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	jvxErrorType JVX_CALLINGCONVENTION test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

#include "codeFragments/simplify/jvxObjects_simplify.h"

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"

#define JVX_STATEMACHINE_FULL_SKIP_ACTIVATE_DEACTIVATE
#include "codeFragments/simplify/jvxStateMachineFull_simplify.h"
#undef JVX_STATEMACHINE_FULL_SKIP_ACTIVATE_DEACTIVATE

	// Link IjvxConfiguration and CjvxProperties
#include "codeFragments/simplify/jvxProperties_simplify.h"

	// Configuration is simple - it need not be connected
	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename = "", jvxInt32 lineno = -1) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections) override;

#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_BASE_CLASS CjvxSimpleMasterDevice
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_BASE_CLASS
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
};

#endif
