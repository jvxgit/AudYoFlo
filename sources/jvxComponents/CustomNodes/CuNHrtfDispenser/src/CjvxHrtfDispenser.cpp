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

		genHrtfDispenser_node::register_callbacks(this, select_hrtf_slot,  select_sofa_file, this);

		// Install property extender interface to this
		genHrtfDispenser_node::extend_if.ex_interface.ptr = static_cast<IjvxPropertyExtender*>(this);

		jvxSize idxSlot = jvx_bitfieldSelection2Id(genHrtfDispenser_node::binaural_rendering_select.slot_definition);

		CjvxProperties::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_AND_WRITE_CONTENT, genHrtfDispenser_node::binaural_rendering_select.slot_definition);
		jvxSize num = genHrtfDispenser_node::binaural_rendering_select.slot_definition.value.entries.size();
		if (num == 0)
		{
			genHrtfDispenser_node::binaural_rendering_select.slot_definition.value.entries.push_back("default");			
		}
		jvx_bitZSet(genHrtfDispenser_node::binaural_rendering_select.slot_definition.value.selection(0), 0);

		// Special logic if a selection was made in configuration
		if (JVX_CHECK_SIZE_SELECTED(idxSlot))
		{
			if(idxSlot < genHrtfDispenser_node::binaural_rendering_select.slot_definition.value.entries.size())
			{ 
				jvx_bitZSet(genHrtfDispenser_node::binaural_rendering_select.slot_definition.value.selection(0), idxSlot);
			}
		}

		JVX_DSP_SAFE_ALLOCATE_OBJECT(theDispenser, ayfHrtfDispenser);
		const char** ptrSlotNames = nullptr;
		JVX_SAFE_ALLOCATE_FIELD_CPP_Z(ptrSlotNames, const char*, num);
		for (i = 0; i < genHrtfDispenser_node::binaural_rendering_select.slot_definition.value.entries.size(); i++)
		{
			ptrSlotNames[i] = genHrtfDispenser_node::binaural_rendering_select.slot_definition.value.entries[i].c_str();
		}
		theDispenser->start(genHrtfDispenser_node::binaural_rendering_select.sofa_directory.value, 
			genHrtfDispenser_node::binaural_rendering_select.slot_definition.value.entries.size(),
			ptrSlotNames);

		JVX_SAFE_FREE_FLD(ptrSlotNames);

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

		genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.resize(genHrtfDispenser_node::binaural_rendering_select.slot_definition.value.entries.size());
		CjvxProperties::_update_property_size(genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.num, 
			genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.category, genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.globalIdx, false);
		structToProperty();
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

jvxErrorType 
CjvxHrtfDispenser::put_configuration(
	jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i, j;
	jvxErrorType res = CjvxNodeBase::put_configuration(
		callMan, processor,
		sectionToContainAllSubsectionsForMe, filename, lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_SELECTED)
		{
			genHrtfDispenser_node::put_configuration__binaural_rendering_select(callMan, processor, sectionToContainAllSubsectionsForMe);
		}
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			jvxConfigData* cfgDat = nullptr;
			processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &cfgDat, "AYF_SLOT_SELECTIONS");
			if (cfgDat)
			{
				jvxSize cnt = 0;
				while (1)
				{
					jvxConfigData* cfgDatSlot = nullptr;
					std::string token = "slot_" + jvx_size2String(cnt);
					cnt++;
					processor->getReferenceEntryCurrentSection_name(cfgDat, &cfgDatSlot, token.c_str());
					if (cfgDatSlot)
					{
						jvxBool sucCase = true;
						jvxApiString astr1;
						jvxApiString astr2;

						jvxConfigData* cfgDatSlotEntry = nullptr;
						processor->getReferenceEntryCurrentSection_name(cfgDatSlot, &cfgDatSlotEntry, "SLOT_NAME");
						if (cfgDatSlotEntry)
						{
							if (processor->getAssignmentString(cfgDatSlotEntry, &astr1) != JVX_NO_ERROR)
							{
								sucCase = sucCase && false;
							}
						}
						else
						{
							sucCase = sucCase && false;
						}
						cfgDatSlotEntry = nullptr;
						processor->getReferenceEntryCurrentSection_name(cfgDatSlot, &cfgDatSlotEntry, "SLOT_DATABASE");
						if (cfgDatSlotEntry)
						{
							if (processor->getAssignmentString(cfgDatSlotEntry, &astr2) != JVX_NO_ERROR)
							{
								sucCase = sucCase && false;
							}
						}
						else
						{
							sucCase = sucCase && false;
						}

						if (sucCase)
						{
							jvxSize num = 0;
							theDispenser->number_slots(num);
							for (i = 0; i < num; i++)
							{
								jvxApiString astr;
								theDispenser->slot_descrption(i, astr);
								if (astr.std_str() == astr1.std_str())
								{
									jvxSize idxDB = JVX_SIZE_UNSELECTED;
									for (j = 0; j < genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.entries.size(); j++)
									{
										if (genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.entries[j] == astr2.std_str())
										{
											idxDB = j;
											break;
										}
									}

									if (JVX_CHECK_SIZE_SELECTED(idxDB))
									{
										theDispenser->select_database(idxDB, i);										
									}
									break; // Break slot loop
								}
							}
						}
					}
					else
					{
						break;
					}
				}

				jvxSize slotId = jvx_bitfieldSelection2Id(genHrtfDispenser_node::binaural_rendering_select.slot_definition);
				jvxSize idxDB = theDispenser->selected_database(slotId);
				jvx_bitZSet(genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.selection(), idxDB);
			}
		}
	}
	return res;
}

jvxErrorType 
CjvxHrtfDispenser::get_configuration(
	jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxSize i;
	jvxErrorType res = CjvxNodeBase::get_configuration(
		callMan, processor, sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		jvxConfigData* cfgDat = nullptr;
		
		genHrtfDispenser_node::get_configuration__binaural_rendering_select(callMan,
			processor, sectionWhereToAddAllSubsections);

		processor->createEmptySection(&cfgDat, "AYF_SLOT_SELECTIONS");
		if (cfgDat)
		{
			jvxSize num = 0;
			theDispenser->number_slots(num);
			for (i = 0; i < num; i++)
			{
				jvxApiString astr;
				theDispenser->slot_descrption(i, astr);
				jvxSize idxDB = theDispenser->selected_database(i);
				std::string nameDataBase = genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.entries[idxDB];
				std::string token = "slot_" + jvx_size2String(i);
				jvxConfigData* cfgDatSlot = nullptr;
				processor->createEmptySection(&cfgDatSlot, token.c_str());
				jvxConfigData* cfgDataSlotEntry = nullptr;
				processor->createAssignmentString(&cfgDataSlotEntry, "SLOT_NAME", astr.c_str());
				processor->addSubsectionToSection(cfgDatSlot, cfgDataSlotEntry);
				processor->createAssignmentString(&cfgDataSlotEntry, "SLOT_DATABASE", nameDataBase.c_str());
				processor->addSubsectionToSection(cfgDatSlot, cfgDataSlotEntry);

				processor->addSubsectionToSection(cfgDat, cfgDatSlot);
			}
			processor->addSubsectionToSection(sectionWhereToAddAllSubsections, cfgDat);
		}
	}
	return res;
}

void
CjvxHrtfDispenser::structToProperty()
{
	jvxSize i;
	jvxSize num = 0;
	theDispenser->number_slots(num);
	for(i = 0; i < num; i++)
	{ 
		jvxSize slotId = theDispenser->selected_database(i);
		if (i < genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.num)
		{
			jvx_bitZSet(genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.selection(i), slotId);
		}
	}
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxHrtfDispenser, select_sofa_file)
{
	jvxSize newIdx = jvx_bitfieldSelection2Id(genHrtfDispenser_node::binaural_rendering_active.select_sofa_db, tune.offsetStart);
	if (JVX_CHECK_SIZE_SELECTED(newIdx))
	{
		theDispenser->select_database(newIdx, tune.offsetStart);
		/*
	 		jvxSize idxDB = theDispenser->selected_database(slotId);
			jvx_bitZSet(genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.selection(), idxDB);
			CjvxProperties::add_property_report_collect(genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.descriptor.c_str());
		*/
		structToProperty();
	}
	return JVX_NO_ERROR;
}


JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxHrtfDispenser, select_hrtf_slot)
{
	/*
	jvxSize newIdx = jvx_bitfieldSelection2Id(genHrtfDispenser_node::binaural_rendering_active.select_sofa_db);
	jvxSize slotId = jvx_bitfieldSelection2Id(genHrtfDispenser_node::binaural_rendering_select.slot_definition);

	if (JVX_CHECK_SIZE_SELECTED(newIdx))
	{
		jvxSize idxDB = theDispenser->selected_database(slotId);
		jvx_bitZSet(genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.value.selection(), idxDB);
		CjvxProperties::add_property_report_collect(genHrtfDispenser_node::binaural_rendering_active.select_sofa_db.descriptor.c_str());
	}
	*/
	return JVX_NO_ERROR;
}

