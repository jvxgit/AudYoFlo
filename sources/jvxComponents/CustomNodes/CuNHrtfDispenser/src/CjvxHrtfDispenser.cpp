#include "CjvxHrtfDispenser.h"

CjvxHrtfDispenser::CjvxHrtfDispenser(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxNodeBase(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	
}

// ===============================================================================================

jvxErrorType
CjvxHrtfDispenser::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxNodeBase::select(owner);
	if (res == JVX_NO_ERROR)
	{
		genHrtfDispenser_node::init__binaural_rendering_select();
		genHrtfDispenser_node::allocate__binaural_rendering_select();
		genHrtfDispenser_node::register__binaural_rendering_select(this);
	}
	return res;
}
jvxErrorType
CjvxHrtfDispenser::unselect()
{
	jvxErrorType res = CjvxNodeBase::_pre_check_unselect();
	if (res == JVX_NO_ERROR)
	{
		genHrtfDispenser_node::unregister__binaural_rendering_select(this);
		genHrtfDispenser_node::deallocate__binaural_rendering_select();
		res = CjvxNodeBase::unselect();
	}
	return res;
}

// ===============================================================================================

jvxErrorType 
CjvxHrtfDispenser::activate()
{
	jvxSize i;
	jvxErrorType res = CjvxNodeBase::activate();
	if (res == JVX_NO_ERROR)
	{
		genHrtfDispenser_node::init__binaural_rendering_active();
		genHrtfDispenser_node::allocate__binaural_rendering_active();
		genHrtfDispenser_node::register__binaural_rendering_active(this);

		genHrtfDispenser_node::init__extend_if();
		genHrtfDispenser_node::allocate__extend_if();
		genHrtfDispenser_node::register__extend_if(this);

		genHrtfDispenser_node::register_callbacks(this, select_sofa_file, this);

		// Install property extender interface to this
		genHrtfDispenser_node::extend_if.ex_interface.ptr = static_cast<IjvxPropertyExtender*>(this);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT, genHrtfDispenser_node::binaural_rendering_select.slot_definition);
		jvxSize num = genHrtfDispenser_node::binaural_rendering_select.slot_definition.value.entries.size();
		if (num == 0)
		{
			genHrtfDispenser_node::binaural_rendering_select.slot_definition.value.entries.push_back("default");
		}
		jvx_bitZSet(genHrtfDispenser_node::binaural_rendering_select.slot_definition.value.selection(0), 0);

		JVX_DSP_SAFE_ALLOCATE_OBJECT(theDispenser, ayfHrtfDispenser);
		theDispenser->start(genHrtfDispenser_node::binaural_rendering_select.sofa_directory.value, num);

		genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.entries.clear();
		jvxSize numEntries = theDispenser->num_sofa_files();
		for (i = 0; i < numEntries; i++)
		{
			std::string database_name;
			theDispenser->name_sofa_file(i, database_name);
			genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.entries.push_back(database_name);
		}
		jvxSize slotId = jvx_bitfieldSelection2Id(genHrtfDispenser_node::binaural_rendering_select.slot_definition);
		jvxSize selDatabase = theDispenser->selected_database(slotId);
		jvx_bitZSet(genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.selection(0), selDatabase);

	}
	return res;
}

jvxErrorType 
CjvxHrtfDispenser::deactivate()
{
	jvxErrorType res = CjvxNodeBase::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		theDispenser->stop();
		genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.entries.clear();

		CjvxProperties::_undo_update_property_access_type(genHrtfDispenser_node::binaural_rendering_select.slot_definition);

		genHrtfDispenser_node::unregister_callbacks(this);
		genHrtfDispenser_node::unregister__extend_if(this);
		genHrtfDispenser_node::deallocate__extend_if();

		genHrtfDispenser_node::unregister__binaural_rendering_active(this);
		genHrtfDispenser_node::deallocate__binaural_rendering_active();
		
		res = CjvxNodeBase::deactivate();
	}
	return res;
}

// ===============================================================================================

jvxErrorType 
CjvxHrtfDispenser::supports_prop_extender_type(jvxPropertyExtenderType tp)
{
	switch (tp)
	{
	case jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_HRTF_DISPENSER:	
		return JVX_NO_ERROR;
	default:
		break;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CjvxHrtfDispenser::prop_extender_specialization(jvxHandle** prop_extender, jvxPropertyExtenderType tp)
{
	switch (tp)
	{
	case jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_HRTF_DISPENSER:
		JVX_PTR_SAFE_ASSIGN(prop_extender, reinterpret_cast<jvxHandle*>(static_cast<IjvxPropertyExtenderHrtfDispenser*>(theDispenser)));
		return JVX_NO_ERROR;
	default:
		break;
	}
	return JVX_ERROR_INVALID_SETTING;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxHrtfDispenser, select_sofa_file)
{
	jvxSize newIdx = jvx_bitfieldSelection2Id(
		genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.selection(),
		genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.entries.size());
	jvxSize slotId = jvx_bitfieldSelection2Id(genHrtfDispenser_node::binaural_rendering_select.slot_definition);

	if (JVX_CHECK_SIZE_SELECTED(newIdx))
	{
		theDispenser->select_database(newIdx, slotId);
	}

	return JVX_NO_ERROR;
}
