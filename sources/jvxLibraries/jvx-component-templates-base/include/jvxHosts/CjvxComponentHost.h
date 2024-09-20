#ifndef __CJVXCOMPONENTHOST_H__
#define __CJVXCOMPONENTHOST_H__


#include "jvxHosts/CjvxHostTypeHandler.h"

class CjvxComponentHost: public CjvxHostTypeHandler
{
protected:
	IjvxDataConnections* datConns = nullptr;
	IjvxUniqueId* uIdInst = nullptr;
	IjvxHiddenInterface* hIfRef = nullptr;
	IjvxObject* myObjectRef = nullptr;

	// std::string myModuleName;
	// std::string myDescription;
	// std::string myDescriptor;
public:
	CjvxComponentHost();
	~CjvxComponentHost();

	jvxErrorType _number_components_system(const jvxComponentIdentification& tp, jvxSize* numPtr);

	jvxErrorType _name_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* name, jvxApiString* fName);

	jvxErrorType _description_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* str);

	jvxErrorType _descriptor_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* str, jvxApiString* substr);

	jvxErrorType _module_reference_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* str, jvxComponentAccessType* acTp);

	jvxErrorType _feature_class_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxBitField* ft);

	jvxErrorType _capabilities_devices_component_system(const jvxComponentIdentification&, jvxSize idx, jvxDeviceCapabilities& caps);

	jvxErrorType _role_component_system(jvxComponentType tp,
			jvxComponentType* parentTp, jvxComponentType* childTp,
			jvxComponentTypeClass* classTp);

	jvxErrorType _number_slots_component_system(const jvxComponentIdentification& tp, jvxSize* szSlots_current,
			jvxSize* szSubSlots_current, jvxSize* szSlots_max, jvxSize* szSubSlots_max);

	jvxErrorType _set_number_subslots_system(const jvxComponentIdentification& tp, jvxSize newVal);

	jvxErrorType _module_reference_selected_component(const jvxComponentIdentification& tp, jvxApiString* str, jvxComponentAccessType* acTp);

	jvxErrorType _description_selected_component(const jvxComponentIdentification& tp, jvxApiString* str);

	jvxErrorType _descriptor_selected_component(const jvxComponentIdentification& tp, jvxApiString* str, jvxApiString* substr);

	jvxErrorType _feature_class_selected_component(const jvxComponentIdentification& tp, jvxBitField* ft);

	jvxErrorType _select_component(jvxComponentIdentification& tp, jvxSize idx,
			IjvxObject* theOwner, jvxBool extend_if_necessary, jvxComponentType tpRemap);

	jvxErrorType _selection_component(const jvxComponentIdentification& tp, jvxSize* idRet, jvxApiString* modNmRet);
	jvxErrorType _activate_selected_component(const jvxComponentIdentification& tp);

	jvxErrorType _is_ready_selected_component(const jvxComponentIdentification& tp, jvxBool* ready, jvxApiString* reasonIfNot);

	jvxErrorType _state_selected_component(const jvxComponentIdentification& tp, jvxState* st);

	jvxErrorType _deactivate_selected_component(const jvxComponentIdentification& tp);

	jvxErrorType _unselect_selected_component(jvxComponentIdentification& tp);

	jvxErrorType _unique_id_selected_component(const jvxComponentIdentification& tp, jvxSize* uId);

	jvxErrorType _switch_state_component(const jvxComponentIdentification& cpId, jvxStateSwitch sswitch);

	jvxErrorType _request_hidden_interface_selected_component(const jvxComponentIdentification& tp, jvxInterfaceType ifTp, jvxHandle** iface);
	
	jvxErrorType _return_hidden_interface_selected_component(const jvxComponentIdentification& tp, jvxInterfaceType ifTp, jvxHandle* iface);

	jvxErrorType _request_object_selected_component(const jvxComponentIdentification& tp, IjvxObject** theObj);

	jvxErrorType _return_object_selected_component(const jvxComponentIdentification& tp, IjvxObject* theObj);

	// Virtual abstract functions to embed part of the code
	virtual jvxErrorType connection_factory_to_be_added(jvxComponentIdentification tp_activated, IjvxConnectorFactory* add_this, IjvxConnectionMasterFactory* and_this) = 0;
	virtual jvxErrorType connection_factory_to_be_removed(jvxComponentIdentification tp_activated, IjvxConnectorFactory* rem_this, IjvxConnectionMasterFactory* and_this) = 0;
	virtual jvxErrorType prerun_stateswitch(jvxStateSwitch ss, jvxComponentIdentification tp) = 0;
	virtual jvxErrorType postrun_stateswitch(jvxStateSwitch ss, jvxComponentIdentification tp, jvxErrorType res) = 0;
	virtual jvxState myState() = 0;
	virtual void reportErrorDescription(const std::string& descr, jvxBool isWarning = false) = 0;
	virtual jvxSize myRegisteredHostId() = 0;

	virtual jvxErrorType map_reference_tool(const jvxComponentIdentification& tp, IjvxObject** theObject, jvxSize filter_id,
		const char* filter_descriptor, jvxBitField filter_stateMask = (jvxBitField)JVX_STATE_DONTCARE,
		IjvxReferenceSelector* decider = nullptr) = 0;
	virtual jvxErrorType map_return_reference_tool(const jvxComponentIdentification&, 
		IjvxObject* theObject) = 0;

	// File CjvxHost_components-tpl.cpp
	template <class T> jvxErrorType t_select_component(std::vector<oneObjType<T>>& registeredObjs,
		jvxComponentIdentification& tp, jvxSize idx,
		IjvxObject* theOwner, jvxBool extend_if_necessary, jvxComponentType tp_store_type = JVX_COMPONENT_UNKNOWN);

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
		IjvxObject* theObject, const jvxComponentIdentification& tpIdOld,
		IjvxCoreStateMachine* theObjectSm, jvxComponentType tpRemapped = JVX_COMPONENT_UNKNOWN);
};

#endif

