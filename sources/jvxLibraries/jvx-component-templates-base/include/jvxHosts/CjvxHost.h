#ifndef __CJVXHOST_H__
#define __CJVXHOST_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "common/CjvxProperties.h"

#include "common/CjvxSequencer.h"

#include "jvxHosts/CjvxHostTypeHandler.h"
#include "jvxHosts/CjvxHostInteraction.h"
#include "jvxHosts/CjvxDataConnections.h"
#include "jvxHosts/CjvxMinHost.h"
#include "jvxHosts/CjvxUniqueId.h"					

#include "pcg_CjvxHost_pcg.h"

/*
#define JVX_HBDX1_CHECK_CALLING_THREAD_ID \
	JVX_THREAD_ID theThreadId = JVX_GET_CURRENT_THREAD_ID(); \
	if(this->_common_set_host.threadId_registered != theThreadId) \
	{ \
		return(JVX_ERROR_THREADING_MISMATCH); \
	}
*/

class CjvxHost: 
	public IjvxHost, public CjvxObject, public CjvxMinHost,
	public CjvxHostInteraction, public IjvxToolsHost, /* <- we need to add this interface to call one function from the higher class layer in this class*/
	public IjvxProperties, public CjvxProperties, 
	public IjvxSequencer, public CjvxSequencer, 
	public IjvxDataConnections, public CjvxDataConnections,	
	public IjvxUniqueId, public CjvxUniqueId,
	public IjvxPropertyAttach,
	public CjvxHost_genpcg
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
	virtual jvxErrorType JVX_CALLINGCONVENTION number_components_system(const jvxComponentIdentification&, jvxSize* num) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION name_component_system(const jvxComponentIdentification&, jvxSize idx, jvxApiString*) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION description_component_system(const jvxComponentIdentification&, jvxSize idx, jvxApiString*) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_component_system(const jvxComponentIdentification&, jvxSize idx, jvxApiString*, jvxApiString*) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION module_reference_component_system(const jvxComponentIdentification&, jvxSize idx, jvxApiString*, jvxComponentAccessType* acTp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION feature_class_component_system(const jvxComponentIdentification&, jvxSize idx, jvxBitField*) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION number_slots_component_system(
		const jvxComponentIdentification&, jvxSize* szSlots, 
		jvxSize* szSubPlots, jvxComponentType* parentTp, 
		jvxComponentType* childTp, jvxSize* szSlots_max, 
		jvxSize* szSubSlots_max ) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION set_number_subslots_system(
		const jvxComponentIdentification&, jvxSize newVal) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION select_component(
		jvxComponentIdentification&, jvxSize, 
		IjvxObject* theOwner,
		jvxBool extend_if_necessary ) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION selection_component(const jvxComponentIdentification&, jvxSize*) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION activate_selected_component(const jvxComponentIdentification&) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready_selected_component(const jvxComponentIdentification&, jvxBool* ready, jvxApiString* reasonIfNot) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate_selected_component(const jvxComponentIdentification&) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_selected_component(jvxComponentIdentification&) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION state_selected_component(const jvxComponentIdentification&, jvxState*) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION description_selected_component(const jvxComponentIdentification&, jvxApiString*) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_selected_component(const jvxComponentIdentification&, jvxApiString*, jvxApiString* substr = NULL) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION module_reference_selected_component(const jvxComponentIdentification&, jvxApiString*, jvxComponentAccessType* acTp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION feature_class_selected_component(const jvxComponentIdentification&, jvxBitField*) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION request_object_selected_component(const jvxComponentIdentification&, IjvxObject**) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_object_selected_component(const jvxComponentIdentification&, IjvxObject*) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unique_id_selected_component(const jvxComponentIdentification& tp, jvxSize* uId) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface_selected_component(const jvxComponentIdentification&, jvxInterfaceType tp, jvxHandle** iface) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface_selected_component(const jvxComponentIdentification&, jvxInterfaceType tp, jvxHandle* iface) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION switch_state_component(const jvxComponentIdentification& cpId, jvxStateSwitch sswitch) override;

	// ====================================================================================
	// Interface IjvxToolsHost
	// ====================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION number_tools(const jvxComponentIdentification&, jvxSize* num) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION identification_tool(const jvxComponentIdentification&, jvxSize idx, jvxApiString* description,
		jvxApiString* descriptor, jvxBool* multipleInstances) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_tool(const jvxComponentIdentification&, IjvxObject** theObject, jvxSize filter_id,
		const char* filter_descriptor, jvxBitField filter_stateMask = (jvxBitField)JVX_STATE_DONTCARE) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_tool(const jvxComponentIdentification&, IjvxObject* theObject) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION instance_tool(jvxComponentType, IjvxObject** theObject, jvxSize filter_id, const char* filter_descriptor) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_instance_tool(jvxComponentType tp, IjvxObject* theObject, jvxSize filter_id, const char* filter_descriptor) override;

	// ====================================================================================
	// Interface IjvxHiddenInterface
	// ====================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION object_hidden_interface(IjvxObject** objRef) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl) override;

	// ====================================================================================
	// Interface: IjvxPropertyExpose
	// ====================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION attach_property_submodule(const char* prefix, IjvxProperties* props) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION detach_property_submodule(IjvxProperties* props) override;

#include "codeFragments/simplify/jvxObjects_simplify.h"

#include "codeFragments/simplify/jvxProperties_simplify.h"
#include "codeFragments/simplify/jvxHostInteraction_simplify.h"
#include "codeFragments/simplify/jvxHostTypeHandler_simplify.h"
#include "codeFragments/simplify/jvxSequencer_simplify.h"

#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"

#include "codeFragments/simplify/jvxDataConnections_simplify.h"
#include "codeFragments/simplify/jvxMinHost_simplify.h"
#include "codeFragments/simplify/jvxUniqueId_simplify.h"

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

	jvxErrorType connection_factory_to_be_added(jvxComponentIdentification tp_activated, IjvxConnectorFactory* add_this, IjvxConnectionMasterFactory* and_this);
	jvxErrorType connection_factory_to_be_removed(jvxComponentIdentification tp_activated, IjvxConnectorFactory* rem_this, IjvxConnectionMasterFactory* and_this);

	virtual void load_dynamic_objects();
	virtual void unload_dynamic_objects();

	void add_self_reference();
	void remove_self_reference();

	// A unique id to identify things within the host scope during one session. This id does not 
	// allow global and infinite identifiation as UUIDs do
	jvxSize myUniqueIdSelf;
	jvxBool bootComplete = false;
private:

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_command_post_set);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(cb_command_pre_get);

	// File CjvxHost-tpl.cpp
	template <class T> jvxErrorType t_reference_tool(
		std::vector<oneObjType<T>>& registeredTypes,
		const jvxComponentIdentification& tp,
		IjvxObject** theObject, jvxSize filter_id,
		const char* filter_descriptor,
		jvxBitField filter_stateMask);

	template <class T> jvxErrorType t_return_reference_tool(
		std::vector<oneObjType<T>>& registeredTypes,
			jvxComponentType tp,
			IjvxObject* theObject);

	// File CjvxHost_components-tpl.cpp
	template <class T> jvxErrorType t_select_component(std::vector<oneObjType<T>>& registeredObjs,
		jvxComponentIdentification& tp, jvxSize idx,
		IjvxObject* theOwner, jvxBool extend_if_necessary);

	template <class T> jvxErrorType t_activate_component(std::vector<oneObjType<T>>& registeredObjs,
		const jvxComponentIdentification& tp);

	template <class T> jvxErrorType t_deactivate_component(std::vector<oneObjType<T>>& registeredObjs,
		const jvxComponentIdentification& tp);

	template <class T> jvxErrorType t_unselect_component(std::vector<oneObjType<T>>& registeredObjs,
		jvxComponentIdentification& tp);

	static jvxErrorType static_local_select(IjvxHiddenInterface* theHinterface,
			IjvxCoreStateMachine* theObjectSm,
			IjvxObject* theObject,
			const jvxComponentIdentification& tpId,
			IjvxObject* theOwner);

	static jvxErrorType static_local_unselect(IjvxHiddenInterface* theHinterface,
		IjvxCoreStateMachine* theObjectSm);

};

#endif
