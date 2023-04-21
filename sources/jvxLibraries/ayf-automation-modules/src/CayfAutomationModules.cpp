#include "CayfAutomationModules.h"

void
CayfAutomationModules::try_connect(
	jvxComponentIdentification tp_audiofile,
	const std::string& prefixChain,
	const std::string& oconNmMaster,
	const std::string& iconNmMaster,
	jvxSize connectionCategory)
{
	// Here, the audio device has been activated.
	// At this point, we need to run the connection

#ifdef FERNCAST_AUTOCONNECT_VERBOSE
	std::cout << "Audio file device was activated. Run the autoconnect rule!" << std::endl;
#endif

	jvxErrorType res = JVX_NO_ERROR;
	jvxSize theDataConnectionDefRule_id = JVX_SIZE_UNSELECTED;
	IjvxDataConnections* con = nullptr;
	con = reqInterface<IjvxDataConnections>(refHostRefPtr);
	if (con)
	{
		// Get the creation rule!
		auto elm = ids_sub_components_file_in.find(tp_audiofile);
		assert(elm != ids_sub_components_file_in.end());

		jvxDataConnectionRuleParameters params(false, false, true, false, true);
		res = con->create_connection_rule((prefixChain + jvx_size2String(tp_audiofile.slotsubid)).c_str(),
			&theDataConnectionDefRule_id, &params, connectionCategory);
		if (res == JVX_NO_ERROR)
		{
			IjvxDataConnectionRule* theDataConnectionDefRuleHdl = NULL;
			jvxSize theDataConnectionDefRule_id_master = JVX_SIZE_UNSELECTED;
			jvxSize theDataConnectionDefRule_id_slave = JVX_SIZE_UNSELECTED;
			res = con->reference_connection_rule_uid(theDataConnectionDefRule_id, &theDataConnectionDefRuleHdl);
			if ((res == JVX_NO_ERROR) && (theDataConnectionDefRuleHdl))
			{
				jvxBool established = false;
				jvxSize proc_id = JVX_SIZE_UNSELECTED;
				jvxBool rep_global = false;

				res = theDataConnectionDefRuleHdl->specify_master(
					tp_audiofile,
					"*", oconNmMaster.c_str());
				assert(res == JVX_NO_ERROR);

				jvxSize bridgeId = 0;
				std::string oconName = oconNmMaster;
				jvxComponentIdentification tpOld = tp_audiofile;
				for (auto& elmC : elm->second)
				{
					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						tpOld,
						"*", oconName.c_str(),
						elmC.cpId,
						"*", elmC.iconNm.c_str(),
						("Bridge_" + jvx_size2String(bridgeId)).c_str());
					assert(res == JVX_NO_ERROR);

					tpOld = elmC.cpId;
					oconName = elmC.oconNm;
					bridgeId++;
				}

				res = theDataConnectionDefRuleHdl->add_bridge_specification(
					tpOld,
					"*", oconName.c_str(),
					jvxComponentIdentification(JVX_COMPONENT_AUDIO_NODE, 0, 0),
					"*", iconNmMaster.c_str(),
					("Bridge_" + jvx_size2String(bridgeId)).c_str());
				assert(res == JVX_NO_ERROR);

				theDataConnectionDefRuleHdl->try_connect_direct(
					con, refHostRefPtr,
					&established,
					reportRefPtr,
					&proc_id,
					&rep_global);
			}
		}
		res = con->remove_connection_rule(theDataConnectionDefRule_id);
	}
}

jvxErrorType
CayfAutomationModules::activate_all_submodules(const jvxComponentIdentification& tp_activated)
{
	// On return value JVX_ERROR_POSTPONE, this code is run again 
	// to better feed the auto connect rules. Therefore we keep track of this
	// list. 

	// Check if the type is to be handled by this module instance
	if (cbPtr)
	{
		jvxErrorType res = cbPtr->allow_master_connect(purposeId, tp_activated);
		if (res != JVX_NO_ERROR)
		{
			// Nothing to do here!
			return res;
		}
	}

	auto elm = ids_sub_components_file_in.find(tp_activated);
	if (elm != ids_sub_components_file_in.end())
	{
		return JVX_ERROR_ALREADY_IN_USE;
	}

	// Activate the bitsream decoder
	//jvxComponentIdentification tp_dec = JVX_COMPONENT_AUDIO_NODE;
	//jvxComponentIdentification tp_fwdbuf = JVX_COMPONENT_AUDIO_NODE;
	IjvxObject* obj_dev = nullptr;
	refHostRefPtr->request_object_selected_component(tp_activated, &obj_dev);

	jvxErrorType res = JVX_NO_ERROR;
	std::list<ayfConnectConfigCpEntryRuntime> realizeChain;
	for (auto& elmM : config.connectedNodes)
	{
		ayfConnectConfigCpEntryRuntime cpElm(elmM);

		cpElm.cpId = cpElm.cpTp;
		res = jvx_activateObjectInModule(refHostRefPtr, cpElm.cpId, cpElm.modName, obj_dev, true, cpElm.manSuffix);
		if (res == JVX_NO_ERROR)
		{
			realizeChain.push_back(cpElm);
		}
		else
		{
			break;
		}
	}

	if(res == JVX_NO_ERROR)
	{ 
		ids_sub_components_file_in[tp_activated] = realizeChain;
		try_connect(tp_activated, config.chainNamePrefix, config.oconNmMaster, config.iconNmMaster, config.connectionCategory);
	}

	return JVX_NO_ERROR;
}

void
CayfAutomationModules::deactivate_all_submodules(const jvxComponentIdentification& tp_deactivated)
{
	auto elm = ids_sub_components_file_in.find(tp_deactivated);
	assert(elm != ids_sub_components_file_in.end());
	for (auto elmI : elm->second)
	{
		jvxErrorType res = jvx_deactivateObjectInModule(refHostRefPtr, elmI.cpId);
		assert(res == JVX_NO_ERROR);
	}
	ids_sub_components_file_in.erase(elm);
}

void
CayfAutomationModules::adapt_all_submodules(const std::string& modName, jvxComponentIdentification tpCp)
{	
	jvxErrorType res = JVX_NO_ERROR;

	std::cout << "Component -" << jvxComponentIdentification_txt(tpCp) << "- :" << modName << std::endl;

	// We need to:
	// 1) In module jvxAuTFileReader -> Set property "/threaded_read" to false
	// 2) In module jvxAuNForwardBuffer -> Set property "/bypass_buffer" to false

	IjvxObject* obj = nullptr;
	refHostRefPtr->request_object_selected_component(tpCp, &obj);
	if (obj)
	{
		IjvxInterfaceFactory* ifFac = nullptr;
		obj->interface_factory(&ifFac);
		if (ifFac)
		{
			IjvxProperties* props = reqInterface<IjvxProperties>(ifFac);
			if (props)
			{
				if (cbPtr)
				{
					cbPtr->adapt_single_property_on_connect(purposeId, modName, props);
				}				
				retInterface<IjvxProperties>(ifFac, props);
			}
		}
		refHostRefPtr->return_object_selected_component(tpCp, obj);
	}
}
