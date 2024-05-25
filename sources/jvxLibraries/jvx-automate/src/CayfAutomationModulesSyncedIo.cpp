#include "CayfAutomationModulesSyncedIo.h"
#include "common/CjvxObjectLog.h"

namespace CayfAutomationModules
{
#if 0
	jvxErrorType CayfAutomationModulesSyncedIo::activate(IjvxReport* report,
		IjvxHost* host,
		ayfAutoConnect_callbacks* cb,
		jvxSize purpId,
		const ayfConnectConfigSyncIo& cfgArg,
		jvxBool allowPostConnection, 
		jvxCBitField targetConnect, 
		CjvxObjectLog* ptrLog)
	{
		jvxErrorType res = CayfAutomationModulesCommon::activate(report, host, purpId, ptrLog);
		cbPtr = cb;	
		config = cfgArg;
		allowPostPonedConnect = allowPostConnection;
		targetFlagsConnection = targetConnect;
		return res;
	}
	jvxErrorType CayfAutomationModulesSyncedIo::deactivate()
	{
		jvxErrorType res = CayfAutomationModulesCommon::deactivate();
		// assert(module_connections.size() == 0);
		cbPtr = nullptr;
		return res;	
	}

	jvxErrorType CayfAutomationModulesSyncedIo::associate_process(jvxSize uIdProcess,
		const std::string& nmChain)
	{
		for (auto& elm : module_connections)
		{
			IayfEstablishedProcessesCommon* sglElmPtr = elm.second;
			
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
		CayfAutomationModulesSyncedIo::deassociate_process(jvxSize uIdProcess)
	{
		for (auto& elm : module_connections)
		{
			IayfEstablishedProcessesCommon* sglElmPtr = elm.second;
			for (auto elmP = sglElmPtr->connectedProcesses().begin(); elmP != sglElmPtr->connectedProcesses().end(); elmP++)
			{
				if (uIdProcess == elmP->processUid)
				{
					sglElmPtr->connectedProcesses().erase(elmP);
					return JVX_NO_ERROR;
				}
			}
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	jvxErrorType
		CayfAutomationModulesSyncedIo::activate_all_submodules(const jvxComponentIdentification& tp_activated)
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
			// Here we copy the args			
		}
		else
		{
			JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
			log << "Trying to activate all submodules for autoconnect purpose id <" << purposeId << "> component location <" << jvxComponentIdentification_txt(tp_activated) << ">: connection not allowed." << std::endl;
			JVX_STOP_LOCK_LOG_REF(objLogRefPtr);
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

		ayfConnectConfigCpEntrySyncIoRuntime cpElm(config);

		cpElm.cpId = cpElm.driveSupportNodeChain.cpTp;
		res = jvx_activateObjectInModule(refHostRefPtr, cpElm.cpId, cpElm.driveSupportNodeChain.modName, obj_dev, true, cpElm.driveSupportNodeChain.cpManipulate.manSuffix, 
			cpElm.driveSupportNodeChain.cpManipulate.attachUi, cpElm.driveSupportNodeChain.cpManipulate.tpRemap);

		if (res == JVX_NO_ERROR)
		{
			JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
			log << "Activated module <" << cpElm.driveSupportNodeChain.modName << "> with suffix <" << cpElm.driveSupportNodeChain.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(cpElm.cpId) << ">." << std::endl;
			JVX_STOP_LOCK_LOG_REF(objLogRefPtr);			
		}
		else
		{
			JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
			log << "Failed to activate module <" << cpElm.driveSupportNodeChain.modName << "> with suffix <" << cpElm.driveSupportNodeChain.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(cpElm.cpId) << ">." << std::endl;
			JVX_STOP_LOCK_LOG_REF(objLogRefPtr);
		}

		if (res == JVX_NO_ERROR)
		{
			// New module has been activated!!
			jvxBool established = false;

			IayfEstablishedProcessesCommon* realizeChainPtr = this->allocate_chain_realization();
			IayfEstablishedProcessesSyncio& realizeChain = *(reinterpret_cast<IayfEstablishedProcessesSyncio*>(realizeChainPtr->specificType(CayfAutomationModules::ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SYNCIO)));
			realizeChain.preset(cpElm);

			realizeChain.supportNodeRuntime.states.subModulesActive = true;
			realizeChain.supportNodeRuntime.states.connectionsEstablishFlags = 0x0;
			realizeChain.supportNodeRuntime.derivedConfig.allowPostPonedConnect = this->allowPostPonedConnect;
			realizeChain.supportNodeRuntime.derivedConfig.targetFlagsConnection = this->targetFlagsConnection;
			module_connections[tp_activated] = realizeChainPtr;

			// We need to lock the rule to avoid that we re issue anything with this rule while running the rule!!
			lockOperation = true;
			try_connect(tp_activated, established);
			lockOperation = false;

			if (!established)
			{
				if (!allowPostPonedConnect)
				{
					// If we do not establish the connection we can deactivate the added objects
					module_connections.erase(tp_activated);
					this->deallocate_chain_realization(realizeChainPtr);
					res = JVX_ERROR_INVALID_SETTING;
				}
			}

			if (res != JVX_NO_ERROR)
			{
				// Here we end up in error case!
				JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
				log << "On error, deactivating  module <" << cpElm.driveSupportNodeChain.modName << "> with suffix <" << cpElm.driveSupportNodeChain.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(cpElm.cpId) << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(objLogRefPtr);
				jvxErrorType resL = jvx_deactivateObjectInModule(refHostRefPtr, cpElm.cpId);
			}
		}
		// Here we can do the important stuff!
		
		return res;
	}

	jvxErrorType
		CayfAutomationModulesSyncedIo::deactivate_all_submodules(const jvxComponentIdentification& tp_deactivated)
	{
		auto elm = module_connections.find(tp_deactivated);
		if (elm != module_connections.end())
		{
			IayfEstablishedProcessesCommon* sglElmPtr = elm->second;
			IayfEstablishedProcessesSyncio& sglElm = *(reinterpret_cast<IayfEstablishedProcessesSyncio*>(sglElmPtr->specificType(CayfAutomationModules::ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SYNCIO)));
			// Check that audio devices are no longer there!
			assert(sglElmPtr->connectedProcesses().size() == 0);

			JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
			log << "Deactivating  module <" << sglElm.supportNodeRuntime.driveSupportNodeChain.modName << "> with suffix <" <<
				sglElm.supportNodeRuntime.driveSupportNodeChain.cpManipulate.manSuffix << "> in location <" << jvxComponentIdentification_txt(sglElm.supportNodeRuntime.cpId) << ">." << std::endl;
			JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

			jvxErrorType res = jvx_deactivateObjectInModule(refHostRefPtr, sglElm.supportNodeRuntime.cpId);
			assert(res == JVX_NO_ERROR);
			module_connections.erase(elm);
			this->deallocate_chain_realization(sglElmPtr);
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	jvxErrorType
		CayfAutomationModulesSyncedIo::adapt_all_submodules(jvxSize uIdProc, const std::string& modName, const std::string& description,
			const jvxComponentIdentification& tpCp, jvxReportCommandRequest req)
	{
		jvxErrorType res = JVX_NO_ERROR;

		// Find the element which fits the processId
		auto elm = module_connections.begin();
		ayfOneConnectedProcess theProc;
		for (; elm != module_connections.end(); elm++)
		{
			IayfEstablishedProcessesCommon* sglElmPtr = elm->second;
			IayfEstablishedProcessesSyncio& sglElm = *(reinterpret_cast<IayfEstablishedProcessesSyncio*>(sglElmPtr->specificType(CayfAutomationModules::ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SYNCIO)));
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
	CayfAutomationModulesSyncedIo::try_connect(jvxComponentIdentification tp_reg, jvxBool& fullyEstablished)
	{
		// Here, the audio device has been activated.
		jvxSize proc_id = JVX_SIZE_UNSELECTED;
		jvxBool rep_global = false;
		jvxSize bridgeId = 0;

		fullyEstablished = false;
		jvxBool establishedLoc = false;

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
			IayfEstablishedProcessesSyncio& sglElm = *(reinterpret_cast<IayfEstablishedProcessesSyncio*>(sglElmPtr->specificType(CayfAutomationModules::ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SYNCIO)));

			// Check if this part must be connected!!

			// =================================================================================================================
			// Connect the primary chain
			// =================================================================================================================
			CayfAutomationModulesSyncedIoPrimaryMixIn::try_connect(con, tp_reg, sglElm.supportNodeRuntime, sglElmPtr->connectedProcesses(), *this, nullptr, nullptr);

			// =================================================================================================================
			// Connect the secondary chain
			// =================================================================================================================
			
			if (!jvx_bitTest(sglElm.supportNodeRuntime.states.connectionsEstablishFlags, 1))
			{
				jvxDataConnectionRuleParameters params(false, false, true, sglElm.supportNodeRuntime.driveTargetCompChain.misc.dbgOut, true);
				params.preventStoreConfig = true;

				std::string chainName = sglElm.supportNodeRuntime.driveTargetCompChain.chainNamePrefix + jvx_size2String(tp_reg.slotid) + "-" + jvx_size2String(tp_reg.slotsubid);
				res = con->create_connection_rule(chainName.c_str(), &theDataConnectionDefRule_id, &params, sglElm.supportNodeRuntime.driveTargetCompChain.misc.connectionCategory);
				if (res == JVX_NO_ERROR)
				{
					jvxSize cnt = 0;
					std::string bridgename = "bridge_" + jvx_size2String(cnt);

					ayfOneConnectedProcess newProcess;
					// We store this as the target chain name that will be used for verification during connect

					JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
					log << "Created rule for chain <" << chainName << ">." << std::endl;
					JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

					IjvxDataConnectionRule* theDataConnectionDefRuleHdl = NULL;

					res = con->reference_connection_rule_uid(theDataConnectionDefRule_id, &theDataConnectionDefRuleHdl);
					if ((res == JVX_NO_ERROR) && (theDataConnectionDefRuleHdl))
					{
						//
						//
						// [SUPPORTNODE] -> [TARGET_NODE] -> [SUPPORTNODE]
						//
						//

						newProcess.chainName = chainName;
						newProcess.processUid = JVX_SIZE_UNSELECTED; // process not connected!
						sglElmPtr->connectedProcesses().push_back(newProcess);

						res = theDataConnectionDefRuleHdl->specify_master(sglElm.supportNodeRuntime.cpId,
							"*", sglElm.supportNodeRuntime.driveTargetCompChain.masterNm.c_str());
						assert(res == JVX_NO_ERROR);

						res = theDataConnectionDefRuleHdl->add_bridge_specification(
							sglElm.supportNodeRuntime.cpId,
							"*", sglElm.supportNodeRuntime.driveTargetCompChain.oconMasterNm.c_str(),
							config.cpTo.connectCp,
							"*", sglElm.supportNodeRuntime.driveTargetCompChain.iconNmConnect.c_str(),
							bridgename.c_str(), false, false, JVX_SIZE_UNSELECTED, config.cpTo.connectLinkId);
						assert(res == JVX_NO_ERROR);

						cnt++;
						bridgename = "bridge_" + jvx_size2String(cnt);

						res = theDataConnectionDefRuleHdl->add_bridge_specification(
							config.cpFrom.connectCp,
							"*", sglElm.supportNodeRuntime.driveTargetCompChain.oconNmConnect.c_str(),
							sglElm.supportNodeRuntime.cpId,
							"*", sglElm.supportNodeRuntime.driveTargetCompChain.iconMasterNm.c_str(),
							bridgename.c_str(), false, false, config.cpFrom.connectLinkId, JVX_SIZE_UNSELECTED);
						assert(res == JVX_NO_ERROR);
						theDataConnectionDefRuleHdl->mark_rule_default();

						cnt++;
						bridgename = "bridge_" + jvx_size2String(cnt);

						resC = theDataConnectionDefRuleHdl->try_connect_direct(
							con, refHostRefPtr,
							&establishedLoc,
							reportRefPtr,
							&proc_id,
							&rep_global);

						// If we manage to connect the chain, the bool "established" is true.
						// resC only observes general functionality. It should always be JVX_NO_ERROR!
						assert(resC == JVX_NO_ERROR);
						if (establishedLoc)
						{
							JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
							log << "Successfully connected chain <" << chainName << ">" << std::endl;
							JVX_STOP_LOCK_LOG_REF(objLogRefPtr);

							jvx_bitSet(sglElm.supportNodeRuntime.states.connectionsEstablishFlags, 1);
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

							JVX_START_LOCK_LOG_REF(objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
							log << "Failed to connect chain <" << chainName << ">, reason: " << jvxErrorType_descr(res) << "." << std::endl;
							JVX_STOP_LOCK_LOG_REF(objLogRefPtr);
						}
					}
					res = con->remove_connection_rule(theDataConnectionDefRule_id);
					theDataConnectionDefRule_id = JVX_SIZE_UNSELECTED;
				}
			}
			if (sglElm.supportNodeRuntime.states.connectionsEstablishFlags == sglElm.supportNodeRuntime.derivedConfig.targetFlagsConnection)
			{
				fullyEstablished = true;
			}
		}// if (con)

	} // CayfAutomationModulesSyncedIo::try_connect

	void
		CayfAutomationModulesSyncedIo::postponed_try_connect()
	{
		for (auto& elm : module_connections)
		{
			IayfEstablishedProcessesCommon* sglElmPtr = elm.second;
			IayfEstablishedProcessesSyncio& sglElm = *(reinterpret_cast<IayfEstablishedProcessesSyncio*>(sglElmPtr->specificType(CayfAutomationModules::ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SYNCIO)));
			if (sglElm.supportNodeRuntime.derivedConfig.allowPostPonedConnect)
			{
				jvxBool established = false;
				if (
					(sglElm.supportNodeRuntime.states.subModulesActive) &&
					(sglElm.supportNodeRuntime.states.connectionsEstablishFlags != sglElm.supportNodeRuntime.derivedConfig.targetFlagsConnection))
				{
					lockOperation = true;
					try_connect(elm.first.cpId, established);
					lockOperation = false;
				}
			}
		}
		std::cout << "Hier" << std::endl;
	}


	
	IayfEstablishedProcessesCommon* 
		CayfAutomationModulesSyncedIo::allocate_chain_realization()
	{
		CayfEstablishedProcessesSyncio* newChainRealization = nullptr;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(newChainRealization, CayfEstablishedProcessesSyncio);
		return newChainRealization;
	}
	
	void 
		CayfAutomationModulesSyncedIo::deallocate_chain_realization(IayfEstablishedProcessesCommon* deallocMe)
	{
		JVX_SAFE_DELETE_OBJECT(deallocMe);
	}
	jvxErrorType 
		CayfAutomationModulesSyncedIo::pre_run_chain_prepare(IjvxObject* obj_dev, IayfEstablishedProcessesCommon* realizeChain)
	{
		return JVX_NO_ERROR;
	}
	jvxErrorType
		CayfAutomationModulesSyncedIo::post_run_chain_prepare(IayfEstablishedProcessesCommon* realizeChain)
	{
		return JVX_NO_ERROR;
	}

	void 
		CayfAutomationModulesSyncedIo::pre_run_chain_connect(jvxComponentIdentification tp_reg,
			IjvxDataConnections* con, IayfEstablishedProcessesCommon* realizeChain)
	{
	}

	void 
		CayfAutomationModulesSyncedIo::report_to_be_disconnected(jvxSize uidProcess)
	{
	}
#endif
};