#ifndef __IJVXHOST_H__
#define __IJVXHOST_H__

JVX_INTERFACE IjvxHost: public IjvxFactoryHost, public IjvxHostTypeHandler
{
	
public:

	virtual JVX_CALLINGCONVENTION ~IjvxHost(){};
	
	// --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  -- 
	// Control main sub components
	// --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  -- 
	
	virtual jvxErrorType JVX_CALLINGCONVENTION boot_complete(jvxBool* bootComplete) = 0;

	// List the components which are available in system
	virtual jvxErrorType JVX_CALLINGCONVENTION number_components_system( const jvxComponentIdentification&, jvxSize* num) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION name_component_system(const jvxComponentIdentification&, jvxSize idx, jvxApiString*) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION description_component_system( const jvxComponentIdentification&, jvxSize idx, jvxApiString*) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_component_system( const jvxComponentIdentification&, jvxSize idx, jvxApiString*, jvxApiString* substr = NULL) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION module_reference_component_system( const jvxComponentIdentification&, jvxSize idx, jvxApiString*, jvxComponentAccessType* acTp) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION feature_class_component_system( const jvxComponentIdentification&, jvxSize idx, jvxBitField*) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION role_component_system(
		jvxComponentType tp,
		jvxComponentType* parentTp = NULL,
		jvxComponentType* childTp = NULL,
		jvxComponentTypeClass* classTp = NULL) = 0;

	// List the slots and the subslots and access components in slots
	virtual jvxErrorType JVX_CALLINGCONVENTION number_slots_component_system( 
		const jvxComponentIdentification&, jvxSize* szSlots, 
		jvxSize* szSubPlots, 
		jvxSize* szSlots_max, 
		jvxSize* szSubSlots_max) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION set_number_subslots_system(
		const jvxComponentIdentification&, jvxSize newVal) = 0;

	// Access and control components in slots

	// Note that we pass non-const references to set slot id or sub id on exit
	virtual jvxErrorType JVX_CALLINGCONVENTION select_component(jvxComponentIdentification&, jvxSize, IjvxObject* owner = NULL, jvxBool extend_if_necessary = false) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION selection_component(const jvxComponentIdentification&, jvxSize*) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION activate_selected_component(const jvxComponentIdentification&) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready_selected_component(const jvxComponentIdentification&, jvxBool* ready, jvxApiString* reasonIfNot) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate_selected_component(const jvxComponentIdentification&) = 0;

	// Note that we pass non-const references to reset slot id or sub id
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_selected_component(jvxComponentIdentification&) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION state_selected_component(const jvxComponentIdentification&, jvxState* ) = 0;		
	virtual jvxErrorType JVX_CALLINGCONVENTION module_reference_selected_component(const jvxComponentIdentification&, jvxApiString*, jvxComponentAccessType* acTp) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION description_selected_component(const jvxComponentIdentification&, jvxApiString* ) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_selected_component(const jvxComponentIdentification&, jvxApiString* str = NULL, jvxApiString* substr = NULL) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION feature_class_selected_component(const jvxComponentIdentification&, jvxBitField*) = 0;

	// Direct access to components in slots
	virtual jvxErrorType JVX_CALLINGCONVENTION request_object_selected_component(const jvxComponentIdentification&, IjvxObject** ) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_object_selected_component(const jvxComponentIdentification&, IjvxObject* ) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface_selected_component(const jvxComponentIdentification&, jvxInterfaceType tp, jvxHandle** iface) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface_selected_component(const jvxComponentIdentification&, jvxInterfaceType tp, jvxHandle* iface ) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION unique_id_selected_component(const jvxComponentIdentification& tp, jvxSize * uId) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION switch_state_component(const jvxComponentIdentification& cpId, jvxStateSwitch sswitch) = 0;

	// virtual jvxErrorType JVX_CALLINGCONVENTION switch_state_component(const jvxComponentIdentification& cpId, jvxStateSwitch sswitch) = 0;
	//virtual jvxErrorType JVX_CALLINGCONVENTION set_external_widget_host(jvxHandle* theWidgetHost, jvxWidgetHostType widgetType) = 0;

};

#endif
