jvxErrorType
number_components_system(const jvxComponentIdentification& tp, jvxSize* numPtr)
{
	return _number_components_system(tp, numPtr);
}

/*
 * Return the names of components of a specific type
 */
jvxErrorType
name_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* str)
{
	return _name_component_system(tp, idx, str);
}

/*
 * Return the names of components of a specific type
 */
jvxErrorType
description_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* str)
{
	return _description_component_system( tp, idx, str);

}

/*
 * Return the names of components of a specific type
 */
jvxErrorType
descriptor_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* str, jvxApiString* substr)
{
	return _descriptor_component_system( tp, idx,  str,  substr);

}

/*
 * Return the names of components of a specific type
 */
jvxErrorType
module_reference_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* str, jvxComponentAccessType* acTp)
{
	return _module_reference_component_system(tp, idx, str, acTp);
}

/*
 * Return feature class identifier
 */
jvxErrorType
feature_class_component_system(const jvxComponentIdentification& tp, jvxSize idx, jvxBitField* ft)
{
	return _feature_class_component_system( tp, idx,  ft);
}

jvxErrorType
role_component_system(jvxComponentType tp,
	jvxComponentType* parentTp, jvxComponentType* childTp, 
	jvxComponentTypeClass* classTp)
{
	return _role_component_system( tp, parentTp,  childTp, classTp);
}

jvxErrorType 
number_slots_component_system(const jvxComponentIdentification& tp, jvxSize* szSlots_current,
	jvxSize* szSubSlots_current, jvxSize* szSlots_max, jvxSize* szSubSlots_max)
{
	return _number_slots_component_system(tp, szSlots_current,
		szSubSlots_current, szSlots_max, szSubSlots_max);
}

jvxErrorType 
set_number_subslots_system(const jvxComponentIdentification& tp, jvxSize newVal)
{
	return _set_number_subslots_system(tp, newVal);
}

// ====================================================================================================================
// ====================================================================================================================
// ====================================================================================================================
// ====================================================================================================================
// ====================================================================================================================

/*
 * Return the names of components of a specific type
 */
jvxErrorType
module_reference_selected_component(const jvxComponentIdentification& tp, jvxApiString* str, jvxComponentAccessType* acTp)
{
	return _module_reference_selected_component(tp, str, acTp);
}

/*
 * Return the names of components of a specific type
 */
jvxErrorType
description_selected_component(const jvxComponentIdentification& tp, jvxApiString* str)
{
	return _description_selected_component(tp, str);
}


/*
 * Return the names of components of a specific type
 */
jvxErrorType
descriptor_selected_component(const jvxComponentIdentification& tp, jvxApiString* str, jvxApiString* substr)
{
	return _descriptor_selected_component(tp, str, substr);
}

/*
 * Return feature class identifier
 */
jvxErrorType
feature_class_selected_component(const jvxComponentIdentification& tp, jvxBitField* ft)
{
	return _feature_class_selected_component(tp, ft);
}

/*
 * Select a component of a specific type
 */
jvxErrorType
select_component(jvxComponentIdentification& tp, jvxSize idx,
	IjvxObject* theOwner, jvxBool extend_if_necessary)
{
	return _select_component(tp, idx, theOwner, extend_if_necessary);
}

/**
 * Return the idx to identify the currently selected component
 */
jvxErrorType
selection_component(const jvxComponentIdentification& tp, jvxSize* idRet)
{
	return _selection_component(tp, idRet);
}

/**
 * Activate a component of a specific type as currently selected. 
 */
jvxErrorType
activate_selected_component(const jvxComponentIdentification& tp)
{
	return _activate_selected_component(tp);
}

jvxErrorType
is_ready_selected_component(const jvxComponentIdentification& tp, jvxBool* ready, jvxApiString* reasonIfNot)
{
	return _is_ready_selected_component(tp, ready, reasonIfNot);

}

/**
 * Return the state of the component currently selected.
 */
jvxErrorType
state_selected_component(const jvxComponentIdentification& tp, jvxState* st)
{
	return _state_selected_component(tp, st);
}

/**
 * Deactivate a component of a specific type as currently selected.
 */
jvxErrorType
deactivate_selected_component(const jvxComponentIdentification& tp)
{
	return _deactivate_selected_component( tp);
}

/**
 * Unselect a component of a specific type as currently selected.
 */
jvxErrorType
unselect_selected_component(jvxComponentIdentification& tp)
{
	return _unselect_selected_component(tp);
}

jvxErrorType
unique_id_selected_component(const jvxComponentIdentification& tp, jvxSize* uId)
{
	return _unique_id_selected_component(tp, uId);
}

jvxErrorType
switch_state_component(const jvxComponentIdentification& cpId, jvxStateSwitch sswitch)
{
	return _switch_state_component( cpId, sswitch);
}

jvxErrorType
request_hidden_interface_selected_component(const jvxComponentIdentification& tp, jvxInterfaceType ifTp, jvxHandle** iface)
{
	return _request_hidden_interface_selected_component(tp,  ifTp, iface);
}

jvxErrorType
return_hidden_interface_selected_component(const jvxComponentIdentification& tp, jvxInterfaceType ifTp, jvxHandle* iface)
{
	return _return_hidden_interface_selected_component(tp, ifTp, iface);
}

// ======================================================================
// ======================================================================

jvxErrorType
request_object_selected_component(const jvxComponentIdentification& tp, IjvxObject** theObj)
{
	return _request_object_selected_component(tp, theObj);
}

jvxErrorType
return_object_selected_component(const jvxComponentIdentification& tp, IjvxObject* theObj)
{
	return _return_object_selected_component(tp, theObj);
}
