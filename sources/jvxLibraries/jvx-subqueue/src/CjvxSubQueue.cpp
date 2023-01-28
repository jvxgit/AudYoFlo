#include "CjvxSubQueue.h"

CjvxSubQueue::CjvxSubQueue(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1io_rearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	/*
	 * Configurations:
	 * 
	 * Enable zero copy
	_common_set_ldslave.zeroCopyBuffering_cfg = true;

	 * Automatic prepare
	_common_set_node_base_1io.prepareOnChainPrepare = true;
	_common_set_node_base_1io.startOnChainStart = true;
	_common_set_node_base_1io.impPrepareOnChainPrepare = true;
	_common_set_node_base_1io.impStartOnChainStart = true;

	 * Default values for data parameters
	_common_set_1io_zerocopy.num_iochans = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.bsize_io = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.rate_io = JVX_SIZE_UNSELECTED;
	_common_set_1io_zerocopy.form_io = JVX_DATAFORMAT_NONE;
	_common_set_1io_zerocopy.subform_io = JVX_DATAFORMAT_GROUP_NONE;
	
	 * Complaints about data prameters are directly forwarded
	forward_complain = true;
	 */

	// Prevent that output parameters are taken from input parameter
	outputArgsFromInputOnInit = false;
}

CjvxSubQueue::~CjvxSubQueue()
{	
} 

#if 0
jvxErrorType
CjvxSpNSpeaker2Binaural::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxBareNode1io_zerocopy::select(owner);
	if (res == JVX_NO_ERROR)
	{
		/*
		 * Activate all properties
		 */
		genSpeaker2Binaural_node::init_all();
		genSpeaker2Binaural_node::allocate_all();
		genSpeaker2Binaural_node::register_all(static_cast<CjvxProperties*>(this));
		genSpeaker2Binaural_node::register_callbacks(static_cast<CjvxProperties*>(this),
			set_bypass_mode,
			select_hrtf_file,
			set_speaker_posthook,
			set_parameter_speaker,
			get_parameter_speaker,
			static_cast<jvxHandle*>(this), NULL);

		update_bypass_mode();
	}
	return res;
}

jvxErrorType
CjvxSpNSpeaker2Binaural::unselect()
{
	jvxErrorType res = CjvxBareNode1io_zerocopy::unselect();
	if (res == JVX_NO_ERROR)
	{
		genSpeaker2Binaural_node::unregister_callbacks(static_cast<CjvxProperties*>(this), NULL);
		genSpeaker2Binaural_node::unregister_all(static_cast<CjvxProperties*>(this));
		genSpeaker2Binaural_node::deallocate_all();
	}
	return res;
}

#endif

jvxErrorType
CjvxSubQueue::activate()
{
	jvxErrorType res = CjvxBareNode1io_rearrange::activate();
	if (res == JVX_NO_ERROR)
	{
		CjvxSubQueue_pcg::init_all();
		CjvxSubQueue_pcg::allocate_all();
		CjvxSubQueue_pcg::register_all(static_cast<CjvxProperties*>(this));

		theMicroConnection = new HjvxMicroConnection(
			"Micro Connection Decoder",
			false,
			"Micro Connection Decoder",
			0, "local-temp-lane",
			JVX_COMPONENT_ACCESS_SUB_COMPONENT,
			JVX_COMPONENT_CUSTOM_DEVICE, "", NULL);
		theMicroConnection->set_location_info(jvxComponentIdentification(JVX_SIZE_SLOT_OFF_SYSTEM,
			JVX_SIZE_SLOT_OFF_SYSTEM, JVX_SIZE_UNSELECTED));

		// Get a reference to the config processor. 
		refCfgParser = reqRefTool<IjvxConfigProcessor>(_common_set.theToolsHost, JVX_COMPONENT_CONFIG_PROCESSOR);
	}
	return res;
}

jvxErrorType
CjvxSubQueue::deactivate()
{	
	jvxErrorType res = CjvxBareNode1io_rearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{		
		 retRefTool<IjvxConfigProcessor>(_common_set.theToolsHost, JVX_COMPONENT_CONFIG_PROCESSOR, refCfgParser);

		// Delete the microconnection
		delete theMicroConnection;
		theMicroConnection = nullptr;

		CjvxSubQueue_pcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxSubQueue_pcg::deallocate_all();

		CjvxBareNode1io_rearrange::deactivate();
	}
	return res;
}


jvxErrorType
CjvxSubQueue::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	
	jvxErrorType res = CjvxBareNode1io_rearrange::disconnect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		clear_all_involved_references();
	}
	return res;
}

void
CjvxSubQueue::update_config_tables()
{
	jvxErrorType res = ctrlSubQueue->lock_queue();
	assert(res == JVX_NO_ERROR);

	std::map<std::string, std::string> ll;
	for (auto elm : currentMode.ptrHis)
	{
		jvxCallManagerConfiguration callMan;
		jvxApiString astr;
		res = ctrlSubQueue->get_configuration(elm.ptrI, &callMan, refCfgParser.cpPtr, &astr);
		assert(res == JVX_NO_ERROR);

		std::string cfgToken = astr.c_str();
		std::string nmI = elm.nmI;

		ll[nmI] = cfgToken;
	}

	// Add to list OR create new element
	auto elmL = theModes.find(currentMode.modeName);
	if (elmL != theModes.end())
	{
		elmL->second = ll;
	}
	else
	{
		theModes[currentMode.modeName] = ll;
	}

	res = ctrlSubQueue->unlock_queue(CjvxSubQueue_interact::jvxSubQueueOperationState::JVX_SUB_QUEUE_OPERATION_STATE_NO_UPDATE);

	assert(res == JVX_NO_ERROR);
}


void
CjvxSubQueue::clear_all_involved_references()
{
	CjvxSubQueue_interact::jvxSubQueueOperationState curStat = CjvxSubQueue_interact::jvxSubQueueOperationState::JVX_SUB_QUEUE_OPERATION_STATE_ERROR;
	jvxErrorType res = JVX_NO_ERROR;

	// Close any existing connection
	if (ctrlSubQueue)
	{
		curStat = ctrlSubQueue->queue_setup_status();		
	}

	if (curStat == CjvxSubQueue_interact::jvxSubQueueOperationState::JVX_SUB_QUEUE_OPERATION_STATE_READY)
	{
		update_config_tables();

		res = theMicroConnection->disconnect_connection();
		theMicroConnection->deactivate_connection();
	}

	if (ctrlSubQueue)
	{
		
		res = ctrlSubQueue->lock_queue();
		assert(res == JVX_NO_ERROR);

		// Clear out all prop references
		for (auto& elm : currentMode.ptrHis)
		{
			if (elm.ptrP)
			{
				CjvxProperties::_unregister_sub_module(elm.ptrP);				
			}
			elm.ptrP = nullptr;
		}


		for (auto elm : currentMode.ptrHis)
		{
			jvxCallManagerConfiguration callMan;
			jvxApiString astr;
			res = ctrlSubQueue->release_component(elm.ptrI);
			assert(res == JVX_NO_ERROR);
		}

		currentMode.ptrHis.clear();
		
		res = ctrlSubQueue->unlock_queue(CjvxSubQueue_interact::jvxSubQueueOperationState::JVX_SUB_QUEUE_OPERATION_STATE_NOT_READY);
		assert(res == JVX_NO_ERROR);
	}
}

jvxErrorType
CjvxSubQueue::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	CjvxSubQueue_interact::jvxSubQueueOperationState reqNewConn = CjvxSubQueue_interact::jvxSubQueueOperationState::JVX_SUB_QUEUE_OPERATION_STATE_ERROR;
	jvxSize i;
	jvxApiString modeName;
	jvxErrorType res = JVX_NO_ERROR;

	// Check current status
	if (ctrlSubQueue)
	{
		reqNewConn = ctrlSubQueue->queue_setup_status();
	}

	// If the queue status is not_ready allocate queue involved nodes
	if (reqNewConn == CjvxSubQueue_interact::jvxSubQueueOperationState::JVX_SUB_QUEUE_OPERATION_STATE_NOT_READY)
	{
		// There should be no references
		assert(currentMode.ptrHis.empty());

		IjvxDataConnectionProcess* theProc = nullptr;
		jvxSize idProc = JVX_SIZE_UNSELECTED;
		if (_common_set_ldslave.theData_in->con_link.master)
		{
			_common_set_ldslave.theData_in->con_link.master->associated_process(&theProc);
			if (theProc)
			{
				theProc->unique_id_connections(&idProc);
			}
		}

		// Start setup
		ctrlSubQueue->lock_queue(&modeName);

		currentMode.modeName = modeName.c_str();
		jvxSize numNew = 0;
		jvxApiStringList strLst;
		numNew = ctrlSubQueue->number_involved_components();

		std::map<std::string, std::string> cfgsMode;
		auto elmCfg = theModes.find(currentMode.modeName);
		if (elmCfg != theModes.end())
		{
			cfgsMode = elmCfg->second;
		}

		if (numNew > 0)
		{
			jvxBool errorDetected = false;
			IjvxHiddenInterface** ptrHI = nullptr;			
			const char** iconns = nullptr;
			std::list<std::string> iconnsStr;
			const char** oconns = nullptr;
			std::list<std::string> oconnsStr;

			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrHI, IjvxHiddenInterface*, numNew);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(iconns, const char*, numNew);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(oconns, const char*, numNew);

			jvxSize uIdl = JVX_SIZE_UNSELECTED;

			// Get a unique id first
			_common_set.theUniqueId->obtain_unique_id(&uIdl, "My Involved Component ID");

			for (i = 0; i < numNew; i++)
			{
				IjvxHiddenInterface* hIPtr = nullptr;
				jvxApiString astrCp = "no-set";
				jvxApiString astrIConn = "default";
				jvxApiString astrOConn = "default";
				jvxComponentType cpTp = JVX_COMPONENT_UNKNOWN;
				res = ctrlSubQueue->name_component(i, astrCp);

				jvxCallManagerConfiguration callMan;
				// Obtain configuration string!!
				std::string cfgToken;

				auto elmInCfg = cfgsMode.find(astrCp.c_str());
				if (elmInCfg != cfgsMode.end())
				{
					cfgToken = elmInCfg->second;
				}

				if (!cfgToken.empty())
				{
					jvxErrorType resL = refCfgParser.cpPtr->parseTextField(cfgToken.c_str(), "tmp-str", 0);
					if (resL != JVX_NO_ERROR)
					{
						jvxApiError err;
						refCfgParser.cpPtr->getParseError(&err);
						std::cout << err.errorDescription.c_str() << " -- " << cfgToken << std::endl;
					}
					else
					{
						jvxConfigData* sec = nullptr;
						refCfgParser.cpPtr->getConfigurationHandle(&sec);

						res = ctrlSubQueue->allocate_component_with_connectors(i, hIPtr, astrIConn, astrOConn, cpTp,
							&callMan, refCfgParser.cpPtr, sec);

						refCfgParser.cpPtr->removeHandle(sec);
					}
				}

				// In case the previous call was skipped
				if(!hIPtr)
				{
						res = ctrlSubQueue->allocate_component_with_connectors(i, hIPtr, astrIConn, astrOConn, cpTp,
							&callMan, refCfgParser.cpPtr, nullptr);
				}

				if (res == JVX_NO_ERROR)
				{
					oneInvolvedComponent newElm;
					newElm.ptrI = hIPtr;
					newElm.nmI = astrCp.c_str();

					currentMode.ptrHis.push_back(newElm);
					iconnsStr.push_back(astrIConn.c_str());
					oconnsStr.push_back(astrOConn.c_str());
				}
				else
				{
					errorDetected = true;
				}
			}

			if (errorDetected)
			{
				res = JVX_ERROR_INVALID_SETTING;
				for (auto elm : currentMode.ptrHis)
				{
					ctrlSubQueue->release_component(elm.ptrI);
				}
				currentMode.ptrHis.clear();
			}
			else
			{
				auto elmHi = currentMode.ptrHis.begin();
				auto elmIc = iconnsStr.begin();
				auto elmOc = oconnsStr.begin();

				for (i = 0; i < numNew; i++)
				{
					ptrHI[i] = elmHi->ptrI;
					iconns[i] = elmIc->c_str();
					oconns[i] = elmOc->c_str();
					elmHi++;
					elmIc++;
					elmOc++;
				}

				theMicroConnection->activate_connection(
					_common_set_min.theHostRef,
					ptrHI, iconns, oconns, numNew, "default", "default",
					"subqueue#" + jvx_size2String(uIdl),
					false, nullptr, idProc);

				res = theMicroConnection->connect_connection(_common_set_ldslave.theData_in,
					&_common_set_ldslave.theData_out,
					static_cast<HjvxMicroConnection_hooks_simple*>(this),
					static_cast<HjvxMicroConnection_hooks_fwd*>(this),
					false);
			}
		}

		if (res == JVX_NO_ERROR)
		{
			jvxSize offProps = 1024;
			ctrlSubQueue->unlock_queue(CjvxSubQueue_interact::jvxSubQueueOperationState::JVX_SUB_QUEUE_OPERATION_STATE_READY);
			for (auto& elm : currentMode.ptrHis)
			{
				elm.ptrI->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&elm.ptrP));
				if (elm.ptrP)
				{
					std::string prefix = "nextPropRef";
					IjvxObject* theObj = nullptr;
					elm.ptrI->object_hidden_interface(&theObj);
					if (theObj)
					{
						jvxApiString astr;
						theObj->descriptor(&astr);
						prefix = astr.c_str();
						CjvxProperties::_register_sub_module(elm.ptrP, offProps, prefix, prefix);
						offProps += 1024;
					}
				}
			}

			CjvxReportCommandRequest new_command(
				jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_UPDATE_PROPERTIES,
				this->_common_set.theComponentType);
			_request_command(new_command);
			
		}
		else
		{
			ctrlSubQueue->unlock_queue(CjvxSubQueue_interact::jvxSubQueueOperationState::JVX_SUB_QUEUE_OPERATION_STATE_ERROR);
		}
	}

	// If a valid connection is given we may run the test function
	if (res == JVX_NO_ERROR)
	{
		res = theMicroConnection->test_connection(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res == JVX_NO_ERROR)
		{
			// Update the input parameter
			update_input_params_on_test();

			// Update the output parameter
			update_output_params_on_test();
		}
		if (res == JVX_ERROR_REQUEST_CALL_AGAIN)
		{
			res = this->test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}
	}
	return res;
}

jvxErrorType
CjvxSubQueue::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		// To do at this position
		return theMicroConnection->transfer_backward_connection(tp, data
			JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		break;

	case JVX_LINKDATA_TRANSFER_REQUEST_DATA:
		if (_common_set_ldslave.theData_in->con_link.connect_from)
		{
			res = _common_set_ldslave.theData_in->con_link.connect_from->
				transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		}
		break;
	default:
		res = CjvxBareNode1io_rearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

jvxErrorType
CjvxSubQueue::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	prepare_autostart();
	return theMicroConnection->prepare_connection(false, true);
}

jvxErrorType
CjvxSubQueue::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = theMicroConnection->postprocess_connection();
	postprocess_autostart();
	return res;
}

jvxErrorType
CjvxSubQueue::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	start_autostart();
	return theMicroConnection->start_connection();
}

jvxErrorType
CjvxSubQueue::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = theMicroConnection->stop_connection();
	stop_autostart();
	return res;
}

jvxErrorType
CjvxSubQueue::process_start_icon(
	jvxSize pipeline_offset,
	jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	return theMicroConnection->prepare_process_connection(nullptr,
		pipeline_offset,
		idx_stage,
		tobeAccessedByStage,
		clbk,
		priv_ptr);
}

jvxErrorType
CjvxSubQueue::process_stop_icon(
	jvxSize idx_stage,
	jvxBool operate_first_call,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock cb,
	jvxHandle* priv_ptr)
{
	return theMicroConnection->postprocess_process_connection(
		idx_stage,
		operate_first_call,
		tobeAccessedByStage,
		cb,
		priv_ptr);
}

jvxErrorType
CjvxSubQueue::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	// Run the decode microconnection
	return theMicroConnection->process_connection(nullptr);
}

jvxErrorType
CjvxSubQueue::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxConfigData* newSec = nullptr;
	jvxConfigData* newSubSec = nullptr;
	jvxConfigData* newSSubSec = nullptr;
	jvxConfigData* newSSSubSec = nullptr;
	jvxConfigData* newSSSSubSec = nullptr;

	jvxErrorType res = CjvxBareNode1io_rearrange::put_configuration(
		callMan,
		processor,
		sectionToContainAllSubsectionsForMe,
		filename,
		lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			std::vector<std::string> warns;
			CjvxSubQueue_pcg::put_configuration_all(
				callMan,
				processor,
				sectionToContainAllSubsectionsForMe,
				&warns);
			JVX_PROPERTY_PUT_CONFIGRATION_WARNINGS_STD_COUT(warns);

			theModes.clear();
			processor->getReferenceEntryCurrentSection_name(sectionToContainAllSubsectionsForMe, &newSec, "subqueue_elements");
			if (newSec)
			{
				jvxSize cnt = 0;
				while (1)
				{
					std::string tokenSec = "subqueue_element_" + jvx_size2String(cnt);
					newSubSec = nullptr;
					processor->getReferenceEntryCurrentSection_name(newSec, &newSubSec, tokenSec.c_str());
					if (newSubSec == nullptr)
					{
						break;
					}

					newSSubSec = nullptr;
					processor->getReferenceEntryCurrentSection_name(newSubSec, &newSSubSec, "mode");
					if (newSSubSec)
					{
						jvxApiString astr;
						std::string mode;
						processor->getAssignmentString(newSSubSec, &astr);
						mode = astr.c_str();
						if (!mode.empty())
						{
							newSSubSec = nullptr;
							processor->getReferenceEntryCurrentSection_name(newSubSec, &newSSubSec, "sub_elements");
							if (newSSubSec)
							{
								jvxSize cnti = 0;
								std::map<std::string, std::string> comps;
								while (1)
								{
									std::string token = "element_" + jvx_size2String(cnti);
									
									newSSSubSec = nullptr;
									processor->getReferenceEntryCurrentSection_name(newSSubSec, &newSSSubSec, token.c_str());
									if (newSSSubSec == nullptr)
									{
										break;
									}

									//
									std::string compName;
									std::string cfgToken;

									newSSSSubSec = nullptr;
									processor->getReferenceEntryCurrentSection_name(newSSSubSec, &newSSSSubSec, "comp_name");
									if (newSSSSubSec)
									{
										astr.clear();
										processor->getAssignmentString(newSSSSubSec, &astr);
										compName = astr.c_str();
									}

									newSSSSubSec = nullptr;
									processor->getReferenceEntryCurrentSection_name(newSSSubSec, &newSSSSubSec, "cfg_token");
									if (newSSSSubSec)
									{
										astr.clear();
										processor->getAssignmentString(newSSSSubSec, &astr);
										cfgToken = astr.c_str();
									}

									if (compName.size() && cfgToken.size())
									{
										comps[compName] = cfgToken;
									}
									cnti++;
								}

								if (comps.size())
								{
									theModes[mode] = comps;
									comps.clear();
								}
							}
						}
					}
					cnt++;
				}
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxSubQueue::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxSize cnt = 0;
	
	jvxConfigData* newSec = nullptr;
	jvxConfigData* newSubSec = nullptr;
	jvxConfigData* newSSubSec = nullptr;
	jvxConfigData* newSSSubSec = nullptr;
	jvxConfigData* newSSSSubSec = nullptr;


	jvxErrorType res = CjvxBareNode1io_rearrange::get_configuration(
		callMan,
		processor,
		sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		CjvxSubQueue_pcg::get_configuration_all(
			callMan,
			processor,
			sectionWhereToAddAllSubsections);

		update_config_tables();
		
		newSec = nullptr;
		processor->createEmptySection(&newSec, "subqueue_elements");
		if (newSec)
		{
			for (auto elm : theModes)
			{
				std::string tokenSec = "subqueue_element_" + jvx_size2String(cnt);
				cnt++;

				newSubSec = nullptr;
				processor->createEmptySection(&newSubSec, tokenSec.c_str());
				if (newSubSec)
				{
					newSSubSec = nullptr;
					processor->createAssignmentString(&newSSubSec, "mode", elm.first.c_str());
					processor->addSubsectionToSection(newSubSec, newSSubSec);
					
					newSSubSec = nullptr;
					processor->createEmptySection(&newSSubSec, "sub_elements");
					if (newSSubSec)
					{
						jvxSize cnti = 0;
						for (auto elmi : elm.second)
						{
							newSSSubSec = nullptr;
							processor->createEmptySection(&newSSSubSec, ("element_" + jvx_size2String(cnti)).c_str());

							if (newSSSubSec)
							{
								newSSSSubSec = nullptr;
								processor->createAssignmentString(&newSSSSubSec, "comp_name", elmi.first.c_str());
								processor->addSubsectionToSection(newSSSubSec, newSSSSubSec);

								newSSSSubSec = nullptr;
								processor->createAssignmentString(&newSSSSubSec, "cfg_token", elmi.second.c_str());
								processor->addSubsectionToSection(newSSSubSec, newSSSSubSec);

								processor->addSubsectionToSection(newSSubSec, newSSSubSec);
							}
							cnti++;
						}

						processor->addSubsectionToSection(newSubSec, newSSubSec);
					}

					processor->addSubsectionToSection(newSec, newSubSec);
				}
			}
			processor->addSubsectionToSection(sectionWhereToAddAllSubsections, newSec);
		}
	}
	return JVX_NO_ERROR;
}

#if 0

// =====================================================================================

#include "jvx-helpers-cpp.h"
jvxErrorType
CjvxSpNSpeaker2Binaural::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (!genSpeaker2Binaural_node::config.bypass.value)
	{
		assert(zeroCopyBuffering_rt == false);
		jvxData** bufsIn = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_ldslave.theData_in, idx_stage);
		jvxData** bufsOut = jvx_process_icon_extract_output_buffers<jvxData>(&_common_set_ldslave.theData_out);

		// Copy input to output. We need two buffers since we do not apply zero copy processing!
		if (engageRendererProc)
		{
			filtLeft.process_N1Filter(bufsIn, bufsOut[0], _common_set_ldslave.theData_in->con_params.number_channels,
				_common_set_ldslave.theData_in->con_params.buffersize);
			filtRight.process_N1Filter(bufsIn, bufsOut[1], _common_set_ldslave.theData_in->con_params.number_channels,
				_common_set_ldslave.theData_in->con_params.buffersize);
		}
	}
	else
	{
		// Nothing to do here, just bypass processing
	}
	

	// Forward the processed data to next chain element
	res = _process_buffers_icon(mt_mask, idx_stage);
	return res;
}

// =====================================================================================



// =====================================================================================

jvxErrorType
CjvxSpNSpeaker2Binaural::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxBareNode1io_zerocopy::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

void 
CjvxSpNSpeaker2Binaural::test_set_output_parameters()
{
	CjvxBareNode1io_zerocopy::test_set_output_parameters();

	if (force2OutputChannels)
	{
		neg_output._update_parameters_fixed(2, JVX_SIZE_UNSELECTED,
			JVX_SIZE_UNSELECTED, JVX_DATAFORMAT_NONE,
			JVX_DATAFORMAT_GROUP_NONE,
			&_common_set_ldslave.theData_out);
	}
}

// =====================================================================================

jvxErrorType 
CjvxSpNSpeaker2Binaural::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1io_zerocopy::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		engageRendererProc = false;
		if (genSpeaker2Binaural_node::config.bypass.value == false)
		{
			engageRendererProc = true;
		}

		if (engageRendererProc)
		{
			filtLeft.prepare_N1Filter(_common_set_ldslave.theData_in->con_params.buffersize);
			filtRight.prepare_N1Filter(_common_set_ldslave.theData_in->con_params.buffersize);
		}
	}
	return res;
}

jvxErrorType 
CjvxSpNSpeaker2Binaural::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1io_zerocopy::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		if (engageRendererProc)
		{
			filtLeft.postprocess_N1Filter();
			filtRight.postprocess_N1Filter();
		}
		engageRendererProc = false;
	}
	return res;
}

// =========================================================================================

jvxErrorType
CjvxSpNSpeaker2Binaural::transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	std::string conf_token;
	std::string conf_txt;
	jvxErrorType resP = JVX_ERROR_INVALID_FORMAT;

	jvxDataConnectionsTransferConfigToken* cfg = (jvxDataConnectionsTransferConfigToken*)data;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_CONFIG_TOKEN:
		conf_token = cfg->token_descr;
		conf_txt = cfg->token_content;

		if (conf_token == "binaural_render") 
		{
			if (genSpeaker2Binaural_node::mode.slave_mode.value)
			{
				resP = read_hrtf_settings(conf_token, conf_txt, "<passed-conf-token>");
				if (resP == JVX_NO_ERROR)
				{
					return resP;
				}
			}
			else
			{
				std::cout << __FUNCTION__ << ": Not accepting transferred configuration: Component is not in slave mode." << std::endl;
			}
		}
		break;
	}
	return CjvxBareNode1io_zerocopy::transfer_forward_icon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
}
#endif
