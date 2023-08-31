#ifndef __CJVXFACTORYHOST_H__
#define __CJVXFACTORYHOST_H__

#include "jvx.h"
#include "jvxFactoryHosts/CjvxInterfaceFactory.h"

#define JVX_MINIMUM_HOST_FUNCTIONALITY

#include "jvxHosts/CjvxHostInteractionTools.h"
#include "jvxHosts/CjvxUniqueId.h"

#define SECTIONNAME_ALL_SUBSECTIONS "factoryhost_allSubSections"
#define SECTIONNAME_ALL_EXTERNAL_ENTRIES "factoryhost_allExternalEntries"

#include "jvxHosts/CjvxInterfaceHostTpl.h"
#include "jvx-helpers.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

template <class T>
class CjvxFactoryHostTpl : public T
{
protected:

	typedef struct
	{
		std::string selector;
		std::string assignmentString;
		jvxValue assignmentValue;
		std::vector<std::string> assignmentStringList;
		std::vector<std::vector<jvxValue> > assignmentValueMatrix;
		jvxConfigSectionTypes tp;
	} oneExternalConfigEntry;

	typedef struct
	{
		std::string selector;
		IjvxConfigurationExtender_report* ref;
	} oneExternalConfigHook;

	std::vector<oneExternalConfigEntry> externalConfigEntries;
	std::vector<oneExternalConfigHook> externalConfigHooks;

	jvxSize myUniqueId;

public:
	JVX_CALLINGCONVENTION CjvxFactoryHostTpl(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		T(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
		myUniqueId = 1;

		// Attach cross link to initialize CjvxHostInteraction
		CjvxHostInteraction::hostRefPass = static_cast<IjvxHiddenInterface*>(this);
	};

	virtual JVX_CALLINGCONVENTION ~CjvxFactoryHostTpl()
	{
	};

	// ===================================================================================================
	// Interface <IjvxStateMachine>
	// ===================================================================================================
	jvxErrorType initialize(IjvxHiddenInterface* theOtherHost) override
	{
		jvxErrorType res = T::_initialize(theOtherHost);
		if (res == JVX_NO_ERROR)
		{
			T::_common_set_host.threadId_registered = JVX_GET_CURRENT_THREAD_ID();
			T::reset();
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION select(IjvxObject* theOwner) override
	{
		jvxErrorType res = T::_select(theOwner);
		if (res == JVX_NO_ERROR)
		{
			T::init__properties_selected();
			T::allocate__properties_selected();
			T::register__properties_selected(static_cast<CjvxProperties*>(this));
		}
		return(res);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxSize i;

		T::_common_set_min.theHostRef = static_cast<IjvxHiddenInterface*>(this);
		res = T::_activate_no_text_log();
		if (res == JVX_NO_ERROR)
		{
			T::_lock_properties_local();
			T::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.component_path);
			T::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.configure_parts);
			T::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.do_unload_dlls);
			T::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.host_output_cout);
			T::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.host_verbose_dll);
			T::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_activate);
			T::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_dbglevel);
			T::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_expressions);
			T::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_filename);
			T::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile);
			T::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW);
			T::_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile);
			T::_unlock_properties_local();

			if (!T::config.use_only_static_objects)
			{
				this->loadAllComponents(JVX_EVALUATE_BITFIELD(CjvxHost_genpcg::properties_selected.do_unload_dlls.value.selection() & 0x1),
					CjvxHost_genpcg::properties_selected.component_path.value, true, true);
			}

			if (CjvxHost_genpcg::properties_selected.textLog_activate.value == c_true)
			{
				if (T::_common_set.theToolsHost)
				{
					jvxErrorType resL = T::_common_set.theToolsHost->reference_tool(JVX_COMPONENT_SYSTEM_TEXT_LOG,
						&T::jvxrtst_bkp.theTextLogger_obj, 0, NULL);
					if ((resL == JVX_NO_ERROR) && T::jvxrtst_bkp.theTextLogger_obj)
					{
						resL = T::jvxrtst_bkp.theTextLogger_obj->request_specialization(reinterpret_cast<jvxHandle**>(&T::jvxrtst_bkp.theTextLogger_hdl), NULL, NULL);
					}
				}

				if (T::jvxrtst_bkp.theTextLogger_hdl)
				{
					std::cout << "Starting text log file:" << std::endl;
					std::cout << " --> Filename = " << CjvxHost_genpcg::properties_selected.textLog_filename.value << std::endl;
					std::cout << " --> Internal file buffersize = " << CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile.value << std::endl;
					std::cout << " --> Internal cbuf buffersize = " << CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW.value << std::endl;
					std::cout << " --> File transfer size = " << CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile.value << std::endl;
					std::cout << " --> Debug level = " << CjvxHost_genpcg::properties_selected.textLog_dbglevel.value << std::endl;

					T::jvxrtst_bkp.theTextLogger_hdl->initialize(
						static_cast<IjvxHiddenInterface*>(this),
						CjvxHost_genpcg::properties_selected.textLog_filename.value.c_str(),
						CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile.value,
						CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile.value,
						CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW.value,
						CjvxHost_genpcg::properties_selected.textLog_dbglevel.value);

					std::cout << " --> Module selection expression = " << std::endl;
					for (i = 0; i < CjvxHost_genpcg::properties_selected.textLog_expressions.value.size(); i++)
					{
						std::cout << " -->-->" << CjvxHost_genpcg::properties_selected.textLog_expressions.value[i] << std::endl;
						T::jvxrtst_bkp.theTextLogger_hdl->addTextLogExpression(CjvxHost_genpcg::properties_selected.textLog_expressions.value[i].c_str());
					}

					T::jvxrtst_bkp.theTextLogger_hdl->start();
				}
			}
		}

		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override
	{
		jvxSize i;
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		if (T::_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			jvxState stat = JVX_STATE_NONE;
			for (i = 0; i < T::_common_set_host.otherComponents.availableOtherComponents.size(); i++)
			{
				T::_common_set_host.otherComponents.availableOtherComponents[i].theObj_single->state(&stat);
				assert(stat == JVX_STATE_NONE);
			}

			if (T::jvxrtst_bkp.theTextLogger_hdl)
			{
				T::jvxrtst_bkp.theTextLogger_hdl->stop();
				T::jvxrtst_bkp.theTextLogger_hdl->terminate();

				T::_common_set.theToolsHost->return_reference_tool(jvxComponentIdentification(JVX_COMPONENT_SYSTEM_TEXT_LOG,
					JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED), T::jvxrtst_bkp.theTextLogger_obj);
				T::jvxrtst_bkp.theTextLogger_obj = NULL;
				T::jvxrtst_bkp.theTextLogger_hdl = NULL;
			}

			T::_lock_properties_local();
			T::_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.component_path);
			T::_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.configure_parts);
			T::_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.do_unload_dlls);
			T::_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.host_output_cout);
			T::_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.host_verbose_dll);
			T::_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_activate);
			T::_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_dbglevel);
			T::_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_expressions);
			T::_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_filename);
			T::_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile);
			T::_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW);
			T::_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile);
			T::_unlock_properties_local();

			res = T::_deactivate_no_text_log();
			assert(res == JVX_NO_ERROR);

			// unload all Dlls
			if (!T::config.use_only_static_objects)
			{
				this->unloadAllComponents();
			}
			T::_common_set_min.theHostRef = NULL;
		}

		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect() override
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (T::_common_set_min.theState == JVX_STATE_SELECTED)
		{
			T::unregister__properties_selected(static_cast<CjvxProperties*>(this));
			T::deallocate__properties_selected();

			res = T::_unselect();
			assert(res == JVX_NO_ERROR);
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION terminate()override
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = T::_terminate();
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION system_ready() override
	{
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION system_about_to_shutdown() override
	{
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION owner(IjvxObject** dependsOn) override
	{
		return T::_owner(dependsOn);
	}

	// ===================================================================================================
	// Interface <IjvxToolsHost>
	// ===================================================================================================
	jvxErrorType number_tools(const jvxComponentIdentification& tp, jvxSize* num)override
	{
		return T::_number_tools(tp, num);
	};

	jvxErrorType identification_tool(const jvxComponentIdentification& tp, jvxSize idx, jvxApiString* description, jvxApiString* descriptor, jvxBool* mulInst)override
	{
		return T::_identification_tool(tp, idx, description, descriptor, mulInst);
	};

	jvxErrorType reference_tool(const jvxComponentIdentification& tp, IjvxObject** theObject, jvxSize filter_id, const char* filter_descriptor,
		jvxBitField filter_stateMask = (jvxBitField)JVX_STATE_DONTCARE, IjvxReferenceSelector* decider = nullptr) override
	{
		// If there is a filter, deactivate id return
		if (filter_descriptor)
		{
			filter_id = JVX_SIZE_UNSELECTED;
		}

		return T::_reference_tool(tp, theObject,
			filter_id, filter_descriptor, filter_stateMask,
			decider);
	};

	jvxErrorType return_reference_tool(const jvxComponentIdentification& tp, IjvxObject* theObject) override
	{
		jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
		jvxSize i;
		int numRet = 0;

		if (theObject == NULL)
		{
			return(JVX_NO_ERROR);
		}

		return T::_return_reference_tool(tp, theObject);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION instance_tool(jvxComponentType tp, IjvxObject** theObject, jvxSize filter_id, const char* filter_descriptor) override
	{
		return T::_instance_tool(tp, theObject, filter_id, filter_descriptor);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION return_instance_tool(jvxComponentType tp, IjvxObject* theObject, jvxSize filter_id, const char* filter_descriptor) override
	{
		return T::_return_instance_tool(tp, theObject, filter_id, filter_descriptor);
	};


	// ===================================================================================================
	// Interface <IjvxConfiguration>
	// ===================================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename = "", jvxInt32 lineno = -1)override
	{
		jvxConfigData* datTmp = NULL, * datTmpLoc = NULL, * datTmpOut = NULL;
		jvxApiValueList datLst;
		IjvxConfiguration* config_local = NULL;
		jvxErrorType res = JVX_NO_ERROR;
		jvxSize targetStateIdx = 0;
		jvxApiString fldStr;
		jvxApiString fldStrL1;
		jvxApiString fldStrL2;
		jvxInt32 valI32 = 0;
		std::string entry;
		std::string desc;
		jvxBool errorDetected = false;
		jvxInt16 selectme = -1;
		jvxSize num = 0, numL1, numL2;
		jvxSize i, j, k;
		jvxValue valD;
		jvxConfigSectionTypes tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
		jvxSize h;
		jvxSize slotid;
		jvxSize slotsubid;
		std::string slottoken;
		jvxSize numSlots = 1;
		jvxValue val;
		jvxConfigData* theSlotNum = NULL;

		// First, deactivate all components which have been previously selected
		if (processor && sectionToContainAllSubsectionsForMe)
		{

			// Get the section for this object
			processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &datTmp, SECTIONNAME_ALL_SUBSECTIONS);
			if (datTmp)
			{
				for (i = 0; i < externalConfigHooks.size(); i++)
				{
					processor->getReferenceEntryCurrentSection_name(datTmp, &datTmpLoc, externalConfigHooks[i].selector.c_str());
					if (datTmpLoc)
					{
						if (externalConfigHooks[i].ref)
						{
							processor->getOriginSection(datTmpLoc, &fldStr, &valI32);
							externalConfigHooks[i].ref->put_configuration_ext(callConf, processor, datTmpLoc, fldStr.c_str(), valI32);
						}
					}
				}

				if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
				{
					processor->getReferenceEntryCurrentSection_name(datTmp, &datTmpLoc, SECTIONNAME_ALL_EXTERNAL_ENTRIES);
					if (datTmpLoc)
					{
						externalConfigEntries.clear();
						processor->getNumberEntriesCurrentSection(datTmpLoc, &numL1);
						for (i = 0; i < numL1; i++)
						{
							datTmpOut = NULL;
							processor->getReferenceEntryCurrentSection_id(datTmpLoc, &datTmpOut, i);
							if (datTmpOut)
							{
								tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
								processor->getTypeCurrentEntry(datTmpOut, &tp);
								processor->getNameCurrentEntry(datTmpOut, &fldStrL1);

								switch (tp)
								{
								case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:

									processor->getAssignmentString(datTmpOut, &fldStrL2);

									this->set_configuration_entry(fldStrL1.c_str(), (jvxHandle*)&fldStrL2, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING, -1);
									break;
								case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
									processor->getAssignmentValue(datTmpOut, &valD);
									this->set_configuration_entry(fldStrL1.c_str(), (jvxHandle*)&valD, JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE, -1);
									break;
								case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
								{
									oneExternalConfigEntry newEntry;
									newEntry.tp = JVX_CONFIG_SECTION_TYPE_STRINGLIST;
									newEntry.selector = fldStrL1.std_str();
									processor->getNumberStrings(datTmpOut, &numL2);
									for (j = 0; j < numL2; j++)
									{
										processor->getString_id(datTmpOut, &fldStrL2, j);
										newEntry.assignmentStringList.push_back(fldStrL2.std_str());
									}
									externalConfigEntries.push_back(newEntry);
								}
								break;
								case JVX_CONFIG_SECTION_TYPE_VALUELIST:
									processor->getNumberValueLists(datTmpOut, &numL2);
									for (j = 0; j < numL2; j++)
									{
										processor->getValueList_id(datTmpOut, &datLst, j);
										this->set_configuration_entry(fldStrL1.c_str(), (jvxHandle*)&datLst, JVX_CONFIG_SECTION_TYPE_VALUELIST, (jvxInt32)j);
									}
									break;
								}

							}
						}
					}// if(datTmpLoc)
				}
			} // if(datTmp)
		}// if(processor && sectionToContainAllSubsectionsForMe)
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)override
	{
		jvxSize i, j, k, h;
		jvxConfigData* datTmp = NULL;
		jvxConfigData* datTmp_add = NULL;
		IjvxConfiguration* config_local = NULL;
		jvxApiString fldStr;
		jvxBool errorDetected = false;
		std::string entry;
		jvxErrorType res = JVX_NO_ERROR;
		jvxData valD = 0;
		jvxConfigData* datTmpLoc = NULL, * datAdd = NULL;
		std::string slottoken;
		jvxConfigData* theSlotNum = NULL;
		jvxSize numSlots = 0;
		if (processor && sectionWhereToAddAllSubsections)
		{
			processor->createEmptySection(&datTmp, SECTIONNAME_ALL_SUBSECTIONS);
			if (callConf->configModeFlags & JVX_CONFIG_MODE_FULL)
			{
				if (externalConfigEntries.size())
				{
					processor->createEmptySection(&datTmpLoc, SECTIONNAME_ALL_EXTERNAL_ENTRIES);
					if (datTmpLoc)
					{
						for (i = 0; i < externalConfigEntries.size(); i++)
						{
							datAdd = NULL;
							switch (externalConfigEntries[i].tp)
							{
							case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
								processor->createAssignmentString(&datAdd, externalConfigEntries[i].selector.c_str(),
									externalConfigEntries[i].assignmentString.c_str());
								break;

							case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
								processor->createAssignmentValue(&datAdd, externalConfigEntries[i].selector.c_str(),
									externalConfigEntries[i].assignmentValue);
								break;
							case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
								processor->createAssignmentStringList(&datAdd, externalConfigEntries[i].selector.c_str());

								for (j = 0; j < externalConfigEntries[i].assignmentStringList.size(); j++)
								{
									processor->addAssignmentStringToList(datAdd, externalConfigEntries[i].assignmentStringList[j].c_str());
								}
								break;
							case JVX_CONFIG_SECTION_TYPE_VALUELIST:
								processor->createAssignmentValueList(&datAdd, externalConfigEntries[i].selector.c_str());
								for (j = 0; j < externalConfigEntries[i].assignmentValueMatrix.size(); j++)
								{
									for (k = 0; k < externalConfigEntries[i].assignmentValueMatrix[j].size(); k++)
									{
										processor->addAssignmentValueToList(datAdd, JVX_SIZE_INT(j), externalConfigEntries[i].assignmentValueMatrix[j][k]);
									}
								}
								break;
							}
							if (datAdd)
							{
								processor->addSubsectionToSection(datTmpLoc, datAdd);
							}
						}
						processor->addSubsectionToSection(datTmp, datTmpLoc);
					}
				}
			}

			// Add config sections by hook callbacks
			for (i = 0; i < externalConfigHooks.size(); i++)
			{
				processor->createEmptySection(&datTmpLoc, externalConfigHooks[i].selector.c_str());
				if (externalConfigHooks[i].ref)
				{
					externalConfigHooks[i].ref->get_configuration_ext(callConf, processor, datTmpLoc);
				}
				processor->addSubsectionToSection(datTmp, datTmpLoc);
			}

			processor->addSubsectionToSection(sectionWhereToAddAllSubsections, datTmp);
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	};


	// ===================================================================================================
	// Interface <IjvxConfigurationExtender>
	// ===================================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION clear_configuration_entries()override
	{
		externalConfigEntries.clear();
		return(JVX_NO_ERROR);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION set_configuration_entry(const char* entryname, jvxHandle* fld, jvxConfigSectionTypes tp, jvxSize id)override
	{
		jvxErrorType res = JVX_NO_ERROR;
		jvxApiStringList* lstStr;
		jvxApiValueList* lstData;
		std::vector<jvxValue> newLst;
		jvxSize i;
		std::string entryStr = entryname;
		if (fld)
		{
			auto elm = jvx_findItemSelectorInList<oneExternalConfigEntry, std::string>(externalConfigEntries, entryStr);
			if (elm == externalConfigEntries.end())
			{
				oneExternalConfigEntry newEntry;
				newEntry.selector = entryname;
				newEntry.tp = tp;
				switch (newEntry.tp)
				{
				case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
					newEntry.assignmentString = ((jvxApiString*)fld)->std_str();
					break;
				case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
					newEntry.assignmentValue = *((jvxValue*)fld);
					break;
				case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
					newEntry.assignmentStringList.clear();
					lstStr = (jvxApiStringList*)fld;
					for (i = 0; i < lstStr->ll(); i++)
					{
						newEntry.assignmentStringList.push_back(lstStr->std_str_at(i));
					}
					break;
				case JVX_CONFIG_SECTION_TYPE_VALUELIST:
					if (JVX_CHECK_SIZE_UNSELECTED(id))
					{
						newEntry.assignmentValueMatrix.clear();
					}
					else
					{
						lstData = (jvxApiValueList*)fld;
						newLst.clear();
						for (i = 0; i < lstData->ll(); i++)
						{
							newLst.push_back(lstData->elm_at(i));
						}

						if (id < newEntry.assignmentValueMatrix.size())
						{
							newEntry.assignmentValueMatrix[id] = newLst;
						}
						else
						{
							for (i = 0; i < (id - newEntry.assignmentValueMatrix.size()); i++)
							{
								std::vector<jvxValue> llst;
								newEntry.assignmentValueMatrix.push_back(llst);
							}
							newEntry.assignmentValueMatrix.push_back(newLst);
						}
					}
					break;
				default:
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
				if (res == JVX_NO_ERROR)
				{
					externalConfigEntries.push_back(newEntry);
				}
			}
			else
			{
				elm->tp = tp;
				switch (elm->tp)
				{
				case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
					elm->assignmentString = ((jvxApiString*)fld)->std_str();
					break;
				case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
					elm->assignmentValue = *((jvxValue*)fld);
					break;
				case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
					elm->assignmentStringList.clear();
					lstStr = (jvxApiStringList*)fld;
					for (i = 0; i < lstStr->ll(); i++)
					{
						elm->assignmentStringList.push_back(lstStr->std_str_at(i));
					}
					break;
				case JVX_CONFIG_SECTION_TYPE_VALUELIST:
					if (JVX_CHECK_SIZE_UNSELECTED(id))
					{
						elm->assignmentValueMatrix.clear();
					}
					else
					{
						lstData = (jvxApiValueList*)fld;
						newLst.clear();
						for (i = 0; i < lstData->ll(); i++)
						{
							newLst.push_back(lstData->elm_at(i));
						}
						elm->assignmentValueMatrix[id] = newLst;
					}
					break;
				default:
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration_entry(const char* entryname, jvxHandle* fld, jvxConfigSectionTypes* tp, jvxSize id)override
	{
		jvxErrorType res = JVX_NO_ERROR;
		std::string entryStr = entryname;
		jvxBool allowCopyTypeSafe = false;
		auto elm = jvx_findItemSelectorInList<oneExternalConfigEntry, std::string>(externalConfigEntries, entryStr);
		if (elm != externalConfigEntries.end())
		{
			if (tp)
			{
				if (*tp == JVX_CONFIG_SECTION_TYPE_UNKNOWN)
				{
					*tp = elm->tp;
				}
				else
				{
					allowCopyTypeSafe = (elm->tp == *tp);
				}
			}
			if (fld)
			{
				if (allowCopyTypeSafe)
				{
					switch (elm->tp)
					{
					case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
						((jvxApiString*)fld)->assign(elm->assignmentString);
						break;
					case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
						*((jvxValue*)fld) = elm->assignmentValue;
						break;
					case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
						((jvxApiStringList*)fld)->assign(elm->assignmentStringList);
						break;
					case JVX_CONFIG_SECTION_TYPE_VALUELIST:
						if (id < elm->assignmentValueMatrix.size())
						{
							((jvxApiValueList*)fld)->assign(elm->assignmentValueMatrix[id]);
						}
						else
						{
							res = JVX_ERROR_ID_OUT_OF_BOUNDS;
						}
						break;
					}
				}
				else
				{
					res = JVX_ERROR_INVALID_SETTING;
				}
			}
		}
		else
		{
			if (tp)
			{
				*tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
			}
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		return(res);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION register_extension(IjvxConfigurationExtender_report* bwd, const char* sectionName)override
	{
		std::string entryStr = sectionName;
		auto elm = jvx_findItemSelectorInList<oneExternalConfigHook, std::string>(externalConfigHooks, entryStr);
		if (elm == externalConfigHooks.end())
		{
			oneExternalConfigHook newElm;
			newElm.selector = entryStr;
			newElm.ref = bwd;
			externalConfigHooks.push_back(newElm);
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_DUPLICATE_ENTRY;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_extension(const char* sectionName)override
	{
		std::string entryStr = sectionName;
		auto elm = jvx_findItemSelectorInList<oneExternalConfigHook, std::string>(externalConfigHooks, entryStr);
		if (elm != externalConfigHooks.end())
		{
			externalConfigHooks.erase(elm);
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	};

	// ===================================================================================================
#include "codeFragments/simplify/jvxHostInteraction_simplify.h"
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
};

#endif