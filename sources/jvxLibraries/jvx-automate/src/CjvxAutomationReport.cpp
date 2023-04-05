#include "CjvxAutomationReport.h"

// Macro for activation of the 
#define JVX_SCHEDULER_POSTPONED_BY_DEFAULT

CjvxAutomationReport::CjvxAutomationReport()
{
	tp_rep_activate_filter.push_back(JVX_COMPONENT_SIGNAL_PROCESSING_TECHNOLOGY);
}

CjvxAutomationReport::~CjvxAutomationReport()
{
	tp_rep_activate_filter.clear();
}

void
CjvxAutomationReport::set_system_references(IjvxReportSystem* reportArg, IjvxHost* refHostRefArg)
{
	refSystemReport = reportArg;
	refHostRef = refHostRefArg;	
}

jvxErrorType
CjvxAutomationReport::request_command(const CjvxReportCommandRequest& request)
{

	jvxApiString ident;
	jvxSize uId = JVX_SIZE_UNSELECTED;
	jvxStateSwitch ss = JVX_STATE_SWITCH_NONE;

	const CjvxReportCommandRequest_id* subIf = NULL;
	const CjvxReportCommandRequest_uid* uidIf = NULL;
	const CjvxReportCommandRequest_ss* ssIf = NULL;
	jvxComponentIdentification tp = request.origin();
	CjvxAutomationReport::callSpecificParameters params;

	jvxReportCommandRequest req = request.request();
	//jvxReportCommandDataType dtp = request->datatype();
	subIf = castCommandRequest<CjvxReportCommandRequest_id>(request);
	if (subIf)
	{
		subIf->ident(&ident);
		handle_report_ident(req, tp, ident.std_str(), &params);
	}

	uidIf = castCommandRequest<CjvxReportCommandRequest_uid>(request);
	if (uidIf)
	{
		uidIf->uid(&uId);
		handle_report_uid(req, tp, uId, &params);
	}

	ssIf = castCommandRequest<CjvxReportCommandRequest_ss>(request);
	if (ssIf)
	{
		ssIf->sswitch(&ss);
		handle_report_ss(req, tp, ss, &params);
	}
	return JVX_NO_ERROR;
}

bool
CjvxAutomationReport::shall_be_handled(jvxComponentType tp)
{
	auto elm = tp_rep_activate_filter.begin();
	for (; elm != tp_rep_activate_filter.end(); elm++)
	{
		if (*elm == tp)
		{
			return true;
		}
	}

	return false;
}

jvxErrorType
CjvxAutomationReport::handle_report_ident(
	jvxReportCommandRequest req,
	jvxComponentIdentification tp,
	const std::string& ident,
	CjvxAutomationReport::callSpecificParameters* params)
{
	jvxSize num = 0;
	jvxSize i;
	jvxBool reportSystem = false;
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize idIdent = JVX_SIZE_UNSELECTED;
	jvxSize idSelected = JVX_SIZE_UNSELECTED;
	IjvxObject* theObj = nullptr;
	
	jvxComponentIdentification tpD = tp;
	if (shall_be_handled(tp.tp))
	{
		// This type was in the proper list
		switch (req)
		{
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_SUBDEVICE:

			// The report of a subdevice is initiated by the technology. Therefore, we need to
			// change the component type at this point.
			// Howeverm the slotsubid is not yet set!
			tpD.tp = (jvxComponentType)(tpD.tp + 1);

			refHostRef->number_components_system(tpD, &num);
			for (i = 0; i < num; i++)
			{
				jvxApiString cpDescr;
				refHostRef->description_component_system(tpD, i, &cpDescr);
				if (cpDescr.std_str() == ident)
				{
					oneHandledComponent newElm;

					newElm.ident = ident;
					
					// Activate next available subslot. This will set the propper slotsubid!
					tpD.slotsubid = JVX_SIZE_DONTCARE;
					res = refHostRef->select_component(tpD, i, nullptr, true);
					if (res == JVX_NO_ERROR)
					{
						// Add entry in component list MUST BE BEFORE ACTIVATE
						refHostRef->request_object_selected_component(tpD, &newElm.component);
						newElm.cpTp = tpD;
						handledComponents[newElm.component] = newElm;

						res = refHostRef->activate_selected_component(tpD);

						if (res == JVX_NO_ERROR)
						{
							reportSystem = true;
						}
					}
				}

				// theHostRef->state_component(
			}
			break;
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_SUBDEVICE:

			// The report of a subdevice is initiated by the technology. Therefore, we need to
			// change the component type at this point.
			// HOWEVER: The slotsubid is not yet correct at this point as it comes from technology.
			// It is the ident that will let us find out which device to address!
			tpD.tp = (jvxComponentType)(tpD.tp + 1);
			
			// Find id of the "died" device slotsubid
			auto elm = handledComponents.begin();			
			for(; elm !=  handledComponents.end(); elm++)
			{
				if (elm->second.ident == ident)
				{
					break;
				}
			}
			assert(elm != handledComponents.end());

			tpD = elm->second.cpTp;

			// Find the involved processes and immediately stop all
			auto elmP = elm->second.ids_process_involded.begin();
			for (; elmP != elm->second.ids_process_involded.end(); elmP++)
			{
				complete_dead_device_sequencer_immediate(elmP);
			}			

			jvxState stat_old = JVX_STATE_NONE;
			jvxState stat = JVX_STATE_NONE;
			jvxBool run_loop = true;
			while (run_loop)
			{
				refHostRef->state_selected_component(tpD, &stat);
				if (stat == stat_old)
				{
					run_loop = false;
				}
				else
				{
					switch (stat)
					{
					case JVX_STATE_ACTIVE:
						refHostRef->deactivate_selected_component(tpD);
						break;
					case JVX_STATE_SELECTED:
						refHostRef->unselect_selected_component(tpD);
						run_loop = false;
						break;
					default:
						std::cout << "Warning: Could not switch state of process in immediate sequencer run, state ist still <" <<
							jvxState_txt(stat) << std::endl;
						assert(0);
						run_loop = false;
					}
				}
				stat_old = stat;
			}

			// Exclude this component from local list
			auto elmCheck = handledComponents.find(theObj);
			if (elmCheck != handledComponents.end())
			{
				assert(elmCheck->second.ids_process_involded.size() == 0);
				handledComponents.erase(elm);
			}

			reportSystem = true;


			break;
		}

		if (reportSystem)
		{
			if (refSystemReport)
			{
				CjvxReportCommandRequest req(
					jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_SYSTEM_STATUS_CHANGED);
				refSystemReport->request_command(req);
			}
		}
	}
	return res;
}

jvxErrorType
CjvxAutomationReport::handle_report_uid(jvxReportCommandRequest req, jvxComponentIdentification tp, jvxSize uid,
	CjvxAutomationReport::callSpecificParameters* params)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxDataConnections* conn = nullptr;
	IjvxSequencer* seq = nullptr;
	std::map<IjvxObject*, oneHandledComponent>::iterator elm;
	switch (req)
	{
	
	case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_TO_BE_DISCONNECTED:
	
		// This function is called right before a connection is diconnected. The connection may
		// even still be operating - therefore we need to run the next sequencer step IMMEDIATELY:
		// This diconnect can not be postponed-
		elm = handledComponents.begin();
		for (; elm != handledComponents.end(); elm++)
		{
			auto elmP = elm->second.ids_process_involded.find(uid);
			if (elmP != elm->second.ids_process_involded.end())
			{
				// Run the sequencer to deactivate the processing chain IMMEDIATELY
				complete_dead_device_sequencer_immediate(elmP);
				assert(elmP->second.theTask == NULL);
				elm->second.ids_process_involded.erase(elmP);

				// Added by HK today to hot-deactivate audio devices by symply deactivating them
				// If it is a single process, remove the entry in the map
				if (elm->second.autoRemove)
				{
					assert(elm->second.ids_process_involded.size() == 0);
					handledComponents.erase(elm);
				}
				break;
			}
		}
		break;

	case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_CONNECTED:

		conn = reqInterface< IjvxDataConnections>(refHostRef);
		seq = reqInterface<IjvxSequencer>(refHostRef);
		if (conn)
		{
			IjvxDataConnectionProcess* proc = nullptr;
			IjvxConnectionMaster* master = nullptr;
			IjvxConnectionMasterFactory* masfac = nullptr;
			IjvxObject* mas_obj = nullptr;
			conn->reference_connection_process_uid(uid, &proc);
			if (proc)
			{
#ifdef JVX_AUTOMATION_VERBOSE
				std::cout << "Connection Process established, id = <" << uid << ">." << std::endl;
#endif
				proc->associated_master(&master);
				{
					if (master)
					{
						master->parent_master_factory(&masfac);
						if (masfac)
						{
							masfac->request_reference_object(&mas_obj);
							if (mas_obj)
							{
								jvxApiString astr;
								mas_obj->description(&astr);

#ifdef JVX_AUTOMATION_VERBOSE
								std::cout << "Connection Process master <" << astr.std_str() << ">." << std::endl;
#endif

								// Add to handler 
								jvxBool foundit = false;
								auto elm = handledComponents.begin();
								for (; elm != handledComponents.end(); elm++)
								{
									if (elm->second.component == mas_obj)
									{
										foundit = true;
										break;
									}
								}

								if (!foundit)
								{
									// Add new entry for this process if not related to a specific component
									if (params->addBeforeStart)
									{
										jvxComponentIdentification tp;
										// proc->(&newCompElm.component, nullptr, nullptr, nullptr);
										oneHandledComponent newCompElm;
										newCompElm.autoRemove = true;
										newCompElm.component = static_cast<IjvxObject*>(proc);
										newCompElm.ident = astr.std_str();
										mas_obj->location_info(newCompElm.cpTp);

										// newCompElm.ids_process_involded
										handledComponents[newCompElm.component] = newCompElm;

										elm = handledComponents.find(newCompElm.component);
										foundit = true;
									}									
								}

								if (foundit)
								{
									jvxComponentIdentification cpId;
									jvxApiString cpName;
									mas_obj->request_specialization(NULL, &cpId, NULL);
									mas_obj->description(&cpName);

									auto elmId = elm->second.ids_process_involded.find(uid);
									if (elmId == elm->second.ids_process_involded.end())
									{
										// Add a specific task
										oneHandledComponent::oneProcessComponent newProcess;
										newProcess.proc_id = uid;
										if (seq)
										{
											CjvxSequencerTask::oneStepSequencer theStep;
											newProcess.theTask = new CjvxSequencerTask;
											newProcess.theTask->reset(("My Immediate task, proc id <" + jvx_size2String(uid) + ">").c_str(), nullptr, nullptr);

											theStep.description = "Prepare <" + cpName.std_str() + ">";
											theStep.element_type = JVX_SEQUENCER_TYPE_COMMAND_PROCESS_PREPARE;
											theStep.label_cond_true = "*";
											theStep.function_id = JVX_SIZE_UNSELECTED;
											theStep.component_id = cpId;
											newProcess.theTask->add_step(
												JVX_SEQUENCER_QUEUE_TYPE_RUN,
												theStep);
											theStep.description = "Start <" + cpName.std_str() + ">";
											theStep.element_type = JVX_SEQUENCER_TYPE_COMMAND_PROCESS_START;
											theStep.label_cond_true = "*";
											theStep.function_id = JVX_SIZE_UNSELECTED;
											theStep.component_id = cpId;
											newProcess.theTask->add_step(
												JVX_SEQUENCER_QUEUE_TYPE_RUN,
												theStep);

											// =========================================================

											theStep.description = "Stop <" + cpName.std_str() + ">";
											theStep.element_type = JVX_SEQUENCER_TYPE_COMMAND_PROCESS_STOP;
											theStep.label_cond_true = "*";
											theStep.function_id = JVX_SIZE_UNSELECTED;
											theStep.component_id = cpId;
											newProcess.theTask->add_step(
												JVX_SEQUENCER_QUEUE_TYPE_LEAVE,
												theStep);

											theStep.description = "Postprocess <" + cpName.std_str() + ">";
											theStep.element_type = JVX_SEQUENCER_TYPE_COMMAND_PROCESS_POSTPROCESS;
											theStep.label_cond_true = "*";
											theStep.function_id = JVX_SIZE_UNSELECTED;
											theStep.component_id = cpId;

											newProcess.theTask->add_step(
												JVX_SEQUENCER_QUEUE_TYPE_LEAVE,
												theStep);
											seq->add_sequencer_task(newProcess.theTask, &newProcess.task_id);
										}
										elm->second.ids_process_involded[uid] = newProcess;
									}
									else
									{
										assert(0); // Duplicate process report!
									}
								}
								masfac->return_reference_object(mas_obj);
								mas_obj = nullptr;
							}

						}
					}
				}
				// proc->set_reference_report(static_cast<IjvxDataConnectionCommon_report*>(this));
				conn->return_reference_connection_process(proc);
			}
			retInterface< IjvxDataConnections>(refHostRef, conn);
			conn = nullptr;
		}
		if (seq)
		{
			retInterface<IjvxSequencer>(refHostRef, seq);
		}
		break;
	}
	return res;
}

jvxErrorType 
CjvxAutomationReport::handle_report_ss(
	jvxReportCommandRequest req,
	jvxComponentIdentification tp,
	jvxStateSwitch ss,
	CjvxAutomationReport::callSpecificParameters* params)
{
	jvxErrorType res = JVX_NO_ERROR;
	return res;
}

void
CjvxAutomationReport::complete_dead_device_sequencer_immediate(
	std::map<jvxSize, CjvxAutomationReport::oneHandledComponent::oneProcessComponent>::iterator& elm)
{

	jvxErrorType resL = JVX_NO_ERROR;
	if (JVX_CHECK_SIZE_SELECTED(elm->second.proc_id))
	{
		if (elm->second.theTask)
		{
			IjvxSequencer* seq = reqInterface<IjvxSequencer>(refHostRef);
			assert(seq);
		
			// A task is pending if it is not waiting and not complete
			// This implies that the sequencer must run all immediate tasks when starting processing.
			// An immediate step that can not be executed since the sequencer has already started the task
			// should be shown as a warning: "WARN:Warning in Chain Prepare: a process chain matching <signal processing device>, and expression <*> could not be matched."
			jvxBool taskPending = (
				(elm->second.theTask->is_complete == false) &&
				(elm->second.theTask->my_status != jvxSequencerTaskState::JVX_STATE_SEQUENCER_TASK_WAITING)
				);
			if (taskPending)
			{
				// Indicate that this procss must be immediately run
				elm->second.theTask->flag_immediate_complete = true;

				// Step forward the sequencer to run this task
				CjvxReportCommandRequest req(jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE,
					JVX_COMPONENT_UNKNOWN,
					jvxReportCommandBroadcastType::JVX_REPORT_COMMAND_BROADCAST_NO_FURTHER,
					nullptr,
					true);
				resL = refSystemReport->request_command(req);
				assert(resL == JVX_NO_ERROR);
				assert(elm->second.theTask->is_complete);
			}
			resL = seq->remove_sequencer_task(elm->second.task_id);
			assert(resL == JVX_NO_ERROR);

			// Here is a dangerous part: if the sequencer is currently activated, we remove the pointer
			// actively. This, we should prevent
			delete elm->second.theTask;
			elm->second.theTask = NULL;

			retInterface<IjvxSequencer>(refHostRef, seq);
		}
	}
}
