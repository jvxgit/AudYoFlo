#include "common/CjvxInputOutputSettings.h"

void
CjvxInputOutputSettings::resetOneTest(jvxRateLocationMode fixedLocationModeArg)
{
	this->protoState = testProtocolStatus::AYF_CONVERT_START;	
	requiresUpdateOutput = false;
	fixedLocationMode = fixedLocationModeArg;
	jvx_neutralDataLinkDescriptor(&tryLast, true);
	memset(stepsTaken, 0, sizeof(stepsTaken));
	stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_START]++;

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
	protoNegotiate.clear();
#endif

}

jvxErrorType
CjvxInputOutputSettings::passFromPredecessor(jvxLinkDataDescriptor* data)
{
	// Take the input parameters and accept. We track changes on the input side to react in a different way
#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
	std::string params = jvx_linkDataParams2String(data);
	protoNegotiate += "<CjvxInputOutputSettings>: Enter logic in function <passFromPredecessor>, parameters: " + params + "\n";
#endif

	in.currentDeviationState = checkDeviation(data, in);

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
	protoNegotiate += "<CjvxInputOutputSettings> Deviation state on enter: " + jvx_bitField2String(in.currentDeviationState) + "\n";
#endif

	stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_INPUT_DEVIATION_ANALYZED]++;
	this->protoState = testProtocolStatus::AYF_CONVERT_INPUT_ACCEPTED;

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxInputOutputSettings::passFromSuccessor(jvxLinkDataDescriptor* preferredByOutput, jvxLinkDataDescriptor& given, jvxLinkDataDescriptor* fromPrevious JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool tryMatchSingle = true;
	jvxLinkDataDescriptor onStart = given;

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
	std::string paramsPref = jvx_linkDataParams2String(preferredByOutput);
	std::string paramsGiven = jvx_linkDataParams2String(&given);
	std::string paramsPrev = jvx_linkDataParams2String(fromPrevious);
	protoNegotiate += "<CjvxInputOutputSettings> Enter logic in function <passFromSuccessor>, \n";
	protoNegotiate += "\t\tParameters preferred by output: " + paramsPref + "\n";
	protoNegotiate += "\t\tParameters given to output: " + paramsGiven + "\n";
	protoNegotiate += "\t\tParameters from predecessor: " + paramsPrev + "\n";
#endif

	// Step 1: Pass backward the parameters from the successor
	// Expect a JVX_NO_ERROR or a JVX_ERROR_COMPROMISE
	if (!checkIdentical(tryLast, *preferredByOutput))
	{
		tryLast = *fromPrevious;

		//if (!jvx_bitTest(stepsTaken, (jvxSize)testProtocolStatus::AYF_CONVERT_TRANSFER_BACK_TRY_UNALTERED))
		//{
		tryLast = *preferredByOutput;

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
		std::string paramsLast = jvx_linkDataParams2String(&tryLast);
		protoNegotiate += "<CjvxInputOutputSettings> Forwarding to predecessor with parameters: " + paramsLast + "\n";
		protoNegotiate += "\t\tParameters preferred by output" + paramsPref + "\n";
		protoNegotiate += "\t\tParameters given to output" + paramsGiven + "\n";
		protoNegotiate += "\t\tParameters from predecessor" + paramsPrev + "\n";
#endif
		res = fromPrevious->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryLast JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
		protoNegotiate += "\t\tResult from predecessor: <";
		protoNegotiate += jvxErrorType_descr((jvxSize)res);
		protoNegotiate += ">\n";
#endif

		stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_TRANSFER_BACK_TRY_UNALTERED]++;
		this->protoState = testProtocolStatus::AYF_CONVERT_TRANSFER_BACK_TRY_UNALTERED;
		// }

		if (res == JVX_NO_ERROR)
		{
			// Take over all parameters
			acceptInputParams(fromPrevious, nullptr);
			copyOutFromIn(in);
			prepareForForward(given);

			stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_LEAVE_ON_AGREE]++;
			this->protoState = testProtocolStatus::AYF_CONVERT_LEAVE_ON_AGREE;

			// The predecessor has accepted the proposed parameters
			tryMatchSingle = false;

		}
		if (res == JVX_ERROR_COMPROMISE)
		{
			// Check if the previous returns the same as before. Then, we need not test it!!
			jvxCBitField devLocal = checkDeviation(fromPrevious, in);
			if (devLocal == 0)
			{
				// If it is the same, we need to try to change single parameters with full compliance
				tryMatchSingle = true;
			}
			else
			{
				// Yes, there is something new!! Let us try the update

				// Update input and output side
				acceptInputParams(fromPrevious, nullptr);

				if (forwardCompromise)
				{
					// Step 2: If we got a compromise, try to go with the compromise!!
					copyOutFromIn(in);
					prepareForForward(given);

					stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_LEAVE_ON_AGREE]++;
					this->protoState = testProtocolStatus::AYF_CONVERT_LEAVE_ON_AGREE;
					tryMatchSingle = false;
				}
				else
				{
					tryMatchSingle = true;
				}
			}
		}
	}

	while (tryMatchSingle)
	{
		if (this->protoState < testProtocolStatus::AYF_CONVERT_TRIED_CHANGE_FORMAT_FULL)
		{
			// Successor not ready for a compromise, we need to try something else		
			// Step 3: If that did not succede, try to find a match by changing single parameters

			// First attempt: Let us try a different format
			jvxBool acceptSingleAsItIs = true;

			// Case where the format deviates from input to output
			if (fromPrevious->con_params.format != preferredByOutput->con_params.format)
			{
				tryLast.con_params = fromPrevious->con_params;
				tryLast.con_params.format = preferredByOutput->con_params.format;
				jvx_bitZSet(tryLast.con_params.additional_flags, (jvxSize)jvxDataLinkDescriptorTestFlags::JVX_LINKDATA_TEST_SINGLE_FORMAT_SHIFT);

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
				std::string paramsLast = jvx_linkDataParams2String(&tryLast);
				protoNegotiate += "<CjvxInputOutputSettings> Trying to adapt *format* on predecessor: " + paramsLast + "\n";
#endif

				res = fromPrevious->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryLast JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
				protoNegotiate += "\t\tResult from predecessor: <";
				protoNegotiate += jvxErrorType_descr((jvxSize)res);
				protoNegotiate += ">\n";
#endif

				if (res == JVX_NO_ERROR)
				{
					// Here, it is very likely that the input side can switch the format to the desired format
					if (forwardCompromise)
					{
						stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_TRIED_CHANGE_FORMAT_FULL]++;
						this->protoState = testProtocolStatus::AYF_CONVERT_TRIED_CHANGE_FORMAT_FULL;
						
						jvxErrorType resLocal = given.con_link.connect_to->transfer_forward_icon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryLast JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
						if (resLocal == JVX_NO_ERROR)
						{
							acceptInputParams(fromPrevious, nullptr);
							copyOutFromIn(in);
							prepareForForward(given);
							acceptSingleAsItIs = false;

						}
					}
				} // if (res == JVX_NO_ERROR) -> a compromise does not really help!!
				jvx_bitFClear(tryLast.con_params.additional_flags);

				if (acceptSingleAsItIs)
				{
					// Accept it as it is
					stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_ACCEPTED_CHANGE_FORMAT]++;
					this->protoState = testProtocolStatus::AYF_CONVERT_ACCEPTED_CHANGE_FORMAT;
					in.form = fromPrevious->con_params.format;
					out.form = preferredByOutput->con_params.format;

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE					
					protoNegotiate += "<CjvxInputOutputSettings> ==> Conversion activated from <";
					protoNegotiate += jvxDataFormat_txt(in.form);
					protoNegotiate += "> to <";
					protoNegotiate += jvxDataFormat_txt(out.form);
					protoNegotiate += "> \n";
#endif
					requiresUpdateOutput = true;
					prepareForForward(given);
					// No update on the output side!! We will need to run the converter for this mismatch but it is ok			
				}
			}

			// ===================================================================================
			// ===================================================================================
			acceptSingleAsItIs = true;
			if (fromPrevious->con_params. number_channels != preferredByOutput->con_params.number_channels)
			{
				tryLast.con_params = fromPrevious->con_params;
				tryLast.con_params.number_channels = preferredByOutput->con_params.number_channels;
				jvx_bitZSet(tryLast.con_params.additional_flags, (jvxSize)jvxDataLinkDescriptorTestFlags::JVX_LINKDATA_TEST_SINGLE_NUM_CHANNELS_SHIFT);

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
				std::string paramsLast = jvx_linkDataParams2String(&tryLast);
				protoNegotiate += "<CjvxInputOutputSettings> Trying to adapt *number channels* on predecessor: " + paramsLast + "\n";
#endif

				res = fromPrevious->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryLast JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
				protoNegotiate += "\t\tResult from predecessor: <";
				protoNegotiate += jvxErrorType_descr((jvxSize)res);
				protoNegotiate += ">\n";
#endif

				if (res == JVX_NO_ERROR)
				{
					if (forwardCompromise)
					{
						stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_TRIED_CHANGE_NCHANS_FULL]++;
						this->protoState = testProtocolStatus::AYF_CONVERT_TRIED_CHANGE_NCHANS_FULL;

						jvxErrorType resLocal = given.con_link.connect_to->transfer_forward_icon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryLast JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
						if (resLocal == JVX_NO_ERROR)
						{
							acceptInputParams(fromPrevious, nullptr);
							copyOutFromIn(in);
							prepareForForward(given);
							acceptSingleAsItIs = false;

						}
					}
				} // if (res == JVX_NO_ERROR) -> a compromise does not really help!!
				jvx_bitFClear(tryLast.con_params.additional_flags);

				if (acceptSingleAsItIs)
				{
					// Accept it as it is
					stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_ACCEPTED_CHANGE_NCHANS]++;
					this->protoState = testProtocolStatus::AYF_CONVERT_ACCEPTED_CHANGE_NCHANS;
					in.nChans = fromPrevious->con_params.number_channels;
					out.nChans = preferredByOutput->con_params.number_channels;

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE					
					protoNegotiate += "<CjvxInputOutputSettings> ==> Conversion activated from <";
					protoNegotiate += jvx_size2String(in.nChans);
					protoNegotiate += "> to <";
					protoNegotiate += jvx_size2String(out.nChans);
					protoNegotiate += "> \n";
#endif

					requiresUpdateOutput = true;
					prepareForForward(given);
					// No update on the output side!! We will need to run the converter for this mismatch but it is ok			
				}
			}

			// ===================================================================================
			// ===================================================================================
			
			acceptSingleAsItIs = true;
			if ((fromPrevious->con_params.buffersize != preferredByOutput->con_params.buffersize) ||
				(fromPrevious->con_params.rate != preferredByOutput->con_params.rate)
				)
			{
				jvx_fixed_resampler_init_conversion(
					&resampling.cc,
					fromPrevious->con_params.rate,
					preferredByOutput->con_params.rate);

				jvxSize bSizeOutput = preferredByOutput->con_params.buffersize;
				jvxSize bSizeInput = fromPrevious->con_params.buffersize;

				switch (fixedLocationMode)
				{
				case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_OUTPUT:

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE					
					protoNegotiate += "<CjvxInputOutputSettings> Mode to adapt parameters on output side.\n";
#endif

					// We derive the output buffersize from the input buffering size
					// Input side will not be checked, it is the output side that needs to adapt
					this->computeBufferRelations(true, &bSizeInput);
					out.rate = preferredByOutput->con_params.rate;
					out.bSize = resampling.bSizeOutMax;

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE					
					protoNegotiate += "<CjvxInputOutputSettings> ==> Conversion activated from <";
					protoNegotiate += jvx_size2String(in.rate);
					protoNegotiate += "/";
					protoNegotiate += jvx_size2String(in.bSize);
					protoNegotiate += "> to <";
					protoNegotiate += jvx_size2String(out.rate);
					protoNegotiate += "/";
					protoNegotiate += jvx_size2String(out.bSize);
					protoNegotiate += "> \n";
#endif

					requiresUpdateOutput = true;
					prepareForForward(given);

					// This, we will pass forward to the next component
					break;

				case jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT:

					// If we keep the input rate fixed, we need to try a new buffersize in the input side!!
					this->computeBufferRelations(false, &bSizeOutput);
					tryLast.con_params = fromPrevious->con_params;
					tryLast.con_params.buffersize = resampling.bSizeInMax;
					tryLast.con_params.segmentation.x = resampling.bSizeInMax;
					jvx_bitZSet(tryLast.con_params.additional_flags, (jvxSize)jvxDataLinkDescriptorTestFlags::JVX_LINKDATA_TEST_SINGLE_BSIZE_SHIFT);
					jvx_bitSet(tryLast.con_params.additional_flags, (jvxSize)jvxDataLinkDescriptorTestFlags::JVX_LINKDATA_TEST_SINGLE_RATE_SHIFT);

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
					std::string paramsLast = jvx_linkDataParams2String(&tryLast);
					protoNegotiate += "<CjvxInputOutputSettings> Trying to adapt *buffersize/rate* on predecessor: " + paramsLast + "\n";
#endif

					res = fromPrevious->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryLast JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
					protoNegotiate += "\t\tResult from predecessor: <";
					protoNegotiate += jvxErrorType_descr((jvxSize)res);
					protoNegotiate += ">\n";
#endif

					if (res == JVX_NO_ERROR)
					{
						if (forwardCompromise)
						{
							stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_TRIED_CHANGE_RATE_BUFFERSIZE_FULL]++;
							this->protoState = testProtocolStatus::AYF_CONVERT_TRIED_CHANGE_RATE_BUFFERSIZE_FULL;

							jvxErrorType resLocal = given.con_link.connect_to->transfer_forward_icon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryLast JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
							if (resLocal == JVX_NO_ERROR)
							{
								acceptInputParams(fromPrevious, nullptr);
								copyOutFromIn(in);
								prepareForForward(given);
								acceptSingleAsItIs = false;
							}
						}
					} // if (res == JVX_NO_ERROR) -> a compromise does not really help!!
					jvx_bitFClear(tryLast.con_params.additional_flags);

					if (acceptSingleAsItIs)
					{
						// Accept it as it is
						stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_ACCEPTED_CHANGE_RATE_BUFFERSIZE]++;
						this->protoState = testProtocolStatus::AYF_CONVERT_ACCEPTED_CHANGE_RATE_BUFFERSIZE;
						in.rate = fromPrevious->con_params.rate;
						in.bSize = fromPrevious->con_params.buffersize;
						out.rate = preferredByOutput->con_params.rate;
						out.bSize = preferredByOutput->con_params.buffersize;

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE					
						protoNegotiate += "<CjvxInputOutputSettings> ==> Conversion activated from <";
						protoNegotiate += jvx_size2String(in.rate);
						protoNegotiate += "/";
						protoNegotiate += jvx_size2String(in.bSize);
						protoNegotiate += "> to <";
						protoNegotiate += jvx_size2String(out.rate);
						protoNegotiate += "/";
						protoNegotiate += jvx_size2String(out.bSize);
						protoNegotiate += "> \n";
#endif

						requiresUpdateOutput = true;
						prepareForForward(given);

						// No update on the output side!! We will need to run the converter for this mismatch but it is ok			
					}
				}
			}	


			// Case where the format deviates from input to output
			if (fromPrevious->con_params.data_flow != preferredByOutput->con_params.data_flow)
			{
				tryLast.con_params = fromPrevious->con_params;
				tryLast.con_params.data_flow = preferredByOutput->con_params.data_flow;
				jvx_bitZSet(tryLast.con_params.additional_flags, (jvxSize)jvxDataLinkDescriptorTestFlags::JVX_LINKDATA_TEST_SINGLE_FORMAT_SHIFT);
				res = fromPrevious->con_link.connect_from->transfer_backward_ocon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryLast JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res == JVX_NO_ERROR)
				{
					// Here, it is very likely that the input side can switch the format to the desired format
					if (forwardCompromise)
					{
						stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_TRIED_CHANGE_DATAFLOW_FULL]++;
						this->protoState = testProtocolStatus::AYF_CONVERT_TRIED_CHANGE_DATAFLOW_FULL;

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
						std::string paramsLast = jvx_linkDataParams2String(&tryLast);
						protoNegotiate += "<CjvxInputOutputSettings> Trying to adapt *dataflow* on predecessor: " + paramsLast + "\n";
#endif
						jvxErrorType resLocal = given.con_link.connect_to->transfer_forward_icon(JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &tryLast JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
						protoNegotiate += "\t\tResult from predecessor: <";
						protoNegotiate += jvxErrorType_descr((jvxSize)res);
						protoNegotiate += ">\n";
#endif
						if (resLocal == JVX_NO_ERROR)
						{
							acceptInputParams(fromPrevious, nullptr);
							copyOutFromIn(in);
							prepareForForward(given);
							acceptSingleAsItIs = false;

						}
					}
				} // if (res == JVX_NO_ERROR) -> a compromise does not really help!!
				jvx_bitFClear(tryLast.con_params.additional_flags);

				if (acceptSingleAsItIs)
				{
					// Accept it as it is
					stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_ACCEPTED_CHANGE_DATAFLOW]++;
					this->protoState = testProtocolStatus::AYF_CONVERT_ACCEPTED_CHANGE_DATAFLOW;
					in.datFlow = fromPrevious->con_params.data_flow;
					out.datFlow = preferredByOutput->con_params.data_flow;

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE					
					protoNegotiate += "<CjvxInputOutputSettings> ==> Conversion activated from <";
					protoNegotiate += jvxDataflow_txt(in.datFlow);
					protoNegotiate += "> to <";
					protoNegotiate += jvxDataflow_txt(out.datFlow);
					protoNegotiate += "> \n";
#endif

					requiresUpdateOutput = true;
					prepareForForward(given);
					// No update on the output side!! We will need to run the converter for this mismatch but it is ok			
				}
			}


		}
		tryMatchSingle = false;
	}

	res = JVX_ERROR_UNSUPPORTED;
	if (!checkIdentical(onStart, given))
	{
		// Nothing has changed, this will not be 
		res = JVX_ERROR_COMPROMISE;
	}

	if (checkIdentical(given, *preferredByOutput))
	{
		// Full match
		res = JVX_NO_ERROR;
	}

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
	std::string paramsLast = jvx_linkDataParams2String(&given);
	protoNegotiate += "<CjvxInputOutputSettings> Leaving with result <";
	protoNegotiate += jvxErrorType_descr(res);
	protoNegotiate += "> and parameters: " + paramsLast + "\n";
#endif
	return res;
}

jvxCBitField 
CjvxInputOutputSettings::checkDeviation(jvxLinkDataDescriptor* data, CjvxInputOutputSettings::procParams& params)
{
	jvxCBitField retVal = 0;
	if (data->con_params.buffersize != params.bSize)
	{
		jvx_bitSet(retVal, (jvxSize)convertCfgFlags::AYF_BSIZE_SHIFT);
	}
	if (data->con_params.rate != params.rate)
	{
		jvx_bitSet(retVal, (jvxSize)convertCfgFlags::AYF_SRATE_SHIFT);
	}
	if (data->con_params.number_channels != params.nChans)
	{
		jvx_bitSet(retVal, (jvxSize)convertCfgFlags::AYF_NUM_CHANNELS_SHIFT);
	}
	if (data->con_params.number_channels != params.nChans)
	{
		jvx_bitSet(retVal, (jvxSize)convertCfgFlags::AYF_NUM_CHANNELS_SHIFT);
	}
	if (data->con_params.format != params.nChans)
	{
		jvx_bitSet(retVal, (jvxSize)convertCfgFlags::AYF_FORMAT_SHIFT);
	}
	if (data->con_params.format_group != params.subForm)
	{
		jvx_bitSet(retVal, (jvxSize)convertCfgFlags::AYF_SUBFORMAT_SHIFT);
	}
	if (data->con_params.data_flow != params.datFlow)
	{
		jvx_bitSet(retVal, (jvxSize)convertCfgFlags::AYF_DATAFLOW_SHIFT);
	}
	return retVal;
}

void
CjvxInputOutputSettings::acceptInputParams(jvxLinkDataDescriptor* data, procParams* inTo)
{
	procParams* to = inTo;
	if (to == nullptr)
	{
		to = &in;
	}

	if (jvx_bitTest(in.currentDeviationState, (jvxSize)convertCfgFlags::AYF_BSIZE_SHIFT))
	{
		to->bSize = data->con_params.buffersize;
	}
	if (jvx_bitTest(in.currentDeviationState, (jvxSize)convertCfgFlags::AYF_SRATE_SHIFT))
	{
		to->rate = data->con_params.rate;
	}
	if (jvx_bitTest(in.currentDeviationState, (jvxSize)convertCfgFlags::AYF_NUM_CHANNELS_SHIFT))
	{
		to->nChans = data->con_params.number_channels;
	}
	if (jvx_bitTest(in.currentDeviationState, (jvxSize)convertCfgFlags::AYF_FORMAT_SHIFT))
	{
		to->form = data->con_params.format;
	}
	if (jvx_bitTest(in.currentDeviationState, (jvxSize)convertCfgFlags::AYF_SUBFORMAT_SHIFT))
	{
		to->subForm = data->con_params.format_group;
	}
	if (jvx_bitTest(in.currentDeviationState, (jvxSize)convertCfgFlags::AYF_DATAFLOW_SHIFT))
	{
		to->datFlow = data->con_params.data_flow;
	}
	protoState = testProtocolStatus::AYF_CONVERT_INPUT_ACCEPTED;
	stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_INPUT_ACCEPTED]++;
}

void
CjvxInputOutputSettings::inputToOutput()
{
	requiresUpdateOutput = false;
	if (in.currentDeviationState)
	{
		copyOutFromIn(in);
		requiresUpdateOutput = true;
	}
	protoState = testProtocolStatus::AYF_CONVERT_INPUT_TO_OUTPUT;
	stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_INPUT_TO_OUTPUT]++;
}

void 
CjvxInputOutputSettings::prepareForForward(jvxLinkDataDescriptor& data)
{
	if (requiresUpdateOutput)
	{
		data.con_params.buffersize = out.bSize;
		data.con_params.rate = out.rate;
		data.con_params.number_channels = out.nChans;
		data.con_params.format = out.form;
		data.con_params.format_group = out.subForm;
		data.con_params.data_flow = out.datFlow;

		data.con_params.segmentation.x = out.bSize; // <- Audio specifc
		data.con_params.segmentation.y = 1;			// <- Audio specifc
	}

	protoState = testProtocolStatus::AYF_CONVERT_OUTPUT_PREPARED;
	stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_OUTPUT_PREPARED]++;

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
	std::string paramsFwd = jvx_linkDataParams2String(&data);
	protoNegotiate += "<CjvxInputOutputSettings> Forwarding towards successor with parameters: " + paramsFwd + "\n";
#endif

}

// ======================================================================
// ======================================================================
// ======================================================================

void
CjvxInputOutputSettings::copyOutFromIn(const procParams& inArg)
{
	out.bSize = inArg.bSize;
	out.rate = inArg.rate;
	out.nChans = inArg.nChans;
	out.form = inArg.form;
	out.subForm = inArg.subForm;
	out.datFlow = inArg.datFlow;

	resampling.cc.gcd = 1;
	resampling.cc.downsamplingFactor = 1;
	resampling.cc.oversamplingFactor = 1;

	// Regular setup for resampler
	computeBufferRelations(true, nullptr);
}


void
CjvxInputOutputSettings::computeBufferRelations(jvxBool fromInputToOutput, jvxSize* bsizeArgOpt)
{
	jvxSize bSize = 0;
	if (fromInputToOutput)
	{
		bSize = in.bSize;
		if (bsizeArgOpt)
		{
			bSize = *bsizeArgOpt;
		}
		resampling.bSizeInMin = bSize;
		resampling.bSizeInMax = bSize;
		if (bSize % resampling.cc.downsamplingFactor)
		{
			// Round buffersize
			resampling.bSizeInMin = bSize / resampling.cc.downsamplingFactor;
			resampling.bSizeInMin *= resampling.cc.downsamplingFactor;
			resampling.bSizeInMax = resampling.bSizeInMin + resampling.cc.downsamplingFactor;
		}
		resampling.bSizeOutMin = resampling.bSizeInMin * resampling.cc.oversamplingFactor / resampling.cc.downsamplingFactor;
		resampling.bSizeOutMax = resampling.bSizeInMax * resampling.cc.oversamplingFactor / resampling.cc.downsamplingFactor;
	}
	else
	{
		bSize = out.bSize;
		if (bsizeArgOpt)
		{
			bSize = *bsizeArgOpt;
		}
		resampling.bSizeOutMin = bSize;
		resampling.bSizeOutMax = bSize;
		if (bSize % resampling.cc.oversamplingFactor)
		{
			resampling.bSizeOutMin = bSize / resampling.cc.oversamplingFactor;
			resampling.bSizeOutMin *= resampling.cc.oversamplingFactor;
			resampling.bSizeOutMax = resampling.bSizeOutMin + resampling.cc.oversamplingFactor;
		}
		resampling.bSizeInMin = resampling.bSizeOutMin * resampling.cc.downsamplingFactor / resampling.cc.oversamplingFactor;
		resampling.bSizeInMax = resampling.bSizeOutMax * resampling.cc.downsamplingFactor / resampling.cc.oversamplingFactor;
	}
}

jvxBool 
CjvxInputOutputSettings::checkIdentical(jvxLinkDataDescriptor& theOne, jvxLinkDataDescriptor& theOther)
{
	jvxBool identicalContent = true;
	if (theOne.con_params.buffersize != theOther.con_params.buffersize)
	{
		identicalContent = false;
	}
	if (theOne.con_params.rate != theOther.con_params.rate)
	{
		identicalContent = false;
	}
	if (theOne.con_params.number_channels != theOther.con_params.number_channels)
	{
		identicalContent = false;
	}
	if (theOne.con_params.format != theOther.con_params.format)
	{
		identicalContent = false;
	}
	if (theOne.con_params.format_group != theOther.con_params.format_group)
	{
		identicalContent = false;
	}
	if (theOne.con_params.data_flow != theOther.con_params.data_flow)
	{
		identicalContent = false;
	}
	return identicalContent;
}

void
CjvxInputOutputSettings::copyInputArgs(CjvxSimpleParameters& args)
{
	args.number_channels = in.nChans;
	args.subformat = in.subForm;
	args.format = in.form;
	args.samplerate = in.rate;
	args.buffersize = in.bSize;
	args.data_flow = in.datFlow;
}

void
CjvxInputOutputSettings::copyOutputArgs(CjvxSimpleParameters& args)
{
	args.number_channels = out.nChans;
	args.subformat = out.subForm;
	args.format = out.form;
	args.samplerate = out.rate;
	args.buffersize = out.bSize;
	args.data_flow = out.datFlow;
}