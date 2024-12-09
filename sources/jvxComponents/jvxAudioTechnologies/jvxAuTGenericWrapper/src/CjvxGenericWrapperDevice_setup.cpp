#include "CjvxGenericWrapperTechnology.h"
#include "CjvxGenericWrapperDevice.h"

void
CjvxGenericWrapperDevice::updateSWSamplerateAndBuffersize_nolock(jvxLinkDataDescriptor* fromDevice, jvxLinkDataDescriptor* toDevice
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	int i,j;
	jvxInt32 tmp = 0;
	jvxInt32 idxFound = -1;
	jvxInt32 delta = JVX_MAX_INT_32_INT32;
	jvxData relation = 1.0;
	jvxBool mayFindValidSetup = false;
	JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ(fdb, static_cast<IjvxObject*>(this));
	JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_I(fdb, fromDevice);

	processingControl.computedParameters.bSize_sw = CjvxAudioDevice_genpcg::properties_active.buffersize.value;
	processingControl.computedParameters.sRate_sw = CjvxAudioDevice_genpcg::properties_active.samplerate.value;
	processingControl.computedParameters.form_sw = (jvxDataFormat)CjvxAudioDevice_genpcg::properties_active.format.value;

	processingControl.computedParameters.bSize_hw = processingControl.computedParameters.bSize_sw;
	processingControl.computedParameters.sRate_hw = processingControl.computedParameters.sRate_sw;
	processingControl.computedParameters.form_hw = (jvxDataFormat)processingControl.computedParameters.form_sw;

	if (onInit.connectedDevice)
	{
		if (fromDevice)
		{
			if (JVX_EVALUATE_BITFIELD(genGenericWrapper_device::properties_active.autoUseHwParams.value.selection() & 0x1))
			{

				jvxBool thereisamismatch = false;
				if (
					(processingControl.computedParameters.bSize_hw != fromDevice->con_params.buffersize) ||
					(processingControl.computedParameters.sRate_hw != fromDevice->con_params.rate) ||
					(processingControl.computedParameters.form_hw != fromDevice->con_params.format))
				{
					jvxLinkDataDescriptor locDescr;
					locDescr.con_params.segmentation.x = processingControl.computedParameters.bSize_hw;
					locDescr.con_params.segmentation.y = 1;
					locDescr.con_params.buffersize = processingControl.computedParameters.bSize_hw;
					locDescr.con_params.rate = processingControl.computedParameters.sRate_hw;
					locDescr.con_params.format = processingControl.computedParameters.form_hw;
					locDescr.con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;
					locDescr.con_params.number_channels = fromDevice->con_params.number_channels;

					// Try to negotiate
					jvxErrorType res = fromDevice->con_link.connect_from->transfer_backward_ocon(
						JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, &locDescr JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					if (res != JVX_NO_ERROR)
					{
						std::cout << "Auto hardware setting failed!" << std::endl;
					}
				}
			}
			processingControl.computedParameters.bSize_hw = JVX_SIZE_INT32(fromDevice->con_params.buffersize);
			processingControl.computedParameters.sRate_hw = JVX_SIZE_INT32(fromDevice->con_params.rate);
			processingControl.computedParameters.form_hw = fromDevice->con_params.format;

		}
		else
		{
			if (JVX_EVALUATE_BITFIELD(genGenericWrapper_device::properties_active.autoUseHwParams.value.selection() & 0x1))
			{
				idxFound = -1;
				delta = JVX_MAX_INT_32_INT32;
				if (runtime.props_hw.samplerate.sellist_exists)
				{
					for (i = 0; i < runtime.props_hw.samplerate.entries.size(); i++)
					{
						jvxInt32 dd = abs(processingControl.computedParameters.sRate_sw - runtime.props_hw.samplerate.entries[i]);
						if (dd < delta)
						{
							idxFound = i;
							delta = dd;
						}
					}
					assert(idxFound >= 0);
					runtime.props_hw.samplerate.selection_id = idxFound;
					relation = (jvxData)runtime.props_hw.samplerate.entries[runtime.props_hw.samplerate.selection_id] /
						(jvxData)processingControl.computedParameters.sRate_sw;
				}
				else
				{
					runtime.props_hw.samplerate.entry = processingControl.computedParameters.sRate_sw;
				}
				lock_settings();
				pushProperty_samplerate_nolock();
				pullProperty_samplerate_nolock();
				unlock_settings();


				idxFound = -1;
				delta = JVX_MAX_INT_32_INT32;
				if (runtime.props_hw.buffersize.sellist_exists)
				{
					for (i = 0; i < runtime.props_hw.buffersize.entries.size(); i++)
					{
						jvxInt32 dd = JVX_DATA2INT32(fabs(relation * processingControl.computedParameters.bSize_sw - runtime.props_hw.buffersize.entries[i]));
						if (dd < delta)
						{
							idxFound = i;
							delta = dd;
						}
					}
					assert(idxFound >= 0);
					runtime.props_hw.buffersize.selection_id = idxFound;
				}
				else
				{
					runtime.props_hw.buffersize.entry = JVX_DATA2INT32(relation * processingControl.computedParameters.bSize_sw);
				}
				lock_settings();
				pushProperty_buffersize_nolock();
				pullProperty_buffersize_nolock();
				unlock_settings();



				idxFound = -1;
				delta = JVX_MAX_INT_32_INT32;
				if (runtime.props_hw.format.sellist_exists)
				{
					for (i = 0; i < runtime.props_hw.format.entries.size(); i++)
					{
						if (processingControl.computedParameters.form_sw == runtime.props_hw.format.entries[i])
						{
							idxFound = i;
							break;
						}
					}
					if (idxFound >= 0)
					{
						runtime.props_hw.format.selection_id = idxFound;
					}
					else
					{
						// Default: set default format to jvxData
						runtime.props_hw.format.selection_id = 0;
					}
				}
				else
				{
					runtime.props_hw.format.entry = processingControl.computedParameters.form_sw;
				}
				lock_settings();
				pushProperty_format_nolock();
				pullProperty_format_nolock();
				unlock_settings();
			}
			processingControl.computedParameters.bSize_hw = runtime.props_hw.buffersize.entry;
			if (runtime.props_hw.buffersize.sellist_exists)
			{
				processingControl.computedParameters.bSize_hw = runtime.props_hw.buffersize.entries[runtime.props_hw.buffersize.selection_id];
			}
			else
			{
				processingControl.computedParameters.bSize_hw = runtime.props_hw.buffersize.entry;
			}
			processingControl.computedParameters.sRate_hw = runtime.props_hw.samplerate.entry;
			if (runtime.props_hw.samplerate.sellist_exists)
			{
				processingControl.computedParameters.sRate_hw = runtime.props_hw.samplerate.entries[runtime.props_hw.samplerate.selection_id];
			}
			else
			{
				processingControl.computedParameters.sRate_hw = runtime.props_hw.samplerate.entry;
			}

			if (runtime.props_hw.format.sellist_exists)
			{
				processingControl.computedParameters.form_hw = runtime.props_hw.format.entries[runtime.props_hw.format.selection_id];
			}
			else
			{
				processingControl.computedParameters.form_hw = (jvxDataFormat)runtime.props_hw.format.entry;
			}
		}
	}

/*
	if(genGenericWrapper_device::properties_active.autoUseHwParams.value.selection & 0x1)
	{


		// Take over processing parameters from wrapped device
		processingControl.computedParameters.bSize_sw = processingControl.computedParameters.bSize_hw;
		processingControl.computedParameters.sRate_sw = processingControl.computedParameters.sRate_hw;
		processingControl.computedParameters.form_sw = processingControl.computedParameters.form_hw;

		CjvxAudioDevice_genpcg::properties_active.buffersize.value = processingControl.computedParameters.bSize_hw;
		CjvxAudioDevice_genpcg::properties_active.samplerate.value = processingControl.computedParameters.sRate_hw;
		CjvxAudioDevice_genpcg::properties_active.format.value = processingControl.computedParameters.form_hw;

	}
*/

	processingControl.computedParameters.downsamplingFactor = (jvxData)processingControl.computedParameters.sRate_sw/(jvxData)processingControl.computedParameters.sRate_hw;
	processingControl.computedParameters.bSize_hw_ds = (jvxInt32)(((jvxData)processingControl.computedParameters.bSize_hw*processingControl.computedParameters.downsamplingFactor)+0.5);

	if((runtime.props_user.autosetup.forceSyncMode)&&(onInit.connectedDevice))
	{
		assert(0);
		jvxSize numBSizes = 0;
		jvxSize numSRates = 0;

		if(runtime.props_hw.buffersize.sellist_exists)
		{
			numBSizes = runtime.props_hw.buffersize.entries.size();
		}
		if(runtime.props_hw.samplerate.sellist_exists)
		{
			numSRates = runtime.props_hw.samplerate.entries.size();
		}

		if(runtime.props_user.autosetup.autofs)
		{
			// setup the next higher allowed samplerate if possible
			if(numSRates > 0)
			{
				int idxSelect = -1;
				jvxInt32 rateChoice = 0;
				for(j = 0; j < numSRates; j++)
				{
					rateChoice = runtime.props_hw.samplerate.entries[j];
					if(rateChoice >= processingControl.computedParameters.sRate_sw)
					{
						idxSelect = j;
						break;
					}
				}
				if(idxSelect >= 0)
				{
					runtime.props_hw.samplerate.selection_id = idxSelect;
				}
			}
		}

		if(numBSizes >= 1)
		{

			jvxInt32 oneSize = runtime.props_hw.buffersize.entries[0];

			jvxInt32 maxSize = oneSize;
			jvxInt32 minSize = oneSize;

			for(i = 1; i < numBSizes; i++)
			{
				 oneSize = runtime.props_hw.buffersize.entries[i];

				if(oneSize > maxSize)
				{
					maxSize = oneSize;
				}
				if(oneSize < minSize)
				{
					minSize = oneSize;
				}
			}

			int cnt = 1;
			bool doCont = true;
			int targetBSize = 0;

			jvxData relativeDeviation = 0.0;

			jvxData oneSamplerate = 0.0;

			jvxData deviation_min = runtime.props_user.autosetup.deviationFsMax;// 10 percent or less deviation
			int numFrames_min = -1;
			int idBsize_min = -1;

			while(doCont)
			{
				int targetBSize = cnt * processingControl.computedParameters.bSize_sw;
				if( targetBSize > maxSize)
				{
					doCont = false;
				}

				if(cnt == runtime.props_user.autosetup.numLoopsMax)
				{
					doCont = false;
				}

				jvxData limDecision = 0.0;
				if(runtime.props_user.autosetup.numLoopsMax > 1)
				{
					limDecision = (jvxData)runtime.props_user.autosetup.disadvantageLoops *
						(((jvxData)cnt-1.0)/((jvxData)runtime.props_user.autosetup.numLoopsMax-1.0)) *
						(jvxData)runtime.props_user.autosetup.deviationFsMax;
				}

				for(i = 0; i < numBSizes; i++)
				{
					oneSize = runtime.props_hw.buffersize.entries[i];
					oneSamplerate = (jvxData)targetBSize/(jvxData)(oneSize) * (jvxData)processingControl.computedParameters.sRate_hw;
					relativeDeviation = fabs(processingControl.computedParameters.sRate_sw - oneSamplerate)/(jvxData)processingControl.computedParameters.sRate_sw;
					//if(relativeDeviation <= deviation_min)

					relativeDeviation += limDecision;// If we loop, it might have to be a "real" benefit

					if((deviation_min - relativeDeviation) >= 0.0)
					{
						// if two setups lead to the same frequency deviation, use the one that does not require resampling
						if(relativeDeviation == deviation_min)
						{
							// Check whether resampling is required
							if(targetBSize == oneSize)
							{
								// Check for multiples of buffersize
								jvxInt32 framesizeOld = runtime.props_hw.buffersize.entries[idBsize_min];
								if((oneSize % framesizeOld) != 0)
								{
									// Only if this is a really new situation and not just a multiple of the buffersize, accept the new minimum
									deviation_min = relativeDeviation;
									numFrames_min = cnt;
									idBsize_min = i;
								}
							}
						}
						else
						{
							deviation_min = relativeDeviation;
							numFrames_min = cnt;
							idBsize_min = i;
						}
					}
				}
				cnt++;
			}

			// If we found one solution
			if(numFrames_min > 0)
			{
				 runtime.props_hw.buffersize.selection_id = idBsize_min;

				// Adapt the buffersize
				processingControl.computedParameters.bSize_hw_ds = numFrames_min*processingControl.computedParameters.bSize_sw;
			}
		}
		// otherwise, module accepts all buffersizes
	}

	// Avoid division by zero
	if(processingControl.computedParameters.bSize_hw_ds == 0)
	{
		processingControl.computedParameters.bSize_hw_ds = 1;
	}

	// Update of downsampling factor acc. to buffersizes
	processingControl.computedParameters.downSamplingFactor_real = (jvxData)processingControl.computedParameters.bSize_hw_ds/(jvxData)processingControl.computedParameters.bSize_hw;
	processingControl.computedParameters.sRate_sw_real = processingControl.computedParameters.sRate_hw * processingControl.computedParameters.downSamplingFactor_real;

	processingControl.computedParameters.dacon_required = true;
	if(processingControl.computedParameters.form_hw == processingControl.computedParameters.form_sw)
	{
		processingControl.computedParameters.dacon_required = false;
	}
	processingControl.computedParameters.resampler_required = true;
	if(processingControl.computedParameters.bSize_hw == processingControl.computedParameters.bSize_hw_ds)
	{
		processingControl.computedParameters.resampler_required = false;
	}

	processingControl.computedParameters.timebasechange_required = true;
	processingControl.computedParameters.num_loops = 1;

	if(processingControl.computedParameters.bSize_hw_ds == processingControl.computedParameters.bSize_sw)
	{
		processingControl.computedParameters.timebasechange_required = false;
		processingControl.computedParameters.num_loops = 1;
		processingControl.computedParameters.bSize_sw = processingControl.computedParameters.bSize_hw_ds;
	}

	// Compute the fill height for circular buffer at start up (which causes the delay)
	processingControl.computedParameters.qual_sc_factor = genGenericWrapper_device::properties_active_higher.estimateSCQuality.value.val();
	processingControl.computedParameters.fillHeightInput = 0;
	processingControl.computedParameters.fillHeightOutput = /*runtime.props_user.resampler.loadEstimate*/
		JVX_DATA2INT32(genGenericWrapper_device::properties_active_higher.estimateLoad.value.val() *
			processingControl.computedParameters.qual_sc_factor * 0.01 * processingControl.computedParameters.bSize_sw +
			processingControl.computedParameters.bSize_sw + processingControl.computedParameters.bSize_hw_ds);

	// Determine resampler delay (input and output - is jvxData)
	processingControl.computedParameters.delayFilterResamplingInput = -1;
	processingControl.computedParameters.delayFilterResamplingOutput = -1;
	if(processingControl.computedParameters.resampler_required)
	{
		processingControl.computedParameters.delayFilterResamplingInput = 0.0;
		processingControl.resampler_proto.theHdl->getSignalDelay_precompute(
			&processingControl.computedParameters.delayFilterResamplingInput,
			processingControl.computedParameters.bSize_hw,
			processingControl.computedParameters.bSize_hw_ds,
			runtime.props_user.resampler.orderFilterResamplingInput);
		processingControl.computedParameters.delayFilterResamplingOutput = 0.0;
			processingControl.resampler_proto.theHdl->getSignalDelay_precompute(
				&processingControl.computedParameters.delayFilterResamplingOutput,
				processingControl.computedParameters.bSize_hw_ds,
				processingControl.computedParameters.bSize_hw,
			runtime.props_user.resampler.orderFilterResamplingOutput);
	}

	if(!processingControl.computedParameters.dacon_required)
	{
		if(!processingControl.computedParameters.resampler_required)
		{
			processingControl.operationDevice = OPERATION_MODE_1;
		}
		else
		{
			processingControl.operationDevice = OPERATION_MODE_3;
		}
	}
	else
	{
		if(!processingControl.computedParameters.resampler_required)
		{
			processingControl.operationDevice = OPERATION_MODE_2;
		}
		else
		{
			processingControl.operationDevice = OPERATION_MODE_4;
		}
	}

	if(processingControl.computedParameters.timebasechange_required)
	{
		processingControl.timebaseDevice = TIMEBASE_CHANGE;
	}
	else
	{
		processingControl.timebaseDevice = NO_TIMEBASE_CHANGE;
	}

	processingControl.clockDevice = TRIGGER_INTERN;

	tmp = 0;
	this->numberActiveHardwareChannelsInput(tmp);
	if(tmp != 0)
	{
		processingControl.clockDevice = TRIGGER_DEVICE;
	}
	tmp = 0;
	this->numberActiveHardwareChannelsOutput(tmp);

	if(tmp != 0)
	{
		processingControl.clockDevice = TRIGGER_DEVICE;
	}

	/*
	for(j = 0; j < (int)this->shareWithUI.inputRoutingTable.size(); j++)
	{
		this->shareWithUI.inputRoutingTable[j].mappedIDActive = -1;
	}

	for(j = 0; j < (int)this->shareWithUI.outputRoutingTable.size(); j++)
	{
		this->shareWithUI.outputRoutingTable[j].mappedIDActive = -1;
	}

	rtpInt32 numChansIn = 0;
	if(refLink)
	{
		refLink->getNumberMaxChannelsInputDevice(&numChansIn);
	}

	int mapCnt = 0;
	for( i = 0; i < numChansIn; i++)
	{
		for(j = 0; j <(int) this->shareWithUI.inputRoutingTable.size(); j++)
		{
			if(shareWithUI.inputRoutingTable[j].isHardware)
			{
				int id = shareWithUI.inputRoutingTable[j].mappedID;
				if(id == i)
				{
					bool isActive = false;
					refLink->getStatusChannelInputDevice(id, &isActive);
					if(isActive)
					{
						shareWithUI.inputRoutingTable[j].mappedIDActive = mapCnt++;
						break;
					}
				}
			}
		}
	}

	rtpInt32 numChansOut = 0;
	if(refLink)
	{
		refLink->getNumberMaxChannelsOutputDevice(&numChansOut);
	}

	mapCnt = 0;
	for( i = 0; i < numChansOut; i++)
	{
		for(j = 0; j < (int)this->shareWithUI.outputRoutingTable.size(); j++)
		{
			if(shareWithUI.outputRoutingTable[j].isHardware)
			{
				int id = shareWithUI.outputRoutingTable[j].mappedID;
				if(id == i)
				{
					bool isActive = false;
					refLink->getStatusChannelOutputDevice(id, &isActive);
					if(isActive)
					{
						shareWithUI.outputRoutingTable[j].mappedIDActive = mapCnt++;
						break;
					}
				}
			}
		}
	}
	*/

	for(i = 0; i < (int)this->onInit.theFilesRef->theInputFiles.size();i++)
	{
		jvxInt32 sRate = this->onInit.theFilesRef->theInputFiles[i].common.samplerate;
		this->onInit.theFilesRef->theInputFiles[i].common.processing.bSize_rs =
		(int)(((jvxData)sRate/(jvxData)processingControl.computedParameters.sRate_sw * (jvxData)processingControl.computedParameters.bSize_sw) + 0.5);

		jvxData downSamplingFactor = (jvxData)this->onInit.theFilesRef->theInputFiles[i].common.processing.bSize_rs/(jvxData)processingControl.computedParameters.bSize_sw;

		this->onInit.theFilesRef->theInputFiles[i].common.processing.sRate_rs = processingControl.computedParameters.sRate_sw*downSamplingFactor;

		if(this->onInit.theFilesRef->theInputFiles[i].common.processing.bSize_rs != processingControl.computedParameters.bSize_sw)
			this->onInit.theFilesRef->theInputFiles[i].common.processing.resampling = true;
		else
			this->onInit.theFilesRef->theInputFiles[i].common.processing.resampling = false;
	}

	for(i = 0; i < (int)this->onInit.theFilesRef->theOutputFiles.size();i++)
	{
		jvxInt32 sRate = this->onInit.theFilesRef->theOutputFiles[i].common.samplerate;
		this->onInit.theFilesRef->theOutputFiles[i].common.processing.bSize_rs =
		(int)(((jvxData)sRate/(jvxData)processingControl.computedParameters.sRate_sw * (jvxData)processingControl.computedParameters.bSize_sw) + 0.5);

		jvxData downSamplingFactor = (jvxData)this->onInit.theFilesRef->theOutputFiles[i].common.processing.bSize_rs/(jvxData)processingControl.computedParameters.bSize_sw;

		this->onInit.theFilesRef->theOutputFiles[i].common.processing.sRate_rs = processingControl.computedParameters.sRate_sw*downSamplingFactor;

		if(this->onInit.theFilesRef->theOutputFiles[i].common.processing.bSize_rs != processingControl.computedParameters.bSize_sw)
			this->onInit.theFilesRef->theOutputFiles[i].common.processing.resampling = true;
		else
			this->onInit.theFilesRef->theOutputFiles[i].common.processing.resampling = false;
	}

	for(i = 0; i < (int)this->onInit.theExtRef->theInputReferences.size();i++)
	{
		jvxInt32 sRate = this->onInit.theExtRef->theInputReferences[i].samplerate;
		this->onInit.theExtRef->theInputReferences[i].processing.bSize_rs =
		(int)(((jvxData)sRate/(jvxData)processingControl.computedParameters.sRate_sw * (jvxData)processingControl.computedParameters.bSize_sw) + 0.5);

		jvxData downSamplingFactor = (jvxData)this->onInit.theExtRef->theInputReferences[i].processing.bSize_rs/(jvxData)processingControl.computedParameters.bSize_sw;

		this->onInit.theExtRef->theInputReferences[i].processing.sRate_rs = processingControl.computedParameters.sRate_sw*downSamplingFactor;

		if(this->onInit.theExtRef->theInputReferences[i].processing.bSize_rs != processingControl.computedParameters.bSize_sw)
			this->onInit.theExtRef->theInputReferences[i].processing.resampling = true;
		else
			this->onInit.theExtRef->theInputReferences[i].processing.resampling = false;
	}

	for(i = 0; i < (int)this->onInit.theExtRef->theOutputReferences.size();i++)
	{
		jvxInt32 sRate =this->onInit.theExtRef->theOutputReferences[i].samplerate;
		this->onInit.theExtRef->theOutputReferences[i].processing.bSize_rs =
		(int)(((jvxData)sRate/(jvxData)processingControl.computedParameters.sRate_sw * (jvxData)processingControl.computedParameters.bSize_sw) + 0.5);

		jvxData downSamplingFactor = (jvxData)this->onInit.theExtRef->theOutputReferences[i].processing.bSize_rs/(jvxData)processingControl.computedParameters.bSize_sw;

		this->onInit.theExtRef->theOutputReferences[i].processing.sRate_rs = processingControl.computedParameters.sRate_sw*downSamplingFactor;

		if(this->onInit.theExtRef->theOutputReferences[i].processing.bSize_rs != processingControl.computedParameters.bSize_sw)
			this->onInit.theExtRef->theOutputReferences[i].processing.resampling = true;
		else
			this->onInit.theExtRef->theOutputReferences[i].processing.resampling = false;
	}

	if(processingControl.computedParameters.resampler_required)
	{
		genGenericWrapper_device::properties_active.setupResampler.value.selection() = 0x1;
	}
	else
	{
		genGenericWrapper_device::properties_active.setupResampler.value.selection() = 0x2;
	}

	if(processingControl.computedParameters.timebasechange_required)
	{
		genGenericWrapper_device::properties_active.setupAsync.value.selection() = 0x1;
	}
	else
	{
		genGenericWrapper_device::properties_active.setupAsync.value.selection() = 0x2;
	}
	if (toDevice)
	{
		local_proc_params params;
		populate_countchannels_datatype(params);
		toDevice->con_params.buffersize = params.buffersize;
		toDevice->con_params.rate = params.samplerate;
		toDevice->con_params.format = params.format;
		toDevice->con_params.number_channels = params.chans_in;

		jvx_bitFClear(toDevice->con_params.additional_flags);
		toDevice->con_params.segmentation.x = toDevice->con_params.buffersize;
		toDevice->con_params.segmentation.y = 1;
		toDevice->con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;

		// release_countchannels_datatype(params); <- not require since local variable

		JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_O(fdb, toDevice);

	}
}


void
CjvxGenericWrapperDevice::numberActiveHardwareChannelsInput(jvxInt32& num)
{
	jvxSize i;

	num = 0;
	for(i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
	{
		if( runtime.channelMappings.inputChannelMapper[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE)
		{
			if(runtime.channelMappings.inputChannelMapper[i].isSelected)
			{
				num++;
			}
		}
	}
}

void
CjvxGenericWrapperDevice::numberActiveHardwareChannelsOutput(jvxInt32& num)
{
	jvxSize i;

	num = 0;
	for(i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
	{
		if( runtime.channelMappings.outputChannelMapper[i].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE)
		{
			if(runtime.channelMappings.outputChannelMapper[i].isSelected)
			{
				num++;
			}
		}
	}
}
