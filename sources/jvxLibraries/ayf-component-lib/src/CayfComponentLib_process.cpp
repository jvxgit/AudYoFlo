#include "CayfComponentLib.h"

jvxErrorType
CayfComponentLib::deployProcParametersStartProcessor(jvxSize numInChans, jvxSize numOutChans, jvxSize bSize, jvxSize sRate,
	jvxDataFormat format, jvxDataFormatGroup formGroup)
{
	jvxErrorType resC = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
	IjvxDataConnections* theConnections = NULL;
	IjvxDataConnectionProcess* theProc = NULL;
	jvxApiString astr;
	theConnections = reqInterface<IjvxDataConnections>(this->hostRef);
	if (theConnections)
	{
		resC = theConnections->reference_connection_process_uid(uId_process, &theProc);
		if (resC != JVX_NO_ERROR)
		{
			goto exit_error;
		}

		procParams.bSize = bSize;
		procParams.numInChans = numInChans;
		procParams.numOutChans = numOutChans;
		procParams.sRate = sRate;
		procParams.format = format;
		procParams.formGroup = formGroup;

		_common_set_ocon.theData_out.con_params.buffersize = procParams.bSize;
		_common_set_ocon.theData_out.con_params.segmentation.x = procParams.bSize;
		_common_set_ocon.theData_out.con_params.number_channels = procParams.numInChans;
		_common_set_ocon.theData_out.con_params.segmentation.y = 1;
		_common_set_ocon.theData_out.con_params.rate = procParams.sRate;
		_common_set_ocon.theData_out.con_params.format = JVX_DATAFORMAT_DATA;
		_common_set_ocon.theData_out.con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;

		neg_input._set_parameters_fixed(procParams.numOutChans, procParams.bSize, procParams.sRate, JVX_DATAFORMAT_DATA,
			JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED);

		// resC = theProc->test_chain(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		jvxSize numTested = 0;
		resC = theConnections->add_process_test(uId_process, &numTested, true);
		if (resC != JVX_NO_ERROR)
		{
			goto exit_error;
		}

		genComponentLib::status.errorcode_test.value = jvxErrorType_descr(resC);
		genComponentLib::status.errordescription_test.value.clear();

		resC = theConnections->ready_for_start(uId_process , &astr);
		if (resC != JVX_NO_ERROR)
		{
			std::cout << "Subsystem not ready, reason: <" << astr.std_str() << ">." << std::endl;
			genComponentLib::status.errorcode_test.value = jvxErrorType_descr(resC);
			genComponentLib::status.errordescription_test.value = astr.std_str();
			goto exit_error;
		}
		else
		{
			std::cout << "Subsystem ready for processing!" << std::endl;
		}

		resC = this->prepare();
		assert(resC == JVX_NO_ERROR);
		resC = theProc->prepare_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (resC != JVX_NO_ERROR)
		{
			goto exit_error;
		}

		resC = this->start();
		assert(resC == JVX_NO_ERROR);
		resC = theProc->start_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (resC != JVX_NO_ERROR)
		{
			goto exit_error;
		}
		theConnections->return_reference_connection_process(theProc);
		retInterface<IjvxDataConnections>(this->hostRef, theConnections);

		this->on_main_node_started();
	}
	return JVX_NO_ERROR;

exit_error:
	if (theConnections)
	{
		if (theProc)
		{
			theConnections->return_reference_connection_process(theProc);
		}
		retInterface<IjvxDataConnections>(this->hostRef, theConnections);
	}
	return resC;
}

jvxErrorType 
CayfComponentLib::process_one_buffer_interleaved(
	jvxData* inInterleaved, jvxSize numSamplesIn, jvxSize numChannelsIn,
	jvxData* outInterleaved, jvxSize numSamlesOut, jvxSize numChannelsOut)
{
	jvxSize i;
	jvxErrorType res = JVX_ERROR_INVALID_SETTING;
	jvxState stat = JVX_STATE_NONE;
	this->state(&stat); 
	if (stat != JVX_STATE_PROCESSING)
	{
		return JVX_ERROR_NOT_READY;
	}

	if (_common_set_ocon.theData_out.con_link.connect_to)
	{
		res = _common_set_ocon.theData_out.con_link.connect_to->process_start_icon();
		if (res == JVX_NO_ERROR)
		{
			jvxSize idxToProc = *_common_set_ocon.theData_out.con_pipeline.idx_stage_ptr;
			jvxData** bufsToNode = (jvxData**)_common_set_ocon.theData_out.con_data.buffers[idxToProc];

			// Interleaved to non-interleaved
			for (i = 0; i < numChannelsIn; i++)
			{
				jvx_convertSamples_from_to<jvxData>(inInterleaved, bufsToNode[i], procParams.bSize, i, numChannelsIn, 0, 1);
			}

			/*============ PASS SAMPLES TO PROCESSING UNIT ==================*/
			res = _common_set_ocon.theData_out.con_link.connect_to->process_buffers_icon();
			assert(res == JVX_NO_ERROR);

			jvxSize idxFromProc = *_common_set_icon.theData_in->con_pipeline.idx_stage_ptr;
			jvxData** bufsFromNode = (jvxData**)_common_set_icon.theData_in->con_data.buffers[idxFromProc];
			// Non-interleaved to interleaved
			for (i = 0; i < numChannelsOut; i++)
			{
				jvx_convertSamples_from_to<jvxData>(bufsFromNode[i], outInterleaved, procParams.bSize, 0, 1, i, numChannelsOut);
			}
			res = _common_set_ocon.theData_out.con_link.connect_to->process_stop_icon();
		}
	}
	assert(res == JVX_NO_ERROR);
	return JVX_NO_ERROR;
}

jvxErrorType
CayfComponentLib::stopProcessor()
{
	jvxErrorType resC = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);
	IjvxDataConnections* theConnections = NULL;
	IjvxDataConnectionProcess* theProc = NULL;
	jvxState stat = JVX_STATE_NONE;
	this->state(&stat);
	if (stat > JVX_STATE_ACTIVE)
	{
		theConnections = reqInterface<IjvxDataConnections>(this->hostRef);
		assert(theConnections);

		resC = theConnections->reference_connection_process_uid(uId_process, &theProc);
		assert(resC == JVX_NO_ERROR);

		if (stat > JVX_STATE_PREPARED)
		{
			resC = theProc->stop_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			assert(resC == JVX_NO_ERROR);
			resC = this->stop();
			assert(resC == JVX_NO_ERROR);
		}

		this->state(&stat);

		if (stat > JVX_STATE_ACTIVE)
		{
			resC = theProc->postprocess_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
			assert(resC == JVX_NO_ERROR);
			resC = this->postprocess();
			assert(resC == JVX_NO_ERROR);
		}
		theConnections->return_reference_connection_process(theProc);
		retInterface<IjvxDataConnections>(this->hostRef, theConnections);
		procParams.reset();
	}
	return JVX_NO_ERROR;
}

