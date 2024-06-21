#include "CayfConnection.h"

namespace AyfConnection
{
	CayfOneNode::CayfOneNode()
	{

	}

	CayfOneNode::~CayfOneNode()
	{
	}

	bool operator == (const CayfOneNode& lhs, const CayfOneNode& rhs)
	{
		return lhs.theObj == rhs.theObj;
	};


	// =============================================================================

	CayfConnection::CayfConnection()
	{
		
	}

	CayfConnection::~CayfConnection()
	{
		
	}

	jvxErrorType
		CayfConnection::provide_constraints(
			const std::string& nameProcessRef,
			const std::string& descriptionConnectionRef,
			const std::string& descriptorConnectionRef)
	{
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		if (_common_set_min.theState == JVX_STATE_INIT)
		{
			descriptionConnection = descriptionConnectionRef;
			descriptorConnection = descriptorConnectionRef;
			nameProcess = nameProcessRef;
			res = JVX_NO_ERROR;
		}
		return res;
	}

	jvxErrorType
		CayfConnection::reset_constraints()
	{
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		if (_common_set_min.theState == JVX_STATE_INIT)
		{
			descriptionConnection = "";
			descriptorConnection = "";
			nameProcess = "";
			res = JVX_NO_ERROR;
		}
		return res;
	}

	jvxErrorType
		CayfConnection::register_node(
			const std::string& modName, 
			IjvxObject* theObj, jvxBool activatedInSystem, 
			IjvxConfigProcessor* confProc, jvxConfigData* secConfig, jvxSize uid,
			const std::string& inConnName, const std::string& outConnName)
	{
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		if (_common_set_min.theState == JVX_STATE_INIT)
		{
			res = JVX_ERROR_DUPLICATE_ENTRY;
			auto elm = std::find(registeredNodes.begin(), registeredNodes.end(), theObj);
			//registeredNodes.find(theObj);
			if (elm == registeredNodes.end())
			{
				CayfOneNode newNode;
				newNode.modName = modName;
				newNode.theObj = theObj;
				newNode.uId = uid;
				theObj->request_specialization(nullptr, &newNode.cpTp, nullptr);
				theObj->state(&newNode.statOnInit);

				// Can be IjvxSimpleNode OR IjvxNode
				newNode.theNode = castFromObject<IjvxNode>(theObj);
				if (!newNode.theNode)
				{
					// Try the simple node - required for audio encoder and decoder
					newNode.theNode = castFromObject<IjvxSimpleNode>(theObj);
				}
				newNode.activatedInSystem = activatedInSystem;
				if (newNode.theNode)
				{
					registeredNodes.push_back(newNode);
					res = JVX_NO_ERROR;
				}
				else
				{
					res = JVX_ERROR_INVALID_FORMAT;
				}
			}
		}
		return res;
	}

	jvxErrorType
		CayfConnection::unregister_node(IjvxObject* theObj)
	{
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		if (_common_set_min.theState == JVX_STATE_INIT)
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
			auto elm = std::find(registeredNodes.begin(), registeredNodes.end(), theObj);
			// registeredNodes.find(theObj);
			if (elm != registeredNodes.end())
			{
				registeredNodes.erase(elm);
				res = JVX_NO_ERROR;
			}
		}
		return res;
	}

	// =================================================================
	jvxErrorType
		CayfConnection::register_node(const std::string& modName, jvxComponentType tp,
			jvxBool activateNode, jvxSize uid, IjvxReferenceSelector* decider,
			const std::string& inConnName, const std::string& outConnName )
	{
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		if (_common_set_min.theState == JVX_STATE_INIT)
		{
			res = JVX_ERROR_NOT_READY;
			if (_common_set_min.theHostRef)
			{
				IjvxHost* theHost = reqInterface<IjvxHost>(_common_set_min.theHostRef);
				IjvxToolsHost* theToolsHost = reqInterface<IjvxToolsHost>(_common_set_min.theHostRef);
				if (theToolsHost && theHost)
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
					jvxComponentIdentification cpTp = tp;
					jvxBool activatedInSystem = false;
					// We may activate the node prior to selection
					/*
					if (activateNode)
					{
						jvx_activateObjectInModule(theHost, cpTp, modName, _common_set_min.theOwner, true);
						refComp<IjvxNode> ptrStrNode = reqRefTool<IjvxNode>(theToolsHost, cpTp, 0, modName.c_str());
						if (ptrStrNode.objPtr)
						{
							res = register_node(modName, ptrStrNode.objPtr);
						}
						retInterface<IjvxToolsHost>(_common_set_min.theHostRef, theToolsHost);
					}
					else
					{*/

					if (activateNode)
					{
						// Here, try to activate new node.
						res = jvx_activateObjectInModule(theHost, cpTp, modName, _common_set_min.theOwner, false); // Number slots NOT extended
						if (res == JVX_NO_ERROR)
						{
							activatedInSystem = true;
						}
					}
					else
					{
						cpTp.slotid = JVX_SIZE_DONTCARE;
						res = JVX_NO_ERROR;
					}

					// Try to obtain access to active component					
					if (res == JVX_NO_ERROR)
					{
						res = JVX_ERROR_ELEMENT_NOT_FOUND;
						refComp<IjvxNode> ptrStrNode = reqRefTool<IjvxNode>(theToolsHost, cpTp, 0, modName.c_str(), JVX_STATE_ACTIVE, decider);
						if (ptrStrNode.objPtr)
						{
							res = register_node(modName, ptrStrNode.objPtr, activatedInSystem, nullptr, nullptr, uid);
						}						
					}
					retInterface<IjvxToolsHost>(_common_set_min.theHostRef, theToolsHost);					
				}
			}
		}
		return res;
	}

	jvxErrorType
		CayfConnection::unregister_nodes(const std::string& modName)
	{
		jvxErrorType res = JVX_ERROR_WRONG_STATE;
		if (_common_set_min.theState == JVX_STATE_INIT)
		{
			res = JVX_ERROR_NOT_READY;
			if (_common_set_min.theHostRef)
			{
				IjvxHost* theHost = reqInterface<IjvxHost>(_common_set_min.theHostRef);
				IjvxToolsHost* theToolsHost = reqInterface<IjvxToolsHost>(_common_set_min.theHostRef);
				if (theToolsHost)
				{
					res = JVX_ERROR_ELEMENT_NOT_FOUND;
					jvxBool contLoop = true;
					while (contLoop)
					{
						jvxBool foundOne = false;
						for (auto& elm : registeredNodes)
						{
							if (elm.modName == modName)
							{
								IjvxObject* oPtr = elm.theNode;
								jvxComponentIdentification cpTp = elm.cpTp;
								jvxBool activatedInSystem = elm.activatedInSystem;
								// This removes a node from the list and prevents loop continuation
								unregister_node(oPtr);
								refComp<IjvxNode> ptrStrNode;
								ptrStrNode.objPtr = oPtr;
								retRefTool<IjvxNode>(theToolsHost, ptrStrNode, cpTp.tp);
								foundOne = true;
								if (activatedInSystem)
								{
									jvx_deactivateObjectInModule(theHost, cpTp);
								}
								res = JVX_NO_ERROR;
								break;
							}
						}
						if (!foundOne)
						{
							contLoop = false;
						}
					}
					retInterface<IjvxToolsHost>(_common_set_min.theHostRef, theToolsHost);
				}
			}
		}
		return res;
	}

	// ========================================================================

	jvxErrorType
		CayfConnection::init_connect(const CayfConnectionConfig& cfgArg,
			IayfConnectionStateSwitchNode* cbSs , jvxSize idConnDependsOn, jvxBool fwdTestChain)
	{
		cfg = cfgArg;
		ssCb = cbSs;
		idProcDepends = idConnDependsOn;
		fwdTestToDepend = fwdTestChain;
		jvxErrorType res = JVX_NO_ERROR;

		res = initialize(cfg.hostRef);
		if (res != JVX_NO_ERROR)
		{
			goto exit_fail;
		}

		res = provide_constraints(cfgArg.nmProcess, cfgArg.descrProcess, cfgArg.descrorProcess);
		if (res != JVX_NO_ERROR)
		{
			goto exit_fail;
		}

		// Run this loop for all already active components
		for (auto elm : cfg.modSpecLinear)
		{
			// Handle all active components in the system
			if (elm.tp != JVX_COMPONENT_UNKNOWN)
			{
				if (!elm.tryActivateComponent)
				{
					res = register_node(elm.modName, elm.tp, elm.tryActivateComponent, elm.uId, elm.decider); // cfg.activateNode);
					if (res != JVX_NO_ERROR)
					{
						std::cout
							<< __FUNCTION__ << ": Failed to register module name <" << elm.modName
							<< ">, error code = <" << jvxErrorType_descr(res) << ">." << std::endl;
					}
				}
			}
			else if (elm.theObj)
			{
				res = register_node(elm.modName, elm.theObj);
			}
			if (res != JVX_NO_ERROR)
			{
				goto exit_fail;
			}
		}

		// Now the not-active components
		for (auto elm : cfg.modSpecLinear)
		{
			if (elm.tp != JVX_COMPONENT_UNKNOWN)
			{
				if (elm.tryActivateComponent)
				{
					res = register_node(elm.modName, elm.tp, elm.tryActivateComponent); // cfg.activateNode);
					if (res != JVX_NO_ERROR)
					{
						std::cout
							<< __FUNCTION__ << ": Failed to register module name <" << elm.modName
							<< ">, error code = <" << jvxErrorType_descr(res) << ">." << std::endl;
					}
				}
			}
			if (res != JVX_NO_ERROR)
			{
				goto exit_fail;
			}
		}

		res = select(cfg.owner);
		if (res != JVX_NO_ERROR)
		{
			goto exit_fail;
		}

		res = activate();
		if (res != JVX_NO_ERROR)
		{
			goto exit_fail;
		}

		return res;

	exit_fail:
		disconnect_terminate();
		return res;
	}


	jvxErrorType
		CayfConnection::disconnect_terminate()
	{
		jvxErrorType res = deactivate();

		res = unselect();
		for (auto elm : cfg.modSpecLinear)
		{
			if (elm.tp != JVX_COMPONENT_UNKNOWN)
			{
				res = unregister_nodes(elm.modName);
			}
			else if (elm.theObj)
			{
				res = unregister_node(elm.theObj);
			}
		}
		res = reset_constraints();
		res = terminate();
		return JVX_NO_ERROR;
	}


	jvxErrorType
		CayfConnection::state(jvxState* stat)
	{
		return _state(stat);
	}

	jvxErrorType
		CayfConnection::test_connection(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
	{
		jvxErrorType res = JVX_ERROR_NOT_READY;
		if (microConn)
		{
			res = microConn->test_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			if (res == JVX_NO_ERROR)
			{
				// Here we may copy the output parameters if the HjvxMicroConnection_hooks_simple* is nullptr
			}
			test_run_complete(res);
		}
		return res;
	}

	jvxErrorType
		CayfConnection::negotiate_connection(jvxLinkDataTransferType tp, jvxHandle* data,
			jvxLinkDataDescriptor_con_params& ldat
			JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		jvxErrorType res = JVX_ERROR_NOT_READY;
		if (microConn)
		{
			res = microConn->transfer_backward_connection(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			if (res == JVX_ERROR_COMPROMISE)
			{
				// What to do here if we have no 
				// this->outConnect.con_params = _common_set_lds

			}
		}
		return res;
	}

	jvxErrorType
		CayfConnection::transfer_forward_connection(jvxLinkDataTransferType tp, jvxHandle* data
			JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		jvxErrorType res = JVX_ERROR_NOT_READY;
		if (microConn)
		{
			res = microConn->transfer_forward_connection(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		return res;
	}

	jvxErrorType 
		CayfConnection::prepare_start()
	{
		jvxErrorType res = prepare_connection();
		if (res == JVX_NO_ERROR)
		{
			res = start_connection();
		}
		return res;
	}

	jvxErrorType 
		CayfConnection::stop_postprocess()
	{
		jvxErrorType res = stop_connection();
		if (res == JVX_NO_ERROR)
		{
			res = postprocess_connection();
		}
		return res;
	}

	jvxErrorType
		CayfConnection::prepare_connection()
	{
		jvxErrorType res = JVX_ERROR_NOT_READY;
		if(_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			if (microConn)
			{
				if (cfg.connectionMode == ayfConnectionOperationMode::AYF_CONNECTION_FLEXIBLE)
				{
					cfg.anchorIn->con_data.number_buffers = cfg.numBuffers;
				}
				res = microConn->prepare_connection(false, false, cfg.copyAttachedData);
				if (res == JVX_NO_ERROR)
				{
					_common_set_min.theState = JVX_STATE_PREPARED;
				}
			}
		}
		return res;
	}

	jvxErrorType
		CayfConnection::postprocess_connection()
	{
		jvxErrorType res = JVX_ERROR_NOT_READY;
		if (_common_set_min.theState == JVX_STATE_PREPARED)
		{
			if (microConn)
			{
				res = microConn->postprocess_connection();
				if (res == JVX_NO_ERROR)
				{
					_common_set_min.theState = JVX_STATE_ACTIVE;
				}
			}
		}
		return res;
	}

	jvxErrorType
		CayfConnection::start_connection()
	{
		jvxErrorType res = JVX_ERROR_NOT_READY;
		if (_common_set_min.theState == JVX_STATE_PREPARED)
		{
			if (microConn)
			{
				res = microConn->start_connection();
				if (res == JVX_NO_ERROR)
				{
					_common_set_min.theState = JVX_STATE_PROCESSING;
				}
			}
		}
		return res;
	}

	jvxErrorType
		CayfConnection::stop_connection()
	{
		jvxErrorType res = JVX_ERROR_NOT_READY;
		if (_common_set_min.theState == JVX_STATE_PROCESSING)
		{
			if (microConn)
			{
				res = microConn->stop_connection();
				if (res == JVX_NO_ERROR)
				{
					_common_set_min.theState = JVX_STATE_PREPARED;
				}
			}
		}
		return res;
	}

	// =================================================================================

	jvxErrorType
		CayfConnection::prepare_process_connection(
			jvxLinkDataDescriptor** lkDataOnReturn,
			jvxSize pipeline_offset,
			jvxSize* idx_stage,
			jvxSize tobeAccessedByStage,
			callback_process_start_in_lock cb,
			jvxHandle* priv_ptr)
	{
		jvxErrorType res = JVX_ERROR_NOT_READY;
		if (microConn)
		{
			res = microConn->prepare_process_connection(lkDataOnReturn,
				pipeline_offset,
				idx_stage,
				tobeAccessedByStage,
				cb,
				priv_ptr);
		}
		return res;
	}

	jvxErrorType
		CayfConnection::process_connection(jvxLinkDataDescriptor** lkDataOnReturn)
	{
		jvxErrorType res = JVX_ERROR_NOT_READY;
		if (microConn)
		{
			res = microConn->process_connection(lkDataOnReturn);
		}
		return res;
	}

	jvxErrorType
		CayfConnection::process_connection(
			jvxLinkDataDescriptor* lkDataIn,
			jvxSize mt_mask, jvxSize idx_stage,
			jvxLinkDataDescriptor* lkDataOut)
	{
		jvxSize i;
		// Run the renderer
		jvxLinkDataDescriptor* ptr = nullptr;

		// Run the chain to prepare all buffers
		microConn->prepare_process_connection(&ptr);

		// Extract buffer pointers from input
		jvxHandle** bufsInput = jvx_process_icon_extract_input_buffers<jvxHandle>(
			lkDataIn, idx_stage);
		jvxSize nChans = lkDataIn->con_params.number_channels;
		jvxSize szBytes = jvxDataFormat_getsize(lkDataIn->con_params.format) * lkDataIn->con_params.buffersize;
		// 
		// Extract buffer pointer to fill in data to be processed by the chain
		jvxHandle** bufsToProcess = jvx_process_icon_extract_input_buffers<jvxHandle>(ptr, *ptr->con_pipeline.idx_stage_ptr);

		for (i = 0; i < nChans; i++)
		{
			// Fill the buffer
			memcpy(bufsToProcess[i], bufsInput[i], szBytes);
		}

		// Actually process the data into the output buffers. Output buffers available on return
		microConn->process_connection(&ptr);

		assert(lkDataOut);

		jvxHandle** bufsOutput = jvx_process_icon_extract_output_buffers<jvxHandle>(*lkDataOut);
		nChans = lkDataOut->con_params.number_channels;
		szBytes = jvxDataFormat_getsize(lkDataOut->con_params.format) * lkDataOut->con_params.buffersize;

		// Extract raw pointer
		jvxData** bufsProcessed = jvx_process_icon_extract_input_buffers<jvxData>(ptr,
			*ptr->con_pipeline.idx_stage_ptr);

		// Complete processing
		microConn->postprocess_process_connection();

		for (i = 0; i < nChans; i++)
		{
			memcpy(bufsOutput[i], bufsProcessed[i], szBytes);
		}
		return JVX_NO_ERROR;
	}

	jvxErrorType
		CayfConnection::postprocess_process_connection(
			jvxSize idx_stage,
			jvxBool operate_first_call,
			jvxSize tobeAccessedByStage,
			callback_process_stop_in_lock cb,
			jvxHandle* priv_ptr)
	{
		jvxErrorType res = JVX_ERROR_NOT_READY;
		if (microConn)
		{
			res = microConn->postprocess_process_connection(idx_stage,
				operate_first_call,
				tobeAccessedByStage,
				cb,
				priv_ptr);
		}
		return res;
	}

	// ========================================================================

	jvxErrorType
		CayfConnection::initialize(IjvxHiddenInterface* hostRef)
	{
		return CjvxObjectMin::_initialize(hostRef);
	}


	jvxErrorType
		CayfConnection::terminate()
	{
		assert(registeredNodes.empty());
		return CjvxObjectMin::_terminate();
	}

	// ========================================================================

	jvxErrorType
		CayfConnection::select(IjvxObject* owner)
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = CjvxObjectMin::_select(owner);
		if (res == JVX_NO_ERROR)
		{
			for (auto& elm : registeredNodes)
			{
				if (elm.statOnInit < JVX_STATE_INIT)
				{
					res = elm.theNode->initialize(_common_set_min.theHostRef);					
				}
				if (res == JVX_NO_ERROR)
				{
					if (elm.statOnInit < JVX_STATE_SELECTED)
					{
						res = JVX_ERROR_UNSUPPORTED;
						if (ssCb)
						{
							res = ssCb->runStateSwitch(JVX_STATE_SWITCH_SELECT, elm.theNode, elm.modName.c_str(), _common_set_min.theOwner);
						}
						if(res == JVX_ERROR_UNSUPPORTED)
						{
							res = elm.theNode->select(_common_set_min.theOwner);
						}
					}
				}
				if (res != JVX_NO_ERROR)
				{
					break;
				}
			}

			if (res != JVX_NO_ERROR)
			{
				goto exit_cleanup;
			}
		}
		goto exit_suc;
	exit_cleanup:
		unselect();
	exit_suc:
		return res;
	}

	jvxErrorType
		CayfConnection::unselect()
	{
		jvxErrorType res = CjvxObjectMin::_pre_check_unselect();
		if (_common_set_min.theState == JVX_STATE_SELECTED)
		{
			for (auto elm : registeredNodes)
			{
				if (elm.statOnInit < JVX_STATE_SELECTED)
				{
					res = JVX_ERROR_UNSUPPORTED;
					if (ssCb)
					{
						res = ssCb->runStateSwitch(JVX_STATE_SWITCH_UNSELECT, elm.theNode, elm.modName.c_str());
					}
					if(res == JVX_ERROR_UNSUPPORTED)
					{
						res = elm.theNode->unselect();
					}
				}
				if (elm.statOnInit < JVX_STATE_INIT)
				{
					elm.theNode->terminate();					
				}
			}
			res = JVX_NO_ERROR;
			CjvxObjectMin::_unselect();
		}
		return res;
	}

	// =================================================================

	jvxErrorType
		CayfConnection::activate()
	{
		jvxErrorType res = CjvxObjectMin::_activate();
		if (res == JVX_NO_ERROR)
		{
			for (auto elm : registeredNodes)
			{
				if (elm.statOnInit < JVX_STATE_ACTIVE)
				{
					res = JVX_ERROR_UNSUPPORTED;
					if (ssCb)
					{
						res = ssCb->runStateSwitch(JVX_STATE_SWITCH_ACTIVATE, elm.theNode, elm.modName.c_str());
					}
					if (res == JVX_ERROR_UNSUPPORTED)
					{
						res = elm.theNode->activate();
					}
				}
				if (res != JVX_NO_ERROR)
				{
					break;
				}
			}

			if (res == JVX_NO_ERROR)
			{
				res = create_micro_connection();
			}
		}

		if (res != JVX_NO_ERROR)
		{
			deactivate();
		}

		return res;
	}

	jvxErrorType
		CayfConnection::deactivate()
	{
		jvxErrorType res = CjvxObjectMin::_pre_check_deactivate();
		if (res == JVX_NO_ERROR)
		{
			res = destroy_micro_connection();
			res = JVX_NO_ERROR;

			for (auto elm : registeredNodes)
			{
				if (elm.statOnInit < JVX_STATE_ACTIVE)
				{
					jvxErrorType resL = JVX_ERROR_UNSUPPORTED;
					if (ssCb)
					{
						resL = ssCb->runStateSwitch(JVX_STATE_SWITCH_DEACTIVATE, elm.theNode, elm.modName.c_str());
					}
					if (resL == JVX_ERROR_UNSUPPORTED)
					{
						resL = elm.theNode->deactivate();
					}
				}
			}
			
			CjvxObjectMin::_deactivate();
		}
		return res;
	}

	// ===========================================================

jvxErrorType
CayfConnection::create_micro_connection()
{
	HjvxMicroConnection* locMicroConnection = nullptr;
	jvxErrorType res = JVX_NO_ERROR;
	

		// Allocate micro connection
		JVX_DSP_SAFE_ALLOCATE_OBJECT(
			locMicroConnection, HjvxMicroConnection(
				descriptionConnection.c_str(),
				false,
				descriptorConnection.c_str(),
				0,
				"Module Name <CayfConnection>",
				JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_AUDIO_DEVICE, "", NULL));

		locMicroConnection->set_location_info(jvxComponentIdentification(JVX_SIZE_SLOT_OFF_SYSTEM,
			JVX_SIZE_SLOT_OFF_SYSTEM, JVX_SIZE_UNSELECTED));

		IjvxHiddenInterface** ptrIf = nullptr;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrIf, IjvxHiddenInterface*, registeredNodes.size());

		const char** ptrIcon = nullptr;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrIcon, const char*, registeredNodes.size());

		const char** ptrOcon = nullptr;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrOcon, const char*, registeredNodes.size());

		jvxSize cnt = 0;
		for (auto& elm : registeredNodes)
		{
			res = JVX_ERROR_NOT_READY;
			if (elm.statOnInit < JVX_STATE_ACTIVE)
			{
				res = elm.theNode->activate();
			}

			jvxState curState = JVX_STATE_NONE;
			elm.theNode->state(&curState);

			if (curState == JVX_STATE_ACTIVE)
			{
				res = JVX_NO_ERROR;
			}

			if (res == JVX_NO_ERROR)
			{
				// node is ready
				ptrIf[cnt] = elm.theNode;

				ptrIcon[cnt] = elm.inputConnectorName.c_str();
				ptrOcon[cnt] = elm.outputConnectorName.c_str();
				cnt++;
			}
			else
			{
				break;
			}
		}

		// Inform that all components are active. We may set some properties at this position
		if (ssCb)
		{
			ssCb->componentsAboutToConnect();
		}

		if (res == JVX_NO_ERROR)
		{
			std::string nm = nameProcess;
			jvxSize cntProc = 1;
			while (1)
			{
				res = locMicroConnection->activate_connection(
					_common_set_min.theHostRef,
					ptrIf, ptrIcon, ptrOcon,
					registeredNodes.size(),
					"default", "default", nm, false, nullptr, 
					idProcDepends, fwdTestToDepend);
				if (res == JVX_NO_ERROR)
				{
					nameProcess = nm;
					break;
				}
				else if (res == JVX_ERROR_ALREADY_IN_USE)
				{
					if (cntProc < numAttemptsNewProcess)
					{
						nm = nameProcess + "(" + jvx_size2String(cntProc) + ")";
						cntProc++;
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
		}

		JVX_DSP_SAFE_DELETE_FIELD(ptrIf);
		JVX_DSP_SAFE_DELETE_FIELD(ptrIcon);
		JVX_DSP_SAFE_DELETE_FIELD(ptrOcon);
	

	// Connect
	if (res == JVX_NO_ERROR)
	{
		auto microConType = (cfg.connectionMode == ayfConnectionOperationMode::AYF_CONNECTION_EFFICIENT ? HjvxMicroConnection::jvxConnectionType::JVX_MICROCONNECTION_ENGAGE : 
			HjvxMicroConnection::jvxConnectionType::JVX_MICROCONNECTION_FLEXIBLE_INOUT_FIXED);
		switch (cfg.connectionMode)
		{
		case ayfConnectionOperationMode::AYF_CONNECTION_EFFICIENT:
			// Important: do not run the initial test! If doing so, default values will be used and test may fail!
			res = locMicroConnection->connect_connection(
				cfg.anchorIn, cfg.anchorOut, 
				microConType,
				cfg.bwdRefSpl, cfg.bwdRefFwd);
			break;
		case ayfConnectionOperationMode::AYF_CONNECTION_FLEXIBLE:

			res = locMicroConnection->connect_connection(cfg.anchorIn, cfg.anchorOut, microConType);
			break;
		default:
			assert(0);
		}
	}

	// Push it into processing
	microConn = locMicroConnection;

	return res;
}

jvxErrorType
CayfConnection::destroy_micro_connection()
{
	HjvxMicroConnection* locMicroConnection = microConn;

	// Pull it out of processing
	microConn = nullptr;

	if (locMicroConnection)
	{
		locMicroConnection->disconnect_connection();
		locMicroConnection->deactivate_connection();
		JVX_DSP_SAFE_DELETE_OBJECT(microConn);
	}
	return JVX_NO_ERROR;
}

jvxSize 
CayfConnection::numRegisteredObjects()
{
	return registeredNodes.size();
}

IjvxObject* 
CayfConnection::objectAtPosition(jvxSize idx)
{
	auto elm = registeredNodes.begin();
	std::advance(elm, idx);
	if (elm != registeredNodes.end())
	{
		return elm->theObj;
	}
	return nullptr;
}

void 
CayfConnection::test_run_complete(jvxErrorType lastResult)
{
	if (ssCb)
	{
		for (auto& elm : registeredNodes)
		{
			ssCb->runTestChainComplete(lastResult, elm.theNode, elm.modName.c_str(), elm.uId);
		}
	}
}

// ==============================================================

	/*
	jvxErrorType
		CayfSimpleConnection::request_hidden_interface(jvxInterfaceType, jvxHandle**)
	{
		return JVX_ERROR_UNSUPPORTED;
	}
	
	jvxErrorType
		CayfSimpleConnection::return_hidden_interface(jvxInterfaceType, jvxHandle*)
	{
		return JVX_ERROR_UNSUPPORTED;
	}

	jvxErrorType
		CayfSimpleConnection::object_hidden_interface(IjvxObject** objRef)
	{
		return JVX_ERROR_UNSUPPORTED;
	}
	*/
}