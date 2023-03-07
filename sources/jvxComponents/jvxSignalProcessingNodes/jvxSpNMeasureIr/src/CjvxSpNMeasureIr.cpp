#include "CjvxSpNMeasureIr.h"

CjvxSpNMeasureIr::CjvxSpNMeasureIr(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	selMeasures = JVX_SIZE_UNSELECTED;

	localTextLog.sz = 256;
	localTextLog.use = false;
	localTextLog.theobj = NULL;
	localTextLog.thehdl = NULL;
	localTextLog.separatorToken = "\n";

}

CjvxSpNMeasureIr::~CjvxSpNMeasureIr()
{
	
}

// ===================================================================

jvxErrorType
CjvxSpNMeasureIr::activate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::activate();
	jvxDspBaseErrorType resDsp = JVX_DSP_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{
		genMeasureIr_node::init_all();
		genMeasureIr_node::allocate_all();
		genMeasureIr_node::register_all(static_cast<CjvxProperties*>(this));
		genMeasureIr_node::register_callbacks(static_cast<CjvxProperties*>(this),
			set_config_posthook,
			trigger_start,
			set_generator_posthook,
			set_measurement_posthook, 
			get_measurement_prehook, 
			add_entry, remove_entry, 
			set_all_channels_posthook,
			get_monitor_prehook,
			set_evaluation_posthook,
			set_result_posthook,
			get_measurement_data_prehook,
			set_result_import_measurement,
			set_config_measurement_posthook,
			get_config_measurement_prehook,
			reinterpret_cast<jvxHandle*>(this), NULL);

		neg_input._update_parameters_fixed(node_inout._common_set_node_params_a_1io.number_channels);

		reconstruct_properties(JVX_SIZE_UNSELECTED, false);

		if (_common_set.theToolsHost)
		{
			jvxErrorType resL = _common_set.theToolsHost->instance_tool(JVX_COMPONENT_LOCAL_TEXT_LOG, &localTextLog.theobj,
				0, NULL);
			if ((resL == JVX_NO_ERROR) && localTextLog.theobj)
			{
				resL = localTextLog.theobj->request_specialization(reinterpret_cast<jvxHandle**>(
					&localTextLog.thehdl), NULL, NULL);
				localTextLog.use = true;
			}
			else
			{
				localTextLog.use = false;
				std::cout << "::" << __FUNCTION__ << ": Error: Failed to open logger object, reason: " << jvxErrorType_txt(resL) << std::endl;
			}
		}
		else
		{
			localTextLog.use = false;
			std::cout << "::" << __FUNCTION__ << "Error: Failed to open logger object for component< " <<
				_common_set.theModuleName << "> since tools host reference is not vailable." << std::endl;
		}

		if (localTextLog.use)
		{
			localTextLog.thehdl->initialize(_common_set_min.theHostRef);
			localTextLog.thehdl->configure(JVX_LOCAL_TEXT_LOG_CONFIG_SEPARATOR_CHAR, (jvxHandle*)localTextLog.separatorToken.c_str());

			localTextLog.thehdl->start(localTextLog.sz);
			localTextLog.thehdl->attach_entry(("Starting local text log" + localTextLog.separatorToken).c_str());

			jvxCallManagerProperties callGate;
			install_property_reference(callGate, jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<IjvxLocalTextLog>(localTextLog.thehdl),
				jPAD("/local_text_log"));
		}
		else
		{
			std::cout << __FUNCTION__ << ": Failed to activate the local text log." << std::endl;
		}	
	}
	return res;
}

jvxErrorType
CjvxSpNMeasureIr::deactivate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		if (localTextLog.use)
		{
			jvxCallManagerProperties callGate;
			uninstall_property_reference(callGate, 
				jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<IjvxLocalTextLog>(localTextLog.thehdl),
				jPAD("/local_text_log"));

			localTextLog.thehdl->stop();
			localTextLog.thehdl->terminate();

			jvxErrorType resL = _common_set.theToolsHost->return_instance_tool(JVX_COMPONENT_LOCAL_TEXT_LOG, localTextLog.theobj,
				0, NULL);
			localTextLog.theobj = NULL;
			localTextLog.thehdl = NULL;
		}

		genMeasureIr_node::unregister_all(static_cast<CjvxProperties*>(this));
		genMeasureIr_node::deallocate_all();

		auto elm = measurements.begin();
		for (; elm != measurements.end(); elm++)
		{
			JVX_DSP_SAFE_DELETE_OBJECT(*elm);
		}
		measurements.clear();

		clear_all_results();
		

		CjvxBareNode1ioRearrange::deactivate();
	}
	return res;
}

// ===================================================================

jvxErrorType 
CjvxSpNMeasureIr::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	if (_common_set_ldslave.theData_in->con_params.number_channels != node_inout._common_set_node_params_a_1io.number_channels)
	{
		// genMeasureIr_node::config.number_input_channels_max.value = _common_set_ldslave.theData_in->con_params.number_channels;
		neg_input._update_parameters_fixed(_common_set_ldslave.theData_in->con_params.number_channels);
	}
	res = CjvxBareNode1ioRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		node_output._common_set_node_params_a_1io.number_channels = (jvxInt32)_common_set_ldslave.theData_out.con_params.number_channels;
		if (
			(genMeasureIr_node::measurements.measurement_out_channels.value.entries.size() != node_output._common_set_node_params_a_1io.number_channels) ||
			(genMeasureIr_node::measurements.measurement_in_channels.value.entries.size() != node_output._common_set_node_params_a_1io.number_channels))
		{
			reconstruct_properties(selMeasures, false);
		}
	}

	return res;
}

void
CjvxSpNMeasureIr::test_set_output_parameters()
{
	// Leave number channels open
	neg_output._update_parameters_fixed(
		node_output._common_set_node_params_a_1io.number_channels,
		node_output._common_set_node_params_a_1io.buffersize,
		node_output._common_set_node_params_a_1io.samplerate,
		(jvxDataFormat)node_output._common_set_node_params_a_1io.format,
		(jvxDataFormatGroup)node_output._common_set_node_params_a_1io.subformat,
		(jvxDataflow)node_output._common_set_node_params_a_1io.data_flow,
		&_common_set_ldslave.theData_out);
}

jvxErrorType
CjvxSpNMeasureIr::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data, JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxLinkDataDescriptor* ld = (jvxLinkDataDescriptor*)data;

	// Accept the proposed number of output channels
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:
		node_output._common_set_node_params_a_1io.number_channels =
			(jvxInt32)ld->con_params.number_channels;
		neg_output._update_parameters_fixed(node_output._common_set_node_params_a_1io.number_channels);
		break;
	}
	res = CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	return res;
}

// ===================================================================

jvxErrorType
CjvxSpNMeasureIr::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1ioRearrange::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	jvxDspBaseErrorType resDsp = JVX_DSP_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{
		jvxSize numResultsAll = 0;

		genMeasureIr_node::monitor.display_text_state.value = "- Preparing Audio Processing -";		

		if (genMeasureIr_node::callbacks.report_evaluate.value)
		{
			std::string txt = "Starting Measurement";
			callback_report_txt(txt);
		}

		auto elm = measurements.begin();
		if (genMeasureIr_node::config.single_measurement.value)
		{
			if (selMeasures < measurements.size())
			{
				std::advance(elm, selMeasures);

				jvxSize numResultsLocal = 0;
				(*elm)->allocate_one_measurement(_common_set_ldslave.theData_out.con_params.rate,
					1024,
					&numResultsLocal);
				numResultsAll += numResultsLocal;
				(*elm)->measurementComplete = false;
				inProcessing.activeMeasures.push_back(*elm);
				std::string txt = "Activating (Single) Measurement " + (*elm)->description;
				callback_report_txt(txt);
			}
		}
		else
		{
			
			for (; elm != measurements.end(); elm++)
			{
				jvxSize numResultsLocal = 0;
				(*elm)->allocate_one_measurement(_common_set_ldslave.theData_out.con_params.rate,
					1024,
					&numResultsLocal);
				numResultsAll += numResultsLocal;
				(*elm)->measurementComplete = false;
				inProcessing.activeMeasures.push_back(*elm);
				std::string txt = "Activating Measurement " + (*elm)->description;
				callback_report_txt(txt);
			}
		}

		genMeasureIr_node::monitor.display_text_state.value = "- Audio Processing in Action -";
		genMeasureIr_node::monitor.glitches_during_measurement.value = c_false;
		genMeasureIr_node::monitor.glitches_all.value = 0;

		inProcessing.currentMeasure = NULL;
		inProcessing.idMeasure = -1;
		inProcessing.completed = false;
		inProcessing.completeReported = false;
		inProcessing.do_start = false;
		inProcessing.cntDownStart = (jvxInt32) (genMeasureIr_node::config.countdown_start.value * _common_set_ldslave.theData_out.con_params.rate);
		if(genMeasureIr_node::config.autostart_measure.value == c_true)
		{
			inProcessing.do_start = true;
		}
	}
	return res;
}

// void copy_measurement(jvxSize cnt, jvxData* ptr);

jvxErrorType
CjvxSpNMeasureIr::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxDspBaseErrorType resDsp = JVX_DSP_NO_ERROR;

	jvxErrorType res = CjvxBareNode1io::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		std::string token = "";
		token += JVX_DATE();
		token += "__";
		token += JVX_TIME();
		token = jvx_replaceCharacter(token, ' ', '_');
		token = jvx_replaceCharacter(token, ':', '-');
		token = jvx_replaceCharacter(token, '/', '-');

		genMeasureIr_node::monitor.display_text_state.value = "- Evaluation of Captured Data -";

		if (genMeasureIr_node::callbacks.report_evaluate.value)
		{
			std::string txt = "Starting Evaluation of Measurement.";
			callback_report_txt(txt);
		}

		// Derive all IR buffers
		for (i = 0; i < inProcessing.activeMeasures.size(); i++)
		{
			if (inProcessing.activeMeasures[i]->measurementComplete)
			{
				if (genMeasureIr_node::callbacks.report_evaluate.value)
				{
					std::string txt = "Evaluating Measurement <";
					txt += inProcessing.activeMeasures[i]->description;
					txt += ">.";

					callback_report_txt(txt);
				}
				// inProcessing.activeMeasures[i]->write_data(path);
				inProcessing.activeMeasures[i]->evaluate_measurement();

				if (genMeasureIr_node::callbacks.report_evaluate.value)
				{
					std::string txt = ".. done.";
					callback_report_txt(txt);
				}
			}
		}

		genMeasureIr_node::monitor.display_text_state.value = "- Copying Captured Data -";

		for (i = 0; i < inProcessing.activeMeasures.size(); i++)
		{
			oneMeasurementResult oneResult;
			oneMeasurementChannel oneChan;
			jvxSize cnt = 0;

			if (inProcessing.activeMeasures[i]->measurementComplete)
			{
				while (1)
				{
					jvxErrorType resL = JVX_NO_ERROR;
					resL = inProcessing.activeMeasures[i]->copy_measurement_channel(cnt, &oneChan);
					if (resL == JVX_NO_ERROR)
					{
						JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(oneChan.bufIr, jvxData, oneChan.lBuf);
						JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(oneChan.bufMeas, jvxData, oneChan.lBuf);
						JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(oneChan.bufTest, jvxData, oneChan.lBuf);
						inProcessing.activeMeasures[i]->copy_measurement_channel(cnt, &oneChan);
						oneResult.resultsChannel.push_back(oneChan);

						oneChan.bufIr = NULL;
						oneChan.bufMeas = NULL;
						oneChan.bufTest = NULL;
						oneChan.lBuf = 0;
						oneChan.nmChan.clear();
						oneChan.nmMeas.clear();
						cnt++;
					}
					else
					{
						break;
					}
				}

				jvxErrorType resL = inProcessing.activeMeasures[i]->copy_measurement(&oneResult);
				if (oneResult.freq_gain_len)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(oneResult.freq_buf, jvxData, oneResult.freq_gain_len);
					JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(oneResult.gain_buf, jvxData, oneResult.freq_gain_len);
					inProcessing.activeMeasures[i]->copy_measurement(&oneResult);
				}

				// Local storage
				inProcessing.activeMeasures[i]->pathStoreFolder = ".";
				inProcessing.activeMeasures[i]->pathStoreFolder += JVX_SEPARATOR_DIR + inProcessing.activeMeasures[i]->description + "_" + token;

				oneResult.nameMeasure = inProcessing.activeMeasures[i]->description;
				oneResult.tokenMeasure = token;
				oneResult.rate = _common_set_ldslave.theData_in->con_params.rate;
				oneResult.pathStoreFolder = inProcessing.activeMeasures[i]->pathStoreFolder;
				oneResult.storeConfig = true;
				if (oneResult.resultsChannel.size())
				{
					collectedResults.push_back(oneResult);
				}
				else
				{
					if (oneResult.freq_buf)
					{
						JVX_DSP_SAFE_DELETE_FIELD(oneResult.freq_buf);
					}
					if (oneResult.gain_buf)
					{
						JVX_DSP_SAFE_DELETE_FIELD(oneResult.gain_buf);
					}
				}
			}
		}

		IjvxObject* theConfObj = NULL;
		IjvxConfigProcessor* proc = NULL;
		_common_set.theToolsHost->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, &theConfObj, 0, NULL);
		if (theConfObj)
		{
			theConfObj->request_specialization(reinterpret_cast<jvxHandle**>(&proc), NULL, NULL);
		}

		IjvxObject* theWriterObj = NULL;
		IjvxRtAudioFileWriter* 	theWriterHdl = NULL;
		_common_set.theToolsHost->instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_WRITER, &theWriterObj, 0, NULL);

		if (theWriterObj)
		{
			theWriterObj->request_specialization(reinterpret_cast<jvxHandle**>(&theWriterHdl), NULL, NULL);
			theWriterHdl->initialize(_common_set_min.theHostRef);
		}

		genMeasureIr_node::monitor.display_text_state.value = "- Storing Captured Data to Files -";

		for (i = 0; i < inProcessing.activeMeasures.size(); i++)
		{
			if (inProcessing.activeMeasures[i]->measurementComplete)
			{
				if (inProcessing.activeMeasures[i]->evaluation.store_data)
				{
					if (genMeasureIr_node::callbacks.report_evaluate.value)
					{
						std::string txt = "Storing Measurement data for measurement <";
						txt += inProcessing.activeMeasures[i]->description;
						txt += "> in folder <";
						txt += inProcessing.activeMeasures[i]->pathStoreFolder;
						txt += ">.";
						callback_report_txt(txt);
					}

					inProcessing.activeMeasures[i]->write_data(token, 
						theWriterHdl, proc, static_cast<IjvxSpNMeasureIr_oneMeasurement_report*>(this));

					// Align protocol filename in both lists
					for (auto& elm : collectedResults)
					{
						if (elm.pathStoreFolder == inProcessing.activeMeasures[i]->pathStoreFolder)
						{
							elm.pathStoreProto = inProcessing.activeMeasures[i]->pathStoreProto;
							break;
						}
					}
					if (genMeasureIr_node::callbacks.report_evaluate.value)
					{
						std::string txt = ".. done.";
						callback_report_txt(txt);
					}
				}
				else
				{
					if (genMeasureIr_node::callbacks.report_evaluate.value)
					{
						std::string txt = "Storing Measurement data for measurement <";
						txt += inProcessing.activeMeasures[i]->description;
						txt += "> is not active.";
						callback_report_txt(txt);
					}
				}
			}
			else
			{
				if (genMeasureIr_node::callbacks.report_evaluate.value)
				{
					std::string txt = "Measurement <";
					txt += inProcessing.activeMeasures[i]->description;
					txt += "> is incomplete.";
					callback_report_txt(txt);
				}
			}
		}

		if (theWriterObj)
		{
			theWriterHdl->terminate();
			_common_set.theToolsHost->return_instance_tool(JVX_COMPONENT_RT_AUDIO_FILE_WRITER, theWriterObj, 0, NULL);
		}
		if (theConfObj)
		{
			_common_set.theToolsHost->return_reference_object(theConfObj);
		}

		for (i = 0; i < inProcessing.activeMeasures.size(); i++)
		{
			// inProcessing.activeMeasures[i]->write_data(path);
			inProcessing.activeMeasures[i]->deallocate_one_measurement();
		}
		inProcessing.activeMeasures.clear();
		//jvxDspBaseErrorType jvx_generatorwave_update(jvx_generatorWave* hdl, jvxUInt16 whatToSet, jvxCBool do_set);

		//jvxDspBaseErrorType jvx_generatorwave_process(jvx_generatorWave* hdl, jvxData* bufferFill, size_t lField, size_t* written);

		genMeasureIr_node::monitor.display_text_state.value = "";

		update_plot_measurements();

		if (genMeasureIr_node::callbacks.report_evaluate.value)
		{
			std::string txt = "All postprocessing done.";
			callback_report_txt(txt);
		}

	}
	if (genMeasureIr_node::measurements.measurement_clear_after.value)
	{
		if (genMeasureIr_node::callbacks.report_evaluate.value)
		{
			std::string txt = "Clearing all data from memory.";
			callback_report_txt(txt);
		}
		clear_all_results();
		update_plot_measurements();
	}

	return res;
}

jvxErrorType
CjvxSpNMeasureIr::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize numW = 0;
	jvxSize i = 0;
	jvxData progress_loop = 0;
	jvxData progress_in_period = 0;
	jvxSize sampleCount = 0;
	jvxSize sampleCountMax = _common_set_ldslave.theData_out.con_params.buffersize;
	jvxSize chanCntMax = _common_set_ldslave.theData_out.con_params.number_channels;
	jvxSize idxStageIn = idx_stage;
	jvxSize idxStageOut = JVX_SIZE_UNSELECTED;
	jvxSize newValueGlitches = JVX_SIZE_UNSELECTED;
	if (JVX_CHECK_SIZE_UNSELECTED(idxStageIn))
	{
		idxStageIn = *_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr;
	}
	idxStageOut = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;

	if (_common_set_ldslave.theData_in->con_data.attached_buffer_single[idxStageIn])
	{
		jvxLinkDataAttached* ptr = _common_set_ldslave.theData_in->con_data.attached_buffer_single[idxStageIn];
		while (ptr)
		{
			if (ptr->tp == JVX_LINKDATA_ATTACHED_REPORT_UPDATE_NUMBER_LOST_FRAMES)
			{
				jvxLinkDataAttachedLostFrames* ptrL = (jvxLinkDataAttachedLostFrames*)ptr;
				newValueGlitches = ptrL->numLost;
				genMeasureIr_node::monitor.glitches_all.value = newValueGlitches;
				break;
			}
			ptr = ptr->next;
		}
	}
	JVX_ATTACHED_LINK_DATA_FORWARD_SINGLE(_common_set_ldslave.theData_in, idxStageIn,
		(&_common_set_ldslave.theData_out), idxStageOut);

	jvxData** bufsIn = (jvxData**)_common_set_ldslave.theData_in->con_data.buffers[idxStageIn];
	jvxData** bufsOut = (jvxData**)_common_set_ldslave.theData_out.con_data.buffers[idxStageOut];

	// Pre-populate buffers with zeros
	for (i = 0; i < chanCntMax; i++)
	{
		memset(bufsOut[i], 0, sizeof(jvxData) * sampleCountMax);
	}

	if (inProcessing.do_start)
	{
		if (inProcessing.cntDownStart > 0)
		{
			inProcessing.cntDownStart -= (jvxInt32)_common_set_ldslave.theData_in->con_params.buffersize;
			inProcessing.cntDownStart = JVX_MAX(inProcessing.cntDownStart, 0);
			genMeasureIr_node::monitor.countdown_togo.value = (jvxData) inProcessing.cntDownStart / (jvxData)_common_set_ldslave.theData_in->con_params.rate;			
		}
		else
		{
			// Fill in the generator signal
			while (sampleCount < sampleCountMax)
			{
				if (inProcessing.currentMeasure)
				{
					if (JVX_CHECK_SIZE_SELECTED(newValueGlitches))
					{
						genMeasureIr_node::monitor.glitches_during_measurement.value = c_true;
					}
					jvxBool src_complete = false;
					inProcessing.currentMeasure->addData(bufsOut, 
						_common_set_ldslave.theData_out.con_params.number_channels,
						bufsIn, 
						_common_set_ldslave.theData_in->con_params.number_channels,
						sampleCount, &numW,
						_common_set_ldslave.theData_out.con_params.buffersize,
						src_complete,
						&progress_loop,
						&progress_in_period);

					sampleCount += numW;

					// This source is complete, switch to next or stop
					if (src_complete)
					{
						inProcessing.currentMeasure->measurementComplete = true;
						inProcessing.currentMeasure = NULL;
					}

					// jvxData* bufOut = bufsOut[idxOut];
				}
				if (inProcessing.currentMeasure == NULL)
				{
					inProcessing.idMeasure++;
					if (inProcessing.idMeasure < inProcessing.activeMeasures.size())
					{
						inProcessing.currentMeasure = inProcessing.activeMeasures[inProcessing.idMeasure];
						genMeasureIr_node::monitor.display_text_state.value = "- Measurement <" + inProcessing.currentMeasure->description + "> -";
					}
					else
					{
						inProcessing.completed = true;
						break;
					}
				}
			}

			if ((inProcessing.completed) && (!inProcessing.completeReported))
			{
				jvxCBitField prio = 0;
				jvx_bitZSet(prio, JVX_REPORT_REQUEST_TRY_TRIGGER_STOP_SEQUENCER_PROCESS_SHIFT);
				_report_command_request(prio);
				inProcessing.completeReported = true;
			}
		}

		genMeasureIr_node::monitor.progress_one_period.value.val() = progress_in_period * genMeasureIr_node::monitor.progress_one_period.value.maxVal;
		genMeasureIr_node::monitor.progress_loops.value.val() = progress_loop * genMeasureIr_node::monitor.progress_one_period.value.maxVal;
		genMeasureIr_node::monitor.progress_measures.value.val() = (jvxData)inProcessing.idMeasure / (jvxData)inProcessing.activeMeasures.size() * genMeasureIr_node::monitor.progress_one_period.value.maxVal;
	}
	// Forward the processed data to next chain element
	res = _process_buffers_icon(mt_mask, idx_stage);
	return res;
}

void
CjvxSpNMeasureIr::callback_report_txt(const std::string& txt)
{
	jvxFloatingPointer fltPtr;
	jvxSize szFld = sizeof(jvxFloatingPointer);// +txt.size() + 1;

	if (localTextLog.thehdl)
	{
		localTextLog.thehdl->attach_entry((txt + localTextLog.separatorToken).c_str());
	}

	fltPtr.sz = szFld;
	fltPtr.copy = NULL;
	fltPtr.deallocate = NULL;
	jvx_bitSet(fltPtr.flags, JVX_FLOATPOINTER_REQUIRES_COPY_SHIFT);
	fltPtr.priv = NULL;
	fltPtr.tagid = JVX_FLOATINGPOINTER_UPDATE_PROPERTY_REPORT_IMMEDIATE; // only available tag id
	
	genMeasureIr_node::callbacks.report_evaluate.value->cb(
		genMeasureIr_node::callbacks.report_evaluate.value->cb_priv,
		reinterpret_cast<jvxFloatingPointer *>(&fltPtr));
}

void 
CjvxSpNMeasureIr::jvxSpNMeasureIr_reportText(const std::string& txt)
{
	if (genMeasureIr_node::callbacks.report_evaluate.value)
	{
		callback_report_txt(txt);
	}
}

void
CjvxSpNMeasureIr::clear_all_results()
{
	auto elmM = collectedResults.begin();
	for (; elmM != collectedResults.end(); elmM++)
	{
		auto elmC = elmM->resultsChannel.begin();
		for (; elmC != elmM->resultsChannel.end(); elmC++)
		{
			JVX_DSP_SAFE_DELETE_FIELD(elmC->bufIr);
			JVX_DSP_SAFE_DELETE_FIELD(elmC->bufMeas);
			JVX_DSP_SAFE_DELETE_FIELD(elmC->bufTest);
		}
		elmM->resultsChannel.clear();
		if (elmM->freq_buf)
		{
			JVX_DSP_SAFE_DELETE_FIELD(elmM->freq_buf);
		}
		if (elmM->gain_buf)
		{
			JVX_DSP_SAFE_DELETE_FIELD(elmM->gain_buf);
		}
	}
	collectedResults.clear();
}

/*
void 
CjvxAuNPlayChannelId::test_set_output_parameters()
{
	
}
*/




/*
jvx_factory_allocate_jvxIntegrateWaveGenerators(&myWaveGenerators);
res = myWaveGenerators->activate(static_cast<CjvxProperties*>(this), "WaveGenerators");
//res = myWaveGenerators->activate(&myProperties, "WaveGenerators");
myWaveGenerators->setParameters(
	CjvxAudioNode_genpcg::properties_parameters.samplerate.value,
	CjvxAudioNode_genpcg::properties_parameters.buffersize.value,
	CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value,
	(jvxDataFormat)CjvxAudioNode_genpcg::properties_parameters.format.value,
	&updateUi1
);
*/
