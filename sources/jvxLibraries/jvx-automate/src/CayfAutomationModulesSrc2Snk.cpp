#include "CayfAutomationModulesCast.h"
#include "common/CjvxObjectLog.h"

namespace CayfAutomationModules
{
	jvxErrorType 
		CayfAutomationModulesSrc2Snk::activate(IjvxReport* report,
		IjvxHost* host,
			ayfAutoConnect_callbacks* cb,
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
			auto elm = module_connections.find(tp_reg);
			assert(elm != module_connections.end());

			IayfEstablishedProcessesCommon* sglElmPtr = elm->second;
			IayfEstablishedProcessesSrc2Snk& sglElm = *(reinterpret_cast<IayfEstablishedProcessesSrc2Snk*>(sglElmPtr->specificType(CayfAutomationModules::ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNK)));

			this->pre_run_chain_connect(tp_reg, con, sglElmPtr);

			jvxDataConnectionRuleParameters params(false, false, true, config.miscArgs.dbgOut, true);
			std::string chainName = CayfAutomationModules::CayfAutomationModulesHelper::chainPrefixToChain(config.chainNamePrefix, tp_reg);
			// jvx_size2String(derived.tpSrc.slotsubid);


			res = con->create_connection_rule(chainName.c_str(),
				&theDataConnectionDefRule_id, &params, config.miscArgs.connectionCategory);
			if (res == JVX_NO_ERROR)
			{
				ayfOneConnectedProcess newProcess;
				// We store this as the target chain name that will be used for verification during connect

				JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
				log << "Created rule for chain <" << chainName << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr, JVX_CREATE_CODE_LOCATION_TAG);

				IjvxDataConnectionRule* theDataConnectionDefRuleHdl = NULL;
				res = con->reference_connection_rule_uid(theDataConnectionDefRule_id, &theDataConnectionDefRuleHdl);
				if ((res == JVX_NO_ERROR) && (theDataConnectionDefRuleHdl))
				{	
					newProcess.chainName = chainName;
					newProcess.processUid = JVX_SIZE_UNSELECTED; // process not connected!
					sglElmPtr->connectedProcesses().push_back(newProcess);

					// Connection: 
					// <NEW COMPONENT MASTER SOURCE> -> MODULES -> <SINK>

					JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
					log << "Specifying master component <" << jvxComponentIdentification_txt(sglElm.derived.tpMaster) <<
						">, master <" << config.nmMaster << ">." << std::endl;
					JVX_STOP_LOCK_LOG_REF(objLogRefPtr, JVX_CREATE_CODE_LOCATION_TAG);

					res = theDataConnectionDefRuleHdl->specify_master(
						sglElm.derived.tpMaster,
						"*", config.nmMaster.c_str());
					assert(res == JVX_NO_ERROR);

					// This call defines the definition of the second part!!
					// <SOMETHING> --derived.tpSrc -> <modules[id=1]> -> derived.tpSink
					// Then, in the next step, the prechain will run from 
					// SOMETHING => derived.tpSink -> <modules[id=0] -> derived.tpSrc
					this->create_bridges(theDataConnectionDefRuleHdl, sglElm.derived.tpSrc,
						sglElm.derived.tpSink, sglElm.lstEntries, sglElmPtr,
						config.oconNmSource, config.iconNmSink, bridgeId, 
						config.oconIdTrigger, config.iconIdTrigger);

					resC = theDataConnectionDefRuleHdl->try_connect_direct(
						con, refHostRefPtr,
						&established,
						nullptr, //reportRefPtr,
						&proc_id,
						&rep_global);
				
					// If we manage to connect the chain, the bool "established" is true.
					// resC only observes general functionality. It should always be JVX_NO_ERROR!
 					assert(resC == JVX_NO_ERROR);
					if (established)
					{
						report_connection_established(proc_id, ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNK, sglElmPtr);
						JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
						log << "Successfully connected chain <" << chainName << ">" << std::endl;
						JVX_STOP_LOCK_LOG_REF(objLogRefPtr, JVX_CREATE_CODE_LOCATION_TAG);
					}
					else
					{
						// Remove the previously added process
						for (auto elmP = sglElmPtr->connectedProcesses().begin(); elmP != sglElmPtr->connectedProcesses().end(); elmP++)
						{
							if (elmP->chainName == chainName)
							{
								sglElmPtr->connectedProcesses().erase(elmP);
								break;
							}
						}

						JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
						log << "Failed to connect chain <" << chainName << ">, reason: " << jvxErrorType_descr(res) << "." << std::endl;
						JVX_STOP_LOCK_LOG_REF(objLogRefPtr, JVX_CREATE_CODE_LOCATION_TAG);
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
			std::list<ayfConnectConfigCpEntryRuntime>& elm,
			IayfEstablishedProcessesCommon* sglElmPtr,
			const std::string& oconNameSrc,
			const std::string& iconNameSink,
			jvxSize& bridgeId,
			jvxSize oconIdTrigger, 
			jvxSize iconIdTrigger)
	{
		jvxErrorType res = JVX_NO_ERROR;
		
		std::string oconName = oconNameSrc;
		// std::string oconNameSrc = config.oconNmSource;
		// std::string oconNameSink = config.iconNmSink;
		jvxComponentIdentification tpOld = tp_src;
		
		for (auto& elmC : elm)
		{
			// ==================================================================================
			JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
			log << "Connect from <" << jvxComponentIdentification_txt(tpOld) <<
				"> , connector <" << oconName << "> to <" << jvxComponentIdentification_txt(elmC.cpId) <<
				"> , connector <" << elmC.iconNm << ">." << std::endl;
			JVX_STOP_LOCK_LOG_REF(objLogRefPtr, JVX_CREATE_CODE_LOCATION_TAG);

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

		// ==================================================================================
		JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
		log << "Connect from <" << jvxComponentIdentification_txt(tpOld) <<
			"> , connector <" << oconName << "> to <" << jvxComponentIdentification_txt(tp_sink) <<
			"> , connector <" << iconNameSink << ">." << std::endl;
		JVX_STOP_LOCK_LOG_REF(objLogRefPtr, JVX_CREATE_CODE_LOCATION_TAG);

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
		
		// We decide if this rule applies in this function
		jvxErrorType res = cbPtr->allow_master_connect(purposeId, tp_activated);
		if (res == JVX_NO_ERROR)
		{
			lockOperation = true;
		}
		else
		{
			return res;
		}
		
		auto elm = module_connections.find(tp_activated);
		if (elm != module_connections.end())
		{
			lockOperation = false;
			return JVX_ERROR_ALREADY_IN_USE;
		}

		// Activate the bitsream decoder
		//jvxComponentIdentification tp_dec = JVX_COMPONENT_AUDIO_NODE;
		//jvxComponentIdentification tp_fwdbuf = JVX_COMPONENT_AUDIO_NODE;
		IjvxObject* obj_dev = nullptr;
		refHostRefPtr->request_object_selected_component(tp_activated, &obj_dev);
		
		IayfEstablishedProcessesCommon* realizeChainPtr = allocate_chain_realization();
		IayfEstablishedProcessesSrc2Snk& sglElm = *(reinterpret_cast<IayfEstablishedProcessesSrc2Snk*>(realizeChainPtr->specificType(CayfAutomationModules::ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNK)));

		// IayfEstablishedProcessesSrc2Snk& realizeChain = *(reinterpret_cast<IayfEstablishedProcessesSrc2Snk*>(realizeChainPtr->specificType(CayfAutomationModules::ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNK)));
		// ========================================================================
		
		// Build up list of components as required for all other operations
		res = this->pre_run_chain_prepare(obj_dev, realizeChainPtr);

		if (res != JVX_NO_ERROR)
		{
			this->post_run_chain_prepare(realizeChainPtr);
		}
		else
		{
			jvxBool established = false;
			module_connections[tp_activated] = realizeChainPtr;

			// Here we copy the args for the connection
			// Use tempory derivative		
			deriveArguments(sglElm.derived, tp_activated, realizeChainPtr);

			// Run the connection code
			try_connect(tp_activated, sglElm.established);

			if(!sglElm.established)
			{
				res = this->on_connection_not_established(tp_activated, realizeChainPtr);				
				if (res != JVX_NO_ERROR)
				{
					lockOperation = false;
					return res;
				}
			}
		}

		/*
		if (res != JVX_NO_ERROR)
		{
			// Here we end up in error case!
			for (auto elmI : realizeChain.lstEntries)
			{
				// On this list, we only see the active nodes!
				JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
				log << "On error, deactivating  module <" << elmI.modName << "> with suffix <" << elmI.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(elmI.cpId) << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

				jvxErrorType resL = jvx_deactivateObjectInModule(refHostRefPtr, elmI.cpId);
			}
		}
		*/
		lockOperation = false;
		return res;
	}

	jvxErrorType 
		CayfAutomationModulesSrc2Snk::on_connection_not_established(jvxComponentIdentification tp_activated, IayfEstablishedProcessesCommon* realizeChainPtr)
	{
		jvxErrorType res = JVX_NO_ERROR;		
		if (!config.allowLateConnect)
		{
			// If we do not establish the connection we can deactivate the added objects
			deactivate_all_submodules(tp_activated);
			module_connections.erase(tp_activated);
			this->deallocate_chain_realization(realizeChainPtr);
			res = JVX_ERROR_INVALID_SETTING;
		}
		return res;
	}

	jvxErrorType
		CayfAutomationModulesSrc2Snk::deactivate_all_submodules(const jvxComponentIdentification& tp_deactivated)
	{
		auto elm = module_connections.find(tp_deactivated);
		if (elm != module_connections.end())
		{
			IayfEstablishedProcessesCommon* sglElmPtr = elm->second;
			this->post_run_chain_prepare(sglElmPtr);

			module_connections.erase(elm);
			this->deallocate_chain_realization(sglElmPtr);
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
			IayfEstablishedProcessesCommon* sglElmPtr = elm.second;			
			IayfEstablishedProcessesSrc2Snk& sglElm = castEstablishProcess<IayfEstablishedProcessesSrc2Snk>(sglElmPtr);
			for (auto& elmP : sglElmPtr->connectedProcesses())
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
			const jvxComponentIdentification& tpCp,
			jvxReportCommandRequest req)
	{
		jvxErrorType res = JVX_NO_ERROR;

		// Find the element which fits the processId
		auto elm = module_connections.begin();
		ayfOneConnectedProcess theProc;
		for (; elm != module_connections.end(); elm++)
		{
			IayfEstablishedProcessesCommon* sglElmPtr = elm->second;
			IayfEstablishedProcessesSrc2Snk& sglElm = castEstablishProcess<IayfEstablishedProcessesSrc2Snk>(sglElmPtr);
			auto elmP = sglElmPtr->connectedProcesses().begin();
			for (; elmP != sglElmPtr->connectedProcesses().end(); elmP++)
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
							cbPtr->adapt_single_property_on_event(purposeId, theProc.chainName, modName, description, req, props);
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
		CayfAutomationModulesSrc2Snk::deriveArguments(ayfConnectDerivedSrc2Snk& derivedArgs, 
			const jvxComponentIdentification& tp_activated,
			IayfEstablishedProcessesCommon* realizeChainArg)
	{
		derivedArgs.tpSrc = tp_activated;
		derivedArgs.tpSink = config.tpAssign;
		derivedArgs.tpMaster = tp_activated;
	}

	void
		CayfAutomationModulesSrc2Snk::print(std::ostream& out)
	{
		jvxSize segId = 0;
		
		// First the pre-chain
		out << "## ##" << config.chainNamePrefix << std::endl;
		this->print(out, segId);
		out << std::endl;
	}

	void
		CayfAutomationModulesSrc2Snk::print(std::ostream& out, jvxSize& segId)
	{
		std::string refIdIn;
		std::string refIdOut;
		if (JVX_CHECK_SIZE_SELECTED(config.oconIdTrigger))
		{
			refIdOut = "[" + jvx_size2String(config.oconIdTrigger) + "]";
		}
		out << "-->" << "<" << "TRIGGER" << ">#" << config.nmMaster << " + " << config.oconNmSource << refIdOut << std::endl;

		printList(out, segId, config.connectedNodes);

		if (JVX_CHECK_SIZE_SELECTED(config.iconIdTrigger))
		{
			refIdIn = "[" + jvx_size2String(config.iconIdTrigger) + "]";
		}
		out << "-->" << jvxComponentIdentification_txt(config.tpAssign) << "+" << config.iconNmSink << refIdIn << "*||" << std::flush;
	}

	void
		CayfAutomationModulesSrc2Snk::printList(std::ostream& out, jvxSize& segId, std::list<ayfConnectConfigCpEntry>& lst)
	{
		std::string refIdIn;
		std::string refIdOut;
		for (auto elm : lst)
		{
			refIdIn.clear();
			refIdOut.clear();
			if (elm.assSegmentId == segId)
			{
				if (JVX_CHECK_SIZE_SELECTED(elm.idIconRefTriggerConnector))
				{
					refIdIn = "[" + jvx_size2String(elm.idIconRefTriggerConnector) + "]";
				}
				if (JVX_CHECK_SIZE_SELECTED(elm.idOconRefTriggerConnector))
				{
					refIdIn = "[" + jvx_size2String(elm.idOconRefTriggerConnector) + "]";
				}
				out << "\t--><" << elm.modName << ">+" << elm.iconNm << refIdIn << "*|--|*" << "<" << elm.modName << ">+" << elm.oconNm << refIdOut << std::endl;
			}
		}
	}

	void 
		CayfAutomationModulesSrc2Snk::postponed_try_connect()
	{	
		if (config.allowLateConnect)
		{
			for (auto& elm : module_connections)
			{
				IayfEstablishedProcessesCommon* sglElmPtr = elm.second;
				IayfEstablishedProcessesSrc2Snk& sglElm = castEstablishProcess<IayfEstablishedProcessesSrc2Snk>(sglElmPtr);
				// Here we copy the args for the connection
			
				if (!sglElm.established)
				{
					deriveArguments(sglElm.derived, elm.first.cpId, sglElmPtr);

					// Run the connection code				
					try_connect(elm.first.cpId, sglElm.established);
				}
			}
		}
	}

	IayfEstablishedProcessesCommon* 
		CayfAutomationModulesSrc2Snk::allocate_chain_realization()
	{
		CayfEstablishedProcessesSrc2Snk* newChainRealization = nullptr;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(newChainRealization, CayfEstablishedProcessesSrc2Snk);
		return newChainRealization;
	}

	void CayfAutomationModulesSrc2Snk::deallocate_chain_realization(IayfEstablishedProcessesCommon* deallocMe) 
	{
		JVX_SAFE_DELETE_OBJECT(deallocMe);
	}

	jvxErrorType 
	CayfAutomationModulesSrc2Snk::pre_run_chain_prepare(IjvxObject* obj_dev, IayfEstablishedProcessesCommon* sglElmPtr)
	{
		jvxErrorType res = JVX_NO_ERROR;
		
		// ========================================================================
		IayfEstablishedProcessesSrc2Snk& sglElm = castEstablishProcess<IayfEstablishedProcessesSrc2Snk>(sglElmPtr);
		for (auto& elmM : config.connectedNodes)
		{
			ayfConnectConfigCpEntryRuntime cpElm(elmM);

			cpElm.cpId = cpElm.cpTp;
			res = jvx_activateObjectInModule(refHostRefPtr, cpElm.cpId, cpElm.modName, obj_dev, true, cpElm.cpManipulate.manSuffix, cpElm.cpManipulate.attachUi, cpElm.cpManipulate.tpRemap);

			if (res == JVX_NO_ERROR)
			{
				JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
				log << "Activated module <" << cpElm.modName << "> with suffix <" << cpElm.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(cpElm.cpId) << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr, JVX_CREATE_CODE_LOCATION_TAG);

				sglElm.lstEntries.push_back(cpElm);
			}
			else
			{
				JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
				log << "Failed to activate module <" << cpElm.modName << "> with suffix <" << cpElm.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(cpElm.cpId) << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr, JVX_CREATE_CODE_LOCATION_TAG);
				break;
			}
		}
		return res;
	};

	void 
		CayfAutomationModulesSrc2Snk::pre_run_chain_connect(
			jvxComponentIdentification tp_reg,
			IjvxDataConnections* con,
			IayfEstablishedProcessesCommon* sglElmPtr)
	{
		// Do nothing here!!
	}

	jvxErrorType
	CayfAutomationModulesSrc2Snk::post_run_chain_prepare(IayfEstablishedProcessesCommon* sglElmPtr)
	{
		jvxErrorType res = JVX_NO_ERROR;
		
		IayfEstablishedProcessesSrc2Snk& sglElm = castEstablishProcess<IayfEstablishedProcessesSrc2Snk>(sglElmPtr);
		for (auto elmI : sglElm.lstEntries)
		{
			JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT, JVX_CREATE_CODE_LOCATION_TAG, "");
			log << "Deactivating  module <" << elmI.modName << "> with suffix <" << elmI.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(elmI.cpId) << ">." << std::endl;
			JVX_STOP_LOCK_LOG_REF(objLogRefPtr, JVX_CREATE_CODE_LOCATION_TAG);

			jvxErrorType res = jvx_deactivateObjectInModule(refHostRefPtr, elmI.cpId);
			assert(res == JVX_NO_ERROR);
		}
		sglElm.lstEntries.clear();
		return res;
	};

	void 
	CayfAutomationModulesSrc2Snk::report_connection_established(jvxSize proId, ayfEstablishedProcessType, IayfEstablishedProcessesCommon* realizeChainPtr)
	{		
	}

	void 
	CayfAutomationModulesSrc2Snk::report_to_be_disconnected(jvxSize uidProcess)
	{
		// We do not know where this process belongs to - pass it to all listeners!
		for (auto elm : module_connections)
		{
			this->report_to_be_disconnected(uidProcess, elm.second);
		}
	}
}
