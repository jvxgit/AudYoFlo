#include "CayfAutomationModulesSrc2Snk.h"
#include "common/CjvxObjectLog.h"

namespace CayfAutomationModules
{
	jvxErrorType 
		CayfAutomationModulesSrc2Snk::activate(IjvxReport* report,
		IjvxHost* host,
			ayfAutoConnectSrc2Snk_callbacks* cb,
		jvxSize purpId,
		const ayfConnectConfigSrc2Snk& cfg,
		CjvxObjectLog* ptrLog)
	{
		reportRefPtr = report;
		refHostRefPtr = host;
		cbPtr = cb;
		purposeId = purpId;
		config = cfg;
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
		return JVX_NO_ERROR;
	}
	void
		CayfAutomationModulesSrc2Snk::try_connect(
			jvxComponentIdentification tp_reg, 
			jvxBool& established)
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

			jvxDataConnectionRuleParameters params(false, false, true, config.dbgOut, true);
			std::string chainName = config.chainNamePrefix + jvx_size2String(derived.tpSrc.slotsubid);


			res = con->create_connection_rule(chainName.c_str(),
				&theDataConnectionDefRule_id, &params, config.connectionCategory);
			if (res == JVX_NO_ERROR)
			{
				ayfOneConnectedProcess newProcess;
				// We store this as the target chain name that will be used for verification during connect

				JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
				log << "Created rule for chain <" << chainName << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

				IjvxDataConnectionRule* theDataConnectionDefRuleHdl = NULL;
				res = con->reference_connection_rule_uid(theDataConnectionDefRule_id, &theDataConnectionDefRuleHdl);
				if ((res == JVX_NO_ERROR) && (theDataConnectionDefRuleHdl))
				{	
					newProcess.chainName = chainName;
					newProcess.processUid = JVX_SIZE_UNSELECTED; // process not connected!
					elm->second.connectedProcesses.push_back(newProcess);					

					// Connection: 
					// <NEW COMPONENT MASTER SOURCE> -> MODULES -> <SINK>

					JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
					log << "Specifying master component <" << jvxComponentIdentification_txt(derived.tpMaster) <<
						">, master <" << config.nmMaster << ">." << std::endl;
					JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

					res = theDataConnectionDefRuleHdl->specify_master(
						derived.tpMaster,
						"*", config.nmMaster.c_str());
					assert(res == JVX_NO_ERROR);

					this->create_bridges(theDataConnectionDefRuleHdl, derived.tpSrc,
						derived.tpSink, elm->second.lstEntries, 
						config.oconNmSource, config.iconNmSink, bridgeId, 0, config.oconIdTrigger, config.iconIdTrigger);

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
						JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
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

						JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
						log << "Failed to connect chain <" << chainName << ">, reason: " << jvxErrorType_descr(res) << "." << std::endl;
						JVX_STOP_LOCK_LOG_REF(objLogRefPtr);
					}
				}
			}
			res = con->remove_connection_rule(theDataConnectionDefRule_id);
		}
	}

	void
		CayfAutomationModulesSrc2Snk::create_bridges(
			IjvxDataConnectionRule* theDataConnectionDefRuleHdl,
			jvxComponentIdentification tp_src,
			jvxComponentIdentification tp_sink,
			std::list<ayfConnectConfigCpEntryRuntime>& lst, 
			const std::string& oconNameSrc,
			const std::string& iconNameSink,
			jvxSize& bridgeId,
			jvxSize segId, 
			jvxSize oconIdTrigger, 
			jvxSize iconIdTrigger)
	{
		jvxErrorType res = JVX_NO_ERROR;
		
		std::string oconName = oconNameSrc;
		// std::string oconNameSrc = config.oconNmSource;
		// std::string oconNameSink = config.iconNmSink;
		jvxComponentIdentification tpOld = tp_src;
		
		for (auto& elmC : lst)
		{
			if (elmC.assSegmentId == segId)
			{
				// ==================================================================================
				JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
				log << "Connect from <" << jvxComponentIdentification_txt(tpOld) <<
					"> , connector <" << oconName << "> to <" << jvxComponentIdentification_txt(elmC.cpId) <<
					"> , connector <" << elmC.iconNm << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

				res = theDataConnectionDefRuleHdl->add_bridge_specification(
					tpOld,
					"*", oconName.c_str(),
					elmC.cpId,
					"*", elmC.iconNm.c_str(),
					("Bridge_" + jvx_size2String(bridgeId)).c_str(), false, false, 
					oconIdTrigger, elmC.idIconRefTriggerConnector);
				assert(res == JVX_NO_ERROR);
				// ==================================================================================

				tpOld = elmC.cpId;
				oconName = elmC.oconNm;
				oconIdTrigger = elmC.idOconRefTriggerConnector;
				bridgeId++;
			}
		}

		// ==================================================================================
		JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
		log << "Connect from <" << jvxComponentIdentification_txt(tpOld) <<
			"> , connector <" << oconName << "> to <" << jvxComponentIdentification_txt(tp_sink) <<
			"> , connector <" << iconNameSink << ">." << std::endl;
		JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

		res = theDataConnectionDefRuleHdl->add_bridge_specification(
			tpOld,
			"*", oconName.c_str(),
			tp_sink,
			"*", iconNameSink.c_str(),
			("Bridge_" + jvx_size2String(bridgeId)).c_str(),false, false, 
			oconIdTrigger,
			iconIdTrigger);
		bridgeId++;
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
		if (!cbPtr)
		{
			return JVX_ERROR_INVALID_SETTING;
		}

		// In the remainder of the function, other components will pop up. Then, the 
		// function must be blocked in this class instance
		if (lockOperation)
		{
			return JVX_ERROR_COMPONENT_BUSY;
		}
		
		// Use tempory derivative
		ayfConnectDerivedSrc2Snk derivedArgs;
		deriveArguments(derivedArgs, tp_activated);

		// We decide if this rule applies in this function
		jvxErrorType res = cbPtr->allow_master_connect(purposeId, tp_activated,
			derivedArgs.tpSrc, derivedArgs.tpSink, config.oconNmSource, config.iconNmSink);
		if (res == JVX_NO_ERROR)
		{
			// Here we copy the args
			derived = derivedArgs;
			lockOperation = true;
		}
		else
		{
			return res;
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
		
		ayfEstablishedProcesses realizeChain;
		for (auto& elmM : config.connectedNodes)
		{
			ayfConnectConfigCpEntryRuntime cpElm(elmM);

			cpElm.cpId = cpElm.cpTp;
			res = jvx_activateObjectInModule(refHostRefPtr, cpElm.cpId, cpElm.modName, obj_dev, true, cpElm.manSuffix);

			if(res == JVX_NO_ERROR)
			{
				JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
				log << "Activated module <" << cpElm.modName << "> with suffix <" << cpElm.manSuffix << "> in location <" << jvxComponentIdentification_txt(cpElm.cpId) << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

				realizeChain.lstEntries.push_back(cpElm);
			}
			else
			{
				JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
				log << "Failed to activate module <" << cpElm.modName << "> with suffix <" << cpElm.manSuffix << "> in location <" << jvxComponentIdentification_txt(cpElm.cpId) << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr);
				break;
			}
		}

		if (res == JVX_NO_ERROR)
		{
			jvxBool established = false;
			module_connections[tp_activated] = realizeChain;
			try_connect(tp_activated, established);

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
				JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
				log << "On error, deactivating  module <" << elmI.modName << "> with suffix <" << elmI.manSuffix << "> in location <" << jvxComponentIdentification_txt(elmI.cpId) << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

				jvxErrorType resL = jvx_deactivateObjectInModule(refHostRefPtr, elmI.cpId);
			}
		}
		lockOperation = false; 
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
				JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
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
			const std::string& description,
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
							cbPtr->adapt_single_property_on_connect(purposeId, theProc.chainName, modName, description, props);
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

	void
		CayfAutomationModulesSrc2Snk::deriveArguments(ayfConnectDerivedSrc2Snk& derivedArgs, const jvxComponentIdentification& tp_activated)
	{
		derivedArgs.tpSrc = tp_activated;
		derivedArgs.tpSink = config.tpInvolved;
		derivedArgs.tpMaster = tp_activated;
	}

	void 
		CayfAutomationModulesSrc2Snk::postponed_try_connect()
	{
	}
}
