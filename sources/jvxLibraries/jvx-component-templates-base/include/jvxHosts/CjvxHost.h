#ifndef __CJVXFULLHOST_H__
#define __CJVXFULLHOST_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "common/CjvxProperties.h"

#include "common/CjvxSequencer.h"

#include "jvxHosts/CjvxHostTypeHandler.h"
#include "jvxHosts/CjvxHostInteraction.h"
#include "jvxHosts/CjvxComponentHostTools.h"
#include "jvxHosts/CjvxDataConnections.h"
#include "jvxHosts/CjvxMinHost.h"
#include "jvxHosts/CjvxUniqueId.h"	
#include "jvxFactoryHosts/CjvxDefaultInterfaceFactory.h"
#include "jvxFactoryHosts/CjvxInterfaceFactory.h"

/*
#define JVX_HBDX1_CHECK_CALLING_THREAD_ID \
	JVX_THREAD_ID theThreadId = JVX_GET_CURRENT_THREAD_ID(); \
	if(this->_common_set_host.threadId_registered != theThreadId) \
	{ \
		return(JVX_ERROR_THREADING_MISMATCH); \
	}
*/
// CjvxConnectionFHost = IjvxDataConnections, CjvxDataConnections, CjvxToolsInterfaceFactory
// CjvxToolsInterfaceFactory = CjvxDefaultInterfaceFactory, IjvxToolsHost, IjvxUniqueId, CjvxUniqueId,
//			IjvxProperties, CjvxProperties,
//			CjvxHostInteraction, IjvxConfiguration, IjvxConfigurationExtender,
//			CjvxFactoryHost_genpcg

class CjvxHost: 
	public CjvxInterfaceFactory<IjvxHost>,
	public CjvxComponentHostTools, 
	public IjvxSequencer, public CjvxSequencer, 
	public IjvxDataConnections, public CjvxDataConnections,	
	public IjvxPropertyAttach
{
public:

	JVX_CALLINGCONVENTION CjvxHost(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxHost();

	// ================================================================================================
	// Interface IjvxCoreStateMachine
	// ================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION initialize(IjvxHiddenInterface* theOtherHost) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION terminate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION system_ready() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION system_about_to_shutdown() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION owner(IjvxObject** dependsOn) override;

	// =================================================================================================
	// Interface IjvxHost
	// =================================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION boot_complete(jvxBool* bootComplete) override;
	
	// Second part is IjvxComponentHost realized by mix-in!
#include "codeFragments/simplify/jvxComponentHost_simplify.h"

	// ====================================================================================
	// Interface IjvxToolsHost
	// ====================================================================================

#include "codeFragments/simplify/jvxComponentHostTools_simplify.h"

	// ====================================================================================
	// Interface IjvxHiddenInterface
	// ====================================================================================
/*
* Implemented in <CjvxDefaultInterfaceFactory<IjvxHost> >
* #include "codeFragments/simplify/jvxObjects_simplify.h"
* #include "codeFragments/simplify/jvxMinHost_simplify.h"
*/
	// We define the functions from <jvxHiddenInterface_simplify> 
	virtual jvxErrorType JVX_CALLINGCONVENTION object_hidden_interface(IjvxObject** objRef) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl) override;

	// ====================================================================================
	// Interface: IjvxPropertyExpose
	// ====================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION attach_property_submodule(const char* prefix, IjvxProperties* props) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION detach_property_submodule(IjvxProperties* props) override;

#include "codeFragments/simplify/jvxHostInteraction_simplify.h"
#include "codeFragments/simplify/jvxHostTypeHandler_simplify.h"
#include "codeFragments/simplify/jvxSequencer_simplify.h"
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#include "codeFragments/simplify/jvxDataConnections_simplify.h"


protected:

	// =======================================================================================
	// Some private member functions
	// =======================================================================================

	//jvxErrorType prepareScannerDevice(IjvxScannerDevice* theDevice);
	//jvxErrorType prepareOneDevice(jvxComponentType tp);
	virtual jvxErrorType JVX_CALLINGCONVENTION sequencer_started(jvxInt64 timestamp_us) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION sequencer_step(jvxInt64 timestamp_us, jvxSize* delta_ms) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION sequencer_stopped(jvxInt64 timestamp_us) override;

	jvxErrorType unselectAllComponents();

	// These two functions extend the host to allow dynamic load of components
	virtual void load_dynamic_objects();
	virtual void unload_dynamic_objects();

	void add_self_reference();
	void remove_self_reference();

	// Overridden member function CjvxComponentHost
	jvxErrorType prerun_stateswitch(jvxStateSwitch ss, jvxComponentIdentification tp) override;
	jvxErrorType postrun_stateswitch(jvxStateSwitch ss, jvxComponentIdentification tp, jvxErrorType res) override;
	jvxErrorType connection_factory_to_be_added(jvxComponentIdentification tp_activated, IjvxConnectorFactory* add_this, IjvxConnectionMasterFactory* and_this) override;
	jvxErrorType connection_factory_to_be_removed(jvxComponentIdentification tp_activated, IjvxConnectorFactory* rem_this, IjvxConnectionMasterFactory* and_this) override;
	jvxState myState() override;
	void reportErrorDescription(const std::string& descr, jvxBool isWarning = false)override;
	jvxSize myRegisteredHostId() override;

	// A unique id to identify things within the host scope during one session. This id does not 
	// allow global and infinite identifiation as UUIDs do
	jvxSize myUniqueIdSelf;
	jvxBool bootComplete = false;
private:

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_command_post_set);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_command_pre_get);

	
};

#endif
