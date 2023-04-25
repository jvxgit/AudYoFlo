#include "CayfAutomationModulesSrc2Snk.h"
#include "common/CjvxObjectLog.h"

namespace CayfAutomationModules
{
	jvxErrorType 
		CayfAutomationModulesSrc2Snk::activate(IjvxReport* report,
		IjvxHost* host,
		ayfAutoConnect_callbacks* cb,
		jvxSize purpId,
		const ayfConnectConfig& cfg,
		ayfTriggerComponent trigCompTypeArg,
		CjvxObjectLog* ptrLog)
	{
		reportRefPtr = report;
		refHostRefPtr = host;
		cbPtr = cb;
		purposeId = purpId;
		config = cfg;
		trigCompType = trigCompTypeArg;
		objLogRefPtr = ptrLog;
		return JVX_NO_ERROR;
	}

	jvxErrorType 
		CayfAutomationModulesSrc2Snk::deactivate()
	{
		assert(module_connections.size() == 0);
		// If we end the program, we may reach here without explicitely deactivatin a single chain.
		// The reason is that we disconnect automation before deactivating non-default components
		/*
		for (auto& elm : ids_sub_components_file_in)
		{
			deactivate_all_submodule_audio_input(elm.first);
		}
		*/

		reportRefPtr = nullptr;
		refHostRefPtr = nullptr;
		cbPtr = nullptr;
		purposeId = JVX_SIZE_UNSELECTED;
		config.chainNamePrefix.clear();
		config.connectedNodes.clear();
		objLogRefPtr = nullptr;
		trigCompType = ayfTriggerComponent::AYF_TRIGGER_COMPONENT_IS_SOURCE;
		return JVX_NO_ERROR;
	}
	void
		CayfAutomationModulesSrc2Snk::try_connect(jvxComponentIdentification tp_reg, jvxComponentIdentification tp_src, jvxComponentIdentification tp_sink, jvxBool& established, jvxBool dbgConnect)
	{
		jvxSize proc_id = JVX_SIZE_UNSELECTED;
		jvxBool rep_global = false;
		jvxSize bridgeId = 0;

		// Here, the audio device has been activated.
		// At this point, we need to run the connection

		jvxErrorType res = JVX_NO_ERROR;
		jvxSize theDataConnectionDefRule_id = JVX_SIZE_UNSELECTED;
		IjvxDataConnections* con = nullptr;
		
		jvxErrorType resC = JVX_NO_ERROR;
		con = reqInterface<IjvxDataConnections>(refHostRefPtr);
		if (con)
		{
			// Get the creation rule!
			auto& elm = module_connections.find(tp_reg);
			assert(elm != module_connections.end());

			jvxDataConnectionRuleParameters params(false, false, true, dbgConnect, true);
			std::string chainName = config.chainNamePrefix + jvx_size2String(tp_src.slotsubid);


			res = con->create_connection_rule(chainName.c_str(),
				&theDataConnectionDefRule_id, &params, config.connectionCategory);
			if (res == JVX_NO_ERROR)
			{
				ayfOneConnectedProcess newProcess;
				// We store this as the target chain name that will be used for verification during connect

				JVX_START_LOCK_LOG_REF(objLogRefPtr, 3);
				log << "Created rule for chain <" << chainName << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

				IjvxDataConnectionRule* theDataConnectionDefRuleHdl = NULL;
				res = con->reference_connection_rule_uid(theDataConnectionDefRule_id, &theDataConnectionDefRuleHdl);
				if ((res == JVX_NO_ERROR) && (theDataConnectionDefRuleHdl))
				{	
					newProcess.chainName = chainName;
					newProcess.processUid = JVX_SIZE_UNSELECTED; // process not connected!
					elm->second.connectedProcesses.push_back(newProcess);

					jvxComponentIdentification tp_master = preset_master();

					// Connection: 
					// <NEW COMPONENT MASTER SOURCE> -> MODULES -> <SINK>

					JVX_START_LOCK_LOG_REF(objLogRefPtr, 3);
					log << "Specifying master component <" << jvxComponentIdentification_txt(tp_master) <<
						">, master <" << config.nmMaster << ">." << std::endl;
					JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

					res = theDataConnectionDefRuleHdl->specify_master(
						tp_master,
						"*", config.nmMaster.c_str());
					assert(res == JVX_NO_ERROR);

					this->create_bridges(theDataConnectionDefRuleHdl, tp_src, tp_sink, elm->second.lstEntries, bridgeId);

					resC = theDataConnectionDefRuleHdl->try_connect_direct(
						con, refHostRefPtr,
						&established,
						reportRefPtr,
						&proc_id,
						&rep_global);
				
					// If we manage to connect the chain, the bool "established" is true.
					// resC only observes general functionality. It should always be JVX_NO_ERROR!
					assert(resC == JVX_NO_ERROR);
					if (established)
					{
						JVX_START_LOCK_LOG_REF(objLogRefPtr, 3);
						log << "Successfully connected chain <" << chainName << ">" << std::endl;
						JVX_STOP_LOCK_LOG_REF(objLogRefPtr);
					}
					else
					{
						// Remove the previously added process
						for (auto elmP = elm->second.connectedProcesses.begin(); elmP != elm->second.connectedProcesses.end(); elmP++)
						{
							if (elmP->chainName == chainName)
							{
								elm->second.connectedProcesses.erase(elmP);
								break;
							}
						}

						JVX_START_LOCK_LOG_REF(objLogRefPtr, 3);
						log << "Failed to connect chain <" << chainName << ">, reason: " << jvxErrorType_descr(res) << "." << std::endl;
						JVX_STOP_LOCK_LOG_REF(objLogRefPtr);
					}
				}
			}
			res = con->remove_connection_rule(theDataConnectionDefRule_id);
		}
	}

	jvxComponentIdentification&
		CayfAutomationModulesSrc2Snk::preset_master()
	{
		return config.tpSrc;
	}

	void
		CayfAutomationModulesSrc2Snk::create_bridges(
			IjvxDataConnectionRule* theDataConnectionDefRuleHdl,
			jvxComponentIdentification tp_src,
			jvxComponentIdentification tp_sink,
			std::list<ayfConnectConfigCpEntryRuntime>& lst, 
			jvxSize& bridgeId)
	{
		jvxErrorType res = JVX_NO_ERROR;
		
		std::string oconName = config.oconNmSource;
		jvxComponentIdentification tpOld = tp_src;
		
		for (auto& elmC : lst)
		{
			// ==================================================================================
			JVX_START_LOCK_LOG_REF(objLogRefPtr, 3);
			log << "Connect from <" << jvxComponentIdentification_txt(tpOld) <<
				"> , connector <" << oconName << "> to <" << jvxComponentIdentification_txt(elmC.cpId) <<
				"> , connector <" << elmC.iconNm << ">." << std::endl;
			JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

			res = theDataConnectionDefRuleHdl->add_bridge_specification(
				tpOld,
				"*", oconName.c_str(),
				elmC.cpId,
				"*", elmC.iconNm.c_str(),
				("Bridge_" + jvx_size2String(bridgeId)).c_str());
			assert(res == JVX_NO_ERROR);
			// ==================================================================================

			tpOld = elmC.cpId;
			oconName = elmC.oconNm;
			bridgeId++;
		}

		// ==================================================================================
		JVX_START_LOCK_LOG_REF(objLogRefPtr, 3);
		log << "Connect from <" << jvxComponentIdentification_txt(tpOld) <<
			"> , connector <" << oconName << "> to <" << jvxComponentIdentification_txt(config.tpSink) <<
			"> , connector <" << config.iconNmSink << ">." << std::endl;
		JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

		res = theDataConnectionDefRuleHdl->add_bridge_specification(
			tpOld,
			"*", oconName.c_str(),
			tp_sink,
			"*", config.iconNmSink.c_str(),
			("Bridge_" + jvx_size2String(bridgeId)).c_str());
		assert(res == JVX_NO_ERROR);	
		// ==================================================================================
	}

	jvxErrorType
		CayfAutomationModulesSrc2Snk::activate_all_submodules(const jvxComponentIdentification& tp_activated)
	{
		// On return value JVX_ERROR_POSTPONE, this code is run again 
		// to better feed the auto connect rules. Therefore we keep track of this
		// list. 

		// Check if the type is to be handled by this module instance
		if (cbPtr)
		{
			jvxComponentIdentification cpCopy;
			switch (trigCompType)
			{
			case ayfTriggerComponent::AYF_TRIGGER_COMPONENT_IS_SOURCE:
				cpCopy = config.tpSrc;
				config.tpSrc = tp_activated;
				break;
			case ayfTriggerComponent::AYF_TRIGGER_COMPONENT_IS_SINK:
				cpCopy = config.tpSink;
				config.tpSink = tp_activated;
				break;
			default: 
				assert(0);
			}

			// We decide if this rule applies in this function
			jvxErrorType res = cbPtr->allow_master_connect(purposeId, tp_activated,
				config.tpSrc, config.tpSink, config.oconNmSource, config.iconNmSink);
			if (res != JVX_NO_ERROR)
			{
				// Nothing to do here!
				switch (trigCompType)
				{
				case ayfTriggerComponent::AYF_TRIGGER_COMPONENT_IS_SOURCE:
					config.tpSrc = cpCopy;
					break;
				case ayfTriggerComponent::AYF_TRIGGER_COMPONENT_IS_SINK:
					config.tpSink = cpCopy;
					break;
				}
				return res;
			}
		}

		auto elm = module_connections.find(tp_activated);
		if (elm != module_connections.end())
		{
			return JVX_ERROR_ALREADY_IN_USE;
		}

		// Activate the bitsream decoder
		//jvxComponentIdentification tp_dec = JVX_COMPONENT_AUDIO_NODE;
		//jvxComponentIdentification tp_fwdbuf = JVX_COMPONENT_AUDIO_NODE;
		IjvxObject* obj_dev = nullptr;
		refHostRefPtr->request_object_selected_component(tp_activated, &obj_dev);

		jvxErrorType res = JVX_NO_ERROR;
		ayfEstablishedProcesses realizeChain;
		for (auto& elmM : config.connectedNodes)
		{
			ayfConnectConfigCpEntryRuntime cpElm(elmM);

			cpElm.cpId = cpElm.cpTp;
			res = jvx_activateObjectInModule(refHostRefPtr, cpElm.cpId, cpElm.modName, obj_dev, true, cpElm.manSuffix);

			if(res == JVX_NO_ERROR)
			{
				JVX_START_LOCK_LOG_REF(objLogRefPtr, 3);
				log << "Activated module <" << cpElm.modName << "> with suffix <" << cpElm.manSuffix << "> in location <" << jvxComponentIdentification_txt(cpElm.cpId) << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr);
				realizeChain.lstEntries.push_back(cpElm);
			}
			else
			{
				JVX_START_LOCK_LOG_REF(objLogRefPtr, 3);
				log << "Failed to activate module <" << cpElm.modName << "> with suffix <" << cpElm.manSuffix << "> in location <" << jvxComponentIdentification_txt(cpElm.cpId) << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr);
				break;
			}
		}

		if (res == JVX_NO_ERROR)
		{
			jvxBool established = false;
			module_connections[tp_activated] = realizeChain;
			try_connect(tp_activated, config.tpSrc, config.tpSink, established, config.dbgOut);

			if(!established)
			{
				// If we do not establish the connection we can deactivate the added objects
				module_connections.erase(tp_activated);
				res = JVX_ERROR_INVALID_SETTING;
			}
		}

		if (res != JVX_NO_ERROR)
		{
			// Here we end up in error case!
			for (auto elmI : realizeChain.lstEntries)
			{
				// On this list, we only see the active nodes!
				JVX_START_LOCK_LOG_REF(objLogRefPtr, 3);
				log << "On error, deactivating  module <" << elmI.modName << "> with suffix <" << elmI.manSuffix << "> in location <" << jvxComponentIdentification_txt(elmI.cpId) << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

				jvxErrorType resL = jvx_deactivateObjectInModule(refHostRefPtr, elmI.cpId);
			}
		}
		return res;
	}

	jvxErrorType
		CayfAutomationModulesSrc2Snk::deactivate_all_submodules(const jvxComponentIdentification& tp_deactivated)
	{
		auto elm = module_connections.find(tp_deactivated);
		if (elm != module_connections.end())
		{
			for (auto elmI : elm->second.lstEntries)
			{
				JVX_START_LOCK_LOG_REF(objLogRefPtr, 3);
				log << "Deactivating  module <" << elmI.modName << "> with suffix <" << elmI.manSuffix << "> in location <" << jvxComponentIdentification_txt(elmI.cpId) << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

				jvxErrorType res = jvx_deactivateObjectInModule(refHostRefPtr, elmI.cpId);
				assert(res == JVX_NO_ERROR);
			}
			module_connections.erase(elm);
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	jvxErrorType 
		CayfAutomationModulesSrc2Snk::associate_process(jvxSize uIdProcess,
			const std::string& nmChain)
	{
		for (auto& elm : module_connections)
		{
			for (auto& elmP : elm.second.connectedProcesses)
			{
				if (nmChain == elmP.chainName)
				{
					elmP.processUid = uIdProcess;
					return JVX_NO_ERROR;
				}
			}
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	jvxErrorType 
		CayfAutomationModulesSrc2Snk::deassociate_process(jvxSize uIdProcess)
	{
		return JVX_NO_ERROR;
	}

	jvxErrorType
		CayfAutomationModulesSrc2Snk::adapt_all_submodules(jvxSize uIdProc,
			const std::string& modName,
			const jvxComponentIdentification& tpCp)
	{
		jvxErrorType res = JVX_NO_ERROR;

		// Find the element which fits the processId
		auto elm = module_connections.begin();
		ayfOneConnectedProcess theProc;
		for (; elm != module_connections.end(); elm++)
		{
			auto elmP = elm->second.connectedProcesses.begin();
			for (; elmP != elm->second.connectedProcesses.end(); elmP++)
			{
				if (elmP->processUid == uIdProc)
				{
					theProc = *elmP;
					break;
				}
			}

			if (JVX_CHECK_SIZE_SELECTED(theProc.processUid))
			{
				break;
			}
		}

		if (JVX_CHECK_SIZE_SELECTED(theProc.processUid))
		{
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
							cbPtr->adapt_single_property_on_connect(purposeId, theProc.chainName, modName, props);
						}
						retInterface<IjvxProperties>(ifFac, props);
					}
				}
				refHostRefPtr->return_object_selected_component(tpCp, obj);
			}
			return JVX_NO_ERROR;

		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
}
