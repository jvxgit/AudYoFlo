#include "CayfAutomationModulesSyncedIo.h"
#include "common/CjvxObjectLog.h"

namespace CayfAutomationModules
{
	// ==================================================================================================
	// Check if this part must be connected!!
	void CayfAutomationModulesSyncedIoPrimaryMixIn::try_connect(
		IjvxDataConnections* con,
		jvxComponentIdentification tp_reg,
		ayfConnectConfigCpEntrySyncIoRuntime& supportNodeRuntime,
		std::list<ayfOneConnectedProcess>& connectedProcesses,
		CayfAutomationModulesCommon& modCommon, 
		CayfAutomationModuleReportConnection* report,
		IayfEstablishedProcessesCommon* realizeChainPtr)
	{
		jvxSize proc_id = JVX_SIZE_UNSELECTED;
		jvxBool rep_global = false;
		jvxSize bridgeId = 0;
		jvxBool establishedLoc = false;

		jvxErrorType res = JVX_NO_ERROR;
		jvxSize theDataConnectionDefRule_id = JVX_SIZE_UNSELECTED;

		if (!jvx_bitTest(supportNodeRuntime.states.connectionsEstablishFlags, 0))
		{
			jvxDataConnectionRuleParameters params(false, false, true, supportNodeRuntime.misc.dbgOut, true);
			params.preventStoreConfig = true;
			std::string chainName = CayfAutomationModules::CayfAutomationModulesHelper::chainPrefixToChain(supportNodeRuntime.chainNamePrefix, tp_reg);
				//jvx_size2String(tp_reg.slotid) + "-" + jvx_size2String(tp_reg.slotsubid);

			res = con->create_connection_rule(chainName.c_str(), &theDataConnectionDefRule_id,
				&params, supportNodeRuntime.misc.connectionCategory);
			if (res == JVX_NO_ERROR)
			{
				jvxSize cnt = 0;
				std::string bridgename = "bridge_" + jvx_size2String(cnt);

				ayfOneConnectedProcess newProcess;
				// We store this as the target chain name that will be used for verification during connect

				JVX_START_LOCK_LOG_REF(modCommon.objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
				log << "Created rule for chain <" << chainName << ">." << std::endl;
				JVX_STOP_LOCK_LOG_REF(modCommon.objLogRefPtr);

				IjvxDataConnectionRule* theDataConnectionDefRuleHdl = NULL;
				res = con->reference_connection_rule_uid(theDataConnectionDefRule_id, &theDataConnectionDefRuleHdl);
				if ((res == JVX_NO_ERROR) && (theDataConnectionDefRuleHdl))
				{
					//
					//
					// [NEW DEVICE] -> [SUPPORTNODE] -> [NEW DEVICE] 
					//
					//
					newProcess.chainName = chainName;
					newProcess.processUid = JVX_SIZE_UNSELECTED; // process not connected!
					connectedProcesses.push_back(newProcess);

					res = theDataConnectionDefRuleHdl->specify_master(tp_reg,
						"*", supportNodeRuntime.masterNm.c_str());
					assert(res == JVX_NO_ERROR);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						tp_reg,
						"*", supportNodeRuntime.oconMasterNm.c_str(),
						supportNodeRuntime.cpId,
						"*", supportNodeRuntime.iconNmConnect.c_str(), bridgename.c_str(), false, false);
					assert(res == JVX_NO_ERROR);

					cnt++;
					bridgename = "bridge_" + jvx_size2String(cnt);

					res = theDataConnectionDefRuleHdl->add_bridge_specification(
						supportNodeRuntime.cpId,
						"*", supportNodeRuntime.oconNmConnect.c_str(),
						tp_reg,
						"*", supportNodeRuntime.iconMasterNm.c_str(), bridgename.c_str(), false, false);
					assert(res == JVX_NO_ERROR);
					theDataConnectionDefRuleHdl->mark_rule_default();

					cnt++;
					bridgename = "bridge_" + jvx_size2String(cnt);

					jvxErrorType resC = theDataConnectionDefRuleHdl->try_connect_direct(
						con, modCommon.refHostRefPtr,
						&establishedLoc,
						modCommon.reportRefPtr,
						&proc_id,
						&rep_global);

					// If we manage to connect the chain, the bool "established" is true.
					// resC only observes general functionality. It should always be JVX_NO_ERROR!
					assert(resC == JVX_NO_ERROR);
					if (establishedLoc)
					{
						if (report)
						{
							report->report_connection_established(proc_id, ayfEstablishedProcessType::AYF_ESTABLISHED_PROCESS_SRC2SNKPRECHAINPRICHAIN, realizeChainPtr);
						}

						JVX_START_LOCK_LOG_REF(modCommon.objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
						log << "Successfully connected chain <" << chainName << ">" << std::endl;
						JVX_STOP_LOCK_LOG_REF(modCommon.objLogRefPtr);

						jvx_bitSet(supportNodeRuntime.states.connectionsEstablishFlags, 0);
					}
					else
					{
						// Remove the previously added process
						for (auto elmP = connectedProcesses.begin(); elmP != connectedProcesses.end(); elmP++)
						{
							if (elmP->chainName == chainName)
							{
								connectedProcesses.erase(elmP);
								break;
							}
						}

						JVX_START_LOCK_LOG_REF(modCommon.objLogRefPtr, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
						log << "Failed to connect chain <" << chainName << ">, reason: " << jvxErrorType_descr(res) << "." << std::endl;
						JVX_STOP_LOCK_LOG_REF(modCommon.objLogRefPtr);
					}
				}
				res = con->remove_connection_rule(theDataConnectionDefRule_id);
				theDataConnectionDefRule_id = JVX_SIZE_UNSELECTED;
			}
		}
	}
};
