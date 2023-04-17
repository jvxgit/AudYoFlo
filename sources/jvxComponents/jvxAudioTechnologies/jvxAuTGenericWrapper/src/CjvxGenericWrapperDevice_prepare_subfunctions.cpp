#include "CjvxGenericWrapperDevice.h"
#include "CjvxGenericWrapperTechnology.h"
#include "CjvxGenericWrapper_common.h"
#include "jvx_dsp_base.h"
#include "jvx_misc/jvx_allocate_fields.h"

// ================================================================================
// ================================================================================
// ================================================================================
void
CjvxGenericWrapperDevice::allocateBufferA()
{
	jvxSize m, i;
	jvxSize szL;
	proc_fields.ptrA_in_sz = 0;
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_fields.ptrA_in_net, jvxByte**, processingControl.numBuffers_sender_to_receiver_hw);
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_fields.ptrA_in_raw, jvxByte**, processingControl.numBuffers_sender_to_receiver_hw);
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_fields.ptrA_in_off, jvxSize*, processingControl.numBuffers_sender_to_receiver_hw);
	for (m = 0; m < processingControl.numBuffers_sender_to_receiver_hw; m++)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_fields.ptrA_in_net[m], jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_in);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_fields.ptrA_in_raw[m], jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_in);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_fields.ptrA_in_off[m], jvxSize, processingControl.inProc.params_fixed_runtime.chanshw_in);
		//memset(proc_fields.ptrA_in[m], 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
		// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

		for (i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
		{

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Start the buffer A
			jvx_allocateDataLinkDescriptor_oneExtBuffer(&theRelocator._common_set_icon.theData_in->con_data.bExt,
				m, i, processingControl.computedParameters.bSize_hw, proc_fields.szElementHW,
				proc_fields.ptrA_in_raw, proc_fields.ptrA_in_net, proc_fields.ptrA_in_off, &szL);
			if (
				(m == 0) && (i == 0))
			{
				proc_fields.ptrA_in_sz = szL;
			}
			else
			{
				assert(proc_fields.ptrA_in_sz == szL);
			}
			//JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in[m][i], jvxByte, proc_fields.szElementHW * processingControl.computedParameters.bSize_hw);
			//memset(proc_fields.ptrA_in[m][i], 0, sizeof(jvxByte) * proc_fields.szElementHW * processingControl.computedParameters.bSize_hw);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
		}
	}
}

void
CjvxGenericWrapperDevice::allocateBufferA_out(jvxSize prepend, jvxSize append, jvxSize align)
{
	jvxSize m, i;
	jvxSize szL;
	proc_fields.ptrA_out_sz = 0;
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_fields.ptrA_out_net, jvxByte**, processingControl.numBuffers_receiver_to_sender_sw);
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_fields.ptrA_out_raw, jvxByte**, processingControl.numBuffers_receiver_to_sender_sw);
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_fields.ptrA_out_off, jvxSize*, processingControl.numBuffers_receiver_to_sender_sw);
	for (m = 0; m < processingControl.numBuffers_receiver_to_sender_sw; m++)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_fields.ptrA_out_net[m], jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_out);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_fields.ptrA_out_raw[m], jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_out);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(proc_fields.ptrA_out_off[m], jvxSize, processingControl.inProc.params_fixed_runtime.chanshw_out);
		//memset(proc_fields.ptrA_in[m], 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
		// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

		for (i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
		{

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Start the buffer A
			jvx_allocateField_c_a((jvxHandle**)&proc_fields.ptrA_out_raw[m][i], processingControl.computedParameters.bSize_sw,
				proc_fields.szElementSW, (prepend + append + align),
				&szL, NULL, NULL);
			intptr_t ptrI = (intptr_t)((jvxByte*)proc_fields.ptrA_out_raw[m][i]);
			intptr_t dec = 0;
			intptr_t inc = prepend;
			if (align)
			{
				// 0x12345 align 4: 0x12345 + 4 - 1
				inc += (align - 1);

				// Here is my attempt: 
				// convert pointer to int, do the align and compute the pointer decrement
				// and finally apply the decrement
				dec = (ptrI + inc) % align;
			}
			proc_fields.ptrA_out_off[m][i] = inc - dec;
			proc_fields.ptrA_out_net[m][i] =
				((jvxByte*)proc_fields.ptrA_out_raw[m][i] + proc_fields.ptrA_out_off[m][i]);
			if (
				(m == 0) && (i == 0))
			{
				proc_fields.ptrA_out_sz = szL;
			}
			else
			{
				assert(proc_fields.ptrA_out_sz == szL);
			}
		}
	}
}

void
CjvxGenericWrapperDevice::deallocateBufferA()
{
	jvxSize m, i;
	for (m = 0; m < processingControl.numBuffers_sender_to_receiver_hw; m++)
	{
		for (i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
		{
			jvx_deallocateDataLinkDescriptor_oneExtBuffer(NULL, m, i, proc_fields.ptrA_in_raw, proc_fields.ptrA_in_net, proc_fields.ptrA_in_off);
			//jvx_deallocateField_c_a((jvxHandle**)&proc_fields.ptrA_out_raw[m][i], NULL, NULL);
		}
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in_net[m], jvxByte*);
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in_raw[m], jvxByte*);
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in_off[m], jvxSize);
	}
	JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in_net, jvxByte**);
	JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in_raw, jvxByte**);
	JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in_off, jvxSize*);
	proc_fields.ptrA_in_sz = 0;
}

void
CjvxGenericWrapperDevice::deallocateBufferA_out()
{
	jvxSize m, i;
	jvxSize szL;
	for (m = 0; m < processingControl.numBuffers_receiver_to_sender_sw; m++)
	{
		//memset(proc_fields.ptrA_in[m], 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
		// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

		for (i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
		{

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Start the buffer A
			jvx_deallocateField_c_a((jvxHandle**)&proc_fields.ptrA_out_raw[m][i], NULL, NULL);
			proc_fields.ptrA_out_off[m][i] = 0;
			proc_fields.ptrA_out_net[m][i] = 0;

		}
		JVX_DSP_SAFE_DELETE_FIELD(proc_fields.ptrA_out_net[m]);
		JVX_DSP_SAFE_DELETE_FIELD(proc_fields.ptrA_out_raw[m]);
		JVX_DSP_SAFE_DELETE_FIELD(proc_fields.ptrA_out_off[m]);

	}
	JVX_DSP_SAFE_DELETE_FIELD(proc_fields.ptrA_out_net);
	JVX_DSP_SAFE_DELETE_FIELD(proc_fields.ptrA_out_raw);
	JVX_DSP_SAFE_DELETE_FIELD(proc_fields.ptrA_out_off);
	proc_fields.ptrA_out_sz = 0;
}
jvxErrorType
CjvxGenericWrapperDevice::prepare_outputfiles()
{
	jvxSize i,j,m;

	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	// =========================================================================
	// Find selected and unselected files
	// =========================================================================
	if(res == JVX_NO_ERROR)
	{
		for(i = 0; i < onInit.theFilesRef->theOutputFiles.size(); i++)
		{
			onInit.theFilesRef->theOutputFiles[i].common.processing.selected = false;
			for(j = 0; j < runtime.channelMappings.outputChannelMapper.size(); j++)
			{
				if(
					(runtime.channelMappings.outputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE) &&
					(runtime.channelMappings.outputChannelMapper[j].isSelected == true) &&
					(runtime.channelMappings.outputChannelMapper[j].refHint == onInit.theFilesRef->theOutputFiles[i].common.uniqueIdHandles)
					)
				{
					onInit.theFilesRef->theOutputFiles[i].common.processing.selected = true;
					break;
				}
			}
		}
	}

	// =========================================================================
	// Open output files and prepare processing
	// =========================================================================
	if(res == JVX_NO_ERROR)
	{
		for(i = 0; i < onInit.theFilesRef->theOutputFiles.size(); i++)
		{
			onInit.theFilesRef->theOutputFiles[i].common.processing.theStatus = FILE_INACTIVE;
			onInit.theFilesRef->theOutputFiles[i].common.processing.deltaT_avrg = 0;

			if(onInit.theFilesRef->theOutputFiles[i].common.processing.selected)
			{
				jvxInt32 numChans = JVX_SIZE_INT32(onInit.theFilesRef->theOutputFiles[i].common.number_channels);
				jvxSize szElm = jvxDataFormat_size[processingControl.inProc.params_fixed_runtime.format];
				jvxSize bSize = processingControl.computedParameters.bSize_sw;
				jvxSize bSizeRs = onInit.theFilesRef->theOutputFiles[i].common.processing.bSize_rs;
				jvxData outRate = (jvxData)onInit.theFilesRef->theOutputFiles[i].common.processing.sRate_rs;
				jvxData inRate = processingControl.computedParameters.sRate_sw_real;

				// ===========================================================
				// == Output file is openend for the first time
				// ===========================================================
				// Reopen it to specify buffersize
				onInit.theFilesRef->theOutputFiles[i].common.processing.theStatus = FILE_ACCESS;

				jvxFileDescriptionEndpoint_open endpoint_open;
				endpoint_open.numberChannels = onInit.theFilesRef->theOutputFiles[i].common.number_channels;// <- accept only wav files
				endpoint_open.samplerate = onInit.theFilesRef->theOutputFiles[i].common.samplerate;
				endpoint_open.fFormatFile = onInit.theFilesRef->theOutputFiles[i].common.fform_file; // <- accept only fixed point files
				endpoint_open.littleEndian = true; // <- set for little endian
				endpoint_open.subtype = onInit.theFilesRef->theOutputFiles[i].common.subformat;
				endpoint_open.numberBitsSample = onInit.theFilesRef->theOutputFiles[i].common.numBitsSample;
				
				onInit.theFilesRef->theOutputFiles[i].common.genericName = false;
				std::string pathRet;
				jvxAssignedFileTags tags;
				jvx_initTagName(tags);
				jvx_tagExprToTagStruct(onInit.theFilesRef->theOutputFiles[i].common.name, tags, pathRet);

				if(pathRet.empty())
				{
					onInit.theFilesRef->theOutputFiles[i].common.genericName = true;
                    jvx_produceFilenameWavOut(pathRet, this->onInit.parentRef->properties_selected_active.defaultPathAudioFiles.value );
	                /*
                    JVX_PRODUCE_FILENAME_GENERIC_WAV_OUT(fNameOut, this->onInit.parentRef->properties_selected_active.defaultPathAudioFiles.value);
					fNameOut = JVX_GENERIC_HOST_FNAME_PREFIX;
					fNameOut += JVX_DATE();
					fNameOut += "__";
					fNameOut += JVX_TIME();
					fNameOut = jvx_replaceCharacter( fNameOut,'/', '_');
					fNameOut = jvx_replaceCharacter( fNameOut,':', '_');
					fNameOut = this->onInit.parentRef->properties_selected_active.defaultPathAudioFiles.value + "/" + fNameOut;
					fNameOut += ".wav";*/
				}
				resL = onInit.theFilesRef->theOutputFiles[i].theWriter->activate(pathRet.c_str(), JVX_FILETYPE_WAV, &endpoint_open);
				if(resL != JVX_NO_ERROR)
				{
					std::cout << __FUNCTION__ << ": Failed to open output file " << pathRet << std::endl;
					onInit.theFilesRef->theOutputFiles[i].common.processing.theStatus = FILE_BROKEN;
				}
				else
				{
					
					for (j = 0; j < JVX_NUMBER_AUDIO_FILE_TAGS; j++)
					{
						if (jvx_bitTest(tags.tagsSet, j))
						{
							onInit.theFilesRef->theOutputFiles[i].theWriter->add_tag((jvxAudioFileTagType)jvx_bitFieldValue32((jvxBitField)1 << JVX_SIZE_INT32(j)), tags.tags[j].c_str());
						}
					}

					onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW = NULL;
					onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS = NULL;
					onInit.theFilesRef->theOutputFiles[i].common.processing.resampler = NULL;

					jvxFileDescriptionEndpoint_prepare endpoint_prep;
					endpoint_prep.numSamplesFrameMax_moveBgrd = onInit.theFilesRef->theOutputFiles[i].common.processing.bSize_rs* tableLookaheadBuffers[onInit.theFilesRef->theOutputFiles[i].common.idxNumberBuffersLookahead];
					endpoint_prep.dFormatAppl = processingControl.inProc.params_fixed_runtime.format;

					resL = onInit.theFilesRef->theOutputFiles[i].theWriter->prepare(
						onInit.theFilesRef->theOutputFiles[i].common.processing.bSize_rs*  tableLookaheadBuffers[onInit.theFilesRef->theOutputFiles[i].common.idxNumberBuffersLookahead],
						onInit.theFilesRef->theOutputFiles[i].common.boostPrio, &endpoint_prep, proc_fields.fileOperationMode);
				}
				JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW, jvxByte**, processingControl.numBuffers_receiver_to_sender_sw);
				for(m = 0; m < processingControl.numBuffers_receiver_to_sender_sw; m++)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW[m], jvxByte*, numChans);
					memset(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW[m], 0, sizeof(jvxByte*)*numChans);
				}

				if(onInit.theFilesRef->theOutputFiles[i].common.processing.resampling)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS, jvxByte*, numChans);
					memset(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS, 0, sizeof(jvxByte*)*numChans);

					JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theFilesRef->theOutputFiles[i].common.processing.resampler, oneResampler, numChans);
					memset(onInit.theFilesRef->theOutputFiles[i].common.processing.resampler, 0, sizeof(oneResampler)*numChans);
				}

				for(m = 0; m < processingControl.numBuffers_receiver_to_sender_sw; m++)
				{
					for(j = 0; j < (unsigned)numChans; j++)
					{
						JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW[m][j], jvxByte, ( bSize* szElm));
						memset(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW[m][j], 0, ( bSize* szElm));
					}
				}

				for(j = 0; j < (unsigned)numChans; j++)
				{
					// If resampling is required
					if(onInit.theFilesRef->theOutputFiles[i].common.processing.resampling)
					{

						JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS[j], jvxByte, bSizeRs * szElm);
						memset(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS[j], 0, bSizeRs * szElm);

						resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_RESAMPLER, &onInit.theFilesRef->theOutputFiles[i].common.processing.resampler[j].theObj, 0, NULL);
						if((resL == JVX_NO_ERROR) && onInit.theFilesRef->theOutputFiles[i].common.processing.resampler[j].theObj)
						{
							onInit.theFilesRef->theOutputFiles[i].common.processing.resampler[j].theObj->request_specialization(
								reinterpret_cast<jvxHandle**>(&onInit.theFilesRef->theOutputFiles[i].common.processing.resampler[j].theHdl),
								NULL, NULL);
							onInit.theFilesRef->theOutputFiles[i].common.processing.resampler[j].theHdl->initialize(_common_set_min.theHostRef);
							resL = onInit.theFilesRef->theOutputFiles[i].common.processing.resampler[j].theHdl->prepare(
								processingControl.computedParameters.bSize_sw,
								onInit.theFilesRef->theOutputFiles[i].common.processing.bSize_rs,
								inRate, outRate, runtime.props_user.resampler.orderFilterResamplingOutput,true,
								processingControl.inProc.params_fixed_runtime.format);
						}
						if(resL != JVX_NO_ERROR)
						{
							assert(0);
						}
					}// if(onInit.theFilesRef->theOutputFiles[i].common.processing.resampling)
				} // for(j = 0; j < (unsigned)numChans; j++)
			}//if(onInit.theFilesRef->theOutputFiles[i].common.processing.selected)
		} // for(i = 0; i < onInit.theFilesRef->theOutputFiles.size(); i++)
	}// if(res == JVX_NO_ERROR)
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess_outputfiles()
{
	jvxSize i,j,m;

	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	// =========================================================================
	// Open output files and prepare processing
	// =========================================================================
	if(res == JVX_NO_ERROR)
	{
		for(i = 0; i < onInit.theFilesRef->theOutputFiles.size(); i++)
		{
			if(onInit.theFilesRef->theOutputFiles[i].common.processing.selected)
			{
				jvxInt32 numChans = JVX_SIZE_INT32(onInit.theFilesRef->theOutputFiles[i].common.number_channels);

				for(j = 0; j < (unsigned)numChans; j++)
				{

					// If resampling is required
					if(onInit.theFilesRef->theOutputFiles[i].common.processing.resampling)
					{
						JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS[j], jvxByte);

						onInit.theFilesRef->theOutputFiles[i].common.processing.resampler[j].theHdl->postprocess();
						onInit.theFilesRef->theOutputFiles[i].common.processing.resampler[j].theHdl->terminate();
						onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RESAMPLER, onInit.theFilesRef->theOutputFiles[i].common.processing.resampler[j].theObj, 0, NULL);
					}// if(onInit.theFilesRef->theOutputFiles[i].common.processing.resampling)

					
				} // for(j = 0; j < (unsigned)numChans; j++)

				for(m = 0; m < processingControl.numBuffers_receiver_to_sender_sw; m++)
				{
					for(j = 0; j < (unsigned)numChans; j++)
					{
						JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW[m][j], jvxByte);
					}
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW[m], jvxByte*);
				}

				onInit.theFilesRef->theOutputFiles[i].theWriter->postprocess();
				onInit.theFilesRef->theOutputFiles[i].theWriter->deactivate();

				JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW, jvxByte**);

				if(onInit.theFilesRef->theOutputFiles[i].common.processing.resampling)
				{
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS, jvxByte*);
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theFilesRef->theOutputFiles[i].common.processing.resampler, oneResampler);
				}

			}//if(onInit.theFilesRef->theOutputFiles[i].common.processing.selected)
			onInit.theFilesRef->theOutputFiles[i].common.processing.theStatus = FILE_INACTIVE;
			onInit.theFilesRef->theOutputFiles[i].common.processing.selected = false;

			onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesRS = NULL;
			onInit.theFilesRef->theOutputFiles[i].common.processing.fieldSamplesSW = NULL;
			onInit.theFilesRef->theOutputFiles[i].common.processing.resampler = NULL;

		} // for(i = 0; i < onInit.theFilesRef->theOutputFiles.size(); i++)
	}// if(res == JVX_NO_ERROR)
	return(res);
}

// ##################################################################################
// ##################################################################################
// ##################################################################################


jvxErrorType
CjvxGenericWrapperDevice::prepare_inputfiles()
{
	jvxSize i,j,m;

	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	// =========================================================================
	// Distinguish between selected and non-selected input files
	// =========================================================================
	if(res == JVX_NO_ERROR)
	{
		//=======================================================
		// Open the input files and compute everything required for processing.
		// We must do this before HW setup since we will set cross references
		//=======================================================

		// ===============================================================
		// Deal with input and output files
		// ===============================================================

		// Cross check which files must be opened according to channel selection
		// We should not see any file removal after state PREPARED has started!!
		for(i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
		{
			onInit.theFilesRef->theInputFiles[i].common.processing.selected = false;
			for(j = 0; j < runtime.channelMappings.inputChannelMapper.size(); j++)
			{
				if(
					(runtime.channelMappings.inputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE) &&
					(runtime.channelMappings.inputChannelMapper[j].isSelected == true) &&
					(runtime.channelMappings.inputChannelMapper[j].refHint == onInit.theFilesRef->theInputFiles[i].common.uniqueIdHandles)
					)
				{
					onInit.theFilesRef->theInputFiles[i].common.processing.selected = true;
					break;
				}
			}
			onInit.theFilesRef->theInputFiles[i].progress = 0;
		}
	}

	// =========================================================================
	// Open input files and prepare processing
	// =========================================================================
	if(res == JVX_NO_ERROR)
	{
		for(i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
		{
			onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_INACTIVE;
			/*
			onInit.theFilesRef->theInputFiles[i].rewind = false;
			onInit.theFilesRef->theInputFiles[i].forward = false;
			onInit.theFilesRef->theInputFiles[i].restart = false;
			*/
			onInit.theFilesRef->theInputFiles[i].common.processing.deltaT_avrg = 0.0;

			if(onInit.theFilesRef->theInputFiles[i].common.processing.selected)
			{
				// ===========================================================
				// == Start by clsoing the file at first to reopen it afterwards with the
				// == right buffersize parameters
				// ===========================================================
#if 0
				onInit.theFilesRef->theInputFiles[i].theReader->deactivate();

				jvxFileDescriptionEndpoint_open endpoint_open;
				endpoint_open.numberChannels = -1;// <- accept only wav files
				endpoint_open.samplerate = -1; // <- accept only wav files
				endpoint_open.fFormatFile = JVX_DATAFORMAT_NONE; // <- accept only wav files
				endpoint_open.littleEndian = true;
				endpoint_open.extended = false;
				endpoint_open.numberBitsSample = 16;
				
				resL = onInit.theFilesRef->theInputFiles[i].theReader->activate(
					onInit.theFilesRef->theInputFiles[i].common.name.c_str(),
					JVX_FILETYPE_WAV, &endpoint_open);
				if(resL != JVX_NO_ERROR)
				{
					onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_BROKEN;
				}
				else
				{
					onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS;
				}
#endif
				onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS;

				jvxFileDescriptionEndpoint_prepare endpoint_prep;
				endpoint_prep.numSamplesFrameMax_moveBgrd = onInit.theFilesRef->theInputFiles[i].common.processing.bSize_rs * tableLookaheadBuffers[onInit.theFilesRef->theInputFiles[i].common.idxNumberBuffersLookahead];
				endpoint_prep.dFormatAppl = processingControl.inProc.params_fixed_runtime.format;

				// Reset the runtime flags
				onInit.theFilesRef->theInputFiles[i].flag_bwd = c_false;
				onInit.theFilesRef->theInputFiles[i].flag_fwd = c_false;
				onInit.theFilesRef->theInputFiles[i].flag_pause = c_false;
				onInit.theFilesRef->theInputFiles[i].flag_play = c_false;
				onInit.theFilesRef->theInputFiles[i].flag_restart = c_false;
				onInit.theFilesRef->theInputFiles[i].progress = 0.0;

				onInit.theFilesRef->theInputFiles[i].theReader->prepare(onInit.theFilesRef->theInputFiles[i].common.processing.bSize_rs* tableLookaheadBuffers[onInit.theFilesRef->theInputFiles[i].common.idxNumberBuffersLookahead],
					onInit.theFilesRef->theInputFiles[i].ctTp, onInit.theFilesRef->theInputFiles[i].common.boostPrio,
					&endpoint_prep, proc_fields.fileOperationMode);

				// ===========================================================
				// == Allocate buffers
				// ===========================================================
				//					if(onInit.theFilesRef->theInputFiles[i].common.number_channels)
				//					{
				jvxInt32 numChans = JVX_SIZE_INT32(onInit.theFilesRef->theInputFiles[i].common.number_channels);
				jvxSize szElm = jvxDataFormat_size[processingControl.inProc.params_fixed_runtime.format];
				jvxSize bSize = processingControl.computedParameters.bSize_sw;
				jvxSize bSizeRs = onInit.theFilesRef->theInputFiles[i].common.processing.bSize_rs;
				jvxData inRate = (jvxData)onInit.theFilesRef->theInputFiles[i].common.processing.sRate_rs;
				jvxData outRate = processingControl.computedParameters.sRate_sw_real;


				JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW_orig, jvxByte**, processingControl.numBuffers_sender_to_receiver_sw);
				JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW, jvxByte**, processingControl.numBuffers_sender_to_receiver_sw);
				for(m = 0; m < processingControl.numBuffers_sender_to_receiver_sw; m++)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW[m], jvxByte*, numChans);
					memset(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW[m], 0, sizeof(jvxByte*)*numChans);

					JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW_orig[m], jvxByte*, numChans);
					memset(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW_orig[m], 0, sizeof(jvxByte*)*numChans);
				}

				if(onInit.theFilesRef->theInputFiles[i].common.processing.resampling)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS, jvxByte*,  numChans);
					memset(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS, 0, sizeof(jvxByte*)*numChans);

					JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theFilesRef->theInputFiles[i].common.processing.resampler, oneResampler, numChans);
					memset(onInit.theFilesRef->theInputFiles[i].common.processing.resampler, 0, sizeof(oneResampler)*numChans);
				}
				else
				{
					onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS = NULL;
					onInit.theFilesRef->theInputFiles[i].common.processing.resampler = NULL;
				}

				for(m = 0; m < processingControl.numBuffers_sender_to_receiver_sw; m++)
				{
					for(j = 0; j < (unsigned)numChans; j++)
					{
						// Allocate fields for all channels but we may use another pointer later
						JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW_orig[m][j], 
							jvxByte, ( bSize* szElm));
						memset(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW_orig[m][j], 0, ( bSize* szElm));
						onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW[m][j] = 
							onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW_orig[m][j];
					}
				}
				
				for(j = 0; j < (unsigned)numChans; j++)
				{

					// If resampling is required
					if(onInit.theFilesRef->theInputFiles[i].common.processing.resampling)
					{
						JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS[j], jvxByte, bSizeRs * szElm);
						memset(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS[j], 0, bSizeRs * szElm);

						resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_RESAMPLER, &onInit.theFilesRef->theInputFiles[i].common.processing.resampler[j].theObj, 0, NULL);
						if((resL == JVX_NO_ERROR) && onInit.theFilesRef->theInputFiles[i].common.processing.resampler[j].theObj)
						{
							onInit.theFilesRef->theInputFiles[i].common.processing.resampler[j].theObj->request_specialization(
								reinterpret_cast<jvxHandle**>(&onInit.theFilesRef->theInputFiles[i].common.processing.resampler[j].theHdl),
								NULL, NULL);
							onInit.theFilesRef->theInputFiles[i].common.processing.resampler[j].theHdl->initialize(_common_set_min.theHostRef);
							resL = onInit.theFilesRef->theInputFiles[i].common.processing.resampler[j].theHdl->prepare(
								bSizeRs,bSize, inRate, outRate, runtime.props_user.resampler.orderFilterResamplingInput,true,
								processingControl.inProc.params_fixed_runtime.format);
						}
						if(resL != JVX_NO_ERROR)
						{
							assert(0);
						}
					}
				}// for(j = 0; j < (unsigned)numChans; j++)

				if (onInit.theFilesRef->theInputFiles[i].common.pauseOnStart)
				{
					onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_ACCESS_PAUSED;
				}

			} // if(onInit.theFilesRef->theInputFiles[i].common.processing.selected)
		} //for(i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
	} // if(res == JVX_NO_ERROR)
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess_inputfiles()
{
	jvxSize i,j,m;
	for(i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
	{
		if(onInit.theFilesRef->theInputFiles[i].common.processing.selected)
		{

			jvxInt32 numChans = JVX_SIZE_INT32(onInit.theFilesRef->theInputFiles[i].common.number_channels);

			for(m = 0; m < processingControl.numBuffers_sender_to_receiver_sw; m++)
			{
				for(j = 0; j < (unsigned)numChans; j++)
				{
					// Allocate fields for all channels but we may use another pointer later
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW_orig[m][j], jvxByte);
				}
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW[m], jvxByte*);
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW_orig[m], jvxByte*);
			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW, jvxByte**);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW_orig, jvxByte**);

			for(j = 0; j < (unsigned)numChans; j++)
			{
				// Allocate fields for all channels but we may use another pointer later
				

				// If resampling is required
				if(onInit.theFilesRef->theInputFiles[i].common.processing.resampling)
				{
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS[j], jvxByte);

					onInit.theFilesRef->theInputFiles[i].common.processing.resampler[j].theHdl->postprocess();
					onInit.theFilesRef->theInputFiles[i].common.processing.resampler[j].theHdl->terminate();
					onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RESAMPLER, onInit.theFilesRef->theInputFiles[i].common.processing.resampler[j].theObj, 0, NULL);
					onInit.theFilesRef->theInputFiles[i].common.processing.resampler[j].theObj = NULL;
					onInit.theFilesRef->theInputFiles[i].common.processing.resampler[j].theHdl = NULL;
				}

			}// for(j = 0; j < (unsigned)numChans; j++)

			if(onInit.theFilesRef->theInputFiles[i].common.processing.resampling)
			{
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS, jvxByte*);
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theFilesRef->theInputFiles[i].common.processing.resampler, oneResampler);
			}
			
			onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesSW = NULL;
			onInit.theFilesRef->theInputFiles[i].common.processing.fieldSamplesRS = NULL;
			onInit.theFilesRef->theInputFiles[i].common.processing.resampler = NULL;
		}

		onInit.theFilesRef->theInputFiles[i].common.processing.theStatus = FILE_INACTIVE;
		/*
		onInit.theFilesRef->theInputFiles[i].rewind = false;
		onInit.theFilesRef->theInputFiles[i].forward = false;
		onInit.theFilesRef->theInputFiles[i].restart = false;
		*/
		onInit.theFilesRef->theInputFiles[i].common.processing.selected = false;

		onInit.theFilesRef->theInputFiles[i].theReader->postprocess();
	}//for(i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
	return(JVX_NO_ERROR);
}

// ##################################################################################
// ##################################################################################
// ##################################################################################


jvxErrorType
CjvxGenericWrapperDevice::prepare_detectmode()
{
	jvxErrorType res = JVX_NO_ERROR;

	// =========================================================================
	// Determine the operation modes
	// =========================================================================
	if(res == JVX_NO_ERROR)
	{
		// INPUT
		if(processingControl.inProc.params_fixed_runtime.chanshw_in == 0)
		{
			proc_fields.seq_operation_in = NOTHING;
		}
		else
		{
			if(processingControl.operationDevice == OPERATION_MODE_1)
			{
				if(processingControl.timebaseDevice == TIMEBASE_CHANGE)
				{
					proc_fields.seq_operation_in = TB_PROC;
					//proc_fields.seq_operation_in = DACON_TB_PROC;
				}
				else
				{
					proc_fields.seq_operation_in = PROC;
				}
			}
			else if(processingControl.operationDevice == OPERATION_MODE_2)
			{
				if(processingControl.timebaseDevice == TIMEBASE_CHANGE)
				{
					proc_fields.seq_operation_in = DACON_TB_PROC;
				}
				else
				{
					proc_fields.seq_operation_in = DACON_PROC;
				}
			}
			else if(processingControl.operationDevice == OPERATION_MODE_3)
			{
				if(processingControl.timebaseDevice == TIMEBASE_CHANGE)
				{
					proc_fields.seq_operation_in = RES_TB_PROC;
				}
				else
				{
					proc_fields.seq_operation_in = RES_PROC;
				}
			}
			else if(processingControl.operationDevice == OPERATION_MODE_4)
			{
				if(processingControl.timebaseDevice == TIMEBASE_CHANGE)
				{
					proc_fields.seq_operation_in = RES_DACON_TB_PROC;
				}
				else
				{
					proc_fields.seq_operation_in = RES_DACON_PROC;
				}
			}
		}

		// OUTPUT OUTPUT
		if(processingControl.inProc.params_fixed_runtime.chanshw_out == 0)
		{
			proc_fields.seq_operation_out = NOTHING;
		}
		else
		{
			if(processingControl.operationDevice == OPERATION_MODE_1)
			{
				if(processingControl.timebaseDevice == TIMEBASE_CHANGE)
				{
					proc_fields.seq_operation_out = TB_PROC;
					//proc_fields.seq_operation_out = DACON_TB_PROC;
				}
				else
				{
					proc_fields.seq_operation_out = PROC;
				}
			}
			else if(processingControl.operationDevice == OPERATION_MODE_2)
			{
				if(processingControl.timebaseDevice == TIMEBASE_CHANGE)
				{
					proc_fields.seq_operation_out = DACON_TB_PROC;
				}
				else
				{
					proc_fields.seq_operation_out = DACON_PROC;
				}
			}
			else if(processingControl.operationDevice == OPERATION_MODE_3)
			{
				if(processingControl.timebaseDevice == TIMEBASE_CHANGE)
				{
					proc_fields.seq_operation_out = RES_TB_PROC;
				}
				else
				{
					proc_fields.seq_operation_out = RES_PROC;
				}
			}
			else if(processingControl.operationDevice == OPERATION_MODE_4)
			{
				if(processingControl.timebaseDevice == TIMEBASE_CHANGE)
				{
					proc_fields.seq_operation_out = RES_DACON_TB_PROC;
				}
				else
				{
					proc_fields.seq_operation_out = RES_DACON_PROC;
				}
			}
		}

		proc_fields.fileOperationMode = JVX_FILEOPERATION_REALTIME;
		switch (proc_fields.seq_operation_in)
		{
		case PROC:
		case RES_PROC:
		case DACON_PROC:
		case RES_DACON_PROC:
			proc_fields.doSelection = SELECTION_SYNC_MODE;
			break;
		case TB_PROC:
		case RES_TB_PROC:
		case DACON_TB_PROC:
		case RES_DACON_TB_PROC:
			proc_fields.doSelection = SELECTION_ASYNC_MODE;
			break;
		case NOTHING:
			switch (proc_fields.seq_operation_out)
			{
			case PROC:
			case RES_PROC:
			case DACON_PROC:
			case RES_DACON_PROC:
				proc_fields.doSelection = SELECTION_SYNC_MODE;
				break;
			case TB_PROC:
			case RES_TB_PROC:
			case DACON_TB_PROC:
			case RES_DACON_TB_PROC:
				proc_fields.doSelection = SELECTION_ASYNC_MODE;
				break;
			case NOTHING:
				proc_fields.doSelection = SELECTION_FILEIO_MODE;
				proc_fields.fileOperationMode = JVX_FILEOPERATION_BATCH;
			}
		}
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess_detectmode()
{
	jvxErrorType res = JVX_NO_ERROR;
	return(res);
}


// ##################################################################################
// ##################################################################################
// ##################################################################################

jvxErrorType
CjvxGenericWrapperDevice::prepare_preplinkmapper_input(jvxLinkDataDescriptor* theData)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	// =========================================================================
	// Create pointer mapping ids in channel mappers where selected
	// =========================================================================
	if (res == JVX_NO_ERROR)
	{
		jvxSize cnt = 0;
		for (i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
		{
			if (runtime.channelMappings.inputChannelMapper[i].isSelected)
			{
				runtime.channelMappings.inputChannelMapper[i].proc.idInChannelStruct = (jvxInt32)cnt;
				cnt++;
			}
			else
			{
				runtime.channelMappings.inputChannelMapper[i].proc.idInChannelStruct = -1;
			}
		}
		assert(cnt == theData->con_params.number_channels);
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::prepare_preplinkmapper_output(jvxLinkDataDescriptor* theData)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	if (res == JVX_NO_ERROR)
	{
		jvxSize cnt = 0;
		for(i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
		{
			if(runtime.channelMappings.outputChannelMapper[i].isSelected)
			{
				runtime.channelMappings.outputChannelMapper[i].proc.idInChannelStruct = (jvxInt32)cnt;
				cnt ++;
			}
			else
			{
				runtime.channelMappings.outputChannelMapper[i].proc.idInChannelStruct = -1;
			}
		}
		assert(cnt == theData->con_params.number_channels);
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess_preplinkmapper_input()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	// =========================================================================
	// Create pointer mapping ids in channel mappers where selected
	// =========================================================================
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
		{
			runtime.channelMappings.inputChannelMapper[i].proc.idInChannelStruct = -1;
		}
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess_preplinkmapper_output()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;	if(res == JVX_NO_ERROR)
	{
		for(i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
		{
			runtime.channelMappings.outputChannelMapper[i].proc.idInChannelStruct = -1;
		}
	}
	return(res);
}

// ##################################################################################
// ##################################################################################
// ##################################################################################

void
CjvxGenericWrapperDevice::populate_countchannels_datatype(local_proc_params& params)
{
	jvxSize i;
	jvxSize cnt = 0;

	// Check that all involved modules are ready
	assert(params.setup == false);

	//processingControl.performance.numAudioThreadFailed = -1;

	//=================================================================
	// COPY PROCESSING PARAMETERS
	//=================================================================
	params.chanshw_in = 0;
	params.chanshw_out = 0;
	params.chansfile_in = 0;
	params.chansfile_out = 0;
	params.chansext_in = 0;
	params.chansext_out = 0;
	params.chansdum_in = 0;
	params.chansdum_out = 0;

	for (i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
	{
		if (runtime.channelMappings.inputChannelMapper[i].isSelected)
		{
			//runtime.channelMappings.inputChannelMapper[i].proc.idInChannelStruct = cnt++;
			switch (runtime.channelMappings.inputChannelMapper[i].theChannelType)
			{
			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE:
				params.chanshw_in++;
				break;
			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE:
				params.chansfile_in++;
				break;
			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL:
				params.chansext_in++;
				break;
			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY:
				params.chansdum_in++;
				break;
			default:
				assert(0);
			}
		}
		else
		{
			//runtime.channelMappings.inputChannelMapper[i].proc.idInChannelStruct = -1;
		}
	}

	cnt = 0;
	for (i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
	{
		if (runtime.channelMappings.outputChannelMapper[i].isSelected)
		{
			//runtime.channelMappings.outputChannelMapper[i].proc.idInChannelStruct = cnt++;
			switch (runtime.channelMappings.outputChannelMapper[i].theChannelType)
			{
			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE:
				params.chanshw_out++;
				break;
			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE:
				params.chansfile_out++;
				break;
			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL:
				processingControl.inProc.params_fixed_runtime.chansext_out++;
				break;
			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY:
				params.chansdum_out++;
				break;
			default:
				assert(0);
			}
		}
		else
		{
			//runtime.channelMappings.outputChannelMapper[i].proc.idInChannelStruct = -1;
		}
	}
	params.format = processingControl.computedParameters.form_sw;
	params.buffersize = processingControl.computedParameters.bSize_sw;
	params.samplerate = processingControl.computedParameters.sRate_sw;

	params.chans_in =
		params.chanshw_in +
		params.chansext_in +
		params.chansdum_in +
		params.chansfile_in;

	params.chans_out =
		params.chanshw_out +
		params.chansext_out +
		params.chansdum_out +
		params.chansfile_out;

	proc_fields.szElementSW = jvxDataFormat_size[processingControl.computedParameters.form_sw];
	proc_fields.szElementHW = jvxDataFormat_size[processingControl.computedParameters.form_hw];

	params.setup = true;
}

void
CjvxGenericWrapperDevice::release_countchannels_datatype(local_proc_params& params)
{
	jvxSize cnt = 0;
	jvxErrorType res = JVX_NO_ERROR;
	// Check that all involved modules are ready
	assert(params.setup);

	//=================================================================
		// COPY PROCESSING PARAMETERS
		//=================================================================
	params.chanshw_in = 0;
	params.chanshw_out = 0;
	params.chansfile_in = 0;
	params.chansfile_out = 0;
	params.chansext_in = 0;
	params.chansext_out = 0;
	params.chansdum_in = 0;
	params.chansdum_out = 0;

	params.format = JVX_DATAFORMAT_NONE;
	params.buffersize = -1;
	params.samplerate = -1;

	proc_fields.szElementSW = 0;
	proc_fields.szElementHW = 0;
	params.setup = false;
}

// ##################################################################################
// ##################################################################################
// ##################################################################################

jvxErrorType
	CjvxGenericWrapperDevice::prepare_timestamps()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	// =========================================================================
	// Create buffer for timestamp production in case of async mode
	// =========================================================================
	if(res == JVX_NO_ERROR)
	{
		// One sample duration is needed in sync mode also..
		proc_fields.forTimeStamps.durOneSample_us_dbl = (1.0/(jvxData)processingControl.computedParameters.sRate_sw_real*1000000.0);

		// Timing handling for async mode
		if(proc_fields.doSelection == SELECTION_ASYNC_MODE)
		{
			// Prepare the circular buffer for timing information..
			proc_fields.forTimeStamps.idxRead = 0;
			proc_fields.forTimeStamps.fillHeight = processingControl.computedParameters.fillHeightInput;

			// Timing information handling
			proc_fields.forTimeStamps.length = (2* processingControl.computedParameters.bSize_sw +
				processingControl.computedParameters.bSize_hw_ds );
			proc_fields.forTimeStamps.buffer = new jvxInt64[proc_fields.forTimeStamps.length];
			for(i = 0; i <(unsigned) proc_fields.forTimeStamps.length; i++)
			{
				proc_fields.forTimeStamps.buffer[i] = -1;
			}

			//processingControl.performance.numAudioThreadFailed = 0;
		}
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess_timestamps()
{
	jvxErrorType res = JVX_NO_ERROR;

	if(res == JVX_NO_ERROR)
	{
		if(proc_fields.doSelection == SELECTION_ASYNC_MODE)
		{
			JVX_DSP_SAFE_DELETE_FIELD(proc_fields.forTimeStamps.buffer);
			proc_fields.forTimeStamps.idxRead = 0;
			proc_fields.forTimeStamps.fillHeight = 0;
			proc_fields.forTimeStamps.length = 0;
		}
	}
	return(res);}

// ##################################################################################
// ##################################################################################
// ##################################################################################

jvxErrorType
CjvxGenericWrapperDevice::prepare_linkdata(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// =========================================================================
	// Allocate data link objects and prepopulate as good as possible
	// =========================================================================
	if(res == JVX_NO_ERROR)
	{
		_common_set_ocon.theData_out.con_params.buffersize = processingControl.computedParameters.bSize_sw;
		_common_set_ocon.theData_out.con_params.format = processingControl.computedParameters.form_sw;
		_common_set_ocon.theData_out.con_data.buffers = NULL;
		_common_set_ocon.theData_out.con_data.number_buffers = processingControl.numBuffers_sender_to_receiver_sw;
		_common_set_ocon.theData_out.con_params.number_channels = processingControl.inProc.params_fixed_runtime.chans_in;
		_common_set_ocon.theData_out.con_params.rate = processingControl.computedParameters.sRate_sw;

		// OLD: _common_set_ocon.theData_out.con_data.user_hints = theRelocator._common_set_icon.theData_in->con_data.user_hints;
		// But we do not want to see the hardware hints in the software chain
		_common_set_ocon.theData_out.con_user.chain_spec_user_hints = NULL;

		// If we forward the input buffers, make sure, we have alignement and headers
		if (proc_fields.seq_operation_in == PROC)
		{
			_common_set_ocon.theData_out.con_data.bExt.align = theRelocator._common_set_icon.theData_in->con_data.bExt.align;
			_common_set_ocon.theData_out.con_data.bExt.append = theRelocator._common_set_icon.theData_in->con_data.bExt.append;
			_common_set_ocon.theData_out.con_data.bExt.prepend = theRelocator._common_set_icon.theData_in->con_data.bExt.prepend;
		}

		// Attach a user hint argument to concatenated list
		processingControl.inProc.procFlags = 0;
		if (proc_fields.doSelection == SELECTION_FILEIO_MODE)
		{
			processingControl.inProc.procFlags = JVX_HINT_DESCRIPTOR_OFFLINE;
		}

		// Start the userHint for sw chain
		_common_set_ocon.theData_out.con_user.chain_spec_user_hints =
			jvx_hintDesriptor_push_front(&processingControl.inProc.procFlags, JVX_DATAPROCESSOR_HINT_DESCRIPTOR_PROCESSING_TYPE_CBITFIELD, 
				_common_set.theComponentType, _common_set_ocon.theData_out.con_user.chain_spec_user_hints);
		
		// Start a new master concatenation
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			res = _common_set_ocon.theData_out.con_link.connect_to->prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));

			// New: if the processing is straight through, adapt the number of buffers of input
			/*
			if ((proc_fields.seq_operation_in == PROC) || (proc_fields.seq_operation_out == PROC))
			{
				if (_common_set_ocon.theData_out.con_data.number_buffers > processingControl.numBuffers_sender_to_receiver_hw)
				{
					processingControl.numBuffers_sender_to_receiver_hw = _common_set_ocon.theData_out.con_data.number_buffers;
					theRelocator._common_set_icon.theData_in->con_data.number_buffers = processingControl.numBuffers_sender_to_receiver_hw;
					processingControl.numBuffers_sender_to_receiver_sw = processingControl.numBuffers_sender_to_receiver_hw;
				}
			}
			*/
			if (proc_fields.seq_operation_in == PROC)
			{
				if (_common_set_ocon.theData_out.con_data.number_buffers > processingControl.numBuffers_sender_to_receiver_hw)
				{
					processingControl.numBuffers_sender_to_receiver_hw = _common_set_ocon.theData_out.con_data.number_buffers;
					theRelocator._common_set_icon.theData_in->con_data.number_buffers = processingControl.numBuffers_sender_to_receiver_hw;
				}
			}
			if(_common_set_ocon.theData_out.con_data.number_buffers > processingControl.numBuffers_sender_to_receiver_sw)
			{
				processingControl.numBuffers_sender_to_receiver_sw = _common_set_ocon.theData_out.con_data.number_buffers;
			}

			if (theRelocator._common_set_icon.theData_in)
			{
				// Create container in which to place buffer references lateron
				res = jvx_allocateDataLinkDescriptor(theRelocator._common_set_icon.theData_in, false);
				assert(res == JVX_NO_ERROR);

				if (proc_fields.seq_operation_in == PROC)
				{
					// Adopt next stage pipeline control
					theRelocator._common_set_icon.theData_in->con_pipeline = _common_set_ocon.theData_out.con_pipeline;
				}
				else
				{
					res = jvx_allocateDataLinkPipelineControl(theRelocator._common_set_icon.theData_in);
					*theRelocator._common_set_icon.theData_in->con_pipeline.idx_stage_ptr = 0;					
				}
			}
		}
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess_linkdata(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* ptrPop = NULL;
	jvxDataProcessorHintDescriptorType flgPop = JVX_DATAPROCESSOR_HINT_DESCRIPTOR_TYPE_NONE;
	jvxComponentIdentification tpPop;
	
	// =========================================================================
	// Allocate data link objects and prepopulate as good as possible
	// =========================================================================

	if (res == JVX_NO_ERROR)
	{
		// Start a new master concatenation
		if (_common_set_ocon.theData_out.con_link.connect_to)
		{
			// Create container in which to place buffer references lateron
			if (theRelocator._common_set_icon.theData_in)
			{
				if (proc_fields.seq_operation_in != PROC)
				{
					jvx_deallocateDataLinkPipelineControl(theRelocator._common_set_icon.theData_in);
				}
				jvx_neutralDataLinkDescriptor_pipeline(&theRelocator._common_set_icon.theData_in->con_pipeline);

				res = jvx_deallocateDataLinkDescriptor(theRelocator._common_set_icon.theData_in, false);
				assert(res == JVX_NO_ERROR);
			}

			_common_set_ocon.theData_out.con_link.connect_to->postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		}

		_common_set_ocon.theData_out.con_params.buffersize = 0;
		_common_set_ocon.theData_out.con_params.format = JVX_DATAFORMAT_NONE;
		_common_set_ocon.theData_out.con_data.buffers = NULL;
		_common_set_ocon.theData_out.con_data.number_buffers = 0;
		_common_set_ocon.theData_out.con_params.number_channels = 0;
		_common_set_ocon.theData_out.con_params.rate = 0;
		_common_set_ocon.theData_out.con_data.bExt.align = 0;
		_common_set_ocon.theData_out.con_data.bExt.append = 0;
		_common_set_ocon.theData_out.con_data.bExt.prepend = 0;
		_common_set_ocon.theData_out.con_data.bExt.f_alloc = NULL;
		_common_set_ocon.theData_out.con_data.bExt.f_dealloc = NULL;
		_common_set_ocon.theData_out.con_data.bExt.f_priv = NULL;	

		_common_set_ocon.theData_out.con_user.chain_spec_user_hints =
			jvx_hintDesriptor_pop_front(_common_set_ocon.theData_out.con_user.chain_spec_user_hints,
				&ptrPop, &flgPop, &tpPop);

		assert(ptrPop == &processingControl.inProc.procFlags);
		assert(flgPop == JVX_DATAPROCESSOR_HINT_DESCRIPTOR_PROCESSING_TYPE_CBITFIELD);

		_common_set_ocon.theData_out.con_user.chain_spec_user_hints = NULL;
	}
	return(res);
}

// ##################################################################################
// ##################################################################################
// ##################################################################################

jvxErrorType
CjvxGenericWrapperDevice::prepare_inputprocessing()
{
	jvxSize i, j;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	if(res == JVX_NO_ERROR)
	{
		jvxData inRate = 0;
		jvxData outRate = 0;
		int cnt = 0;
		//int idp = 0;

		// Allocate data buffers
		// ======================================================
		// INPUT INPUT
		// ======================================================

		//assert(shareWithUI.inputRoutingTable.size() == proc_fields.chansInDev+proc_fields.chansInFIO);
		// Only in synch mode assert(processingControl.computedParameters.bSize_hw_ds == processingControl.computedParameters.bSize_sw);

		// Set all pointers to NULL
		proc_fields.resamplersIn_obj = NULL;
		proc_fields.resamplersIn_hdl = NULL;
		proc_fields.convertersIn_obj = NULL;
		proc_fields.convertersIn_hdl = NULL;

		proc_fields.circularBuffersIn = NULL;

		proc_fields.ptrA_in_net = NULL;
		proc_fields.ptrA_in_raw = NULL;
		proc_fields.ptrA_in_off = NULL;
		proc_fields.ptrB_in = NULL;
		proc_fields.ptrC_in = NULL;
		//proc_fields.mapToOutput_in = NULL;

		// Helper variables
		inRate = (jvxData)processingControl.computedParameters.sRate_hw;
		outRate = processingControl.computedParameters.sRate_sw_real;

		switch(proc_fields.seq_operation_in)
		{
		case TB_PROC:
		case RES_PROC:
		case RES_TB_PROC:
		case DACON_PROC:
		case DACON_TB_PROC:
		case RES_DACON_PROC:
		case RES_DACON_TB_PROC:
			runtime.channelMappings.inputChannelMapper_hwrev.resize(processingControl.inProc.params_fixed_runtime.chanshw_in);
			//JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.mapToOutput_in, jvxSize, processingControl.inProc.params_fixed_runtime.chanshw_in);

			for(i = 0; i <(unsigned) processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				// Find the correct channel output index
				for(j = 0; j < runtime.channelMappings.inputChannelMapper.size(); j++)
				{
					if(runtime.channelMappings.inputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE)
					{
						if(runtime.channelMappings.inputChannelMapper[j].toHwFile.pointsToIdSelectionThisDevice == i)
						{
							runtime.channelMappings.inputChannelMapper_hwrev[i] = runtime.channelMappings.inputChannelMapper[j].toSw.pointsToId_inclFiles;
							//proc_fields.mapToOutput_in[i] = runtime.channelMappings.inputChannelMapper[j].toSw.pointsToId_inclFiles;
						}
					}
				}
			}
		}

		switch(proc_fields.seq_operation_in)
		{
		case NOTHING:

			// Only file input. File input has been completely handled before
			break;
		case PROC:

			// Some consistency checks
			assert(processingControl.computedParameters.bSize_hw == processingControl.computedParameters.bSize_hw_ds);
			assert(processingControl.computedParameters.bSize_hw == processingControl.computedParameters.bSize_sw);
			assert(processingControl.computedParameters.form_hw == processingControl.computedParameters.form_sw);

			// Input buffers are allocated in audio node, nothing to do here
			break;

		case TB_PROC:

			// Some consistency checks
			assert(processingControl.computedParameters.bSize_hw == processingControl.computedParameters.bSize_hw_ds);
			assert(processingControl.computedParameters.form_hw == processingControl.computedParameters.form_sw);

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Circular Buffer
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.circularBuffersIn, CIRCBUFFER*,processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.circularBuffersIn, 0, sizeof(CIRCBUFFER*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			for(i = 0; i <(unsigned) processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start circular buffer
				JVX_DSP_SAFE_ALLOCATE_OBJECT(proc_fields.circularBuffersIn[i], CIRCBUFFER);
				resL = proc_fields.circularBuffersIn[i]->initialize((2* processingControl.computedParameters.bSize_sw +
					processingControl.computedParameters.bSize_hw_ds ),
					processingControl.computedParameters.form_sw);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.circularBuffersIn[i]->start(processingControl.computedParameters.fillHeightInput);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			}
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer A
			allocateBufferA();
			break;

		case RES_PROC:

			// Some consistency checks
			assert(processingControl.computedParameters.bSize_sw == processingControl.computedParameters.bSize_hw_ds);
			assert(processingControl.computedParameters.form_hw == processingControl.computedParameters.form_sw);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate resamplers
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersIn_hdl, IjvxResampler*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.resamplersIn_hdl, 0, sizeof(IjvxResampler*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersIn_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.resamplersIn_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start resamples
				proc_fields.resamplersIn_obj[i] = NULL;
				proc_fields.resamplersIn_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_RESAMPLER,  &proc_fields.resamplersIn_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_obj[i]->request_specialization(reinterpret_cast<jvxHandle**>(
					&proc_fields.resamplersIn_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_hdl[i]->prepare(processingControl.computedParameters.bSize_hw,
					processingControl.computedParameters.bSize_hw_ds,
					inRate, outRate, runtime.props_user.resampler.orderFilterResamplingInput,
					true, processingControl.computedParameters.form_hw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			}

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer A
			allocateBufferA();
			/*
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in, jvxByte**, processingControl.numBuffers_sender_to_receiver_hw);
			for (m = 0; m < processingControl.numBuffers_sender_to_receiver_hw; m++)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in[m], jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_in);
				memset(proc_fields.ptrA_in[m], 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				for (i = 0; i <(unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
				{

					// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
					// Start the buffer A
					JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in[m][i], jvxByte, proc_fields.szElementHW * processingControl.computedParameters.bSize_hw);
					memset(proc_fields.ptrA_in[m][i], 0, sizeof(jvxByte) * proc_fields.szElementHW * processingControl.computedParameters.bSize_hw);
					// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				}
			}*/


			break;

		case RES_TB_PROC:

			// Some consistency checks
			assert(processingControl.computedParameters.form_hw == processingControl.computedParameters.form_sw);

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate circular buffer
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.circularBuffersIn, CIRCBUFFER*,processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.circularBuffersIn, 0, sizeof(CIRCBUFFER*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate resamplers
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersIn_hdl, IjvxResampler*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.resamplersIn_hdl, 0, sizeof(IjvxResampler*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersIn_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.resamplersIn_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer B
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_in, jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.ptrB_in, 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_in);

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start circular buffer
				JVX_DSP_SAFE_ALLOCATE_OBJECT(proc_fields.circularBuffersIn[i], CIRCBUFFER);
				resL = proc_fields.circularBuffersIn[i]->initialize((
					2* processingControl.computedParameters.bSize_sw * 
					processingControl.computedParameters.qual_sc_factor +
					processingControl.computedParameters.bSize_hw_ds ),
					processingControl.computedParameters.form_hw);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.circularBuffersIn[i]->start(processingControl.computedParameters.fillHeightInput);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start resamples
				proc_fields.resamplersIn_obj[i] = NULL;
				proc_fields.resamplersIn_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_RESAMPLER,  &proc_fields.resamplersIn_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_obj[i]->request_specialization(reinterpret_cast<jvxHandle**>(
					&proc_fields.resamplersIn_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_hdl[i]->prepare(processingControl.computedParameters.bSize_hw,
					processingControl.computedParameters.bSize_hw_ds,
					inRate, outRate, runtime.props_user.resampler.orderFilterResamplingInput,
					true, processingControl.computedParameters.form_hw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// STart buffer B
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_in[i], jvxByte, proc_fields.szElementHW* processingControl.computedParameters.bSize_hw_ds);
				memset(proc_fields.ptrB_in[i], 0, sizeof(jvxByte) * proc_fields.szElementHW * processingControl.computedParameters.bSize_hw_ds);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			}

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer A
			allocateBufferA();

			/*
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in, jvxByte**, processingControl.numBuffers_sender_to_receiver_hw);
			for(m = 0; m < processingControl.numBuffers_sender_to_receiver_hw; m++)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in[m], jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_in);
				memset(proc_fields.ptrA_in[m], 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				for(i = 0; i <(unsigned) processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
				{

					// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
					// Start the buffer A
					JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in[m][i], jvxByte, proc_fields.szElementHW * processingControl.computedParameters.bSize_hw);
					memset(proc_fields.ptrA_in[m][i], 0, sizeof(jvxByte) * proc_fields.szElementHW * processingControl.computedParameters.bSize_hw);
					// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				}
			}
			*/
			break;

		case DACON_PROC:

			// Some consistency checks
			assert(processingControl.computedParameters.bSize_hw == processingControl.computedParameters.bSize_sw);
			assert(processingControl.computedParameters.bSize_hw == processingControl.computedParameters.bSize_hw_ds);

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate converters
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersIn_hdl, IjvxDataConverter*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.convertersIn_hdl, 0, sizeof(IjvxDataConverter*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersIn_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.convertersIn_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start converters
				proc_fields.convertersIn_obj[i] = NULL;
				proc_fields.convertersIn_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_DATACONVERTER,  &proc_fields.convertersIn_obj[i] ,0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_obj[i]->request_specialization(reinterpret_cast<jvxHandle**>(&proc_fields.convertersIn_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_hdl[i]->prepare(processingControl.computedParameters.form_hw,
					processingControl.computedParameters.form_sw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			}
			
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer A
			allocateBufferA();
			/*
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in, jvxByte**, processingControl.numBuffers_sender_to_receiver_hw);
			for(m = 0; m < processingControl.numBuffers_sender_to_receiver_hw; m++)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in[m], jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_in);
				memset(proc_fields.ptrA_in[m], 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				for(i = 0; i <(unsigned) processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
				{

					// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
					// Start the buffer A
					JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in[m][i], jvxByte, proc_fields.szElementHW * processingControl.computedParameters.bSize_hw);
					memset(proc_fields.ptrA_in[m][i], 0, sizeof(jvxByte) * proc_fields.szElementHW * processingControl.computedParameters.bSize_hw);
					// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				}
			}
			*/
			break;

		case DACON_TB_PROC:

			// Some consistency checks
			assert(processingControl.computedParameters.bSize_hw == processingControl.computedParameters.bSize_hw_ds);

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate circular buffer
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.circularBuffersIn, CIRCBUFFER*,processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.circularBuffersIn, 0, sizeof(CIRCBUFFER*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate converters
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersIn_hdl, IjvxDataConverter*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.convertersIn_hdl, 0, sizeof(IjvxDataConverter*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersIn_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.convertersIn_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer B
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_in, jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.ptrB_in, 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start circular buffer
				JVX_DSP_SAFE_ALLOCATE_OBJECT(proc_fields.circularBuffersIn[i], CIRCBUFFER);
				resL = proc_fields.circularBuffersIn[i]->initialize((2* processingControl.computedParameters.bSize_sw *
					processingControl.computedParameters.qual_sc_factor +
					processingControl.computedParameters.bSize_hw_ds ),
					processingControl.computedParameters.form_sw);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.circularBuffersIn[i]->start(processingControl.computedParameters.fillHeightInput);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start converters
				proc_fields.convertersIn_obj[i] = NULL;
				proc_fields.convertersIn_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_DATACONVERTER,  &proc_fields.convertersIn_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_obj[i]->request_specialization(
					reinterpret_cast<jvxHandle**>(&proc_fields.convertersIn_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_hdl[i]->prepare(processingControl.computedParameters.form_hw,
					processingControl.computedParameters.form_sw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// STart buffer B
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_in[i], jvxByte, proc_fields.szElementSW* processingControl.computedParameters.bSize_hw);
				memset(proc_fields.ptrB_in[i], 0, sizeof(jvxByte) * proc_fields.szElementSW * processingControl.computedParameters.bSize_hw);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			}

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer A
			allocateBufferA();
			/*
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in, jvxByte**, processingControl.numBuffers_sender_to_receiver_hw);
			for(m = 0; m < processingControl.numBuffers_sender_to_receiver_hw; m++)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in[m], jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_in);
				memset(proc_fields.ptrA_in[m], 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				for(i = 0; i <(unsigned) processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
				{

					// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
					// Start the buffer A
					JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in[m][i], jvxByte, proc_fields.szElementHW * processingControl.computedParameters.bSize_hw);
					memset(proc_fields.ptrA_in[m][i], 0, sizeof(jvxByte) * proc_fields.szElementHW * processingControl.computedParameters.bSize_hw);
					// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				}
			}*/
			break;

		case RES_DACON_PROC:

			// Some consistency checks
			assert(processingControl.computedParameters.bSize_sw == processingControl.computedParameters.bSize_hw_ds);

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate resamplers
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersIn_hdl, IjvxResampler*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.resamplersIn_hdl, 0, sizeof(IjvxResampler*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersIn_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.resamplersIn_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate converters
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersIn_hdl, IjvxDataConverter*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.convertersIn_hdl, 0, sizeof(IjvxDataConverter*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersIn_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.convertersIn_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *


			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer B
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_in, jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.ptrB_in, 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start resamplers
				proc_fields.resamplersIn_obj[i] = NULL;
				proc_fields.resamplersIn_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_RESAMPLER,  &proc_fields.resamplersIn_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_obj[i]->request_specialization(
					reinterpret_cast<jvxHandle**>(&proc_fields.resamplersIn_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_hdl[i]->prepare(processingControl.computedParameters.bSize_hw,
					processingControl.computedParameters.bSize_hw_ds,
					inRate, outRate, runtime.props_user.resampler.orderFilterResamplingInput,
					true, processingControl.computedParameters.form_hw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start converters
				proc_fields.convertersIn_obj[i] = NULL;
				proc_fields.convertersIn_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_DATACONVERTER,  &proc_fields.convertersIn_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_obj[i]->request_specialization(
					reinterpret_cast<jvxHandle**>(&proc_fields.convertersIn_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_hdl[i]->prepare(processingControl.computedParameters.form_hw,
					processingControl.computedParameters.form_sw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// STart buffer B
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_in[i], jvxByte, proc_fields.szElementHW* processingControl.computedParameters.bSize_hw_ds);
				memset(proc_fields.ptrB_in[i], 0, sizeof(jvxByte) * proc_fields.szElementHW * processingControl.computedParameters.bSize_hw_ds);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			}

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer A
			allocateBufferA();
			/*
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in, jvxByte**, processingControl.numBuffers_sender_to_receiver_hw);
			for(m = 0; m < processingControl.numBuffers_sender_to_receiver_hw; m++)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in[m], jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_in);
				memset(proc_fields.ptrA_in[m], 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				for(i = 0; i <(unsigned) processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
				{

					// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
					// Start the buffer A
					JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in[m][i], jvxByte, proc_fields.szElementHW * processingControl.computedParameters.bSize_hw);
					memset(proc_fields.ptrA_in[m][i], 0, sizeof(jvxByte) * proc_fields.szElementHW * processingControl.computedParameters.bSize_hw);
					// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				}
			}*/
			break;

		case RES_DACON_TB_PROC:

			// Some consistency checks

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate circular buffer
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.circularBuffersIn, CIRCBUFFER*,processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.circularBuffersIn, 0, sizeof(CIRCBUFFER*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate resamplers
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersIn_hdl, IjvxResampler*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.resamplersIn_hdl, 0, sizeof(IjvxResampler*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersIn_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.resamplersIn_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate converters
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersIn_hdl, IjvxDataConverter*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.convertersIn_hdl, 0, sizeof(IjvxDataConverter*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersIn_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.convertersIn_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer B
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_in, jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.ptrB_in, 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer C
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrC_in, jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_in);
			memset(proc_fields.ptrC_in, 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start resamplers
				proc_fields.resamplersIn_obj[i] = NULL;
				proc_fields.resamplersIn_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_RESAMPLER,  &proc_fields.resamplersIn_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_obj[i]->request_specialization(reinterpret_cast<jvxHandle**>(&proc_fields.resamplersIn_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_hdl[i]->prepare(processingControl.computedParameters.bSize_hw,
					processingControl.computedParameters.bSize_hw_ds,
					inRate, outRate, runtime.props_user.resampler.orderFilterResamplingInput,
					true, processingControl.computedParameters.form_hw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start converters
				proc_fields.convertersIn_obj[i] = NULL;
				proc_fields.convertersIn_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_DATACONVERTER,  &proc_fields.convertersIn_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_obj[i]->request_specialization(reinterpret_cast<jvxHandle**>(&proc_fields.convertersIn_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_hdl[i]->prepare(processingControl.computedParameters.form_hw,
					processingControl.computedParameters.form_sw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start circular buffer
				JVX_DSP_SAFE_ALLOCATE_OBJECT(proc_fields.circularBuffersIn[i], CIRCBUFFER);
				resL = proc_fields.circularBuffersIn[i]->initialize((2* processingControl.computedParameters.bSize_sw 
					* processingControl.computedParameters.qual_sc_factor +
					processingControl.computedParameters.bSize_hw_ds ),
					processingControl.computedParameters.form_sw);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.circularBuffersIn[i]->start(processingControl.computedParameters.fillHeightInput);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// STart buffer B
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_in[i], jvxByte, proc_fields.szElementHW* processingControl.computedParameters.bSize_hw_ds);
				memset(proc_fields.ptrB_in[i], 0, sizeof(jvxByte) * proc_fields.szElementHW * processingControl.computedParameters.bSize_hw_ds);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// STart buffer C
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrC_in[i], jvxByte, proc_fields.szElementSW* processingControl.computedParameters.bSize_hw_ds);
				memset(proc_fields.ptrC_in[i], 0, sizeof(jvxByte) * proc_fields.szElementSW * processingControl.computedParameters.bSize_hw_ds);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			}

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer A
			allocateBufferA();
			/*
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in, jvxByte**, processingControl.numBuffers_sender_to_receiver_hw);
			for(m = 0; m < processingControl.numBuffers_sender_to_receiver_hw; m++)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in[m], jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_in);
				memset(proc_fields.ptrA_in[m], 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_in);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				for(i = 0; i <(unsigned) processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
				{

					// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
					// Start the buffer A
					JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrA_in[m][i], jvxByte, proc_fields.szElementHW * processingControl.computedParameters.bSize_hw);
					memset(proc_fields.ptrA_in[m][i], 0, sizeof(jvxByte) * proc_fields.szElementHW * processingControl.computedParameters.bSize_hw);
					// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				}
			}*/
			break;

		default:
			// Not yet implemented
			break;
		}
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess_inputprocessing()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	if(res == JVX_NO_ERROR)
	{
		switch(proc_fields.seq_operation_in)
		{
		case NOTHING:

			// Only file input. File input has been completely handled before
			break;
		case PROC:

			// Input buffers are allocated in audio node, nothing to do here
			break;

		case TB_PROC:


			for(i = 0; i <(unsigned) processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				resL = proc_fields.circularBuffersIn[i]->stop();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.circularBuffersIn[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				JVX_DSP_SAFE_DELETE_OBJECT(proc_fields.circularBuffersIn[i]);

			}

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.circularBuffersIn, CIRCBUFFER*);
			deallocateBufferA();
			/*
			for(m = 0; m < processingControl.numBuffers_sender_to_receiver_hw; m++)
			{
				for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
				{
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in[m][i], jvxByte);
				}
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in[m], jvxByte*);
			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in, jvxByte*);*/
			break;

		case RES_PROC:


			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{

				resL = proc_fields.resamplersIn_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RESAMPLER, proc_fields.resamplersIn_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.resamplersIn_obj[i] = NULL;
				proc_fields.resamplersIn_hdl[i] = NULL;

			}

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersIn_hdl, IjvxResampler*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersIn_obj, IjvxObject*);
			deallocateBufferA();
			/*
			for(m = 0; m < processingControl.numBuffers_sender_to_receiver_hw; m++)
			{
				for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
				{
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in[m][i], jvxByte);
				}
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in[m], jvxByte*);
			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in, jvxByte*);*/
			break;

		case RES_TB_PROC:

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				resL = proc_fields.circularBuffersIn[i]->stop();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.circularBuffersIn[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				JVX_DSP_SAFE_DELETE_OBJECT(proc_fields.circularBuffersIn[i]);

				resL = proc_fields.resamplersIn_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RESAMPLER, proc_fields.resamplersIn_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.resamplersIn_obj[i] = NULL;
				proc_fields.resamplersIn_hdl[i] = NULL;

				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_in[i], jvxByte);
			}

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.circularBuffersIn, CIRCBUFFER*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersIn_hdl, IjvxResampler*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersIn_obj, IjvxObject*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_in, jvxByte*);
			deallocateBufferA();
			/*
			for(m = 0; m < processingControl.numBuffers_sender_to_receiver_hw; m++)
			{
				for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
				{
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in[m][i], jvxByte);
				}
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in[m], jvxByte*);
			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in, jvxByte*);
			*/
			break;

		case DACON_PROC:

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				resL = proc_fields.convertersIn_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_DATACONVERTER, proc_fields.convertersIn_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.convertersIn_obj[i] = NULL;
				proc_fields.convertersIn_hdl[i] = NULL;

			}

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersIn_hdl, IjvxDataConverter*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersIn_obj, IjvxObject*);
			deallocateBufferA();
			/*
			for(m = 0; m < processingControl.numBuffers_sender_to_receiver_hw; m++)
			{
				for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
				{
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in[m][i], jvxByte);
				}
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in[m], jvxByte*);
			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in, jvxByte*);
			*/
			break;

		case DACON_TB_PROC:

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				resL = proc_fields.circularBuffersIn[i]->stop();
				resL = proc_fields.circularBuffersIn[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				JVX_SAFE_DELETE_OBJ(proc_fields.circularBuffersIn[i]);

				resL = proc_fields.convertersIn_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_DATACONVERTER, proc_fields.convertersIn_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.convertersIn_obj[i] = NULL;
				proc_fields.convertersIn_hdl[i] = NULL;

				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_in[i], jvxByte);
			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.circularBuffersIn, CIRCBUFFER*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersIn_hdl, IjvxDataConverter*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersIn_obj, IjvxObject*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_in, jvxByte*);
			deallocateBufferA();
			/*
			for(m = 0; m < processingControl.numBuffers_sender_to_receiver_hw; m++)
			{
				for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
				{
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in[m][i], jvxByte);
				}
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in[m], jvxByte*);
			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in, jvxByte*);
			*/
			break;

		case RES_DACON_PROC:

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				resL = proc_fields.resamplersIn_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RESAMPLER, proc_fields.resamplersIn_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.resamplersIn_obj[i] = NULL;
				proc_fields.resamplersIn_hdl[i] = NULL;

				resL = proc_fields.convertersIn_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_DATACONVERTER, proc_fields.convertersIn_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.convertersIn_obj[i] = NULL;

				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_in[i], jvxByte);
			}

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersIn_hdl, IjvxResampler*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersIn_obj, IjvxObject*);

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersIn_hdl, IjvxDataConverter*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersIn_obj, IjvxObject*);

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_in, jvxByte*);
			deallocateBufferA();
			/*
			for(m = 0; m < processingControl.numBuffers_sender_to_receiver_hw; m++)
			{
				for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
				{
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in[m][i], jvxByte);
				}
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in[m], jvxByte*);
			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in, jvxByte*);*/
			break;

		case RES_DACON_TB_PROC:

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
			{
				resL = proc_fields.resamplersIn_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersIn_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RESAMPLER, proc_fields.resamplersIn_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.resamplersIn_obj[i] = NULL;
				proc_fields.resamplersIn_hdl[i] = NULL;

				resL = proc_fields.convertersIn_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersIn_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_DATACONVERTER, proc_fields.convertersIn_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.convertersIn_obj[i] = NULL;
				proc_fields.convertersIn_hdl[i] = NULL;

				resL = proc_fields.circularBuffersIn[i]->stop();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.circularBuffersIn[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				JVX_DSP_SAFE_DELETE_OBJECT(proc_fields.circularBuffersIn[i]);

				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_in[i], jvxByte);
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrC_in[i], jvxByte);
			}

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.circularBuffersIn, CIRCBUFFER*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersIn_hdl, IjvxResampler*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersIn_obj, IjvxObject*);

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersIn_hdl, IjvxDataConverter*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersIn_obj, IjvxObject*);

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_in, jvxByte*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrC_in, jvxByte*);
			deallocateBufferA();
			/*
			for(m = 0; m < processingControl.numBuffers_sender_to_receiver_hw; m++)
			{
				for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_in; i++)
				{
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in[m][i], jvxByte);
				}
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in[m], jvxByte*);
			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrA_in, jvxByte*);
			*/
			break;

		default:
			// Not yet implemented
			break;
		}

		switch(proc_fields.seq_operation_in)
		{
		case TB_PROC:
		case RES_PROC:
		case DACON_PROC:
		case DACON_TB_PROC:
		case RES_DACON_PROC:
		case RES_DACON_TB_PROC:
			runtime.channelMappings.inputChannelMapper_hwrev.clear();// JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.mapToOutput_in, jvxSize);
		}

		// Set all pointers to NULL
		proc_fields.resamplersIn_obj = NULL;
		proc_fields.resamplersIn_hdl = NULL;
		proc_fields.convertersIn_obj = NULL;
		proc_fields.convertersIn_hdl = NULL;

		proc_fields.circularBuffersIn = NULL;

		proc_fields.ptrA_in_raw = NULL;
		proc_fields.ptrA_in_net = NULL;
		proc_fields.ptrA_in_off = NULL;
		proc_fields.ptrA_in_sz = 0;

		proc_fields.ptrB_in = NULL;
		proc_fields.ptrC_in = NULL;

		//proc_fields.mapToOutput_in = NULL;

	}
	return(res);
}

// ##################################################################################
// ##################################################################################
// ##################################################################################

jvxErrorType
CjvxGenericWrapperDevice::prepare_outputprocessing(jvxSize prepend, jvxSize append, jvxSize align)
{
	jvxSize i,j;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	if(res == JVX_NO_ERROR)
	{
		jvxData inRate = 0;
		jvxData outRate = 0;
		int cnt = 0;
		//int idp = 0;

		// Allocate data buffers
		// ======================================================
		// INPUT INPUT
		// ======================================================

		//assert(shareWithUI.inputRoutingTable.size() == proc_fields.chansInDev+proc_fields.chansInFIO);
		// Only in synch mode assert(processingControl.computedParameters.bSize_hw_ds == processingControl.computedParameters.bSize_sw);

		// Set all pointers to NULL
		proc_fields.resamplersOut_obj = NULL;
		proc_fields.resamplersOut_hdl = NULL;

		proc_fields.convertersOut_obj = NULL;
		proc_fields.convertersOut_hdl = NULL;
		proc_fields.circularBuffersOut = NULL;

		proc_fields.ptrA_out_net = NULL;
		proc_fields.ptrA_out_raw = NULL;
		proc_fields.ptrA_out_off = NULL;
		proc_fields.ptrB_out = NULL;
		proc_fields.ptrC_out = NULL;

		// Helper variables
		inRate = processingControl.computedParameters.sRate_sw_real;
		outRate = (jvxData)processingControl.computedParameters.sRate_hw;

		switch(proc_fields.seq_operation_out)
		{
		case TB_PROC:
		case RES_PROC:
		case RES_TB_PROC:
		case DACON_PROC:
		case DACON_TB_PROC:
		case RES_DACON_PROC:
		case RES_DACON_TB_PROC:
			//JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.mapToInput_out, jvxSize, processingControl.inProc.params_fixed_runtime.chanshw_out);
			runtime.channelMappings.outputChannelMapper_hwrev.resize(processingControl.inProc.params_fixed_runtime.chanshw_out);

			for(i = 0; i <(unsigned) processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{
				// Find the correct channel output index
				for(j = 0; j < runtime.channelMappings.outputChannelMapper.size(); j++)
				{
					if(runtime.channelMappings.outputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE)
					{
						if(runtime.channelMappings.outputChannelMapper[j].toHwFile.pointsToIdSelectionThisDevice == i)
						{
							runtime.channelMappings.outputChannelMapper_hwrev[i] = runtime.channelMappings.outputChannelMapper[j].toSw.pointsToId_inclFiles;
							//proc_fields.mapToInput_out[i] = runtime.channelMappings.outputChannelMapper[j].toSw.pointsToId_inclFiles;
						}
					}
				}
			}
		}

		switch(proc_fields.seq_operation_out)
		{
		case NOTHING:

			// Only file input. File input has been completely handled before
			break;
		case PROC:

			// Some consistency checks
			assert(processingControl.computedParameters.bSize_hw == processingControl.computedParameters.bSize_hw_ds);
			assert(processingControl.computedParameters.bSize_hw == processingControl.computedParameters.bSize_sw);
			assert(processingControl.computedParameters.form_hw == processingControl.computedParameters.form_sw);

			// Input buffers are allocated in audio node, nothing to do here
			break;

		case TB_PROC:

			// Some consistency checks
			assert(processingControl.computedParameters.bSize_hw == processingControl.computedParameters.bSize_hw_ds);
			assert(processingControl.computedParameters.form_hw == processingControl.computedParameters.form_sw);

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Circular Buffer
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.circularBuffersOut, CIRCBUFFER*,processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.circularBuffersOut, 0, sizeof(CIRCBUFFER*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *


			for(i = 0; i <(unsigned) processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start circular buffer
				JVX_DSP_SAFE_ALLOCATE_OBJECT(proc_fields.circularBuffersOut[i], CIRCBUFFER);
				resL = proc_fields.circularBuffersOut[i]->initialize((2* processingControl.computedParameters.bSize_sw
					* processingControl.computedParameters.qual_sc_factor +
					processingControl.computedParameters.bSize_hw_ds ),
					processingControl.computedParameters.form_sw);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.circularBuffersOut[i]->start(processingControl.computedParameters.fillHeightOutput);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			}

			allocateBufferA_out(prepend, append, align);

			break;

		case RES_PROC:

			// Some consistency checks
			assert(processingControl.computedParameters.bSize_sw == processingControl.computedParameters.bSize_hw_ds);
			assert(processingControl.computedParameters.form_hw == processingControl.computedParameters.form_sw);

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate resamplers
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersOut_hdl, IjvxResampler*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.resamplersOut_hdl, 0, sizeof(IjvxResampler*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersOut_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.resamplersOut_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start resamples
				proc_fields.resamplersOut_obj[i] = NULL;
				proc_fields.resamplersOut_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_RESAMPLER, &proc_fields.resamplersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_obj[i]->request_specialization(reinterpret_cast<jvxHandle**>(&proc_fields.resamplersOut_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_hdl[i]->prepare(processingControl.computedParameters.bSize_sw,
					processingControl.computedParameters.bSize_hw,
					inRate, outRate, runtime.props_user.resampler.orderFilterResamplingOutput,
					true, processingControl.computedParameters.form_sw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			}
			allocateBufferA_out(prepend, append, align);
			break;

		case RES_TB_PROC:

			// Some consistency checks
			assert(processingControl.computedParameters.form_hw == processingControl.computedParameters.form_sw);

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate circular buffer
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.circularBuffersOut, CIRCBUFFER*,processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.circularBuffersOut, 0, sizeof(CIRCBUFFER*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate resamplers
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersOut_hdl, IjvxResampler*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.resamplersOut_hdl, 0, sizeof(IjvxResampler*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersOut_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.resamplersOut_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer B
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_out, jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.ptrB_out, 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_out);

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start circular buffer
				JVX_DSP_SAFE_ALLOCATE_OBJECT(proc_fields.circularBuffersOut[i], CIRCBUFFER);
				resL = proc_fields.circularBuffersOut[i]->initialize((2* processingControl.computedParameters.bSize_sw *
					processingControl.computedParameters.qual_sc_factor +
					processingControl.computedParameters.bSize_hw_ds ),
					processingControl.computedParameters.form_sw);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.circularBuffersOut[i]->start(processingControl.computedParameters.fillHeightOutput);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start resamples
				proc_fields.resamplersOut_obj[i] = NULL;
				proc_fields.resamplersOut_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_RESAMPLER,  &proc_fields.resamplersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_obj[i]->request_specialization(
					reinterpret_cast<jvxHandle**>(&proc_fields.resamplersOut_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_hdl[i]->prepare(processingControl.computedParameters.bSize_hw_ds,
					processingControl.computedParameters.bSize_hw,
					inRate, outRate, runtime.props_user.resampler.orderFilterResamplingOutput,
					true, processingControl.computedParameters.form_hw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// STart buffer B
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_out[i], jvxByte, proc_fields.szElementSW* processingControl.computedParameters.bSize_hw_ds);
				memset(proc_fields.ptrB_out[i], 0, sizeof(jvxByte) * proc_fields.szElementSW * processingControl.computedParameters.bSize_hw_ds);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			}
			allocateBufferA_out(prepend, append, align);
			break;

		case DACON_PROC:

			// Some consistency checks
			assert(processingControl.computedParameters.bSize_hw == processingControl.computedParameters.bSize_hw_ds);
			assert(processingControl.computedParameters.bSize_hw == processingControl.computedParameters.bSize_sw);

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate converters
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersOut_hdl, IjvxDataConverter*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.convertersOut_hdl, 0, sizeof(IjvxDataConverter*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersOut_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.convertersOut_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start converters
				proc_fields.convertersOut_obj[i] = NULL;
				proc_fields.convertersOut_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_DATACONVERTER,  &proc_fields.convertersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_obj[i]->request_specialization(
					reinterpret_cast<jvxHandle**>(&proc_fields.convertersOut_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_hdl[i]->prepare(processingControl.computedParameters.form_sw,
					processingControl.computedParameters.form_hw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			}
			allocateBufferA_out(prepend, append, align);
			break;

		case DACON_TB_PROC:

			// Some consistency checks
			assert(processingControl.computedParameters.bSize_hw == processingControl.computedParameters.bSize_hw_ds);

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate circular buffer
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.circularBuffersOut, CIRCBUFFER*,processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.circularBuffersOut, 0, sizeof(CIRCBUFFER*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate converters
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersOut_hdl, IjvxDataConverter*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.convertersOut_hdl, 0, sizeof(IjvxDataConverter*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersOut_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.convertersOut_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer B
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_out, jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.ptrB_out, 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start circular buffer
				JVX_DSP_SAFE_ALLOCATE_OBJECT(proc_fields.circularBuffersOut[i], CIRCBUFFER);
				resL = proc_fields.circularBuffersOut[i]->initialize((2* processingControl.computedParameters.bSize_sw *
					processingControl.computedParameters.qual_sc_factor +
					processingControl.computedParameters.bSize_hw_ds ),
					processingControl.computedParameters.form_sw);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.circularBuffersOut[i]->start(processingControl.computedParameters.fillHeightOutput);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start converters
				proc_fields.convertersOut_obj[i] = NULL;
				proc_fields.convertersOut_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_DATACONVERTER,  &proc_fields.convertersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_obj[i]->request_specialization(reinterpret_cast<jvxHandle**>(&proc_fields.convertersOut_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_hdl[i]->prepare(processingControl.computedParameters.form_sw,
					processingControl.computedParameters.form_hw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// STart buffer B
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_out[i], jvxByte, proc_fields.szElementSW* processingControl.computedParameters.bSize_hw);
				memset(proc_fields.ptrB_out[i], 0, sizeof(jvxByte) * proc_fields.szElementSW * processingControl.computedParameters.bSize_hw);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			}
			allocateBufferA_out(prepend, append, align);
			break;

		case RES_DACON_PROC:

			// Some consistency checks
			assert(processingControl.computedParameters.bSize_hw_ds == processingControl.computedParameters.bSize_sw);

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate resamplers
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersOut_hdl, IjvxResampler*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.resamplersOut_hdl, 0, sizeof(IjvxResampler*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersOut_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.resamplersOut_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate converters
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersOut_hdl, IjvxDataConverter*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.convertersOut_hdl, 0, sizeof(IjvxDataConverter*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersOut_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.convertersOut_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer B
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_out, jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.ptrB_out, 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start resamplers
				proc_fields.resamplersOut_obj[i] = NULL;
				proc_fields.resamplersOut_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_RESAMPLER,  &proc_fields.resamplersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_obj[i]->request_specialization(reinterpret_cast<jvxHandle**>(&proc_fields.resamplersOut_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_hdl[i]->prepare(processingControl.computedParameters.bSize_sw,
					processingControl.computedParameters.bSize_hw,
					inRate, outRate, runtime.props_user.resampler.orderFilterResamplingOutput,
					true, processingControl.computedParameters.form_hw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start converters
				proc_fields.convertersOut_obj[i] = NULL;
				proc_fields.convertersOut_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_DATACONVERTER,  &proc_fields.convertersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_obj[i]->request_specialization(reinterpret_cast<jvxHandle**>(&proc_fields.convertersOut_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_hdl[i]->prepare(processingControl.computedParameters.form_sw,
					processingControl.computedParameters.form_hw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *


				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// STart buffer B
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_out[i], jvxByte, proc_fields.szElementHW* processingControl.computedParameters.bSize_sw);
				memset(proc_fields.ptrB_out[i], 0, sizeof(jvxByte) * proc_fields.szElementHW * processingControl.computedParameters.bSize_sw);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			}
			allocateBufferA_out(prepend, append, align);

			break;

		case RES_DACON_TB_PROC:

			// Some consistency checks

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate circular buffer
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.circularBuffersOut, CIRCBUFFER*,processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.circularBuffersOut, 0, sizeof(CIRCBUFFER*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate resamplers
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersOut_hdl, IjvxResampler*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.resamplersOut_hdl, 0, sizeof(IjvxResampler*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.resamplersOut_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.resamplersOut_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate converters
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersOut_hdl, IjvxDataConverter*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.convertersOut_hdl, 0, sizeof(IjvxDataConverter*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.convertersOut_obj, IjvxObject*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.convertersOut_obj, 0, sizeof(IjvxObject*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer B
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_out, jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.ptrB_out, 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			// Allocate Buffer C
			JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrC_out, jvxByte*, processingControl.inProc.params_fixed_runtime.chanshw_out);
			memset(proc_fields.ptrC_out, 0, sizeof(char*)*processingControl.inProc.params_fixed_runtime.chanshw_out);
			// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start resamplers
				proc_fields.resamplersOut_obj[i] = NULL;
				proc_fields.resamplersOut_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_RESAMPLER,  &proc_fields.resamplersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_obj[i]->request_specialization(
					reinterpret_cast<jvxHandle**>(&proc_fields.resamplersOut_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_hdl[i]->prepare(processingControl.computedParameters.bSize_hw_ds,
					processingControl.computedParameters.bSize_hw,
					inRate, outRate, runtime.props_user.resampler.orderFilterResamplingOutput,
					true, processingControl.computedParameters.form_hw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start converters
				proc_fields.convertersOut_obj[i] = NULL;
				proc_fields.convertersOut_hdl[i] = NULL;
				resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_DATACONVERTER,  &proc_fields.convertersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_obj[i]->request_specialization(reinterpret_cast<jvxHandle**>(&proc_fields.convertersOut_hdl[i]), NULL, NULL);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_hdl[i]->initialize(onInit.parentRef->_common_set_min.theHostRef);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_hdl[i]->prepare(processingControl.computedParameters.form_sw,
					processingControl.computedParameters.form_hw);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// Start circular buffer
				JVX_DSP_SAFE_ALLOCATE_OBJECT(proc_fields.circularBuffersOut[i], CIRCBUFFER);
				resL = proc_fields.circularBuffersOut[i]->initialize((2* processingControl.computedParameters.bSize_sw *
					processingControl.computedParameters.qual_sc_factor +
					processingControl.computedParameters.bSize_hw_ds ),
					processingControl.computedParameters.form_sw);
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.circularBuffersOut[i]->start(processingControl.computedParameters.fillHeightOutput);
				assert(resL == JVX_NO_ERROR);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// STart buffer B
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrB_out[i], jvxByte, proc_fields.szElementSW* processingControl.computedParameters.bSize_hw_ds);
				memset(proc_fields.ptrB_out[i], 0, sizeof(jvxByte) * proc_fields.szElementSW * processingControl.computedParameters.bSize_hw_ds);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *

				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
				// STart buffer C
				JVX_DSP_SAFE_ALLOCATE_FIELD(proc_fields.ptrC_out[i], jvxByte, proc_fields.szElementHW* processingControl.computedParameters.bSize_hw_ds);
				memset(proc_fields.ptrC_out[i], 0, sizeof(jvxByte) * proc_fields.szElementHW * processingControl.computedParameters.bSize_hw_ds);
				// *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  * *  *
			}
			allocateBufferA_out(prepend, append, align);

			break;

		default:
			// Not yet implemented
			break;
		}
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess_outputprocessing()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	if(res == JVX_NO_ERROR)
	{
		switch(proc_fields.seq_operation_out)
		{
		case NOTHING:

			// Only file input. File input has been completely handled before
			break;
		case PROC:

			// Input buffers are allocated in audio node, nothing to do here
			break;

		case TB_PROC:

			for(i = 0; i <(unsigned) processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{

				resL = proc_fields.circularBuffersOut[i]->stop();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.circularBuffersOut[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				JVX_DSP_SAFE_DELETE_OBJECT(proc_fields.circularBuffersOut[i]);

			}

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.circularBuffersOut, CIRCBUFFER*);

			deallocateBufferA_out();
			break;

		case RES_PROC:

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{

				resL = proc_fields.resamplersOut_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RESAMPLER, proc_fields.resamplersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.resamplersOut_obj[i] = NULL;
				proc_fields.resamplersOut_hdl[i] = NULL;

			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersOut_hdl, IjvxResampler*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersOut_obj, IjvxObject*);

			deallocateBufferA_out();
			break;

		case RES_TB_PROC:

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{
				resL = proc_fields.circularBuffersOut[i]->stop();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.circularBuffersOut[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				JVX_DSP_SAFE_DELETE_OBJECT(proc_fields.circularBuffersOut[i]);

				resL = proc_fields.resamplersOut_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RESAMPLER, proc_fields.resamplersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.resamplersOut_obj[i] = NULL;
				proc_fields.resamplersOut_hdl[i] = NULL;

				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_out[i], jvxByte);
			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.circularBuffersOut, CIRCBUFFER*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersOut_hdl, IjvxResampler*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersOut_obj, IjvxObject*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_out, jvxByte*);
			deallocateBufferA_out();
			break;

		case DACON_PROC:

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{
				resL = proc_fields.convertersOut_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_DATACONVERTER, proc_fields.convertersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.convertersOut_obj[i] = NULL;
				proc_fields.convertersOut_hdl[i] = NULL;

			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersOut_hdl, IjvxDataConverter*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersOut_obj, IjvxObject*);
			deallocateBufferA_out();
			break;

		case DACON_TB_PROC:

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{
				resL = proc_fields.circularBuffersOut[i]->stop();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.circularBuffersOut[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				JVX_DSP_SAFE_DELETE_OBJECT(proc_fields.circularBuffersOut[i]);

				resL = proc_fields.convertersOut_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_DATACONVERTER, proc_fields.convertersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.convertersOut_obj[i] = NULL;
				proc_fields.convertersOut_hdl[i] = NULL;

				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_out[i], jvxByte);
			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.circularBuffersOut, CIRCBUFFER*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersOut_hdl, IjvxDataConverter*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersOut_obj, IjvxObject*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_out, jvxByte*);
			deallocateBufferA_out();
			break;

		case RES_DACON_PROC:

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{
				resL = proc_fields.resamplersOut_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RESAMPLER, proc_fields.resamplersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.resamplersOut_obj[i] = NULL;
				proc_fields.resamplersOut_hdl[i] = NULL;

				resL = proc_fields.convertersOut_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_DATACONVERTER, proc_fields.convertersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.convertersOut_obj[i] = NULL;
				proc_fields.convertersOut_hdl[i] = NULL;

				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_out[i], jvxByte);
			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersOut_hdl, IjvxResampler*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersOut_obj, IjvxObject*);

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersOut_hdl, IjvxDataConverter*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersOut_obj, IjvxObject*);

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_out, jvxByte*);
			deallocateBufferA_out();

			break;

		case RES_DACON_TB_PROC:

			for(i = 0; i < (unsigned)processingControl.inProc.params_fixed_runtime.chanshw_out; i++)
			{
				resL = proc_fields.resamplersOut_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.resamplersOut_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RESAMPLER, proc_fields.resamplersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.resamplersOut_obj[i] = NULL;
				proc_fields.resamplersOut_hdl[i] = NULL;

				resL = proc_fields.convertersOut_hdl[i]->postprocess();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.convertersOut_hdl[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				resL = onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_DATACONVERTER, proc_fields.convertersOut_obj[i], 0, NULL);
				assert(resL == JVX_NO_ERROR);
				proc_fields.convertersOut_obj[i] = NULL;
				proc_fields.convertersOut_hdl[i] = NULL;

				resL = proc_fields.circularBuffersOut[i]->stop();
				assert(resL == JVX_NO_ERROR);
				resL = proc_fields.circularBuffersOut[i]->terminate();
				assert(resL == JVX_NO_ERROR);
				JVX_DSP_SAFE_DELETE_OBJECT(proc_fields.circularBuffersOut[i]);

				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_out[i], jvxByte);
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrC_out[i], jvxByte);
			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.circularBuffersOut, CIRCBUFFER*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersOut_hdl, IjvxResampler*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.resamplersOut_obj, IjvxObject*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersOut_hdl, IjvxDataConverter*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.convertersOut_obj, IjvxObject*);

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrB_out, jvxByte*);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(proc_fields.ptrC_out, jvxByte*);

			deallocateBufferA_out();
			break;

		default:
			// Not yet implemented
			break;
		}
		
		switch(proc_fields.seq_operation_out)
		{
		case TB_PROC:
		case RES_PROC:
		case DACON_PROC:
		case DACON_TB_PROC:
		case RES_DACON_PROC:
		case RES_DACON_TB_PROC:
			//JVX_DSP_SAFE_DELETE_FIELD(proc_fields.mapToInput_out, jvxSize, processingControl.inProc.params_fixed_runtime.chanshw_out);
			runtime.channelMappings.outputChannelMapper_hwrev.clear();
		}

		// Set all pointers to NULL
		proc_fields.resamplersOut_obj = NULL;
		proc_fields.resamplersOut_hdl = NULL;

		proc_fields.convertersOut_obj = NULL;
		proc_fields.convertersOut_hdl = NULL;
		proc_fields.circularBuffersOut = NULL;

		proc_fields.ptrA_out_net = NULL;
		proc_fields.ptrA_out_raw = NULL;
		proc_fields.ptrA_out_off = NULL;
		proc_fields.ptrB_out = NULL;
		proc_fields.ptrC_out = NULL;
	//	proc_fields.mapToInput_out = NULL;
	}
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::prepare_inputreferences()
{
	jvxSize i,j,m;

	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	// =========================================================================
	// Distinguish between selected and non-selected input files
	// =========================================================================
	if(res == JVX_NO_ERROR)
	{
		//=======================================================
		// Open the input files and compute everything required for processing.
		// We must do this before HW setup since we will set cross references
		//=======================================================

		// ===============================================================
		// Deal with input and output files
		// ===============================================================

		// Cross check which files must be opened according to channel selection
		// We should not see any file removal after state PREPARED has started!!
		for(i = 0; i < onInit.theExtRef->theInputReferences.size(); i++)
		{
			onInit.theExtRef->theInputReferences[i].processing.selected = false;
			onInit.theExtRef->theInputReferences[i].processing.numberLostFrames = 0;
			for(j = 0; j < runtime.channelMappings.inputChannelMapper.size(); j++)
			{
				if(
					(runtime.channelMappings.inputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL) &&
					(runtime.channelMappings.inputChannelMapper[j].isSelected == true) &&
					(runtime.channelMappings.inputChannelMapper[j].refHint == onInit.theExtRef->theInputReferences[i].uniqueIdHandles)
					)
				{
					onInit.theExtRef->theInputReferences[i].processing.selected = true;
					break;
				}
			}
		}
	}

	// =========================================================================
	// Open input files and prepare processing
	// =========================================================================
	if(res == JVX_NO_ERROR)
	{
		for(i = 0; i < onInit.theExtRef->theInputReferences.size(); i++)
		{
			onInit.theExtRef->theInputReferences[i].processing.theStatus = FILE_INACTIVE;
			if(onInit.theExtRef->theInputReferences[i].processing.selected)
			{
				resL = onInit.theExtRef->theInputReferences[i].theCallback->prepare(
					true,
					onInit.theExtRef->theInputReferences[i].uniqueIdHandles,
					onInit.theExtRef->theInputReferences[i].processing.bSize_rs,
					processingControl.inProc.params_fixed_runtime.format);

				if(resL != JVX_NO_ERROR)
				{
					onInit.theExtRef->theInputReferences[i].processing.theStatus = FILE_BROKEN;
				}
				else
				{
					onInit.theExtRef->theInputReferences[i].processing.theStatus = FILE_ACCESS;
				}

				// ===========================================================
				// == Allocate buffers
				// ===========================================================
				//					if(onInit.theFilesRef->theInputFiles[i].common.number_channels)
				//					{
				jvxInt32 numChans = onInit.theExtRef->theInputReferences[i].number_channels;
				jvxSize szElm = jvxDataFormat_size[processingControl.inProc.params_fixed_runtime.format];
				jvxSize bSize = processingControl.computedParameters.bSize_sw;
				jvxSize bSizeRs = onInit.theExtRef->theInputReferences[i].processing.bSize_rs;
				jvxData inRate = (jvxData)onInit.theExtRef->theInputReferences[i].processing.sRate_rs;
				jvxData outRate = processingControl.computedParameters.sRate_sw_real;

				JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW_orig, jvxByte**, processingControl.numBuffers_sender_to_receiver_sw);
				JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW, jvxByte**, processingControl.numBuffers_sender_to_receiver_sw);
				for(m = 0; m < processingControl.numBuffers_sender_to_receiver_sw; m++)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW[m], jvxByte*, numChans);
					memset(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW[m], 0, sizeof(jvxByte*)*numChans);

					JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW_orig[m], jvxByte*, numChans);
					memset(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW_orig[m], 0, sizeof(jvxByte*)*numChans);
				}


				if(onInit.theExtRef->theInputReferences[i].processing.resampling)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesRS, jvxByte*,  numChans);
					memset(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesRS, 0, sizeof(jvxByte*)*numChans);

					JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theExtRef->theInputReferences[i].processing.resampler, oneResampler, numChans);
					memset(onInit.theExtRef->theInputReferences[i].processing.resampler, 0, sizeof(oneResampler)*numChans);
				}
				else
				{
					onInit.theExtRef->theInputReferences[i].processing.fieldSamplesRS = NULL;
					onInit.theExtRef->theInputReferences[i].processing.resampler = NULL;
				}

				for(m = 0; m < processingControl.numBuffers_sender_to_receiver_sw; m++)
				{
					for(j = 0; j < (unsigned)numChans; j++)
					{
						// Allocate fields for all channels but we may use another pointer later
						JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW_orig[m][j], jvxByte, ( bSize* szElm));
						memset(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW_orig[m][j], 0, ( bSize* szElm));
						onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW[m][j] = onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW_orig[m][j];
					}
				}

				for(j = 0; j < (unsigned)numChans; j++)
				{

					// If resampling is required
					if(onInit.theExtRef->theInputReferences[i].processing.resampling)
					{
						JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesRS[j], jvxByte, bSizeRs * szElm);
						memset(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesRS[j], 0, bSizeRs * szElm);

						resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_RESAMPLER,  &onInit.theExtRef->theInputReferences[i].processing.resampler[j].theObj, 0, NULL);
						if((resL == JVX_NO_ERROR) && onInit.theExtRef->theInputReferences[i].processing.resampler[j].theObj)
						{
							onInit.theExtRef->theInputReferences[i].processing.resampler[j].theObj->request_specialization(
								reinterpret_cast<jvxHandle**>(&onInit.theExtRef->theInputReferences[i].processing.resampler[j].theHdl),
								NULL, NULL);
							onInit.theExtRef->theInputReferences[i].processing.resampler[j].theHdl->initialize(_common_set_min.theHostRef);
							resL = onInit.theExtRef->theInputReferences[i].processing.resampler[j].theHdl->prepare(
								bSizeRs,bSize, inRate, outRate, runtime.props_user.resampler.orderFilterResamplingInput,true,
								processingControl.inProc.params_fixed_runtime.format);
						}
						if(resL != JVX_NO_ERROR)
						{
							assert(0);
						}
					}
				}// for(j = 0; j < (unsigned)numChans; j++)
			} // if(onInit.theFilesRef->theInputFiles[i].common.processing.selected)
		} //for(i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
	} // if(res == JVX_NO_ERROR)
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess_inputreferences()
{
	jvxSize i,j,m;
	for(i = 0; i < onInit.theExtRef->theInputReferences.size(); i++)
	{
		if(onInit.theExtRef->theInputReferences[i].processing.selected)
		{
			onInit.theExtRef->theInputReferences[i].theCallback->postprocess(
				true,
				onInit.theExtRef->theInputReferences[i].uniqueIdHandles);

			jvxInt32 numChans = onInit.theExtRef->theInputReferences[i].number_channels;

			for(m = 0; m < processingControl.numBuffers_sender_to_receiver_sw; m++)
			{
				for(j = 0; j < (unsigned)numChans; j++)
				{
					// Allocate fields for all channels but we may use another pointer later
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW_orig[m][j], jvxByte);
				}
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW[m], jvxByte*);
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW_orig[m], jvxByte*);
			}
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW, jvxByte**);
			JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW_orig, jvxByte**);

			for(j = 0; j < (unsigned)numChans; j++)
			{

				// If resampling is required
				if(onInit.theExtRef->theInputReferences[i].processing.resampling)
				{
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesRS[j], jvxByte);

					onInit.theExtRef->theInputReferences[i].processing.resampler[j].theHdl->postprocess();
					onInit.theExtRef->theInputReferences[i].processing.resampler[j].theHdl->terminate();
					onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RESAMPLER,onInit.theExtRef->theInputReferences[i].processing.resampler[j].theObj, 0, NULL);
					onInit.theExtRef->theInputReferences[i].processing.resampler[j].theObj = NULL;
					onInit.theExtRef->theInputReferences[i].processing.resampler[j].theHdl = NULL;
				}

			}// for(j = 0; j < (unsigned)numChans; j++)

			if(onInit.theExtRef->theInputReferences[i].processing.resampling)
			{
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theExtRef->theInputReferences[i].processing.fieldSamplesRS, jvxByte*);
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theExtRef->theInputReferences[i].processing.resampler, oneResampler);
			}
			onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW_orig = NULL;
			onInit.theExtRef->theInputReferences[i].processing.fieldSamplesSW = NULL;
			onInit.theExtRef->theInputReferences[i].processing.fieldSamplesRS = NULL;
			onInit.theExtRef->theInputReferences[i].processing.resampler = NULL;
		}

		onInit.theExtRef->theInputReferences[i].processing.theStatus = FILE_INACTIVE;
		onInit.theExtRef->theInputReferences[i].processing.selected = false;

	}//for(i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperDevice::prepare_outputreferences()
{
	jvxSize i,j,m;

	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	// =========================================================================
	// Find selected and unselected files
	// =========================================================================
	if(res == JVX_NO_ERROR)
	{
		for(i = 0; i < onInit.theExtRef->theOutputReferences.size(); i++)
		{
			onInit.theExtRef->theOutputReferences[i].processing.selected = false;
			onInit.theExtRef->theOutputReferences[i].processing.numberLostFrames = 0;

			for(j = 0; j < runtime.channelMappings.outputChannelMapper.size(); j++)
			{
				if(
					(runtime.channelMappings.outputChannelMapper[j].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL) &&
					(runtime.channelMappings.outputChannelMapper[j].isSelected == true) &&
					(runtime.channelMappings.outputChannelMapper[j].refHint == onInit.theExtRef->theOutputReferences[i].uniqueIdHandles)
					)
				{
					onInit.theExtRef->theOutputReferences[i].processing.selected = true;
					break;
				}
			}
		}
	}

	// =========================================================================
	// Open output files and prepare processing
	// =========================================================================
	if(res == JVX_NO_ERROR)
	{
		for(i = 0; i < onInit.theExtRef->theOutputReferences.size(); i++)
		{
			onInit.theExtRef->theOutputReferences[i].processing.theStatus = FILE_INACTIVE;

			if(onInit.theExtRef->theOutputReferences[i].processing.selected)
			{
				resL = onInit.theExtRef->theOutputReferences[i].theCallback->prepare(
					false,
					onInit.theExtRef->theInputReferences[i].uniqueIdHandles,
					onInit.theExtRef->theInputReferences[i].processing.bSize_rs,
					processingControl.inProc.params_fixed_runtime.format);

				if(resL != JVX_NO_ERROR)
				{
					onInit.theExtRef->theOutputReferences[i].processing.theStatus = FILE_BROKEN;
				}
				else
				{
					onInit.theExtRef->theOutputReferences[i].processing.theStatus = FILE_ACCESS;
				}


				jvxInt32 numChans = onInit.theExtRef->theOutputReferences[i].number_channels;
				jvxSize szElm = jvxDataFormat_size[processingControl.inProc.params_fixed_runtime.format];
				jvxSize bSize = processingControl.computedParameters.bSize_sw;
				jvxSize bSizeRs = onInit.theExtRef->theOutputReferences[i].processing.bSize_rs;
				jvxData outRate = (jvxData)onInit.theExtRef->theOutputReferences[i].processing.sRate_rs;
				jvxData inRate = processingControl.computedParameters.sRate_sw_real;

				// ===========================================================
				// == Output file is openend for the first time
				// ===========================================================
				// Reopen it to specify buffersize
				onInit.theExtRef->theOutputReferences[i].processing.theStatus = FILE_ACCESS;

				onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesSW = NULL;
				onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesRS = NULL;
				onInit.theExtRef->theOutputReferences[i].processing.resampler = NULL;

				JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesSW, jvxByte**, 
					processingControl.numBuffers_receiver_to_sender_sw);
				for(m = 0; m < processingControl.numBuffers_receiver_to_sender_sw; m++)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesSW[m], jvxByte*, numChans);
					memset(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesSW[m], 0, sizeof(jvxByte*)*numChans);
				}

				if(onInit.theExtRef->theOutputReferences[i].processing.resampling)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesRS, jvxByte*, numChans);
					memset(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesRS, 0, sizeof(jvxByte*)*numChans);

					JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theExtRef->theOutputReferences[i].processing.resampler, oneResampler, numChans);
					memset(onInit.theExtRef->theOutputReferences[i].processing.resampler, 0, sizeof(oneResampler)*numChans);
				}


				for(m = 0; m < processingControl.numBuffers_receiver_to_sender_sw; m++)
				{
					for(j = 0; j < (unsigned)numChans; j++)
					{
						JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesSW[m][j], jvxByte, ( bSize* szElm));
						memset(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesSW[m][j], 0, ( bSize* szElm));
					}
				}


				for(j = 0; j < (unsigned)numChans; j++)
				{
					// If resampling is required
					if(onInit.theExtRef->theOutputReferences[i].processing.resampling)
					{

						JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesRS[j], jvxByte, bSizeRs * szElm);
						memset(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesRS[j], 0, bSizeRs * szElm);

						resL = onInit.parentRef->runtime.theToolsHost->instance_tool(JVX_COMPONENT_RESAMPLER,  &onInit.theExtRef->theOutputReferences[i].processing.resampler[j].theObj, 0, NULL);
						if((resL == JVX_NO_ERROR) && onInit.theExtRef->theOutputReferences[i].processing.resampler[j].theObj)
						{
							onInit.theExtRef->theOutputReferences[i].processing.resampler[j].theObj->request_specialization(
								reinterpret_cast<jvxHandle**>(&onInit.theExtRef->theOutputReferences[i].processing.resampler[j].theHdl),
								NULL, NULL);
							onInit.theExtRef->theOutputReferences[i].processing.resampler[j].theHdl->initialize(_common_set_min.theHostRef);
							resL = onInit.theExtRef->theOutputReferences[i].processing.resampler[j].theHdl->prepare(
								processingControl.computedParameters.bSize_sw,
								onInit.theExtRef->theOutputReferences[i].processing.bSize_rs,
								inRate, outRate, runtime.props_user.resampler.orderFilterResamplingOutput,true,
								processingControl.inProc.params_fixed_runtime.format);
						}
						if(resL != JVX_NO_ERROR)
						{
							assert(0);
						}
					}// if(onInit.theFilesRef->theOutputFiles[i].common.processing.resampling)
				} // for(j = 0; j < (unsigned)numChans; j++)
			}//if(onInit.theFilesRef->theOutputFiles[i].common.processing.selected)
		} // for(i = 0; i < onInit.theFilesRef->theOutputFiles.size(); i++)
	}// if(res == JVX_NO_ERROR)
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess_outputreferences()
{
	jvxSize i,j,m;

	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	// =========================================================================
	// Open output files and prepare processing
	// =========================================================================
	if(res == JVX_NO_ERROR)
	{
		for(i = 0; i < onInit.theExtRef->theOutputReferences.size(); i++)
		{
			if(onInit.theExtRef->theOutputReferences[i].processing.selected)
			{
				resL = onInit.theExtRef->theOutputReferences[i].theCallback->postprocess(
					false,
					onInit.theExtRef->theInputReferences[i].uniqueIdHandles);

				jvxInt32 numChans = onInit.theExtRef->theOutputReferences[i].number_channels;

				for(j = 0; j < (unsigned)numChans; j++)
				{

					// If resampling is required
					if(onInit.theExtRef->theOutputReferences[i].processing.resampling)
					{
						JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesRS[j], jvxByte);

						onInit.theExtRef->theOutputReferences[i].processing.resampler[j].theHdl->postprocess();
						onInit.theExtRef->theOutputReferences[i].processing.resampler[j].theHdl->terminate();
						onInit.parentRef->runtime.theToolsHost->return_instance_tool(JVX_COMPONENT_RESAMPLER, onInit.theExtRef->theOutputReferences[i].processing.resampler[j].theObj, 0, NULL);
					}// if(onInit.theFilesRef->theOutputFiles[i].common.processing.resampling)

					
				} // for(j = 0; j < (unsigned)numChans; j++)

				for(m = 0; m < processingControl.numBuffers_receiver_to_sender_sw; m++)
				{
					for(j = 0; j < (unsigned)numChans; j++)
					{
						JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesSW[m][j], jvxByte);
					}
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesSW[m], jvxByte*);
				}

				JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesSW, jvxByte*);

				if(onInit.theExtRef->theOutputReferences[i].processing.resampling)
				{
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesRS, jvxByte*);
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(onInit.theExtRef->theOutputReferences[i].processing.resampler, oneResampler);
				}

			}//if(onInit.theFilesRef->theOutputFiles[i].common.processing.selected)
			onInit.theExtRef->theOutputReferences[i].processing.theStatus = FILE_INACTIVE;
			onInit.theExtRef->theOutputReferences[i].processing.selected = false;

			onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesRS = NULL;
			onInit.theExtRef->theOutputReferences[i].processing.fieldSamplesSW = NULL;
			onInit.theExtRef->theOutputReferences[i].processing.resampler = NULL;

		} // for(i = 0; i < onInit.theFilesRef->theOutputFiles.size(); i++)
	}// if(res == JVX_NO_ERROR)
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::prepare_inputDummies()
{
	jvxSize i, m;

	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	//=======================================================
	// Open the input files and compute everything required for processing.
	// We must do this before HW setup since we will set cross references
	//=======================================================
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < onInit.theDummyRef->inputs.size(); i++)
		{
			onInit.theDummyRef->inputs[i].wasStarted = false;
			for (m = 0; m < runtime.channelMappings.inputChannelMapper.size(); m++)
			{
				if (
					(runtime.channelMappings.inputChannelMapper[m].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY) &&
					(runtime.channelMappings.inputChannelMapper[m].refHint == onInit.theDummyRef->inputs[i].uniqueId) &&
					(runtime.channelMappings.inputChannelMapper[m].isSelected))
				{
					onInit.theDummyRef->inputs[i].wasStarted = true;
					break;
				}
			}
			if (onInit.theDummyRef->inputs[i].wasStarted)
			{
				jvxSize szElm = jvxDataFormat_size[processingControl.computedParameters.form_sw];
				jvxSize bSize = processingControl.computedParameters.bSize_sw;

				JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theDummyRef->inputs[i].buf, jvxByte*, processingControl.numBuffers_sender_to_receiver_sw);
				memset(onInit.theDummyRef->inputs[i].buf, 0, sizeof(jvxByte*) * processingControl.numBuffers_sender_to_receiver_sw);
			}
		} 
	} // if(res == JVX_NO_ERROR)
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::prepare_outputDummies()
{
	jvxSize i, j, m;

	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	//=======================================================
	// Open the input files and compute everything required for processing.
	// We must do this before HW setup since we will set cross references
	//=======================================================
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < onInit.theDummyRef->outputs.size(); i++)
		{
			onInit.theDummyRef->outputs[i].wasStarted = false;
			for (m = 0; m < runtime.channelMappings.outputChannelMapper.size(); m++)
			{
				if (
					(runtime.channelMappings.outputChannelMapper[m].theChannelType == JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DUMMY) &&
					(runtime.channelMappings.outputChannelMapper[m].refHint == onInit.theDummyRef->outputs[i].uniqueId) &&
					(runtime.channelMappings.outputChannelMapper[m].isSelected))
				{
					onInit.theDummyRef->outputs[i].wasStarted = true;
					break;
				}
			}
			if (onInit.theDummyRef->outputs[i].wasStarted)
			{
				jvxSize szElm = jvxDataFormat_size[processingControl.computedParameters.form_sw];
				jvxSize bSize = processingControl.computedParameters.bSize_sw;

				JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theDummyRef->outputs[i].buf, jvxByte*, processingControl.numBuffers_receiver_to_sender_sw);
				for (j = 0; j < processingControl.numBuffers_receiver_to_sender_sw; j++)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD(onInit.theDummyRef->outputs[i].buf[j], jvxByte, szElm*bSize);
					memset(onInit.theDummyRef->outputs[i].buf[j], 0, sizeof(jvxByte)* szElm*bSize);
				}
			}
		} //for(i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
	} // if(res == JVX_NO_ERROR)
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess_inputDummies()
{
	jvxSize i;

	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	//=======================================================
	// Open the input files and compute everything required for processing.
	// We must do this before HW setup since we will set cross references
	//=======================================================
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < onInit.theDummyRef->inputs.size(); i++)
		{
			if (onInit.theDummyRef->inputs[i].wasStarted)
			{
				JVX_DSP_SAFE_DELETE_FIELD(onInit.theDummyRef->inputs[i].buf);
				onInit.theDummyRef->inputs[i].buf = NULL;
			}
			onInit.theDummyRef->inputs[i].wasStarted = false;
		} //for(i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
	} // if(res == JVX_NO_ERROR)
	return(res);
}

jvxErrorType
CjvxGenericWrapperDevice::postprocess_outputDummies()
{
	jvxSize i, j;

	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;

	//=======================================================
	// Open the input files and compute everything required for processing.
	// We must do this before HW setup since we will set cross references
	//=======================================================
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < onInit.theDummyRef->outputs.size(); i++)
		{
			if (onInit.theDummyRef->outputs[i].wasStarted)
			{
				for (j = 0; j < processingControl.numBuffers_receiver_to_sender_sw; j++)
				{
					JVX_DSP_SAFE_DELETE_FIELD(onInit.theDummyRef->outputs[i].buf[j]);
				}
				JVX_DSP_SAFE_DELETE_FIELD(onInit.theDummyRef->outputs[i].buf);
				onInit.theDummyRef->outputs[i].buf = NULL;
			}
			onInit.theDummyRef->outputs[i].wasStarted = false;
		} //for(i = 0; i < onInit.theFilesRef->theInputFiles.size(); i++)
	} // if(res == JVX_NO_ERROR)
	return(res);
}
