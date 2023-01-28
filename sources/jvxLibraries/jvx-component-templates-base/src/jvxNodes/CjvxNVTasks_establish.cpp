#include "jvx.h"
#include "jvxNodes/CjvxNVTasks.h"

// =================================================================================

jvxErrorType 
CjvxNVTasks::report_select_input_vtask(
	jvxSize ctxtIdx,
	jvxSize ctxtSubIdx,
	IjvxConnectorBridge* bri,
	IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common,
	IjvxInputConnector* expose_connector,
	IjvxInputConnector** replace_connector)
{
	/*
	* 
	 */
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize ctxtTypeId = ctxtIdx;

	auto elmTL = _common_set_node_vtask.variableTasks.find(ctxtIdx);
	if (elmTL != _common_set_node_vtask.variableTasks.end())
	{
		auto elmRT = elmTL->second.activeRuntimeTasks.find(ass_connection_common);
		jvxSize ctxtId = JVX_SIZE_UNSELECTED;
		std::vector<oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>>* icons = NULL;
		jvxSize nIcons = 0;
		jvxBitField* iconsSelected = NULL;
		if (elmRT != elmTL->second.activeRuntimeTasks.end())
		{
			// There is already one connected
			ctxtId = elmRT->second.idCtxt;
			ctxtTypeId = elmRT->second.idCtxtGroup;
			icons = &elmRT->second.icons;
			iconsSelected = &elmRT->second.iconsSelected;
		}
		else
		{
			// First to request this group
			if (elmTL->second.numCurrentTasks < elmTL->second.numMaxTasks)
			{
				jvxOneConnectorTaskRuntime newElm;

				/*
				allocate_new_runtime_task(newElm, ass_connection_common,
					elmTL->second.idOffThisTasks);
					*/

				newElm.associatedCommon = ass_connection_common;
				newElm.associatedMaster = NULL;
				if (elmTL->second.withMaster)
				{
					// Get an associated master here!
					newElm.associatedMaster = NULL;
				}
				newElm.idCtxtGroup = elmTL->second.idOffThisTasks;
				newElm.idCtxt = elmTL->second.idOffThisTasks;
				while (1)
				{
					jvxBool inUse = false;
					auto elmS = elmTL->second.activeRuntimeTasks.begin();
					if (elmS == elmTL->second.activeRuntimeTasks.end())
					{
						// First instance here
						break;
					}

					while (elmS != elmTL->second.activeRuntimeTasks.end())
					{
						if (elmS->second.idCtxt == newElm.idCtxt)
						{
							inUse = true;
							break;
						}
						elmS++;
					}
						
					if (!inUse)
					{
						break;
					}
					else
					{
						newElm.idCtxt++;
					}
				}

				// Here we found the ctxtId!
				newElm.icons.resize(elmTL->second.descriptor_in.size());
				newElm.ocons.resize(elmTL->second.descriptor_out.size());
				jvx_bitFClear(newElm.iconsSelected);
				jvx_bitFClear(newElm.oconsSelected);

				elmTL->second.numCurrentTasks++;
				elmTL->second.activeRuntimeTasks[newElm.associatedCommon] = newElm;

				ctxtId = elmTL->second.activeRuntimeTasks[newElm.associatedCommon].idCtxt;
				ctxtTypeId = elmTL->second.activeRuntimeTasks[newElm.associatedCommon].idCtxtGroup;
				icons = &elmTL->second.activeRuntimeTasks[newElm.associatedCommon].icons;
				iconsSelected = &elmTL->second.activeRuntimeTasks[newElm.associatedCommon].iconsSelected;				

				// Update the shown tasks - should be within the context of a full UI update as it is part of a select to create a connection
				update_exposed_tasks();

			} // if (elmTL->second.numCurrentTasks < elmTL->second.numMaxTasks)
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		auto elmII = elmTL->second.icon.begin();
		std::advance(elmII, ctxtSubIdx);
		jvxErrorType resL = attach_single_input_connector(
			elmII->con, ctxtIdx, ctxtId,
			bri,
			master,
			ass_connection_common,
			*icons,
			iconsSelected);

		/*
		auto elmD = elmTL->second.activeRuntimeTasks.begin();
		for (; elmD != elmTL->second.activeRuntimeTasks.end(); elmD++)
		{
			std::cout << "Task:" << std::endl;
			auto elmDD = elmD->second.icons.begin();
			for (; elmDD != elmD->second.icons.end(); elmDD++)
			{
				std::cout << "Connector: " << elmDD->con << std::endl;
			}
		}*/

		if (resL == JVX_NO_ERROR)
		{
			*replace_connector = (*icons)[ctxtSubIdx].con;
		}
		else
		{
			res = JVX_ERROR_INTERNAL;
		}
	}

	return res;
}

jvxErrorType 
CjvxNVTasks::report_select_output_vtask(
	jvxSize ctxtIdx,
	jvxSize ctxtSubIdx,
	IjvxConnectorBridge* bri,
	IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common,
	IjvxOutputConnector* expose_connector,
	IjvxOutputConnector** replace_connector)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize ctxtTypeId = ctxtIdx;
	auto elmTL = _common_set_node_vtask.variableTasks.find(ctxtIdx);
	if (elmTL != _common_set_node_vtask.variableTasks.end())
	{
		auto elmRT = elmTL->second.activeRuntimeTasks.find(ass_connection_common);
		jvxSize ctxtId = JVX_SIZE_UNSELECTED;
		std::vector<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output> >* ocons = NULL;
		jvxSize nOcons = 0;
		jvxBitField* oconsSelected = NULL;
		if (elmRT != elmTL->second.activeRuntimeTasks.end())
		{
			// There is already one connected
			ctxtId = elmRT->second.idCtxt;
			ctxtTypeId = elmRT->second.idCtxtGroup;
			ocons = &elmRT->second.ocons;
			oconsSelected = &elmRT->second.oconsSelected;
		}
		else
		{
			// First to request this group
			if (elmTL->second.numCurrentTasks < elmTL->second.numMaxTasks)
			{
				jvxOneConnectorTaskRuntime newElm;

				/*
				allocate_new_runtime_task(newElm, ass_connection_common,
					elmTL->second.idOffThisTasks);
					*/

				newElm.associatedCommon = ass_connection_common;
				newElm.associatedMaster = NULL;
				if (elmTL->second.withMaster)
				{
					// Get an associated master here!
					newElm.associatedMaster = NULL;
				}
				newElm.idCtxt = elmTL->second.idOffThisTasks;
				while (1)
				{
					jvxBool inUse = false;
					auto elmS = elmTL->second.activeRuntimeTasks.begin();
					if (elmS == elmTL->second.activeRuntimeTasks.end())
					{
						break;
					}

					while (elmS != elmTL->second.activeRuntimeTasks.end())
					{
						if (elmS->second.idCtxt == newElm.idCtxt)
						{
							inUse = true;
							break;
						}
					}
					if (!inUse)
					{
						break;
					}
					else
					{
						newElm.idCtxt++;
					}				
				}

				// Here we found the ctxtId!
				newElm.icons.resize(elmTL->second.descriptor_in.size());
				newElm.ocons.resize(elmTL->second.descriptor_out.size());
				jvx_bitFClear(newElm.iconsSelected);
				jvx_bitFClear(newElm.oconsSelected);

				elmTL->second.numCurrentTasks++;
				elmTL->second.activeRuntimeTasks[newElm.associatedCommon] = newElm;

				ctxtId = elmTL->second.activeRuntimeTasks[newElm.associatedCommon].idCtxt;
				ctxtTypeId = elmTL->second.activeRuntimeTasks[newElm.associatedCommon].idCtxtGroup;
				ocons = &elmTL->second.activeRuntimeTasks[newElm.associatedCommon].ocons;
				oconsSelected = &elmTL->second.activeRuntimeTasks[newElm.associatedCommon].oconsSelected;
			} // if (elmTL->second.numCurrentTasks < elmTL->second.numMaxTasks)
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		auto elmOO = elmTL->second.ocon.begin();
		std::advance(elmOO, ctxtSubIdx);
		jvxErrorType resL = attach_single_output_connector(
			elmOO->con,
			ctxtTypeId,
			ctxtId,
			bri,
			master,
			ass_connection_common,
			*ocons,
			oconsSelected);

		if (resL == JVX_NO_ERROR)
		{
			*replace_connector = (*ocons)[ctxtSubIdx].con;
		}
		else
		{
			res = JVX_ERROR_INTERNAL;
		}
	}

	return res;
}

jvxErrorType
CjvxNVTasks::report_unselect_input_vtask(
	jvxSize ctxtIdx,
	jvxSize ctxtSubIdx,
	IjvxInputConnector* expose_connector,
	IjvxInputConnector* replace_connector)
{
	jvxErrorType res = JVX_NO_ERROR;
	auto elmTL = _common_set_node_vtask.variableTasks.find(ctxtIdx); // This is the ctxtIdGroup
	if (elmTL != _common_set_node_vtask.variableTasks.end())
	{
		jvxBool foundIt = false;
		auto elmRT = elmTL->second.activeRuntimeTasks.begin();
		for (; elmRT != elmTL->second.activeRuntimeTasks.end(); elmRT++)
		{
			jvxSize cnt = 0;
			auto elmR = elmRT->second.icons.begin();
			for (; elmR != elmRT->second.icons.end(); elmR++)
			{
				if (elmR->con == replace_connector)
				{
					break;
				}
				cnt++;
			}
			if (elmR != elmRT->second.icons.end())
			{
				// Found it!
				assert(elmR->stat == JVX_STATE_INIT);
				delete elmR->con;
				elmR->con = NULL;
				elmR->stat = JVX_STATE_NONE;
				assert(ctxtSubIdx == cnt); // this must match!
				jvx_bitClear(elmRT->second.iconsSelected, cnt);
				foundIt = true;
				break;
			}
		}

		assert(foundIt);
		if (
			(!JVX_EVALUATE_BITFIELD(elmRT->second.oconsSelected)) &&
			(!JVX_EVALUATE_BITFIELD(elmRT->second.iconsSelected)))
		{
			elmTL->second.activeRuntimeTasks.erase(elmRT);
		}
	}
	return res;
}

jvxErrorType
CjvxNVTasks::report_unselect_output_vtask(
	jvxSize ctxtIdx,
	jvxSize ctxtSubIdx,
	IjvxOutputConnector* expose_connector,
	IjvxOutputConnector* replace_connector)
{
	jvxErrorType res = JVX_NO_ERROR;

	auto elmTL = _common_set_node_vtask.variableTasks.find(ctxtIdx);
	if (elmTL != _common_set_node_vtask.variableTasks.end())
	{
		jvxBool foundIt = false;
		auto elmRT = elmTL->second.activeRuntimeTasks.begin();
		for (; elmRT != elmTL->second.activeRuntimeTasks.end(); elmRT++)
		{
			jvxSize cnt = 0;
			auto elmR = elmRT->second.ocons.begin();
			for (; elmR != elmRT->second.ocons.end(); elmR++)
			{
				if (elmR->con == replace_connector)
				{
					break;
				}
				cnt++;
			}
			if (elmR != elmRT->second.ocons.end())
			{
				assert(elmR->stat == JVX_STATE_INIT);
				delete elmR->con;
				elmR->con = NULL;
				elmR->stat = JVX_STATE_NONE;
				assert(cnt == ctxtSubIdx);
				jvx_bitClear(elmRT->second.oconsSelected, ctxtSubIdx);
				foundIt = true;
				break;
			}
		}

		assert(foundIt);
		if (
			(!JVX_EVALUATE_BITFIELD(elmRT->second.oconsSelected)) &&
			(!JVX_EVALUATE_BITFIELD(elmRT->second.iconsSelected)))
		{
			elmTL->second.activeRuntimeTasks.erase(elmRT);
		}
	}
	return res;
}

// ========================================================================================

jvxErrorType 
CjvxNVTasks::number_next_icon_vtask(
	jvxSize* num,
	jvxSize ctxtId,
	jvxSize ctxtSubId)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* > ocon;
	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* icon = NULL;

	if (num)
	{
		*num = 0;
	}
	res = find_i_map(ctxtId, ctxtSubId, icon, ocon);
	if (res == JVX_NO_ERROR)
	{
		if (num)
		{
			*num = ocon.size();
		}		
	}
	return res;
}

jvxErrorType 
CjvxNVTasks::reference_next_icon_vtask(
	jvxSize idx,
	IjvxConnectionIterator** it,
	jvxSize ctxtId,
	jvxSize ctxtSubId)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* > ocon;
	oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* icon = NULL;
	if (it)
		*it = nullptr;

	res = find_i_map(ctxtId, ctxtSubId, icon, ocon);
	if (res == JVX_NO_ERROR)
	{
		if (idx < ocon.size())
		{
			IjvxInputConnector* licon = nullptr;
			auto elm = ocon.begin();
			std::advance(elm, idx);
			oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* ptr = *elm;
			if(ptr->con)
			{
				ptr->con->connected_icon(&licon);
				if (licon)
				{
					if (it)
					{
						*it = static_cast<IjvxConnectionIterator*>(licon);
					}
				}
			}
		}
		else
		{
			res = JVX_ERROR_ID_OUT_OF_BOUNDS;
		}
	}
	return res;
}

// ========================================================================================

jvxErrorType
CjvxNVTasks::attach_single_input_connector(CjvxInputConnectorVtask* copyThis, 
	jvxSize ctxtTypeIdx, jvxSize ctxtIdx,
	IjvxConnectorBridge* bri,
	IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common, 
	std::vector<oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>>& icons,
	jvxBitField* activeInputs)
{
	jvxSize subId = copyThis->context_subid();
	if (
		(subId < icons.size()) && (icons[subId].con == NULL))
	{
		CjvxInputConnectorVtask* theNewIcon = new CjvxInputConnectorVtask(
			ctxtIdx,
			copyThis->_common_nvtask->_common_set_comnvtask.descriptor.c_str(),
			copyThis->_common_nvtask->_common_set_comnvtask.myParent,
			copyThis->common_vtask._common_set_comvtask.cbRef,
			copyThis->_common_nvtask->_common_set_comnvtask.associatedMaster,
			copyThis->_common_nvtask->_common_set_comnvtask.object,
			copyThis->_common_nvtask->_common_set_comnvtask.ctxtSubId,
			CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_USE);
		theNewIcon->_common_nvtask->_modify_ctxtid(ctxtTypeIdx);
		theNewIcon->select_connect_icon(bri, master, ass_connection_common, NULL);
		icons[subId].con = theNewIcon;
		icons[subId].stat = JVX_STATE_INIT;
		jvx_bitSet(*activeInputs, subId);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType
CjvxNVTasks::attach_single_output_connector(
	CjvxOutputConnectorVtask* copyThis,
	jvxSize ctxtTypeIdx,
	jvxSize ctxtIdx,
	IjvxConnectorBridge* bri,
	IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common,
	std::vector < oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output> >& ocons,
	jvxBitField* activeOutputs)
{
	jvxSize subId = copyThis->context_subid();
	if (
		(subId < ocons.size()) && (ocons[subId].con == NULL))
	{
		CjvxOutputConnectorVtask* theNewOcon = new CjvxOutputConnectorVtask(
			ctxtIdx,
			copyThis->_common_nvtask->_common_set_comnvtask.descriptor.c_str(),
			copyThis->_common_nvtask->_common_set_comnvtask.myParent,
			copyThis->common_vtask._common_set_comvtask.cbRef,
			copyThis->_common_nvtask->_common_set_comnvtask.associatedMaster,
			copyThis->_common_nvtask->_common_set_comnvtask.object,
			copyThis->_common_nvtask->_common_set_comnvtask.ctxtSubId,
			CjvxCommonVtask::vTaskConnectiorRole::JVX_VTASK_CONNECTOR_USE);
		theNewOcon->_common_nvtask->_modify_ctxtid(ctxtTypeIdx);
		theNewOcon->select_connect_ocon(bri, master, ass_connection_common, NULL); 
		ocons[subId].con = theNewOcon;
		ocons[subId].stat = JVX_STATE_INIT;
		jvx_bitSet(*activeOutputs, subId);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

// ====================================================================================