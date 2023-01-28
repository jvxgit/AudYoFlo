#ifndef __CJVXNETWORKTECHNOLOGY_H__
#define __CJVXNETWORKTECHNOLOGY_H__

#include "jvxAudioTechnologies/CjvxAudioTechnology.h"
#include "jvxLsocket_common.h"
#include "pcg_exports_technology.h"
#include "jvx-helpers.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxNetworkTechnology: 
	public CjvxAudioTechnology, 
	public IjvxConfiguration, 
	public CjvxComponentTempConfig,
	public genNetworkMaster_technology
{
	friend class CjvxNetworkMasterDevice;
protected:

	jvx_connect_socket_type tp;
	jvxSize numReqDevices;


public:

	JVX_CALLINGCONVENTION CjvxNetworkTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxNetworkTechnology();

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect()override;

#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
#undef JVX_INTERFACE_SUPPORT_PROPERTIES

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename = "", jvxInt32 lineno = -1) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	void informDeviceSelected();
	void informDeviceUnselected();

	virtual jvxErrorType initializeUnderlyingConnection() = 0;
	virtual IjvxDevice* allocateOneDevice(const char* fldTxt, jvxSize idx) = 0;
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif