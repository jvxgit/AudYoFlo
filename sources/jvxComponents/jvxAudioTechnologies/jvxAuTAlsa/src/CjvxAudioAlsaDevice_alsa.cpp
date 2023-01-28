#include "CjvxAudioAlsaDevice.h"
#include "jvx_config.h"
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

#define VERBOSE_DISPLAY_PROPERTIES
#define VERBOSE_DISPLAY_PROPERTIES_FORMAT

static pthread_t NOT_USED = 0;
#define PRE_ALLOCATION_SIZE (100*1024 * 1024)
/*
static void setscheduler(void)
 {
          struct sched_param sched_param;

          if (sched_getparam(0, &sched_param) < 0) {
                  printf("Scheduler getparam failed...\n");
                  return;
          }
          sched_param.sched_priority = sched_get_priority_max(SCHED_RR);
          if (!sched_setscheduler(0, SCHED_RR, &sched_param)) {
                  printf("Scheduler set to Round Robin with priority %i...\n", sched_param.sched_priority);
                  fflush(stdout);
                  return;
          }
          printf("!!!Scheduler set to Round Robin with priority %i FAILED!!!\n", sched_param.sched_priority);
}

static void setscheduler_thread(pthread_t th)
 {
          struct sched_param sched_param;

          if (sched_getparam(th, &sched_param) < 0) {
                  printf("Scheduler getparam failed...\n");
                  return;
          }
          sched_param.sched_priority = sched_get_priority_max(SCHED_RR);
          if (!sched_setscheduler(th, SCHED_RR, &sched_param)) {
                  printf("Scheduler set to Round Robin with priority %i...\n", sched_param.sched_priority);
                  fflush(stdout);
                  return;
          }
          printf("!!!Scheduler set to Round Robin with priority %i FAILED!!!\n", sched_param.sched_priority);
}
*/

static
int xrun_recovery(snd_pcm_t *handle, int err)
{
	if (err == -EPIPE)
	{
		/* under-run */
		err = snd_pcm_prepare(handle);
		if (err < 0)
		{
			std::cout << "ALSA: Can't recovery from underrun, prepare failed: " << snd_strerror(err) << std::endl;
			return 0;
		}
		else if (err == -ESTRPIPE)
		{
			while ((err = snd_pcm_resume(handle)) == -EAGAIN)
			{
				sleep(1); /* wait until the suspend flag is released */
			}
			if (err < 0)
			{
				err = snd_pcm_prepare(handle);
				if (err < 0)
				{
					std::cout << "ALSA: Can't recovery from suspend, prepare failed: " << snd_strerror(err) << std::endl;
				}
			}
			return 0;
		}
	}
	return err;
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

static jvxData phase = 0.0;
static void generateSine_i_i32_write_buf(char* singleBuffer, int numChannels, int buffersize, jvxData f0, int sRate)
{
	int i, j;
	jvxData phaseStart = 0;
	int incrementBytes = numChannels * sizeof(int);
	char* ptrBuffer = NULL;

	for (j = 0; j < numChannels; j++)
	{
		ptrBuffer = singleBuffer + sizeof(int) * j;
		phaseStart = phase;

		for (i = 0; i < buffersize; i++)
		{
			jvxData oneVal = cos(phaseStart) * 0.5;
			int value = JVX_DATA_2_INT32(oneVal);
			memcpy(ptrBuffer, &value, sizeof(int));
			ptrBuffer += incrementBytes;
			phaseStart += 2 * M_PI * f0 / sRate;
		}
	}
	phase = phaseStart;
	while (phase > 2 * M_PI)
	{
		phase -= 2 * M_PI;
	}
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

static void generateSine_i_i16(jvxData f0, jvxData sRate, const snd_pcm_channel_area_t* areaOut, int numChannels, int offset, int periodsize)
{
	int i, j;
	char* ptrOut = NULL;
	int incrementBytes = 0;
	jvxData phaseStart = 0;
	int offsetBytes = 0;

	for (j = 0; j < numChannels; j++)
	{
		phaseStart = phase;
		incrementBytes = areaOut[j].step / 8;
		ptrOut = (char*) areaOut[j].addr + areaOut[j].first / 8;
		offsetBytes = (offset * incrementBytes);
		ptrOut += offsetBytes;

		for (i = 0; i < periodsize; i++)
		{
			jvxData oneVal = cos(phaseStart) * 0.5;
			int value = JVX_DATA_2_INT16(oneVal);
			memcpy(ptrOut, &value, sizeof(short));
			ptrOut += incrementBytes;
			phaseStart += 2 * M_PI * f0 / sRate;
		}
	}
	phase = phaseStart;
	while (phase > 2 * M_PI)
	{
		phase -= 2 * M_PI;
	}
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

JVX_THREAD_ENTRY_FUNCTION(read_write_loop_pspon, arg)
{
	CjvxAudioAlsaDevice* this_pointer = (CjvxAudioAlsaDevice*) arg;
	if(this_pointer)
	{
		this_pointer->read_write_loop_inclass_pspon();
	}
	JVX_RETURN_THREAD(0);
}

JVX_THREAD_ENTRY_FUNCTION(read_write_loop_pspoff, arg)
{
	CjvxAudioAlsaDevice* this_pointer = (CjvxAudioAlsaDevice*) arg;
	if(this_pointer)
	{
		this_pointer->read_write_loop_inclass_pspoff();
	}
	JVX_RETURN_THREAD(0);
}

JVX_THREAD_ENTRY_FUNCTION(mmap_read_write_loop, arg)
{
	CjvxAudioAlsaDevice* this_pointer = (CjvxAudioAlsaDevice*) arg;
	if(this_pointer)
	{
		this_pointer->mmap_read_write_loop_inclass();
	}
	JVX_RETURN_THREAD(0);
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

// Callback for async PCM handler..
void hdlPcmCallback_rw(snd_async_handler_t* hdl)
{
	CjvxAudioAlsaDevice* this_pointer = (CjvxAudioAlsaDevice*) snd_async_handler_get_callback_private(hdl);
	if (this_pointer)
	{
		this_pointer->read_write_callback();
	}
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

// Callback for async PCM handler..
void hdlPcmCallback_mmap(snd_async_handler_t* hdl)
{
	CjvxAudioAlsaDevice* this_pointer = (CjvxAudioAlsaDevice*) (snd_async_handler_get_callback_private(hdl));
	if (this_pointer)
	{
		this_pointer->mmap_callback();
	}
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

jvxErrorType
CjvxAudioAlsaDevice::read_write_loop_inclass_pspon()
{
	int err;
	snd_pcm_sframes_t avail = 0, space = 0, samples = 0;
	snd_pcm_state_t state;
	jvxInt64 tStamp;
	int cnt_stt = 0;
	int i, j;
	std::cout << "ALSA: Primary Implementation" << std::endl;

	while (runtime.inAction.continueLoop)
	{
#ifdef WRITE_LOGFILE
		if(shareWithUi.logfileAvailable)
		{
			if(shareWithUi.writelog)
			{
				// Measurement point RP A
				space = -1;
				samples = -1;
				if(runtime.inAction.output.pcmHandle)
				{
					space = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
				}
				if(runtime.inAction.input.pcmHandle)
				{
					samples = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
				}
				jvxInt32* ptr = (jvxInt32*)this->runtime.logfile.RPA.fld;
				ptr[0] = space;
				ptr[1] = samples;
				ptr[2] = 1;
				runtime.logfile.wrapper.addFieldToLogFile(this->runtime.logfile.RPA.idx, NULL, true);
			}
		}
#endif

		// Write audio samples currently available

		if (runtime.inAction.output.pcmHandle)
		{
			if (runtime.inAction.readWriteInterleaved)
			{
				err = snd_pcm_writei(runtime.inAction.output.pcmHandle, runtime.ram.fld_outputi, runtime.inAction.buffersize);
			}
			else
			{
				err = snd_pcm_writen(runtime.inAction.output.pcmHandle, runtime.ram.fld_outputn, runtime.inAction.buffersize);
			}
		}
		else
		{
			err = 0;
		}

#ifdef WRITE_LOGFILE
		// Log timing
		if(shareWithUi.logfileAvailable)
		{
			if(shareWithUi.writelog)
			{
				// Measurement point RP B
				space = -1;
				samples = -1;
				if(runtime.inAction.output.pcmHandle)
				{
					space = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
				}
				if(runtime.inAction.input.pcmHandle)
				{
					samples = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
				}
				jvxInt32* ptr = (jvxInt32*)this->runtime.logfile.RPB.fld;
				ptr[0] = space;
				ptr[1] = samples;
				ptr[2] = 2;
				runtime.logfile.wrapper.addFieldToLogFile(this->runtime.logfile.RPB.idx, NULL,true);
			}
		}
#endif

		if (err < 0)
		{
			// err is inly lower 0 if pcmHandle is non-zero!
			// Mesurement point RP ERR1
			/*
			 space = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
			 samples = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
			 */

			// In case writing caused a problem, do recovery
			state = snd_pcm_state(runtime.inAction.output.pcmHandle);
			if (state == SND_PCM_STATE_XRUN)
			{
				if (this->_common_set_min.theHostRef)
				{
					this->_report_text_message("ALSA: Output underrun", JVX_REPORT_PRIORITY_WARNING);
				}

				//std::cout << "Underrun detected, output!" << std::endl;
				err = xrun_recovery(runtime.inAction.output.pcmHandle, -EPIPE);
				if (err < 0)
				{
					std::cout << "ALSA: Unexpected Error #101: " << snd_strerror(err) << std::endl;
					exit(0);
				}

				this->fillBuffersBeforeStart_readwrite();

				// Start pcm if not done so automatically
				err = snd_pcm_start(runtime.inAction.output.pcmHandle);
				if (err < 0)
				{
					std::cout << "ALSA: Unexpected Error #103: " << snd_strerror(err) << std::endl;
					exit(0);
				}
			}

			// Mesurement point RP ERR2
			/*
			 space = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
			 samples = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
			 */
		}
		else
		{

			// Write was all fine
			if (runtime.inAction.input.pcmHandle)
			{
				if (runtime.inAction.readWriteInterleaved)
				{
					err = snd_pcm_readi(runtime.inAction.input.pcmHandle, runtime.ram.fld_inputi, runtime.inAction.buffersize);
				}
				else
				{
					err = snd_pcm_readn(runtime.inAction.input.pcmHandle, runtime.ram.fld_inputn, runtime.inAction.buffersize);
				}
			}
			else
			{
				err = 0;
			}

#ifdef WRITE_LOGFILE
			if(shareWithUi.logfileAvailable)
			{
				if(shareWithUi.writelog)
				{
					// Measurement point RP C
					space = -1;
					samples = -1;
					if(runtime.inAction.output.pcmHandle)
					{
						space = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
					}
					if(runtime.inAction.input.pcmHandle)
					{
						samples = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
					}
					jvxInt32* ptr = (jvxInt32*)this->runtime.logfile.RPC.fld;
					ptr[0] = space;
					ptr[1] = samples;
					ptr[2] = 3;
					runtime.logfile.wrapper.addFieldToLogFile(this->runtime.logfile.RPC.idx, NULL,true);
				}
			}
#endif

			if (err >= 0)
			{
				jvxTick tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.inAction.refTimer);

				if (_common_set_ldslave.theData_out.con_link.connect_to)
				{
				    _common_set_ldslave.theData_out.con_link.connect_to->process_start_icon();
				}
				
				jvxSize idxToProc = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;
				jvxSize idxFromProc = *_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr;

				// Samples from HW buffers to SW buffers
				copyBuffers_hwToSw_interleaved(runtime.theArea_rw_input,
							       _common_set_ldslave.theData_out.con_data.buffers[idxToProc],
							       0, runtime.inAction.buffersize);

				/*============ PASS SAMPLES TO PROCESSING UNIT ==================*/
				if (_common_set_ldslave.theData_out.con_link.connect_to)
				{
				    _common_set_ldslave.theData_out.con_link.connect_to->process_buffers_icon();
				}
				
				// Samples from SW buffers to HW buffers
				copyBuffers_swToHw_interleaved(runtime.theArea_rw_output, _common_set_ldslave.theData_in->con_data.buffers[idxFromProc],
						0, runtime.inAction.buffersize);

				if (_common_set_ldslave.theData_out.con_link.connect_to)
				{
				    _common_set_ldslave.theData_out.con_link.connect_to->process_stop_icon();
				}

#ifdef WRITE_LOGFILE
				if(shareWithUi.logfileAvailable)
				{
					if(shareWithUi.writelog)
					{
						// Measurement point RP D
						space = -1;
						samples = -1;
						if(runtime.inAction.output.pcmHandle)
						{
							space = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
						}
						if(runtime.inAction.input.pcmHandle)
						{
							samples = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
						}
						jvxInt32* ptr = (jvxInt32*)this->runtime.logfile.RPD.fld;
						ptr[0] = space;
						ptr[1] = samples;
						ptr[2] = 4;
						runtime.logfile.wrapper.addFieldToLogFile(this->runtime.logfile.RPD.idx, NULL, true);
					}
				}
#endif
				// Convert the output
			}
			else
			{
				// Mesurement point RP ERR3
				/*
				 space = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
				 samples = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
				 */

				// If err < 0, pcm handle exists
				// In case writing caused a problem, do recovery
				state = snd_pcm_state(runtime.inAction.input.pcmHandle);
				if (state == SND_PCM_STATE_XRUN)
				{
					if (this->_common_set_min.theHostRef)
					{
						this->_report_text_message("ALSA: Input overrun", JVX_REPORT_PRIORITY_WARNING);
					}					//std::cout << "Overrun detected, input!" << std::endl;

					err = xrun_recovery(runtime.inAction.input.pcmHandle, -EPIPE);
					if (err < 0)
					{
						std::cout << "ALSA: Unexpected Error #101: " << snd_strerror(err) << std::endl;
						exit(0);
					}

					this->fillBuffersBeforeStart_readwrite();

					// Start pcm if not done so automatically
					err = snd_pcm_start(runtime.inAction.input.pcmHandle);
					if (err < 0)
					{
						std::cout << "ALSA: Unexpected Error #103: " << snd_strerror(err) << std::endl;
						exit(0);
					}
				}

				// Mesurement point RP ERR4
				/*
				 space = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
				 samples = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
				 */
			}
		}
	}
	return JVX_NO_ERROR;
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

jvxErrorType
CjvxAudioAlsaDevice::read_write_loop_inclass_pspoff()
{
	int err;
	snd_pcm_sframes_t avail = 0, space = 0, samples = 0;
	snd_pcm_state_t state;
	int cnt_stt = 0;
	int i, j;
	std::cout << "ALSA: Secondary Implementation" << std::endl;

	while (runtime.inAction.continueLoop)
	{
		if (runtime.inAction.input.pcmHandle)
		{
			if (runtime.inAction.readWriteInterleaved)
			{
				err = snd_pcm_readi(runtime.inAction.input.pcmHandle, runtime.ram.fld_inputi, runtime.inAction.buffersize);
			}
			else
			{
				err = snd_pcm_readn(runtime.inAction.input.pcmHandle, runtime.ram.fld_inputn, runtime.inAction.buffersize);
			}
		}
		else
		{
			err = 0;
		}

#ifdef WRITE_LOGFILE
		if(shareWithUi.logfileAvailable)
		{
			if(shareWithUi.writelog)
			{
				// Measurement point RP A
				space = -1;
				samples = -1;
				if(runtime.inAction.output.pcmHandle)
				{
					space = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
				}
				if(runtime.inAction.input.pcmHandle)
				{
					samples = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
				}
				jvxInt32* ptr = (jvxInt32*)this->runtime.logfile.RPA.fld;
				ptr[0] = space;
				ptr[1] = samples;
				ptr[2] = 3;
				runtime.logfile.wrapper.addFieldToLogFile(this->runtime.logfile.RPA.idx, NULL,true);
			}
		}
#endif

		if (err >= 0)
		{
			jvxTick tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.inAction.refTimer);

			if (_common_set_ldslave.theData_out.con_link.connect_to)
			{
			    _common_set_ldslave.theData_out.con_link.connect_to->process_start_icon();
			}

			jvxSize idxToProc = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;
			jvxSize idxFromProc = *_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr;

			// Samples from HW buffers to SW buffers
			copyBuffers_hwToSw_interleaved(runtime.theArea_rw_input, _common_set_ldslave.theData_out.con_data.buffers[idxToProc], 0, runtime.inAction.buffersize);

			/*============ PASS SAMPLES TO PROCESSING UNIT ==================*/
			if (_common_set_ldslave.theData_out.con_link.connect_to)
			{
			    _common_set_ldslave.theData_out.con_link.connect_to->process_buffers_icon();
			}
			
			copyBuffers_swToHw_interleaved(runtime.theArea_rw_output, _common_set_ldslave.theData_in->con_data.buffers[idxFromProc], 0, runtime.inAction.buffersize);

			if (_common_set_ldslave.theData_out.con_link.connect_to)
			{
			    _common_set_ldslave.theData_out.con_link.connect_to->process_stop_icon();
			}
			
			if (runtime.inAction.output.pcmHandle)
			{
				// Write audio samples currently available
				if (runtime.inAction.readWriteInterleaved)
				{
					err = snd_pcm_writei(runtime.inAction.output.pcmHandle, runtime.ram.fld_outputi, runtime.inAction.buffersize);
				}
				else
				{
					err = snd_pcm_writen(runtime.inAction.output.pcmHandle, runtime.ram.fld_outputn, runtime.inAction.buffersize);
				}
			}
			else
			{
				err = 0;
			}

#ifdef WRITE_LOGFILE
			if(shareWithUi.logfileAvailable)
			{
				if(shareWithUi.writelog)
				{
					// Measurement point RP B
					space = -1;
					samples = -1;
					if(runtime.inAction.output.pcmHandle)
					{
						space = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
					}
					if(runtime.inAction.input.pcmHandle)
					{
						samples = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
					}
					jvxInt32* ptr = (jvxInt32*)this->runtime.logfile.RPB.fld;
					ptr[0] = space;
					ptr[1] = samples;
					ptr[2] = 3;
					runtime.logfile.wrapper.addFieldToLogFile(this->runtime.logfile.RPB.idx, NULL,true);
				}
			}
#endif

			if (err < 0)
			{
				// In case writing caused a problem, do recovery
				state = snd_pcm_state(runtime.inAction.output.pcmHandle);
				if (state == SND_PCM_STATE_XRUN)
				{
					if (this->_common_set_min.theHostRef)
					{
						this->_report_text_message("ALSA: Output underrun", JVX_REPORT_PRIORITY_WARNING);
					}

					err = xrun_recovery(runtime.inAction.output.pcmHandle, -EPIPE);
					if (err < 0)
					{
						std::cout << "ALSA: Unexpected Error #101: " << snd_strerror(err) << std::endl;
						exit(0);
					}

					this->fillBuffersBeforeStart_readwrite();

					// Start pcm if not done so automatically
					err = snd_pcm_start(runtime.inAction.output.pcmHandle);
					if (err < 0)
					{
						std::cout << "ALSA: Unexpected Error #103: " << snd_strerror(err) << std::endl;
						exit(0);
					}
				}
			}
		}
		else
		{
			state = snd_pcm_state(runtime.inAction.input.pcmHandle);
			if (state == SND_PCM_STATE_XRUN)
			{
				if (this->_common_set_min.theHostRef)
				{
					this->_report_text_message("ALSA: Input overrun", JVX_REPORT_PRIORITY_WARNING);
				}

				err = xrun_recovery(runtime.inAction.input.pcmHandle, -EPIPE);
				if (err < 0)
				{
					std::cout << "ALSA: Unexpected Error #101: " << snd_strerror(err) << std::endl;
					exit(0);
				}

				this->fillBuffersBeforeStart_readwrite();

				// Start pcm if not done so automatically
				err = snd_pcm_start(runtime.inAction.output.pcmHandle);
				if (err < 0)
				{
					std::cout << "ALSA: Unexpected Error #103: " << snd_strerror(err) << std::endl;
					exit(0);
				}
			}
		}
	}
	return JVX_NO_ERROR;
}

// Audio processing callbacks
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

jvxErrorType
CjvxAudioAlsaDevice::mmap_read_write_loop_inclass()
{
	int err, commitres;
	snd_pcm_state_t state;
	int cnt_stt = 0;
	int i, j;
	snd_pcm_sframes_t avail = 0, space = 0, samples = 0;
	snd_pcm_uframes_t frames = 0;
	const snd_pcm_channel_area_t *my_areas;
	snd_pcm_uframes_t offset;
//std::cout << "In thread - in loop" << std::endl;

	while (runtime.inAction.continueLoop)
	{
		// If we are here, enough samples are available
		avail = 0;
		while ((avail < runtime.inAction.buffersize) && (avail >= 0))
		{
			avail = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
			if(avail < runtime.inAction.buffersize)
			{
   				snd_pcm_wait(runtime.inAction.output.pcmHandle, 1000);
			}
		}

		if (avail > 0)
		{
			frames = runtime.inAction.buffersize;
			err = snd_pcm_mmap_begin(runtime.inAction.output.pcmHandle, &my_areas, &offset, &frames);

			jvxSize idxFromProc = *_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr;
	
			this->copyBuffers_swToHw_interleaved(my_areas, _common_set_ldslave.theData_in->con_data.buffers[idxFromProc], offset, frames);
			commitres = snd_pcm_mmap_commit(runtime.inAction.output.pcmHandle, offset, frames);

			if (_common_set_ldslave.theData_out.con_link.connect_to)
			{
			    _common_set_ldslave.theData_out.con_link.connect_to->process_stop_icon();
			}

			// Now input side
			avail = 0;
			while ((avail < runtime.inAction.buffersize) && (avail >= 0))
			{
				avail = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
				if(avail < runtime.inAction.buffersize)
				{
   					snd_pcm_wait(runtime.inAction.input.pcmHandle, 1000);
				}
			}
			if (avail >= 0)
			{
				frames = runtime.inAction.buffersize;
				err = snd_pcm_mmap_begin(runtime.inAction.input.pcmHandle, &my_areas, &offset, &frames);
				frames = runtime.inAction.buffersize;

				if (_common_set_ldslave.theData_out.con_link.connect_to)
				{
				    _common_set_ldslave.theData_out.con_link.connect_to->process_start_icon();
				}

				jvxSize idxToProc = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;
	
				this->copyBuffers_hwToSw_interleaved(my_areas, _common_set_ldslave.theData_out.con_data.buffers[idxToProc], offset, frames);
				commitres = snd_pcm_mmap_commit(runtime.inAction.input.pcmHandle, offset, frames);

				jvxTick tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.inAction.refTimer);

				/*============ PASS SAMPLES TO PROCESSING UNIT ==================*/
				if (_common_set_ldslave.theData_out.con_link.connect_to)
				{
				    _common_set_ldslave.theData_out.con_link.connect_to->process_buffers_icon();
				}	
			}				//if(avail >= 0)
			else
			{
				// In case writing caused a problem, do recovery
				state = snd_pcm_state(runtime.inAction.input.pcmHandle);
				if (state == SND_PCM_STATE_XRUN)
				{
					if (this->_common_set_min.theHostRef)
					{
						this->_report_text_message("ALSA: Input overrun", JVX_REPORT_PRIORITY_WARNING);
					}
					err = xrun_recovery(runtime.inAction.input.pcmHandle, -EPIPE);
					if (err < 0)
					{
						std::cout << "ALSA: Unexpected Error #101: " << snd_strerror(err) << std::endl;
						exit(0);
					}

					this->fillBuffersBeforeStart_mmap();

					// Start pcm if not done so automatically
					err = snd_pcm_start(runtime.inAction.input.pcmHandle);
					if (err < 0)
					{
						std::cout << "ALSA: Unexpected Error #103: " << snd_strerror(err) << std::endl;
						exit(0);
					}
				}
			}
		}				//if(avail >= 0)
		else
		{
			// In case writing caused a problem, do recovery
			state = snd_pcm_state(runtime.inAction.output.pcmHandle);
			if (state == SND_PCM_STATE_XRUN)
			{
				if (this->_common_set_min.theHostRef)
				{
					this->_report_text_message("ALSA: Output underrun", JVX_REPORT_PRIORITY_WARNING);
				}
				err = xrun_recovery(runtime.inAction.output.pcmHandle, -EPIPE);
				if (err < 0)
				{
					std::cout << "ALSA: Unexpected Error #101: " << snd_strerror(err) << std::endl;
					exit(0);
				}

				this->fillBuffersBeforeStart_mmap();

				// Start pcm if not done so automatically
				err = snd_pcm_start(runtime.inAction.output.pcmHandle);
				if (err < 0)
				{
					std::cout << "ALSA: Unexpected Error #103: " << snd_strerror(err) << std::endl;
					exit(0);
				}
			}
		}
	}				//while(runtime.inAction.continueLoop)
	return JVX_NO_ERROR;
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

void
CjvxAudioAlsaDevice::read_write_callback()
{
	int err;
	snd_pcm_sframes_t avail = 0, space = 0, samples = 0;
	;
	snd_pcm_state_t state;
	int cnt_stt = 0;
	int i, j;

#ifdef WRITE_LOGFILE
	if(shareWithUi.logfileAvailable)
	{
		if(shareWithUi.writelog)
		{
			// Measurement point RP A
			space = -1;
			samples = -1;
			if(runtime.inAction.output.pcmHandle)
			{
				space = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
			}
			if(runtime.inAction.input.pcmHandle)
			{
				samples = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
			}
			jvxInt32* ptr = (jvxInt32*)this->runtime.logfile.RPA.fld;
			ptr[0] = space;
			ptr[1] = samples;
			ptr[2] = 3;
			runtime.logfile.wrapper.addFieldToLogFile(this->runtime.logfile.RPA.idx, NULL,true);
		}
	}
#endif

	if (runtime.inAction.input.pcmHandle)
	{
		if (runtime.inAction.readWriteInterleaved)
		{
			err = snd_pcm_readi(runtime.inAction.input.pcmHandle, runtime.ram.fld_inputi, runtime.inAction.buffersize);
		}
		else
		{
			err = snd_pcm_readn(runtime.inAction.input.pcmHandle, runtime.ram.fld_inputn, runtime.inAction.buffersize);
		}
	}
	else
	{
		err = 0;
	}

#ifdef WRITE_LOGFILE
	if(shareWithUi.logfileAvailable)
	{
		if(shareWithUi.writelog)
		{
			// Measurement point RP B
			space = -1;
			samples = -1;
			if(runtime.inAction.output.pcmHandle)
			{
				space = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
			}
			if(runtime.inAction.input.pcmHandle)
			{
				samples = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
			}
			jvxInt32* ptr = (jvxInt32*)this->runtime.logfile.RPB.fld;
			ptr[0] = space;
			ptr[1] = samples;
			ptr[2] = 3;
			runtime.logfile.wrapper.addFieldToLogFile(this->runtime.logfile.RPB.idx, NULL,true);
		}
	}
#endif

	if (err >= 0)
	{
		jvxTick tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.inAction.refTimer);

		if (_common_set_ldslave.theData_out.con_link.connect_to)
		{
		    _common_set_ldslave.theData_out.con_link.connect_to->process_start_icon();
		}
		
		jvxSize idxToProc = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;
		jvxSize idxFromProc = *_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr;
		
		this->copyBuffers_hwToSw_interleaved(runtime.theArea_rw_input, _common_set_ldslave.theData_out.con_data.buffers[idxToProc], 0, runtime.inAction.buffersize);

		/*============ PASS SAMPLES TO PROCESSING UNIT ==================*/
		if (_common_set_ldslave.theData_out.con_link.connect_to)
		{
		    _common_set_ldslave.theData_out.con_link.connect_to->process_buffers_icon();
		}

		this->copyBuffers_swToHw_interleaved(runtime.theArea_rw_output, _common_set_ldslave.theData_in->con_data.buffers[idxFromProc], 0, runtime.inAction.buffersize);

		if (_common_set_ldslave.theData_out.con_link.connect_to)
		{
		    _common_set_ldslave.theData_out.con_link.connect_to->process_stop_icon();
		}		

#ifdef WRITE_LOGFILE
		if(shareWithUi.logfileAvailable)
		{
			if(shareWithUi.writelog)
			{
				// Measurement point RP C
				space = -1;
				samples = -1;
				if(runtime.inAction.output.pcmHandle)
				{
					space = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
				}
				if(runtime.inAction.input.pcmHandle)
				{
					samples = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
				}
				jvxInt32* ptr = (jvxInt32*)this->runtime.logfile.RPC.fld;
				ptr[0] = space;
				ptr[1] = samples;
				ptr[2] = 3;
				runtime.logfile.wrapper.addFieldToLogFile(this->runtime.logfile.RPC.idx, NULL,true);
			}
		}
#endif

		if (runtime.inAction.output.pcmHandle)
		{
			// Write audio samples currently available
			if (runtime.inAction.readWriteInterleaved)
			{
				err = snd_pcm_writei(runtime.inAction.output.pcmHandle, runtime.ram.fld_outputi, runtime.inAction.buffersize);
			}
			else
			{
				err = snd_pcm_writen(runtime.inAction.output.pcmHandle, runtime.ram.fld_outputn, runtime.inAction.buffersize);
			}
		}
		else
		{
			err = 0;
		}

#ifdef WRITE_LOGFILE
		if(shareWithUi.logfileAvailable)
		{
			if(shareWithUi.writelog)
			{
				// Measurement point RP D
				space = -1;
				samples = -1;
				if(runtime.inAction.output.pcmHandle)
				{
					space = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
				}
				if(runtime.inAction.input.pcmHandle)
				{
					samples = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
				}
				jvxInt32* ptr = (jvxInt32*)this->runtime.logfile.RPD.fld;
				ptr[0] = space;
				ptr[1] = samples;
				ptr[2] = 3;
				runtime.logfile.wrapper.addFieldToLogFile(this->runtime.logfile.RPD.idx, NULL,true);
			}
		}
#endif

		if (err < 0)
		{
			// In case writing caused a problem, do recovery
			state = snd_pcm_state(runtime.inAction.output.pcmHandle);
			if (state == SND_PCM_STATE_XRUN)
			{
				if (this->_common_set_min.theHostRef)
				{
					this->_report_text_message("ALSA: Output underrun", JVX_REPORT_PRIORITY_WARNING);
				}

				err = xrun_recovery(runtime.inAction.output.pcmHandle, -EPIPE);
				if (err < 0)
				{
					std::cout << "ALSA: Unexpected Error #101: " << snd_strerror(err) << std::endl;
					exit(0);
				}

				this->fillBuffersBeforeStart_readwrite();

				// Start pcm if not done so automatically
				err = snd_pcm_start(runtime.inAction.output.pcmHandle);
				if (err < 0)
				{
					std::cout << "ALSA: Unexpected Error #103: " << snd_strerror(err) << std::endl;
					exit(0);
				}
			}
		}
	}
	else
	{
		state = snd_pcm_state(runtime.inAction.input.pcmHandle);
		if (state == SND_PCM_STATE_XRUN)
		{
			if (this->_common_set_min.theHostRef)
			{
				this->_report_text_message("ALSA: Input overrun", JVX_REPORT_PRIORITY_WARNING);
			}

			err = xrun_recovery(runtime.inAction.input.pcmHandle, -EPIPE);
			if (err < 0)
			{
				std::cout << "ALSA: Unexpected Error #101: " << snd_strerror(err) << std::endl;
				exit(0);
			}

			this->fillBuffersBeforeStart_readwrite();

			// Start pcm if not done so automatically
			err = snd_pcm_start(runtime.inAction.output.pcmHandle);
			if (err < 0)
			{
				std::cout << "ALSA: Unexpected Error #103: " << snd_strerror(err) << std::endl;
				exit(0);
			}
		}
	}
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

void
CjvxAudioAlsaDevice::mmap_callback()
{
	int err, commitres;
	snd_pcm_state_t state;
	int cnt_stt = 0;
	int i, j;
	snd_pcm_uframes_t avail = 0;
	snd_pcm_uframes_t frames = 0;
	const snd_pcm_channel_area_t *my_areas;
	snd_pcm_uframes_t offset;
//std::cout << "In thread - in loop" << std::endl;

	bool readComplete = false;
	bool writeComplete = false;

	while (!(readComplete & writeComplete))
	{
		// If we are here, enough samples are available
		frames = runtime.inAction.buffersize;
		err = snd_pcm_mmap_begin(runtime.inAction.output.pcmHandle, &my_areas, &offset, &frames);

		jvxSize idxFromProc = *_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr;
		
		if (err == 0)
		{
			if (frames >= runtime.inAction.buffersize)
			{
				frames = runtime.inAction.buffersize;
			}
			else
			{
				frames = 0;
			}
			if (frames > 0)
			{
				//std::cout << "offset: " << offset << ", frames:" << frames << std::endl;
				readComplete = true;

				switch (_common_set_ldslave.theData_in->con_params.format)
				{
				case JVX_DATAFORMAT_DATA:

					switch (runtime.inAction.format)
					{
					case SND_PCM_FORMAT_S32:
						// Copy samples
						for (i = 0; i < _inproc.numOutputs; i++)
						{
							jvxInt32 idMap = inProcessing.map_output_chans[i];
							if(idMap >= 0)
							{
								jvxData* ptrSrc_dbl = (jvxData*) _common_set_ldslave.theData_in->con_data.buffers[idxFromProc][i];
								jvxInt32* ptrTarget_int32 = (jvxInt32*) my_areas[idMap].addr;
                                ptrTarget_int32 += my_areas[idMap].first / 32;
                                ptrTarget_int32 += offset * my_areas[idMap].step / 32;

								for (j = 0; j < runtime.inAction.buffersize; j++)
								{
									*ptrTarget_int32 = JVX_DATA_2_INT32(*ptrSrc_dbl);
									ptrSrc_dbl++;
									ptrTarget_int32 += my_areas[i].step / 32;
								}
							}
						}

						break;
					case SND_PCM_FORMAT_S16:
						// Copy samples
						for (i = 0; i < _inproc.numOutputs; i++)
						{
							jvxInt32 idMap = inProcessing.map_output_chans[i];
							if (idMap >= 0)
							{
							    jvxData* ptrSrc_dbl = (jvxData*) _common_set_ldslave.theData_in->con_data.buffers[idxFromProc][i];
								jvxInt16* ptrTarget_int16 = (jvxInt16*) my_areas[idMap].addr;
								ptrTarget_int16 += my_areas[i].first / 16;
								ptrTarget_int16 += offset * my_areas[i].step / 16;

								for (j = 0; j < runtime.inAction.buffersize; j++)
								{
									*ptrTarget_int16 = JVX_DATA_2_INT16(*ptrSrc_dbl);
									ptrSrc_dbl++;
									ptrTarget_int16 += my_areas[i].step / 16;
								}
							}
						}
						break;
					default:
						assert(0);
					}
					/*
					 jvxInt32* ptrTarget_int32 = (jvxInt32*)my_areas[0].addr;
					 ptrTarget_int32 += my_areas[0].first/32;
					 ptrTarget_int32 += offset * my_areas[0].step/32;
					 generateSine_i_i32_write_buf((char*)ptrTarget_int32, 2, 1024, 1000, 44100);
					 */
				}
			}
			commitres = snd_pcm_mmap_commit(runtime.inAction.output.pcmHandle, offset, frames);

			if (_common_set_ldslave.theData_out.con_link.connect_to)
			{
			    _common_set_ldslave.theData_out.con_link.connect_to->process_stop_icon();
			}
		}

		// Now input side
		frames = runtime.inAction.buffersize;
		err = snd_pcm_mmap_begin(runtime.inAction.input.pcmHandle, &my_areas, &offset, &frames);
		if (err == 0)
		{
			if (frames >= runtime.inAction.buffersize)
			{
				frames = runtime.inAction.buffersize;
			}
			else
			{
				frames = 0;
			}
			jvxTick tt = JVX_GET_TICKCOUNT_US_GET_CURRENT(&runtime.inAction.refTimer);
			if (frames > 0)
			{
				writeComplete = true;

				if (_common_set_ldslave.theData_out.con_link.connect_to)
				{
				    _common_set_ldslave.theData_out.con_link.connect_to->process_start_icon();
				}

				jvxSize idxToProc = *_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr;
	
				// Convert the input
				switch (_common_set_ldslave.theData_out.con_params.format)
				{
				case JVX_DATAFORMAT_DATA:

					switch (runtime.inAction.format)
					{
					case SND_PCM_FORMAT_S32:
						// Copy samples
						for (i = 0; i < _inproc.numInputs; i++)
						{
							jvxInt32 idMap = inProcessing.map_output_chans[i];
							if (idMap >= 0)
							{
								jvxData* ptrTarget_dbl = (jvxData*) _common_set_ldslave.theData_out.con_data.buffers[idxToProc][i];
								jvxInt32* ptrSrc_int32 = (jvxInt32*) my_areas[idMap].addr;
								ptrSrc_int32 += my_areas[i].first / 32;
								ptrSrc_int32 += offset * my_areas[i].step / 32;

								for (j = 0; j < runtime.inAction.buffersize; j++)
								{
									*ptrTarget_dbl++ = JVX_INT32_2_DATA(*ptrSrc_int32);
									ptrSrc_int32 += my_areas[i].step / 32;
								}
							}
						}
						break;
					case SND_PCM_FORMAT_S16:
						// Copy samples
						for (i = 0; i < _inproc.numInputs; i++)
						{
							jvxInt32 idMap = inProcessing.map_output_chans[i];
							if (idMap >= 0)
							{
								jvxData* ptrTarget_dbl = (jvxData*) _common_set_ldslave.theData_out.con_data.buffers[idxToProc][i];
								jvxInt16* ptrSrc_int16 = (jvxInt16*) my_areas[i].addr;
								ptrSrc_int16 += my_areas[i].first / 16;
								ptrSrc_int16 += offset * my_areas[i].step / 16;

								for (j = 0; j < runtime.inAction.buffersize; j++)
								{
									*ptrTarget_dbl++ = JVX_INT16_2_DATA(*ptrSrc_int16);
									ptrSrc_int16 += my_areas[i].step / 16;
								}
							}
						}
						break;
					default:
						assert(0);
					}
				}
			}
			commitres = snd_pcm_mmap_commit(runtime.inAction.input.pcmHandle, offset, frames);

			/*============ PASS SAMPLES TO PROCESSING UNIT ==================*/
			// I am the master. I will start the chain here!
			if (_common_set_ldslave.theData_out.con_link.connect_to)
			{
			    _common_set_ldslave.theData_out.con_link.connect_to->process_buffers_icon();
			}

			//runtime.ram.numOutputs, runtime.ram.numInputs,
			//runtime.inAction.buffersize, _formatProcessing, tt);
		}				//if(err == 0)
	} // while(!(readComplete & writeComplete))
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

void
CjvxAudioAlsaDevice::fillBuffersBeforeStart_readwrite()
{
	int resA = 0;
	int i;
	int limitWrite = 0;
	int stillToWrite = -1;
	int writeHere = 0;

	int writeMax = 2*runtime.inAction.buffersize;

	if (runtime.inAction.output.pcmHandle)
	{
		// Fill output FIFO for startup
		int numSpaceinFifo = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);

		if(writeMax > 0)
		{
			stillToWrite = writeMax;
		}

		while (numSpaceinFifo > 0)
		{
			writeHere = runtime.inAction.buffersize;
			if(stillToWrite >= 0)
			{
				writeHere = JVX_MIN(writeHere, stillToWrite);
			}
			writeHere = JVX_MIN(writeHere, numSpaceinFifo);

			if(writeHere > 0)
			{
				// If we are here, the output buffer has run empty!
				if (runtime.inAction.readWriteInterleaved)
				{
					memset(runtime.ram.fld_outputi, 0, (runtime.ram.szOneElement_hw * runtime.inAction.numOutputChansHw * writeHere));
					resA = snd_pcm_writei(runtime.inAction.output.pcmHandle, runtime.ram.fld_outputi, writeHere);
				}
				else
				{
					for (i = 0; i < runtime.inAction.numOutputChansHw; i++)
					{
						memset(runtime.ram.fld_outputn[i], 0, runtime.ram.szOneElement_hw * writeHere);
					}
					resA = snd_pcm_writen(runtime.inAction.output.pcmHandle, runtime.ram.fld_outputn, writeHere);
				}
				if (resA < 0)
				{
					std::cout << "ALSA: Unexpected Error #201a (output write error): " << snd_strerror(resA) << std::endl;
					exit(0);
				}
				if(stillToWrite >= 0)
				{
					stillToWrite -= writeHere;
				}

				numSpaceinFifo = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
			}
			else
			{
				break;
			}
		}
	}

	if (runtime.inAction.input.pcmHandle)
	{
		// Empty input buffer for startup (in case of restart)
		int numSamplesInFifo = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
		while (numSamplesInFifo > 0)
		{
			if (runtime.inAction.readWriteInterleaved)
			{
				resA = snd_pcm_readi(runtime.inAction.input.pcmHandle, runtime.ram.fld_inputi, runtime.inAction.buffersize);
			}
			else
			{
				resA = snd_pcm_readn(runtime.inAction.input.pcmHandle, runtime.ram.fld_inputn, runtime.inAction.buffersize);
			}
			if (resA < 0)
			{
				std::cout << "ALSA: Unexpected Error #201b (input read error): " << snd_strerror(resA) << std::endl;
				exit(0);
			}
			numSamplesInFifo = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
		}
	}
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

void
CjvxAudioAlsaDevice::fillBuffersBeforeStart_mmap()
{
	int resA = 0;
	int i;
	int limitWrite = 0;
	snd_pcm_uframes_t avail = 0;
	snd_pcm_uframes_t frames = 0;
	const snd_pcm_channel_area_t *my_areas;
	snd_pcm_uframes_t offset;
	int j;
	int stillToWrite = -1;
	int writeHere = 0;

	int writeMax = 2*runtime.inAction.buffersize;

	if (runtime.inAction.output.pcmHandle)
	{
		if(writeMax > 0)
		{
			stillToWrite = writeMax;
		}

		// Fill output FIFO for startup
		int numSpaceinFifo = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
		while (numSpaceinFifo > 0)
		{
			writeHere = runtime.inAction.buffersize;
			if(stillToWrite >= 0)
			{
				writeHere = JVX_MIN(writeHere, stillToWrite);
			}
			writeHere = JVX_MIN(writeHere, numSpaceinFifo);

			if(writeHere > 0)
			{
				//frames = runtime.inAction.buffersize;
				frames = writeHere;

				resA = snd_pcm_mmap_begin(runtime.inAction.output.pcmHandle, &my_areas, &offset, &frames);
				if (resA == 0)
				{
					this->copyBuffers_swToHw_interleaved(my_areas, _common_set_ldslave.theData_in->con_data.buffers[0], offset, frames);
				}
				resA = snd_pcm_mmap_commit(runtime.inAction.output.pcmHandle, offset, frames);
				if(stillToWrite >= 0)
				{
					stillToWrite -= frames;
				}
				numSpaceinFifo = snd_pcm_avail_update(runtime.inAction.output.pcmHandle);
			}
			else
			{
				break;
			}
		}
	}

	if (runtime.inAction.input.pcmHandle)
	{
		// Empty input buffer for startup (in case of restart)
		int numSamplesInFifo = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
		while (numSamplesInFifo > 0)
		{
			frames = runtime.inAction.buffersize;
			resA = snd_pcm_mmap_begin(runtime.inAction.input.pcmHandle, &my_areas, &offset, &frames);
			if (resA == 0)
			{
				this->copyBuffers_hwToSw_interleaved(my_areas, _common_set_ldslave.theData_out.con_data.buffers[0], offset, frames);
			}
			resA = snd_pcm_mmap_commit(runtime.inAction.input.pcmHandle, offset, frames);
			numSamplesInFifo = snd_pcm_avail_update(runtime.inAction.input.pcmHandle);
		}
	}
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

void
CjvxAudioAlsaDevice::copyBuffers_hwToSw_interleaved(const snd_pcm_channel_area_t* hw, void** output, jvxInt32 offset,
                                                    jvxInt32 numSamples)
{
	int i, j;

	int step = runtime.ram.szOneElement_hw * 8;
	switch (_common_set_ldslave.theData_out.con_params.format)
	{
	case JVX_DATAFORMAT_DATA:

		switch (runtime.inAction.format)
		{
		case SND_PCM_FORMAT_S32:
			// Copy samples
			for (i = 0; i < _inproc.numInputs; i++)
			{
				jvxInt32 idMap = inProcessing.map_input_chans[i];
				if (idMap >= 0)
				{
					jvxData* ptrTarget_dbl = (jvxData*) output[i];
					jvxInt32* ptrSrc_int32 = (jvxInt32*) hw[idMap].addr;
					int theIncr = hw[idMap].step / step;

					// Offset in input buffer (not all channels need to be selected
					ptrSrc_int32 += hw[idMap].first / step; //runtime.ram.refsInput[i].idPhysicalBuffer;
					ptrSrc_int32 += offset * theIncr;

					for (j = 0; j < numSamples; j++)
					{
						*ptrTarget_dbl++ = JVX_INT32_2_DATA(*ptrSrc_int32);
						ptrSrc_int32 += theIncr;
					}
				}
			}
			break;
		case SND_PCM_FORMAT_S16:
			// Copy samples
			for (i = 0; i < _inproc.numInputs; i++)
			{
				jvxInt32 idMap = inProcessing.map_input_chans[i];
				if (idMap >= 0)
				{
					jvxData* ptrTarget_dbl = (jvxData*) output[i];
					jvxInt16* ptrSrc_int16 = (jvxInt16*) hw[idMap].addr;
					int theIncr = hw[idMap].step / step;

					ptrSrc_int16 += hw[idMap].first / step; //runtime.ram.refsInput[i].idPhysicalBuffer;
					ptrSrc_int16 += offset * theIncr;

					for (j = 0; j < numSamples; j++)
					{
						*ptrTarget_dbl++ = JVX_INT16_2_DATA(*ptrSrc_int16);
						ptrSrc_int16 += theIncr;
					}
				}
			}
			break;
		default:
			assert(0);
		}
		break;

	case JVX_DATAFORMAT_32BIT_LE:

		switch (runtime.inAction.format)
		{
		case SND_PCM_FORMAT_S32:
			// Copy samples
			for (i = 0; i < _inproc.numInputs; i++)
			{
				jvxInt32 idMap = inProcessing.map_input_chans[i];
				if (idMap >= 0)
				{
					jvxInt32* ptrTarget_i32 = (jvxInt32*) output[i];
					jvxInt32* ptrSrc_int32 = (jvxInt32*) hw[idMap].addr;
					int theIncr = hw[idMap].step / step;

					ptrSrc_int32 += hw[idMap].first / step; //runtime.ram.refsInput[i].idPhysicalBuffer;
					ptrSrc_int32 += offset * theIncr;

					for (j = 0; j < numSamples; j++)
					{
						*ptrTarget_i32++ = *ptrSrc_int32;
						ptrSrc_int32 += theIncr;
					}
				}
			}

#ifdef JVX_ALSA_LOGTOFILE
             if(JVX_CHECK_SIZE_SELECTED(uId_input))
                        {
                            jvxInt32* ptrTarget_int32 = (jvxInt32*) hw[0].addr;
                            ptrTarget_int32 += hw[0].first / 32;
                            ptrTarget_int32 += offset * hw[0].step / 32;
                            theLogger.process_logging_lock(uId_input,
                                                           (jvxHandle**)&ptrTarget_int32,
                                                           1, JVX_DATAFORMAT_32BIT_LE,
                                                           runtime.inAction.buffersize * runtime.inAction.numInputChansHw,
                                                           NULL);
                        }
#endif
			break;
		case SND_PCM_FORMAT_S16:
			// Copy samples
			for (i = 0; i < _inproc.numInputs; i++)
			{
				jvxInt32 idMap = inProcessing.map_input_chans[i];
				if (idMap >= 0)
				{
					jvxInt32* ptrTarget_i32 = (jvxInt32*) output[i];
					jvxInt16* ptrSrc_int16 = (jvxInt16*) hw[idMap].addr;
					int theIncr = hw[idMap].step / step;

					ptrSrc_int16 += hw[idMap].first / step; //runtime.ram.refsInput[i].idPhysicalBuffer;
					ptrSrc_int16 += offset * theIncr;

					for (j = 0; j < numSamples; j++)
					{
						*ptrTarget_i32++ = *ptrSrc_int16 << 16;
						ptrSrc_int16 += theIncr;
					}
				}
			}
			break;
		default:
			assert(0);
		}
		break;

	case JVX_DATAFORMAT_16BIT_LE:

		switch (runtime.inAction.format)
		{
		case SND_PCM_FORMAT_S32:
			// Copy samples
			for (i = 0; i < _inproc.numInputs; i++)
			{
				jvxInt32 idMap = inProcessing.map_input_chans[i];
				if (idMap >= 0)
				{
					jvxInt16* ptrTarget_i16 = (jvxInt16*) output[i];
					jvxInt32* ptrSrc_int32 = (jvxInt32*) hw[idMap].addr;
					int theIncr = hw[idMap].step / step;

					ptrSrc_int32 += hw[idMap].first / step; //runtime.ram.refsInput[i].idPhysicalBuffer;
					ptrSrc_int32 += offset * theIncr;

					for (j = 0; j < numSamples; j++)
					{
						*ptrTarget_i16++ = (*ptrSrc_int32 & 0xFFFF0000) >> 16;
						ptrSrc_int32 += theIncr;
					}
				}
			}
			break;
		case SND_PCM_FORMAT_S16:
			// Copy samples
			for (i = 0; i < _inproc.numInputs; i++)
			{
				jvxInt32 idMap = inProcessing.map_input_chans[i];
				if (idMap >= 0)
				{
					jvxInt16* ptrTarget_i16 = (jvxInt16*) output[i];
					jvxInt16* ptrSrc_int16 = (jvxInt16*) hw[idMap].addr;
					int theIncr = hw[idMap].step / step;

					ptrSrc_int16 += hw[idMap].first / step; //runtime.ram.refsInput[i].idPhysicalBuffer;
					ptrSrc_int16 += offset * theIncr;

					for (j = 0; j < numSamples; j++)
					{
						*ptrTarget_i16++ = *ptrSrc_int16;
						ptrSrc_int16 += theIncr;
					}
				}
			}
			break;
		default:
			assert(0);
		}
		break;
	}
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================


void
CjvxAudioAlsaDevice::copyBuffers_swToHw_interleaved(const snd_pcm_channel_area_t* hw, void** input, jvxInt32 offset, jvxInt32 numSamples)
{
	int i, j;
	int step = runtime.ram.szOneElement_hw * 8;
	switch (_common_set_ldslave.theData_out.con_params.format)
	{
	case JVX_DATAFORMAT_DATA:

		switch (runtime.inAction.format)
		{
		case SND_PCM_FORMAT_S32:
			// Copy samples
			for (i = 0; i < _inproc.numOutputs; i++)
			{
				jvxInt32 idMap = inProcessing.map_output_chans[i];
				if (idMap >= 0)
				{

					jvxData* ptrSrc_dbl = (jvxData*) input[i];
					jvxInt32* ptrTarget_int32 = (jvxInt32*) hw[idMap].addr;
					int theIncr = hw[idMap].step / step;

					ptrTarget_int32 += hw[idMap].first / step; //runtime.ram.refsInput[i].idPhysicalBuffer;
					ptrTarget_int32 += offset * theIncr;

					for (j = 0; j < numSamples; j++)
					{
						*ptrTarget_int32 = JVX_DATA_2_INT32(*ptrSrc_dbl);
						ptrSrc_dbl++;
						ptrTarget_int32 += theIncr;
					}
				}
			}
			break;
		case SND_PCM_FORMAT_S16:
			// Copy samples
			for (i = 0; i < _inproc.numOutputs; i++)
			{
				jvxInt32 idMap = inProcessing.map_output_chans[i];
				if (idMap >= 0)
				{
					jvxData* ptrSrc_dbl = (jvxData*) input[i];
					jvxInt16* ptrTarget_int16 = (jvxInt16*) hw[idMap].addr;
					int theIncr = hw[idMap].step / step ;

					ptrTarget_int16 += hw[idMap].first / step; //runtime.ram.refsInput[i].idPhysicalBuffer;
					ptrTarget_int16 += offset * theIncr;

					for (j = 0; j < numSamples; j++)
					{
						*ptrTarget_int16 = JVX_DATA_2_INT16(*ptrSrc_dbl);
						ptrSrc_dbl++;
						ptrTarget_int16 += theIncr;
					}
				}
			}
			break;
		default:
			assert(0);
		}
		break;

	case JVX_DATAFORMAT_32BIT_LE:

		switch (runtime.inAction.format)
		{
		case SND_PCM_FORMAT_S32:
			// Copy samples
			for (i = 0; i < _inproc.numOutputs; i++)
			{
				jvxInt32 idMap = inProcessing.map_output_chans[i];
				if (idMap >= 0)
				{
					//float oneVal = 0;
					jvxInt32* ptrSrc_i32 = (jvxInt32*) input[i];
					jvxInt32* ptrTarget_int32 = (jvxInt32*) hw[idMap].addr;
					int theIncr = hw[idMap].step / step;

					ptrTarget_int32 += hw[idMap].first / step; //runtime.ram.refsInput[i].idPhysicalBuffer;
					ptrTarget_int32 += offset * theIncr;

					for (j = 0; j < numSamples; j++)
					{
						*ptrTarget_int32 = *ptrSrc_i32;
						ptrSrc_i32++;
						ptrTarget_int32 += theIncr;
					}
				}
			}
			break;
		case SND_PCM_FORMAT_S16:
			// Copy samples
			for (i = 0; i < _inproc.numOutputs; i++)
			{
				jvxInt32 idMap = inProcessing.map_output_chans[i];
				if (idMap >= 0)
				{
					jvxInt32* ptrSrc_i32 = (jvxInt32*) input[i];
					jvxInt16* ptrTarget_int16 = (jvxInt16*) hw[idMap].addr;
					int theIncr = hw[idMap].step / step;

					ptrTarget_int16 += hw[idMap].first / step; //runtime.ram.refsInput[i].idPhysicalBuffer;
					ptrTarget_int16 += offset * theIncr;

					for (j = 0; j < numSamples; j++)
					{
						*ptrTarget_int16 = (*ptrSrc_i32 & 0xFFFF0000) >> 16;
						ptrSrc_i32++;
						ptrTarget_int16 += theIncr;
					}
				}
			}
			break;
		default:
			assert(0);
		}
		break;

	case JVX_DATAFORMAT_16BIT_LE:

		switch (runtime.inAction.format)
		{
		case SND_PCM_FORMAT_S32:
			// Copy samples
			for (i = 0; i < _inproc.numOutputs; i++)
			{
				jvxInt32 idMap = inProcessing.map_output_chans[i];
				if (idMap >= 0)
				{
					//float oneVal = 0;
					jvxInt16* ptrSrc_i16 = (jvxInt16*) input[i];
					jvxInt32* ptrTarget_int32 = (jvxInt32*) hw[idMap].addr;
					int theIncr = hw[idMap].step / step;

					ptrTarget_int32 += hw[idMap].first / step; //runtime.ram.refsInput[i].idPhysicalBuffer;
					ptrTarget_int32 += offset * theIncr;

					for (j = 0; j < numSamples; j++)
					{
						*ptrTarget_int32 = ((jvxInt32) * ptrSrc_i16) << 16;
						ptrSrc_i16++;
						ptrTarget_int32 += theIncr;
					}
				}
			}
			break;
		case SND_PCM_FORMAT_S16:
			// Copy samples
			for (i = 0; i < _inproc.numOutputs; i++)
			{
				jvxInt32 idMap = inProcessing.map_output_chans[i];
				if (idMap >= 0)
				{
					jvxInt16* ptrSrc_i16 = (jvxInt16*) input[i];
					jvxInt16* ptrTarget_int16 = (jvxInt16*) hw[idMap].addr;
					int theIncr = hw[idMap].step / step;

					ptrTarget_int16 += hw[idMap].first / step; //runtime.ram.refsInput[i].idPhysicalBuffer;
					ptrTarget_int16 += offset * theIncr;

					for (j = 0; j < numSamples; j++)
					{
						*ptrTarget_int16 = *ptrSrc_i16;
						ptrSrc_i16++;
						ptrTarget_int16 += theIncr;
					}
				}
			}
			break;
		default:
			assert(0);
		}
		break;
	}
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

jvxErrorType
CjvxAudioAlsaDevice::activateAlsaProperties()
{
	jvxErrorType res = JVX_NO_ERROR;
	int resA = 0;
	int i;
	jvxSize j,k;
	bool veto = false;

	shareWithUi.asyncCallbacks = false;
	shareWithUi.psp = true;
	//shareWithUi.writelog = false;
	//	shareWithUi.logfilename = "alsaTimingLog.rtpdata";
	//shareWithUi.logfileAvailable = false;

	if(thisDevice.isPlugConnection)
	{
		this->openHandles(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle,
				runtime.inAction.input.params_hw, runtime.inAction.output.params_hw,
				true, thisDevice.systemNameDevice); //, params_sw_input, params_sw_output);

		if(runtime.inAction.input.pcmHandle)
		{
			resA = snd_pcm_hw_params_get_channels_min(runtime.inAction.input.params_hw, &detectedProps.inovwrtnumChannelMin);
			resA = snd_pcm_hw_params_get_channels_max(runtime.inAction.input.params_hw, &detectedProps.inovwrtnumChannelMax);
		}
		if(runtime.inAction.output.pcmHandle)
		{
			resA = snd_pcm_hw_params_get_channels_min(runtime.inAction.output.params_hw, &detectedProps.outovwrtnumChannelMin);
			resA = snd_pcm_hw_params_get_channels_max(runtime.inAction.output.params_hw, &detectedProps.outovwrtnumChannelMax);
		}
		this->closeHandles(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle,
				runtime.inAction.input.params_hw, runtime.inAction.output.params_hw);
		this->openHandles(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle,
				runtime.inAction.input.params_hw,
				runtime.inAction.output.params_hw,
				true, "plug" + thisDevice.systemNameDevice); //, params_sw_input, params_sw_output);
	}
	else
	{
		this->openHandles(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle,
				runtime.inAction.input.params_hw, runtime.inAction.output.params_hw,
				true, thisDevice.systemNameDevice); //, params_sw_input, params_sw_output);
	}


	this->updateCurrentProperties(true, true, true);

	// Store initial settings for this device to return later
	detectedPropsOnInit = detectedProps;
	runtime.sampleratesOnInit = runtime.samplerates;
	this->runtime.sizesBuffersOnInit = this->runtime.sizesBuffers;

	//this->closeHandles(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.output.params_hw);		//, params_sw_input, params_sw_output);
	return (res);
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

jvxErrorType
CjvxAudioAlsaDevice::deactivateAlsaProperties()
{
	jvxErrorType res = JVX_NO_ERROR;
	int resA = 0;
	int i;
	bool veto = false;

	this->closeHandles(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle,
			runtime.inAction.input.params_hw, runtime.inAction.output.params_hw);

	this->resetAlsa();

	return (res);
}

jvxErrorType
CjvxAudioAlsaDevice::updateCurrentProperties(jvxBool initRun, jvxBool updateRates, jvxBool updateSizes)
{
	jvxErrorType res = JVX_NO_ERROR;
	int resA = 0;
	int i;
	jvxSize j,k;
	bool veto = false;

	_lock_properties_local();

	if(initRun)
	{
		CjvxAudioDevice::properties_active.samplerate.value = -1;
		CjvxAudioDevice::properties_active.buffersize.value = -1;
	}
	// ==================================================================
	// Display all properties
	// ==================================================================
	// ==========================================================================
	// Testing access types
	// ==========================================================================
	detectedProps.availableAccessTypes.clear();
	for (i = 0; i < numAccessTypes; i++)
	{
		veto = false;

#ifdef VERBOSE_DISPLAY_PROPERTIES_
		std::cout << "ALSA: Testing Access type " << accessTypesStrings[i] << std::endl;
#endif
		if (runtime.inAction.input.params_hw)
		{
			resA = snd_pcm_hw_params_test_access(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw, accessTypes[i]);
			if (resA == 0)
			{
#ifdef VERBOSE_DISPLAY_PROPERTIES_
				std::cout << "ALSA: --> supported by input device" << std::endl;
#endif
			}
			else
			{
				veto = true;
#ifdef VERBOSE_DISPLAY_PROPERTIES_
				std::cout << "ALSA: --> NOT supported by input device" << std::endl;
#endif
			}
		}

		if (runtime.inAction.output.params_hw)
		{
			resA = snd_pcm_hw_params_test_access(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw, accessTypes[i]);
			if (resA == 0)
			{
#ifdef VERBOSE_DISPLAY_PROPERTIES_
				std::cout << "ALSA: --> supported by output device" << std::endl;
#endif
			}
			else
			{
				veto = true;
#ifdef VERBOSE_DISPLAY_PROPERTIES_
				std::cout << "ALSA: --> NOT supported by output device" << std::endl;
#endif
			}
		}
		if (!veto)
		{
			detectedProps.availableAccessTypes.push_back(accessTypes[i]);
		}
	}		//for(i = 0; i < numAccessTypes; i++)

#ifdef VERBOSE_DISPLAY_PROPERTIES

	// DISPLAY ACCESS TYPES
	for (i = 0; i < detectedProps.availableAccessTypes.size(); i++)
	{
		std::cout << "ALSA: (" << accessTypesStrings[detectedProps.availableAccessTypes[i]] << ")" << std::endl;
	}
#endif

	// ==========================================================================
	// Test all possible data types
	// ==========================================================================
	detectedProps.availableFormatTypes.clear();

	for (i = 0; i < numFormatTypes; i++)
	{
		veto = false;
#ifdef VERBOSE_DISPLAY_PROPERTIES_FORMAT
		std::cout << "ALSA: Testing Format type " << formatTypesStrings[i] << std::endl;
#endif
		if (runtime.inAction.input.params_hw)
		{
			resA = snd_pcm_hw_params_test_format(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw, formatTypes[i]);
			if (resA == 0)
			{
#ifdef VERBOSE_DISPLAY_PROPERTIES_FORMAT
				std::cout << "ALSA: --> supported by input device" << std::endl;
#endif
			}
			else
			{
				veto = true;
#ifdef VERBOSE_DISPLAY_PROPERTIES_FORMAT
				std::cout << "ALSA: --> NOT supported by input device" << std::endl;
#endif
			}
		}

		if (runtime.inAction.output.params_hw)
		{
			resA = snd_pcm_hw_params_test_format(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw, formatTypes[i]);
			if (resA == 0)
			{
#ifdef VERBOSE_DISPLAY_PROPERTIES_FORMAT
				std::cout << "ALSA: --> supported by output device" << std::endl;
#endif
			}
			else
			{
				veto = true;
#ifdef VERBOSE_DISPLAY_PROPERTIES_FORMAT
				std::cout << "ALSA: --> NOT supported by output device" << std::endl;
#endif
			}
		}
		if (!veto)
		{
			detectedProps.availableFormatTypes.push_back(formatTypes[i]);
		}
	}

#ifdef VERBOSE_DISPLAY_PROPERTIES

	// DISPLAY AVAILABLE FORMAT TYPES
	for (i = 0; i < detectedProps.availableFormatTypes.size(); i++)
	{
		std::cout << "ALSA: (" << formatTypesStrings[detectedProps.availableFormatTypes[i]] << ")" << std::endl;
	}
#endif

	// ====================================================================
	// Scan for the number of input and output channels available
	// ====================================================================
	detectedProps.input.numChannelMin = 0;
	detectedProps.input.numChannelMax = 0;
	detectedProps.output.numChannelMin = 0;
	detectedProps.output.numChannelMax = 0;

	// ==========================================================================
	CjvxAudioDevice::properties_active.inputchannelselection.value.entries.clear();
	if(initRun)
	{
	  CjvxAudioDevice::properties_active.inputchannelselection.value.selection() = 0;
		CjvxAudioDevice::properties_active.inputchannelselection.value.exclusive = 0;
	}
	if (runtime.inAction.input.params_hw)
	{
		resA = snd_pcm_hw_params_get_channels_min(runtime.inAction.input.params_hw, &detectedProps.input.numChannelMin);
		resA = snd_pcm_hw_params_get_channels_max(runtime.inAction.input.params_hw, &detectedProps.input.numChannelMax);

		if(thisDevice.isPlugConnection)
		{
			detectedProps.input.numChannelMin = detectedProps.inovwrtnumChannelMin;
			detectedProps.input.numChannelMax = detectedProps.inovwrtnumChannelMax;
		}

#ifdef VERBOSE_DISPLAY_PROPERTIES
		std::cout << "ALSA: Number channels input: " << "(" << detectedProps.input.numChannelMin << "..." << detectedProps.input.numChannelMax << ")" << std::endl;
#endif

		for (i = 0; i < detectedProps.input.numChannelMax; i++)
		{
			CjvxAudioDevice::properties_active.inputchannelselection.value.entries.push_back(("In #" + jvx_int2String(i) + ":" + _common_set_min.theDescription).c_str());
			if(initRun)
			{
			  CjvxAudioDevice::properties_active.inputchannelselection.value.selection() |= ((jvxUInt64) 1 << i);
			}
		}
	}

	CjvxAudioDevice::properties_active.outputchannelselection.value.entries.clear();
	if(initRun)
	{
	  CjvxAudioDevice::properties_active.outputchannelselection.value.selection() = 0;
		CjvxAudioDevice::properties_active.outputchannelselection.value.exclusive = 0;
	}
	if (runtime.inAction.output.params_hw)
	{
		resA = snd_pcm_hw_params_get_channels_min(runtime.inAction.output.params_hw, &detectedProps.output.numChannelMin);
		resA = snd_pcm_hw_params_get_channels_max(runtime.inAction.output.params_hw, &detectedProps.output.numChannelMax);

		if(thisDevice.isPlugConnection)
		{
			detectedProps.output.numChannelMin = detectedProps.outovwrtnumChannelMin;
			detectedProps.output.numChannelMax = detectedProps.outovwrtnumChannelMax;
		}

#ifdef VERBOSE_DISPLAY_PROPERTIES
		std::cout << "ALSA: Number channels output: " << "(" << detectedProps.output.numChannelMin << "..." << detectedProps.output.numChannelMax << ")" << std::endl;
#endif

		for (i = 0; i < detectedProps.output.numChannelMax; i++)
		{
			CjvxAudioDevice::properties_active.outputchannelselection.value.entries.push_back(("Out #" + jvx_int2String(i) + ":" + _common_set_min.theDescription).c_str());
			if(initRun)
			{
			  CjvxAudioDevice::properties_active.outputchannelselection.value.selection() |= ((jvxUInt64) 1 << i);
			}
		}
	}

	if(!initRun)
	{
	  CjvxAudioDevice::properties_active.inputchannelselection.value.selection() &=
				((jvxBitField)1 << CjvxAudioDevice::properties_active.inputchannelselection.value.entries.size()) -1;

	  CjvxAudioDevice::properties_active.outputchannelselection.value.selection() &=
				((jvxBitField)1 << CjvxAudioDevice::properties_active.outputchannelselection.value.entries.size()) -1;
	}




	if(initRun || updateRates)
	{
		// ==========================================================================
		// Scan for samplerates available
		// ====================================================================
		detectedProps.sRate.valMin = 0;
		detectedProps.sRate.valMax = 0;
		detectedProps.sRate.dirMin = 0;
		detectedProps.sRate.dirMax = 0;

		if (runtime.inAction.input.params_hw)
		{
			resA = snd_pcm_hw_params_get_rate_min(runtime.inAction.input.params_hw, &detectedProps.sRate.valMin, &detectedProps.sRate.dirMin);
			if (resA != 0)
			{
				std::cout << "ALSA: Error obtaining samplerate input, error: " << snd_strerror(resA) << std::endl;
			}
			resA = snd_pcm_hw_params_get_rate_max(runtime.inAction.input.params_hw, &detectedProps.sRate.valMax, &detectedProps.sRate.dirMax);
			if (resA != 0)
			{
				std::cout << "ALSA: Error obtaining samplerate input, error: " << snd_strerror(resA) << std::endl;
			}

#ifdef VERBOSE_DISPLAY_PROPERTIES
			std::cout << "ALSA: Samplerate: " << "(" << detectedProps.sRate.valMin << "..." << detectedProps.sRate.valMax << ")" << std::endl;
			//std::cout << "ALSA: \t Dir: (" << detectedProps.sRate.dirMin <<
			//  "..." << detectedProps.sRate.dirMax << ")" << std::endl;
#endif
		}
		else if (runtime.inAction.output.params_hw)
		{
			resA = snd_pcm_hw_params_get_rate_min(runtime.inAction.output.params_hw, &detectedProps.sRate.valMin, &detectedProps.sRate.dirMin);
			if (resA != 0)
			{
				std::cout << "ALSA: Error obtaining samplerate output, error: " << snd_strerror(resA) << std::endl;
			}
			resA = snd_pcm_hw_params_get_rate_max(runtime.inAction.output.params_hw, &detectedProps.sRate.valMax, &detectedProps.sRate.dirMax);
			if (resA != 0)
			{
				std::cout << "ALSA: Error obtaining samplerate output, error: " << snd_strerror(resA) << std::endl;
			}

#ifdef VERBOSE_DISPLAY_PROPERTIES
			std::cout << "ALSA: Samplerate: " << "(" << detectedProps.sRate.valMin << "..." << detectedProps.sRate.valMax << ")" << std::endl;
			//std::cout << "ALSA: \t Dir: (" << detectedProps.sRate.dirMin <<
			//  "... " << detectedProps.sRate.dirMax << ")" << std::endl;
#endif
		}

		// =======================================================0
		// Actually test the sample rates
		// ====================================================================

		i= 0;
		while(1)
		{
			if(jvxUsefulSRates[i] >0)
			{
#ifdef VERBOSE_DISPLAY_PROPERTIES_
				std::cout << "ALSA: Testing samplerate " << jvxUsefulSRates[i] << std::endl;
#endif
				if (jvxUsefulSRates[i] >= detectedProps.sRate.valMin)
				{
					veto = false;
					if (runtime.inAction.input.params_hw)
					{
						resA = snd_pcm_hw_params_test_rate(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw, jvxUsefulSRates[i], 0);
						if (resA == 0)
						{
#ifdef VERBOSE_DISPLAY_PROPERTIES_
							std::cout << "ALSA: --> supported by input device" << std::endl;
#endif
						}
						else
						{
#ifdef VERBOSE_DISPLAY_PROPERTIES_
							std::cout << "ALSA: --> NOT supported by input device" << std::endl;
#endif
							veto = true;
						}
					}
					if (runtime.inAction.output.params_hw)
					{
						resA = snd_pcm_hw_params_test_rate(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw, jvxUsefulSRates[i], 0);
						if (resA == 0)
						{
#ifdef VERBOSE_DISPLAY_PROPERTIES_
							std::cout << "ALSA: --> supported by output device" << std::endl;
#endif
						}
						else
						{
#ifdef VERBOSE_DISPLAY_PROPERTIES_
							std::cout << "ALSA: --> NOT supported by output device" << std::endl;
#endif
							veto = true;
						}
					}
					if (!veto)
					{
						this->runtime.samplerates.push_back(jvxUsefulSRates[i]);
					}
				}
				i++;
			}
			else
			{
				break;
			}
		}


#ifdef VERBOSE_DISPLAY_PROPERTIES_
		for(i = 0; i < this->runtime.samplerates.size(); i++)
		{
			std::cout << "ALSA: (" << this->runtime.samplerates[i] << ")" << std::endl;
		}
#endif
	}
	else
	{
		int dirMin;
		int dirMax;
		unsigned int valMin;
		unsigned int valMax;
		if (runtime.inAction.input.params_hw)
		{
			resA = snd_pcm_hw_params_get_rate_min(runtime.inAction.input.params_hw, &valMin, &dirMin);
			if (resA != 0)
			{
				std::cout << "ALSA: Error obtaining samplerate input, error: " << snd_strerror(resA) << std::endl;
			}
			resA = snd_pcm_hw_params_get_rate_max(runtime.inAction.input.params_hw, &valMax, &dirMax);
			if (resA != 0)
			{
				std::cout << "ALSA: Error obtaining samplerate input, error: " << snd_strerror(resA) << std::endl;
			}

#ifdef VERBOSE_DISPLAY_PROPERTIES
			std::cout << "ALSA: Samplerate (constraint): " << "(" << valMin << "..." << valMax << ")" << std::endl;
			//std::cout << "ALSA: \t Dir: (" << detectedProps.sRate.dirMin <<
			//  "..." << detectedProps.sRate.dirMax << ")" << std::endl;
#endif
		}
		else if (runtime.inAction.output.params_hw)
		{
			resA = snd_pcm_hw_params_get_rate_min(runtime.inAction.output.params_hw, &valMin, &dirMin);
			if (resA != 0)
			{
				std::cout << "ALSA: Error obtaining samplerate output, error: " << snd_strerror(resA) << std::endl;
			}
			resA = snd_pcm_hw_params_get_rate_max(runtime.inAction.output.params_hw, &valMax, &dirMax);
			if (resA != 0)
			{
				std::cout << "ALSA: Error obtaining samplerate output, error: " << snd_strerror(resA) << std::endl;
			}

#ifdef VERBOSE_DISPLAY_PROPERTIES
			std::cout << "ALSA: Samplerate (constraint): " << "(" << valMin << "..." << valMax << ")" << std::endl;
			//std::cout << "ALSA: \t Dir: (" << detectedProps.sRate.dirMin <<
			//  "... " << detectedProps.sRate.dirMax << ")" << std::endl;
#endif
		}
		detectedProps.sRate = detectedPropsOnInit.sRate;
		this->runtime.samplerates = this->runtime.sampleratesOnInit;
	}

	genAlsa_device::properties_active.ratesselection.value.entries.clear();
	for(i = 0; i < this->runtime.samplerates.size(); i++)
	{
		genAlsa_device::properties_active.ratesselection.value.entries.push_back(jvx_int2String(runtime.samplerates[i]));
	}

	genAlsa_device::properties_active.ratesselection.value.selection() = 0;
	if(CjvxAudioDevice::properties_active.samplerate.value > 0)
	{
		jvxInt32 dist, distMin = INT32_MAX;
		jvxSize idxMin = JVX_SIZE_UNSELECTED;
		for(i = 0; i < this->runtime.samplerates.size(); i++)
		{
			dist = abs(CjvxAudioDevice::properties_active.samplerate.value - runtime.samplerates[i]);
			if(dist < distMin)
			{
				distMin = dist;
				idxMin = i;
			}
		}
		assert(JVX_CHECK_SIZE_SELECTED(idxMin));
		jvx_bitSet(genAlsa_device::properties_active.ratesselection.value.selection(), idxMin);
	}

	if(genAlsa_device::properties_active.ratesselection.value.selection() == 0)
	{
		assert(runtime.samplerates.size());
		CjvxAudioDevice::properties_active.samplerate.value = runtime.samplerates[0];
		genAlsa_device::properties_active.ratesselection.value.selection() = (1 << 0);
	}


	if(initRun || updateSizes)
	{
		runtime.sizesBuffers.clear();

		// ====================================================================
		// Scan for periods
		// ====================================================================
		detectedProps.input.period.valMin = 0;
		detectedProps.input.period.valMax = 0;

		if (runtime.inAction.input.params_hw)
		{
			resA = snd_pcm_hw_params_get_periods_min(runtime.inAction.input.params_hw, &detectedProps.input.period.valMin, &detectedProps.input.period.dirMin);
			resA = snd_pcm_hw_params_get_periods_max(runtime.inAction.input.params_hw, &detectedProps.input.period.valMax, &detectedProps.input.period.dirMax);

#ifdef VERBOSE_DISPLAY_PROPERTIES
			std::cout << "ALSA: Input Period: " << "(" << detectedProps.input.period.valMin << "..." << detectedProps.input.period.valMax << ")" << std::endl;
			//std::cout << "ALSA: \t Dir: " << detectedProps.period.dir << ", Dir min: " <<
			//  detectedProps.period.dirMin << ", Dir max: " << detectedProps.period.dirMax <<  std::endl;
#endif

		}

		detectedProps.output.period.valMin = 0;
		detectedProps.output.period.valMax = 0;
		if (runtime.inAction.output.params_hw)
		{
			resA = snd_pcm_hw_params_get_periods_min(runtime.inAction.output.params_hw, &detectedProps.output.period.valMin, &detectedProps.output.period.dirMin);
			resA = snd_pcm_hw_params_get_periods_max(runtime.inAction.output.params_hw, &detectedProps.output.period.valMax, &detectedProps.output.period.dirMax);

#ifdef VERBOSE_DISPLAY_PROPERTIES
			std::cout << "ALSA: Output Period: " << "(" << detectedProps.output.period.valMin << "..." << detectedProps.output.period.valMax << ")" << std::endl;
			//std::cout << "ALSA: \t Dir: " << "(" <<
			//  detectedProps.period.dirMin << "..." << detectedProps.period.dirMax << ")" << std::endl;
#endif

		}

		// ====================================================================
		// Check the available buffersizes
		// ====================================================================
		detectedProps.input.buffersize.valMin = 0;
		detectedProps.input.buffersize.valMax = 0;

		if (runtime.inAction.input.params_hw)
		{
			resA = snd_pcm_hw_params_get_buffer_size_min(runtime.inAction.input.params_hw, &detectedProps.input.buffersize.valMin);
			resA = snd_pcm_hw_params_get_buffer_size_max(runtime.inAction.input.params_hw, &detectedProps.input.buffersize.valMax);
#ifdef VERBOSE_DISPLAY_PROPERTIES
			std::cout << "ALSA: Input Buffersize: " << "(" << detectedProps.input.buffersize.valMin << "..." << detectedProps.input.buffersize.valMax << ")" << std::endl;
#endif
		}

		detectedProps.output.buffersize.valMin = 0;
		detectedProps.output.buffersize.valMax = 0;

		if (runtime.inAction.output.params_hw)
		{
			resA = snd_pcm_hw_params_get_buffer_size_min(runtime.inAction.output.params_hw, &detectedProps.output.buffersize.valMin);
			resA = snd_pcm_hw_params_get_buffer_size_max(runtime.inAction.output.params_hw, &detectedProps.output.buffersize.valMax);
#ifdef VERBOSE_DISPLAY_PROPERTIES
			std::cout << "ALSA: Output Buffersize: " << "(" << detectedProps.output.buffersize.valMin << "..." << detectedProps.output.buffersize.valMax << ")" << std::endl;
#endif
		}


		for (i = 0; i < runtime.usefulBSizes.size(); i++)
		{
			veto = false;

			if (runtime.inAction.input.params_hw)
			{
				jvxBool isUseful = false;

				for(k = detectedProps.input.period.valMin; k <= detectedProps.input.period.valMax; k++)
				{
					int bSizeCase = runtime.usefulBSizes[i] * k;
#ifdef VERBOSE_DISPLAY_PROPERTIES_
					std::cout << "ALSA: Testing input buffersize " << i << std::endl;
#endif
					resA = snd_pcm_hw_params_test_buffer_size(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw, bSizeCase);
					if (resA == 0)
					{
#ifdef VERBOSE_DISPLAY_PROPERTIES_
						std::cout << "ALSA: --> supported by input device." << std::endl;
#endif
						isUseful = true;
						break;
					}
					else
					{
#ifdef VERBOSE_DISPLAY_PROPERTIES_
						std::cout << "ALSA: --> NOT supported by input device." << std::endl;
#endif

					}
				}
				if(!isUseful)
				{
					veto = true;
				}
			}

			if (runtime.inAction.output.params_hw)
			{
				jvxBool isUseful = false;

				for(k = detectedProps.output.period.valMin; k <= detectedProps.output.period.valMax; k++)
				{

					int bSizeCase = runtime.usefulBSizes[i] * k;
#ifdef VERBOSE_DISPLAY_PROPERTIES_
					std::cout << "ALSA: Testing output buffersize " << i << std::endl;
#endif
					resA = snd_pcm_hw_params_test_buffer_size(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw, bSizeCase);
					if (resA == 0)
					{
#ifdef VERBOSE_DISPLAY_PROPERTIES_
						std::cout << "ALSA: --> supported by output device." << std::endl;
#endif
						isUseful = true;
						break;
					}
					else
					{
#ifdef VERBOSE_DISPLAY_PROPERTIES_
						std::cout << "ALSA: --> NOT supported by output device." << std::endl;
#endif

					}
				}
				if(!isUseful)
				{
					veto = true;
				}
			}

			if(!veto)
			{
				runtime.sizesBuffers.push_back(runtime.usefulBSizes[i]);
			}
		}
	}
	else
	{
		detectedProps.input.buffersize = detectedPropsOnInit.input.buffersize;
		detectedProps.input.period = detectedPropsOnInit.input.period;
		detectedProps.output.buffersize = detectedPropsOnInit.output.buffersize;
		detectedProps.output.period = detectedPropsOnInit.output.period;
		runtime.sizesBuffers = runtime.sizesBuffersOnInit;
	}

	genAlsa_device::properties_active.sizesselection.value.entries.clear();
	for(i = 0; i < runtime.sizesBuffers.size(); i++)
	{
		genAlsa_device::properties_active.sizesselection.value.entries.push_back(jvx_int2String(runtime.sizesBuffers[i]));
	}

#ifdef VERBOSE_DISPLAY_PROPERTIES_
	for(i = 0; i < runtime.sizesBuffers.size(); i++)
	{
		std::cout << "ALSA: (" << runtime.sizesBuffers.[i] << ")" << std::endl;
	}
#endif

	genAlsa_device::properties_active.sizesselection.value.selection() = 0;
	if(CjvxAudioDevice::properties_active.buffersize.value > 0)
	{
		jvxInt32 dist, distMin = INT32_MAX;
		jvxSize idxMin = JVX_SIZE_UNSELECTED;
		for(i = 0; i < runtime.sizesBuffers.size(); i++)
		{
			dist = abs(runtime.sizesBuffers[i] - CjvxAudioDevice::properties_active.buffersize.value);
			if(dist < distMin)
			{
				distMin = dist;
				idxMin = i;
			}
		}
		assert(JVX_CHECK_SIZE_SELECTED(idxMin));
		jvx_bitSet(genAlsa_device::properties_active.sizesselection.value.selection(), idxMin);

	}
	else
	{
		assert(runtime.sizesBuffers.size() > 0);
		genAlsa_device::properties_active.sizesselection.value.selection() = (jvxBitField) 1 << (runtime.sizesBuffers.size() - 1);
	}

	_unlock_properties_local();
	return(JVX_NO_ERROR);
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

jvxBool
CjvxAudioAlsaDevice::setSamplerate_alsa(jvxInt32& newrate)
{
	int resA;
	if (!(runtime.inAction.input.pcmHandle || runtime.inAction.output.pcmHandle))
	{
		//this->closeHandles(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.output.params_hw);
		// No, stop pcm handles only if deactivated device
		assert(0);
	}


	if(runtime.inAction.input.pcmHandle)
	{
		resA = snd_pcm_hw_params_any(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw);
		resA = snd_pcm_hw_params_set_rate(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw, newrate, 0);

		if (resA < 0)
		{
			std::cout << "ALSA: Error setting input sample rate to " << runtime.inAction.sRate << ": " <<
					snd_strerror(resA) << std::endl;
			return false;
		}
	}

	if(runtime.inAction.output.params_hw)
	{
		resA = snd_pcm_hw_params_any(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw);
		resA = snd_pcm_hw_params_set_rate(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw, newrate, 0);

		if (resA < 0)
		{
			std::cout << "ALSA: Error setting output sample rate to " << runtime.inAction.sRate << ": " <<
					snd_strerror(resA) << std::endl;
			return false;
		}
	}

	updateCurrentProperties(false, false, true);

	return(true);
}

jvxBool
CjvxAudioAlsaDevice::setBuffersize_alsa(jvxInt32& buffersize, jvxInt32& periodsInput, jvxInt32& periodsOutput)
{
	snd_pcm_uframes_t bsmax, bsmin;
	unsigned permax, permin;
	int resA;
	jvxSize i;

	if (!(runtime.inAction.input.pcmHandle || runtime.inAction.output.pcmHandle))
	{
		//this->closeHandles(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.output.params_hw);
		// No, stop pcm handles only if deactivated device
		assert(0);
	}

	if(runtime.inAction.input.params_hw)
	{
		resA = snd_pcm_hw_params_any(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw);
		resA = snd_pcm_hw_params_test_buffer_size(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw,
				periodsInput * buffersize);
		if(resA == 0)
		{
			resA = snd_pcm_hw_params_set_buffer_size(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw,
					periodsInput * buffersize);
			if (resA < 0)
			{
				snd_pcm_hw_params_get_buffer_size_max 	( 	runtime.inAction.input.params_hw, &bsmax);
				snd_pcm_hw_params_get_buffer_size_min 	( 	runtime.inAction.input.params_hw, &bsmin);
				std::cout << "ALSA: Error setting buffersize to " << periodsInput * buffersize << ": " << snd_strerror(resA) <<
						", allowed values must be between " << bsmin << " and " << bsmax << std::endl;
				return false;
			}

			resA = snd_pcm_hw_params_set_periods(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw,
					periodsInput, 0);
			if (resA < 0)
			{
				std::cout << "ALSA: Error setting input number periods to " << periodsInput << ": " << snd_strerror(resA) << std::endl;
				return false;
			}
		}
		else
		{
			std::cout << "Specified setting does not work, need to find better number of periods for input!" << std::endl;

			// Specified buffersize does not work, start to adapt number of periods
			snd_pcm_hw_params_get_buffer_size_max 	( 	runtime.inAction.input.params_hw, &bsmax);
			snd_pcm_hw_params_get_buffer_size_min 	( 	runtime.inAction.input.params_hw, &bsmin);
			snd_pcm_hw_params_get_periods_max 	( 	runtime.inAction.input.params_hw, &permax, NULL);
			snd_pcm_hw_params_get_periods_min 	( 	runtime.inAction.input.params_hw, &permin, NULL);

			for(i = permin; i <= permax; i++ )
			{
				runtime.inAction.periodsInput = i;
				resA = snd_pcm_hw_params_test_buffer_size(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw,
						periodsInput * buffersize);
				if(resA == 0)
				{
					std::cout << "Overriding number of periods for input to " << periodsInput << std::endl;

					resA = snd_pcm_hw_params_set_buffer_size(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw,
							periodsInput * buffersize);
					if (resA < 0)
					{
						snd_pcm_hw_params_get_buffer_size_max 	( 	runtime.inAction.input.params_hw, &bsmax);
						snd_pcm_hw_params_get_buffer_size_min 	( 	runtime.inAction.input.params_hw, &bsmin);
						std::cout << "ALSA: Error setting buffersize to " << periodsInput * buffersize << ": " << snd_strerror(resA) <<
								", allowed values must be between " << bsmin << " and " << bsmax << std::endl;
						return false;
					}

					resA = snd_pcm_hw_params_set_periods(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw,
							periodsInput, 0);
					if (resA < 0)
					{
						std::cout << "ALSA: Error setting input number periods to " << periodsInput << ": " <<
								snd_strerror(resA) << std::endl;
						return false;
					}
					break;
				}
			}
		}
	}

	if(runtime.inAction.output.params_hw)
	{
		resA = snd_pcm_hw_params_any(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw);
		resA = snd_pcm_hw_params_test_buffer_size(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw,
				periodsOutput * buffersize);

		if(resA == 0)
		{
			resA = snd_pcm_hw_params_set_buffer_size(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw,
					periodsOutput * buffersize);
			if (resA < 0)
			{
				snd_pcm_hw_params_get_buffer_size_max 	( 	runtime.inAction.input.params_hw, &bsmax);
				snd_pcm_hw_params_get_buffer_size_min 	( 	runtime.inAction.input.params_hw, &bsmin);
				std::cout << "ALSA: Error setting buffersize to " << periodsOutput * buffersize << ": " << snd_strerror(resA) <<
						", allowed values must be between " << bsmin << " and " << bsmax << std::endl;
				return false;
			}

			resA = snd_pcm_hw_params_set_periods(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw,
					periodsOutput, 0);
			if (resA < 0)
			{
				std::cout << "ALSA: Error setting output number periods to " << periodsOutput << ": " <<
						snd_strerror(resA) << std::endl;
				return(false);
			}
		}
		else
		{
			std::cout << "Specified setting does not work, need to find better number of periods for output!" << std::endl;

			// Specified buffersize does not work, start to adapt number of periods
			snd_pcm_hw_params_get_buffer_size_max 	( 	runtime.inAction.output.params_hw, &bsmax);
			snd_pcm_hw_params_get_buffer_size_min 	( 	runtime.inAction.output.params_hw, &bsmin);
			snd_pcm_hw_params_get_periods_max 	( 	runtime.inAction.output.params_hw, &permax, NULL);
			snd_pcm_hw_params_get_periods_min 	( 	runtime.inAction.output.params_hw, &permin, NULL);

			for(i = permin; i <= permax; i++ )
			{
				runtime.inAction.periodsOutput = i;
				resA = snd_pcm_hw_params_test_buffer_size(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw,
						periodsOutput * buffersize);
				if(resA == 0)
				{
					std::cout << "Overriding number of periods for output to " << periodsOutput << std::endl;

					resA = snd_pcm_hw_params_set_buffer_size(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw,
							periodsOutput * buffersize);
					if (resA < 0)
					{
						snd_pcm_hw_params_get_buffer_size_max 	( 	runtime.inAction.output.params_hw, &bsmax);
						snd_pcm_hw_params_get_buffer_size_min 	( 	runtime.inAction.output.params_hw, &bsmin);
						std::cout << "ALSA: Error setting buffersize to " << periodsOutput * buffersize << ": " <<
								snd_strerror(resA) <<
								", allowed values must be between " << bsmin << " and " << bsmax << std::endl;
						return false;
					}

					resA = snd_pcm_hw_params_set_periods(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw,
							periodsOutput, 0);
					if (resA < 0)
					{
						std::cout << "ALSA: Error setting output number periods to " << periodsOutput << ": " <<
								snd_strerror(resA) << std::endl;
						return false;
					}
					break;
				}
			}
		}
	}

	// ===================================================================================
	// Pass HW params to device instance
	// ===================================================================================
/*
	if (runtime.inAction.input.params_hw)
	{
		resA = snd_pcm_hw_params(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #5-a: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
	}


	if (runtime.inAction.output.params_hw)
	{
		resA = snd_pcm_hw_params(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #5-b: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
	}*/
	updateCurrentProperties(false, true, false);
	return(true);
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

void
CjvxAudioAlsaDevice::resetAlsa()
{
	detectedProps.availableFormatTypes.clear();
	detectedProps.availableAccessTypes.clear();
	detectedProps.input.period.valMin = 0;
	detectedProps.input.period.valMax = 0;
	detectedProps.output.period.valMin = 0;
	detectedProps.output.period.valMax = 0;
	detectedProps.sRate.valMin = 0;
	detectedProps.sRate.valMax = 0;

	runtime.inAction.accessMode = SND_PCM_ACCESS_RW_INTERLEAVED;
	runtime.inAction.format = SND_PCM_FORMAT_S32;
	runtime.inAction.sRate = 44100;
	runtime.inAction.periodsInput = 2;
	runtime.inAction.periodsOutput = 2;
	runtime.inAction.buffersize = 1024;
	runtime.inAction.continueLoop = false;
	runtime.inAction.readWriteInterleaved = false;
	runtime.inAction.asyncCallback = false;
	runtime.inAction.input.params_hw = NULL;
	runtime.inAction.input.params_sw = NULL;
	runtime.inAction.input.pcmHandle = NULL;
	runtime.inAction.output.params_hw = NULL;
	runtime.inAction.output.params_sw = NULL;
	runtime.inAction.output.pcmHandle = NULL;


	runtime.ram.fld_inputi = NULL;
	runtime.ram.fld_outputi = NULL;
	runtime.ram.fld_inputn = NULL;
	runtime.ram.fld_outputn = NULL;
	runtime.ram.hdlCallback = NULL;
	runtime.ram.szOneElement_hw = 0;

	runtime.theArea_rw_output = NULL;
	runtime.theArea_rw_input = NULL;
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

jvxErrorType
CjvxAudioAlsaDevice::prepareAlsa()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	int resA;
	jvxInt16 idFound = -1;

	// This, we need to do in a more flexible way later
	for(i = 0; i < genAlsa_device::properties_active.accessType.value.entries.size(); i++)
	{
	  if(jvx_bitTest(genAlsa_device::properties_active.accessType.value.selection(), i))
		{
			idFound = i;
			break;
		}
	}
	assert(idFound >= 0);
	runtime.inAction.accessMode = detectedProps.availableAccessTypes[idFound];



	// This, we need to do in a more flexible way later
	idFound = -1;
	for(i = 0; i < genAlsa_device::properties_active.formatType.value.entries.size(); i++)
	{
	  if(jvx_bitTest(genAlsa_device::properties_active.formatType.value.selection(), i))
		{
			idFound = i;
			break;
		}
	}
	assert(idFound >= 0);

	runtime.inAction.format = detectedProps.availableFormatTypes[idFound];
	runtime.inAction.sRate = _common_set_ldslave.theData_out.con_params.rate;
	runtime.inAction.periodsInput = genAlsa_device::properties_active.periodsInput.value.val();
	runtime.inAction.periodsOutput = genAlsa_device::properties_active.periodsOutput.value.val();
	runtime.inAction.buffersize = _common_set_ldslave.theData_out.con_params.buffersize;
	runtime.inAction.asyncCallback = shareWithUi.asyncCallbacks;

#ifdef WRITE_LOGFILE
	this->shareWithUi.logfileAvailable = false;
	runtime.logfile.theHandle = NULL;
	this->_hostRefObject->requestInterface((void**) &runtime.logfile.theHandle, JVX_INTERFACE_WRITELOGFILES);
	if (runtime.logfile.theHandle)
	{
		this->_hostRefObject->obtainUniqueSessionId(&str, NULL);
		if (str)
		{
			std::string fName = shareWithUi.logfilename + "." + str->bString;
			this->_hostRefObject->deallocate(str);

			if (runtime.logfile.wrapper.activate(runtime.logfile.theHandle, 100, false, 1000) == HrtpLogFileWriter::HJVX_LOGFW_ERROR_NO_ERROR)
			{
				runtime.logfile.wrapper.addLine("Buffer fill heights, RPA", 3, JVX_DATAFORMAT_32BIT_LE, runtime.logfile.RPA.idx);
				runtime.logfile.wrapper.addLine("Buffer fill heights, RPA", 3, JVX_DATAFORMAT_32BIT_LE, runtime.logfile.RPB.idx);
				runtime.logfile.wrapper.addLine("Buffer fill heights, RPA", 3, JVX_DATAFORMAT_32BIT_LE, runtime.logfile.RPC.idx);
				runtime.logfile.wrapper.addLine("Buffer fill heights, RPA", 3, JVX_DATAFORMAT_32BIT_LE, runtime.logfile.RPD.idx);

				runtime.logfile.RPA.fld = NULL;
				runtime.logfile.RPB.fld = NULL;
				runtime.logfile.RPC.fld = NULL;
				runtime.logfile.RPD.fld = NULL;

				std::vector<std::string> errMsg;
				if (runtime.logfile.wrapper.startProcessing(fName, errMsg) == HrtpLogFileWriter::HJVX_LOGFW_ERROR_NO_ERROR)
				{
					this->shareWithUi.logfileAvailable = true;
					runtime.logfile.wrapper.getFieldRefLogFile(runtime.logfile.RPA.idx, &runtime.logfile.RPA.fld);
					runtime.logfile.wrapper.getFieldRefLogFile(runtime.logfile.RPB.idx, &runtime.logfile.RPB.fld);
					runtime.logfile.wrapper.getFieldRefLogFile(runtime.logfile.RPC.idx, &runtime.logfile.RPC.fld);
					runtime.logfile.wrapper.getFieldRefLogFile(runtime.logfile.RPD.idx, &runtime.logfile.RPD.fld);
				}
			}
		}
	}
#endif

#ifdef VERBOSE_DISPLAY_PROPERTIES

	std::cout << "ALSA: Access Type selection: " << accessTypesStrings[runtime.inAction.accessMode] << std::endl;
	std::cout << "ALSA: Format Type selection: " << formatTypesStrings[runtime.inAction.format] << std::endl;
	std::cout << "ALSA: Samplerate selection: " << _common_set_ldslave.theData_out.con_params.rate << std::endl;
	std::cout << "ALSA: Buffersize selection: " << _common_set_ldslave.theData_out.con_params.buffersize << std::endl;
	std::cout << "ALSA: Period selection input: " << runtime.inAction.periodsInput << std::endl;
	std::cout << "ALSA: Period selection output: " << runtime.inAction.periodsOutput << std::endl;
	std::cout << "ALSA: Async Callback: " << runtime.inAction.asyncCallback << std::endl;

#endif

	// Open PCM handle object and HW as well as SW objects
	/*

	if(thisDevice.isPlugConnection)
	{
		this->openHandles(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.output.params_hw, true, "plug" + thisDevice.systemNameDevice);		// <- (!this->runtime.inAction.asyncCallback) -- if using the ASYNC option, async does no longer work.. BUG?
	}
	else
	{
		this->openHandles(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.output.params_hw, true, thisDevice.systemNameDevice);		// <- (!this->runtime.inAction.asyncCallback) -- if using the ASYNC option, async does no longer work.. BUG?
	}*/

	if (!(runtime.inAction.input.pcmHandle || runtime.inAction.output.pcmHandle))
	{
		//this->closeHandles(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.output.params_hw);
		// No, stop pcm handles only if deactivated device
		return (JVX_ERROR_INTERNAL);
	}

	// ===================================================================================
	// Prepare input device, HW params
	// ===================================================================================
	runtime.inAction.numInputChansHw = 0;

	jvxBool configurationError = false;
	snd_pcm_uframes_t bsmax, bsmin;
	unsigned permax, permin;

    //runtime.inAction.periodsInput = JVX_MAX(2,  runtime.inAction.periodsInput);

	if (runtime.inAction.input.params_hw)
    {
        // Reset input paramaters
        resA = snd_pcm_hw_params_any(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw);
        if (resA < 0)
        {
            std::cout << "ALSA: Error resetting hardware parameters output due to new start: " << snd_strerror(resA) << std::endl;
            // configurationError = true;
        }

        runtime.inAction.numInputChansHw = _inproc.minChanCntInput + 1;
        runtime.inAction.numInputChansHw  = JVX_MAX(detectedProps.input.numChannelMin, runtime.inAction.numInputChansHw );
        if(genAlsa_device::properties_active.activateAllChannelsInput.value == c_true)
        {
            runtime.inAction.numInputChansHw  = detectedProps.input.numChannelMax;
        }
 		resA = snd_pcm_hw_params_set_channels (runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw,
 				runtime.inAction.numInputChansHw );
		if (resA < 0)
		{
			std::cout << "ALSA: Error setting number of input channels to " << runtime.inAction.numInputChansHw << ": " << snd_strerror(resA) << std::endl;
			// configurationError = true;
		}

		resA = snd_pcm_hw_params_set_access(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.accessMode);
		if (resA < 0)
		{
			std::cout << "ALSA: Error setting input access type to " << runtime.inAction.accessMode << ": " << snd_strerror(resA) << std::endl;
			configurationError = true;
		}

		resA = snd_pcm_hw_params_set_format(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.format);
		if (resA < 0)
		{
			std::cout << "ALSA: Error setting input format to " << runtime.inAction.format << ": " << snd_strerror(resA) << std::endl;
			configurationError = true;
		}

		resA = snd_pcm_hw_params_set_rate(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.sRate, 0);
		if (resA < 0)
		{
			std::cout << "ALSA: Error setting input sample rate to " << runtime.inAction.sRate << ": " << snd_strerror(resA) << std::endl;
			configurationError = true;
		}

		resA = snd_pcm_hw_params_test_buffer_size(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.periodsInput * runtime.inAction.buffersize);
		if(resA == 0)
		{
			resA = snd_pcm_hw_params_set_buffer_size(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.periodsInput * runtime.inAction.buffersize);
			if (resA < 0)
			{
				snd_pcm_hw_params_get_buffer_size_max 	( 	runtime.inAction.input.params_hw, &bsmax);
				snd_pcm_hw_params_get_buffer_size_min 	( 	runtime.inAction.input.params_hw, &bsmin);
				std::cout << "ALSA: Error setting buffersize to " << runtime.inAction.periodsInput * runtime.inAction.buffersize << ": " << snd_strerror(resA) <<
						", allowed values must be between " << bsmin << " and " << bsmax << std::endl;
				configurationError = true;
			}

			resA = snd_pcm_hw_params_set_periods(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.periodsInput, 0);
			if (resA < 0)
			{
				std::cout << "ALSA: Error setting input number periods to " << runtime.inAction.periodsInput << ": " << snd_strerror(resA) << std::endl;
				configurationError = true;
			}
		}
		else
		{
			std::cout << "Specified setting does not work, need to find better number of periods for input!" << std::endl;

			// Specified buffersize does not work, start to adapt number of periods
			snd_pcm_hw_params_get_buffer_size_max 	( 	runtime.inAction.input.params_hw, &bsmax);
			snd_pcm_hw_params_get_buffer_size_min 	( 	runtime.inAction.input.params_hw, &bsmin);
			snd_pcm_hw_params_get_periods_max 	( 	runtime.inAction.input.params_hw, &permax, NULL);
			snd_pcm_hw_params_get_periods_min 	( 	runtime.inAction.input.params_hw, &permin, NULL);

			for(i = permin; i <= permax; i++ )
			{
				runtime.inAction.periodsInput = i;
				resA = snd_pcm_hw_params_test_buffer_size(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.periodsInput * runtime.inAction.buffersize);
				if(resA == 0)
				{
					std::cout << "Overriding number of periods for input to " << runtime.inAction.periodsInput << std::endl;

					resA = snd_pcm_hw_params_set_buffer_size(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.periodsInput * runtime.inAction.buffersize);
					if (resA < 0)
					{
						snd_pcm_hw_params_get_buffer_size_max 	( 	runtime.inAction.input.params_hw, &bsmax);
						snd_pcm_hw_params_get_buffer_size_min 	( 	runtime.inAction.input.params_hw, &bsmin);
						std::cout << "ALSA: Error setting buffersize to " << runtime.inAction.periodsInput * runtime.inAction.buffersize << ": " << snd_strerror(resA) <<
								", allowed values must be between " << bsmin << " and " << bsmax << std::endl;
						configurationError = true;
					}

					resA = snd_pcm_hw_params_set_periods(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.periodsInput, 0);
					if (resA < 0)
					{
						std::cout << "ALSA: Error setting input number periods to " << runtime.inAction.periodsInput << ": " << snd_strerror(resA) << std::endl;
						configurationError = true;
					}
					break;
				}
			}
		}
	}

	if(configurationError)
	{
		//this->closeHandles(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.output.params_hw);
		return (JVX_ERROR_INTERNAL);
	}

	// ===================================================================================
	// Prepare output device, HW params
	// ===================================================================================
	runtime.inAction.numOutputChansHw = 0;
	if (runtime.inAction.output.params_hw)
	{
        resA = snd_pcm_hw_params_any(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw);
        if (resA < 0)
        {
            std::cout << "ALSA: Error resetting hardware parameters output due to new start: " << snd_strerror(resA) << std::endl;
            // configurationError = true;
        }

		runtime.inAction.numOutputChansHw = _inproc.minChanCntOutput + 1;
        runtime.inAction.numOutputChansHw  = JVX_MAX(detectedProps.output.numChannelMin, runtime.inAction.numOutputChansHw );
        if(genAlsa_device::properties_active.activateAllChannelsOutput.value == c_true)
        {
            runtime.inAction.numOutputChansHw  = detectedProps.output.numChannelMax;
        }
		resA = snd_pcm_hw_params_set_channels (runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw,
				runtime.inAction.numOutputChansHw );
		if (resA < 0)
		{
			std::cout << "ALSA: Error setting number of output channels to " << runtime.inAction.numOutputChansHw << ": " << snd_strerror(resA) << std::endl;
			// configurationError = true;
		}

		resA = snd_pcm_hw_params_set_access(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw, runtime.inAction.accessMode);
		if (resA < 0)
		{
			std::cout << "ALSA: Error setting output access type to " << runtime.inAction.accessMode << ": " << snd_strerror(resA) << std::endl;
			configurationError = true;
		}
		resA = snd_pcm_hw_params_set_format(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw, runtime.inAction.format);
		if (resA < 0)
		{
			std::cout << "ALSA: Error setting output format to " << runtime.inAction.format << ": " << snd_strerror(resA) << std::endl;
			configurationError = true;
		}
		resA = snd_pcm_hw_params_set_rate(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw, runtime.inAction.sRate, 0);
		if (resA < 0)
		{
			std::cout << "ALSA: Error setting output sample rate to " << runtime.inAction.sRate << ": " << snd_strerror(resA) << std::endl;
			configurationError = true;
		}

		resA = snd_pcm_hw_params_test_buffer_size(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw, runtime.inAction.periodsOutput * runtime.inAction.buffersize);
		if(resA == 0)
		{
			resA = snd_pcm_hw_params_set_buffer_size(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw, runtime.inAction.periodsOutput * runtime.inAction.buffersize);
			if (resA < 0)
			{
				snd_pcm_hw_params_get_buffer_size_max 	( 	runtime.inAction.input.params_hw, &bsmax);
				snd_pcm_hw_params_get_buffer_size_min 	( 	runtime.inAction.input.params_hw, &bsmin);
				std::cout << "ALSA: Error setting buffersize to " << runtime.inAction.periodsOutput * runtime.inAction.buffersize << ": " << snd_strerror(resA) <<
						", allowed values must be between " << bsmin << " and " << bsmax << std::endl;
				configurationError = true;
			}

			resA = snd_pcm_hw_params_set_periods(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw, runtime.inAction.periodsOutput, 0);
			if (resA < 0)
			{
				std::cout << "ALSA: Error setting output number periods to " << runtime.inAction.periodsOutput << ": " << snd_strerror(resA) << std::endl;
				configurationError = true;
			}
		}
		else
		{
			std::cout << "Specified setting does not work, need to find better number of periods for output!" << std::endl;

			// Specified buffersize does not work, start to adapt number of periods
			snd_pcm_hw_params_get_buffer_size_max 	( 	runtime.inAction.output.params_hw, &bsmax);
			snd_pcm_hw_params_get_buffer_size_min 	( 	runtime.inAction.output.params_hw, &bsmin);
			snd_pcm_hw_params_get_periods_max 	( 	runtime.inAction.output.params_hw, &permax, NULL);
			snd_pcm_hw_params_get_periods_min 	( 	runtime.inAction.output.params_hw, &permin, NULL);

			for(i = permin; i <= permax; i++ )
			{
				runtime.inAction.periodsOutput = i;
				resA = snd_pcm_hw_params_test_buffer_size(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw, runtime.inAction.periodsOutput * runtime.inAction.buffersize);
				if(resA == 0)
				{
					std::cout << "Overriding number of periods for output to " << runtime.inAction.periodsOutput << std::endl;

					resA = snd_pcm_hw_params_set_buffer_size(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw, runtime.inAction.periodsOutput * runtime.inAction.buffersize);
					if (resA < 0)
					{
						snd_pcm_hw_params_get_buffer_size_max 	( 	runtime.inAction.output.params_hw, &bsmax);
						snd_pcm_hw_params_get_buffer_size_min 	( 	runtime.inAction.output.params_hw, &bsmin);
						std::cout << "ALSA: Error setting buffersize to " << runtime.inAction.periodsOutput * runtime.inAction.buffersize << ": " << snd_strerror(resA) <<
								", allowed values must be between " << bsmin << " and " << bsmax << std::endl;
						configurationError = true;
					}

					resA = snd_pcm_hw_params_set_periods(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw, runtime.inAction.periodsOutput, 0);
					if (resA < 0)
					{
						std::cout << "ALSA: Error setting output number periods to " << runtime.inAction.periodsOutput << ": " << snd_strerror(resA) << std::endl;
						configurationError = true;
					}
					break;
				}
			}
		}
	}

	if(configurationError)
	{
		//this->closeHandles(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.output.params_hw);
		return (JVX_ERROR_INTERNAL);
	}

	// ===================================================================================
	// Pass HW params to device instance
	// ===================================================================================

	if (runtime.inAction.input.params_hw)
	{
		resA = snd_pcm_hw_params(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_hw);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #5-a: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
	}

	// ===================================================================================
	// Pass HW params to device instance
	// ===================================================================================

	if (runtime.inAction.output.params_hw)
	{
		resA = snd_pcm_hw_params(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_hw);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #5-b: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
	}

#ifdef JVX_ALSA_LOGTOFILE

    jvxErrorType resL = JVX_NO_ERROR;
    uId_input = JVX_SIZE_UNSELECTED;
    uId_output = JVX_SIZE_UNSELECTED;

    oneEntry newEnt;
    if(
            (runtime.inAction.format == SND_PCM_FORMAT_S32)||(runtime.inAction.accessMode == SND_PCM_ACCESS_RW_INTERLEAVED))
    {
        newEnt.format = JVX_DATAFORMAT_32BIT_LE;
        if(runtime.inAction.numInputChansHw)
        {
            newEnt.bSize = runtime.inAction.buffersize * runtime.inAction.numInputChansHw;
            newEnt.numChannels = runtime.inAction.numInputChansHw;
            resL = theLogger.add_one_lane(1,
                                         newEnt.bSize,
                                         newEnt.format,
                                         "Store raw memory audio data, input", 0,
                                         newEnt.uId);

            uId_input = newEnt.uId;
            registered_logs[newEnt.uId] = newEnt;
        }

        // = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

        if(runtime.inAction.numOutputChansHw)
        {
            newEnt.bSize = runtime.inAction.buffersize * runtime.inAction.numOutputChansHw;
            newEnt.numChannels = runtime.inAction.numOutputChansHw;
            resL = theLogger.add_one_lane(1,
                                         newEnt.bSize,
                                         newEnt.format,
                                         "Store raw memory audio data, output", 0,
                                         newEnt.uId);

            uId_output = newEnt.uId;
            registered_logs[newEnt.uId] = newEnt;
        }

        theLogger.add_tag(jvx_makePathExpr(newEnt.description, "buffersize").c_str(), jvx_size2String(runtime.inAction.buffersize).c_str());
        theLogger.add_tag(jvx_makePathExpr(newEnt.description, "format").c_str(), jvxDataFormat_txt(JVX_DATAFORMAT_32BIT_LE));
        theLogger.add_tag(jvx_makePathExpr(newEnt.description, "channels_input").c_str(), jvx_int322String(runtime.inAction.numInputChansHw).c_str());
        theLogger.add_tag(jvx_makePathExpr(newEnt.description, "channels_output").c_str(), jvx_int322String(runtime.inAction.numOutputChansHw).c_str());

        theLogger.activate_logging(".", "alsa_log");
    }
#endif

	// ===================================================================================
	// Specify SW params input
	// ===================================================================================

	if (runtime.inAction.input.pcmHandle)
	{

		snd_pcm_uframes_t numFrames;
		int val;

		snd_pcm_sw_params_malloc(&runtime.inAction.input.params_sw);

		// Get the input sw parameters
		resA = snd_pcm_sw_params_current(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_sw);
		if (resA != 0)
		{
			std::cout << "ALSA: Error obtaining SW params input for PCM " << thisDevice.systemNameDevice << ", reason: " << snd_strerror(resA) << std::endl;
			exit(0);
		}

		resA = snd_pcm_sw_params_set_period_event(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_sw, 1);
		if (res < 0)
		{
			std::cout << "ALSA: Unexpected Error #16: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
		resA = snd_pcm_sw_params_get_period_event(runtime.inAction.input.params_sw, &val);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #17: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
		else
		{
			std::cout << "ALSA: Period event: " << val << std::endl;
		}

		// We start ALSA explicitely, therefore we set the threshold to be higher than the buffersize
		resA = snd_pcm_sw_params_set_start_threshold(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_sw, runtime.inAction.periodsOutput * runtime.inAction.buffersize + 1);
		//setup.buffersize * 2 + 1);

		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #18: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
		resA = snd_pcm_sw_params_get_start_threshold(runtime.inAction.input.params_sw, &numFrames);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #19: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
		else
		{
			std::cout << "ALSA: Start Threshold in frames: " << numFrames << std::endl;
		}

		resA = snd_pcm_sw_params_set_avail_min(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_sw, runtime.inAction.buffersize);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #20: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
		resA = snd_pcm_sw_params_get_avail_min(runtime.inAction.input.params_sw, &numFrames);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #21: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
		else
		{
			std::cout << "ALSA: Avail min in frames: " << numFrames << std::endl;
		}
		resA = snd_pcm_sw_params(runtime.inAction.input.pcmHandle, runtime.inAction.input.params_sw);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #22: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
	}

	// ===================================================================================
	// Specify SW params output
	// ===================================================================================

	if (runtime.inAction.output.pcmHandle)
	{

		snd_pcm_uframes_t numFrames;
		int val;

		snd_pcm_sw_params_malloc(&runtime.inAction.output.params_sw);

		// Get the input sw parameters
		resA = snd_pcm_sw_params_current(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_sw);
		if (resA != 0)
		{
			std::cout << "ALSA: Error obtaining SW params output for PCM " << thisDevice.systemNameDevice << ", reason: " << snd_strerror(resA) << std::endl;
			exit(0);
		}

		resA = snd_pcm_sw_params_set_period_event(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_sw, 1);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #24: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
		resA = snd_pcm_sw_params_get_period_event(runtime.inAction.output.params_sw, &val);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #25: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
		else
		{
			std::cout << "ALSA: Period event: " << val << std::endl;
		}

		// We start ALSA explicitely, therefore we set the threshold to be higher than the buffersize
		resA = snd_pcm_sw_params_set_start_threshold(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_sw, runtime.inAction.periodsOutput * runtime.inAction.buffersize + 1);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #26: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
		resA = snd_pcm_sw_params_get_start_threshold(runtime.inAction.output.params_sw, &numFrames);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #27: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
		else
		{
			std::cout << "ALSA: Start Threshold in frames: " << numFrames << std::endl;
		}

		resA = snd_pcm_sw_params_set_avail_min(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_sw, runtime.inAction.buffersize);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #28: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
		resA = snd_pcm_sw_params_get_avail_min(runtime.inAction.output.params_sw, &numFrames);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #29: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
		else
		{
			std::cout << "ALSA: Avail min in frames: " << numFrames << std::endl;
		}
		resA = snd_pcm_sw_params(runtime.inAction.output.pcmHandle, runtime.inAction.output.params_sw);
		if (resA < 0)
		{
			std::cout << "ALSA: Unexpected Error #30: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
	}

	JVX_GET_TICKCOUNT_US_SETREF(&runtime.inAction.refTimer);
	runtime.inAction.timer_old = 0;
	runtime.inAction.timer_minDt = 0xFFFFFF;
	runtime.inAction.timer_maxDt = 0;

	// =======================================================
	// ALLOCATE HW BUFFERS IF DESIRED AND START
	// =======================================================
	runtime.ram.fld_inputi = NULL;
	runtime.ram.fld_outputi = NULL;
	runtime.ram.fld_inputn = NULL;
	runtime.ram.fld_outputn = NULL;

	switch (runtime.inAction.format)
	{
	//	      case SND_PCM_FORMAT_S32_LE:
	case SND_PCM_FORMAT_S32:
		runtime.ram.szOneElement_hw = sizeof(jvxInt32);
		break;
	case SND_PCM_FORMAT_S16:
		runtime.ram.szOneElement_hw = sizeof(jvxInt16);
		break;
	default:
		std::cout << "ALSA: Error: Format " << formatTypesStrings[runtime.inAction.format] << " not supported." << std::endl;
		exit(0);
	}

	// ***************************************************************
	// Different access schemes handled in different ways..
	// ***************************************************************
	runtime.theArea_rw_input = NULL;
	runtime.theArea_rw_output = NULL;

	if (runtime.inAction.accessMode == SND_PCM_ACCESS_RW_INTERLEAVED)
	{
		runtime.inAction.readWriteInterleaved = true;

		// ***************************************************************
		// Allocate the "hardware" buffers
		// ***************************************************************

		if (runtime.inAction.numInputChansHw)
		{
			runtime.ram.fld_inputi = new char[runtime.ram.szOneElement_hw * runtime.inAction.numInputChansHw * runtime.inAction.buffersize];
			memset(runtime.ram.fld_inputi, 0, (runtime.ram.szOneElement_hw * runtime.inAction.numInputChansHw * runtime.inAction.buffersize));
			this->runtime.theArea_rw_input = new snd_pcm_channel_area_t[runtime.inAction.numInputChansHw];
			for (i = 0; i < runtime.inAction.numInputChansHw; i++)
			{
				this->runtime.theArea_rw_input[i].addr = runtime.ram.fld_inputi;
				this->runtime.theArea_rw_input[i].first = i * runtime.ram.szOneElement_hw * 8;
				this->runtime.theArea_rw_input[i].step = runtime.inAction.numInputChansHw * runtime.ram.szOneElement_hw * 8;
			}
		}
		else
		{
			runtime.ram.fld_inputi = NULL;
		}

		if (runtime.inAction.numOutputChansHw)
		{
			runtime.ram.fld_outputi = new char[runtime.ram.szOneElement_hw * runtime.inAction.numOutputChansHw * runtime.inAction.buffersize];
			memset(runtime.ram.fld_outputi, 0, (runtime.ram.szOneElement_hw * runtime.inAction.numOutputChansHw * runtime.inAction.buffersize));
			this->runtime.theArea_rw_output = new snd_pcm_channel_area_t[runtime.inAction.numOutputChansHw];
			for (i = 0; i < runtime.inAction.numOutputChansHw; i++)
			{
				this->runtime.theArea_rw_output[i].addr = runtime.ram.fld_outputi;
				this->runtime.theArea_rw_output[i].first = i * runtime.ram.szOneElement_hw * 8;
				this->runtime.theArea_rw_output[i].step = runtime.inAction.numOutputChansHw * runtime.ram.szOneElement_hw * 8;
			}
		}
		else
		{
			runtime.ram.fld_outputi = NULL;
		}
	}
	else if (runtime.inAction.accessMode == SND_PCM_ACCESS_RW_NONINTERLEAVED)
	{

		runtime.inAction.readWriteInterleaved = false;

		// Allocate the "hardware" buffers
		if (runtime.inAction.numInputChansHw > 0)
		{
			runtime.ram.fld_inputn = new void*[runtime.inAction.numInputChansHw];
			this->runtime.theArea_rw_input = new snd_pcm_channel_area_t[runtime.inAction.numInputChansHw];
		}
		else
		{
			runtime.ram.fld_inputn = NULL;
		}
		if (runtime.inAction.numOutputChansHw > 0)
		{
			runtime.ram.fld_outputn = new void*[runtime.inAction.numOutputChansHw];
			this->runtime.theArea_rw_output = new snd_pcm_channel_area_t[runtime.inAction.numOutputChansHw];
		}
		else
		{
			runtime.ram.fld_outputn = NULL;
		}

		for (i = 0; i < runtime.inAction.numInputChansHw; i++)
		{
			runtime.ram.fld_inputn[i] = new char[runtime.ram.szOneElement_hw * runtime.inAction.buffersize];
			memset(runtime.ram.fld_inputn[i], 0, runtime.ram.szOneElement_hw * runtime.inAction.buffersize);
			this->runtime.theArea_rw_input[i].addr = runtime.ram.fld_inputn[i];
			this->runtime.theArea_rw_input[i].first = 0;
			this->runtime.theArea_rw_input[i].step = runtime.ram.szOneElement_hw * 8;
		}
		for (i = 0; i < runtime.inAction.numOutputChansHw; i++)
		{
			runtime.ram.fld_outputn[i] = new char[runtime.ram.szOneElement_hw * runtime.inAction.buffersize];
			memset(runtime.ram.fld_outputn[i], 0, runtime.ram.szOneElement_hw * runtime.inAction.buffersize);
			this->runtime.theArea_rw_output[i].addr = runtime.ram.fld_outputn[i];
			this->runtime.theArea_rw_output[i].first = 0;
			this->runtime.theArea_rw_output[i].step = runtime.ram.szOneElement_hw * 8;
		}

	}

	inProcessing.map_input_chans.clear();
    for (i = 0; i < CjvxAudioDevice::properties_active.inputchannelselection.value.entries.size(); i++)
    {
      if (jvx_bitTest(CjvxAudioDevice::properties_active.inputchannelselection.value.selection(), i))
        {
        	inProcessing.map_input_chans.push_back(i);
        }
    }
    assert(inProcessing.map_input_chans.size() == _inproc.numInputs);

	inProcessing.map_output_chans.clear();
    for (i = 0; i < CjvxAudioDevice::properties_active.outputchannelselection.value.entries.size(); i++)
    {
      if (jvx_bitTest(CjvxAudioDevice::properties_active.outputchannelselection.value.selection(), i))
        {
        	inProcessing.map_output_chans.push_back(i);
        }
    }
    assert(inProcessing.map_output_chans.size() == _inproc.numOutputs);

    return (res);
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

jvxErrorType
CjvxAudioAlsaDevice::startAlsa()
{
	jvxErrorType res = JVX_NO_ERROR;
	int resA = 0;
	bool linkSuccess = false;

#ifdef WRITE_LOGFILE
	IrtpWriteLogfiles** retHdl = NULL;
	jvxInt64 writtenToMod = 0;
	jvxInt64 writtenToFile = 0;
	std::string err;
#endif

	JVX_PREPARE_RT_START(PRE_ALLOCATION_SIZE);

#ifdef WRITE_LOGFILE
	if(this->shareWithUi.logfileAvailable)
	{
		runtime.logfile.wrapper.startProcessing();
	}
#endif
	if(
		(runtime.inAction.accessMode == SND_PCM_ACCESS_RW_INTERLEAVED) ||
		(runtime.inAction.accessMode == SND_PCM_ACCESS_RW_NONINTERLEAVED))
	{
		// ***************************************************************
		// Fill output FIFO with samples (if output is there)
		// ***************************************************************
		if(runtime.inAction.output.pcmHandle)
		{
			// If duplex, link input with output
			if(runtime.inAction.input.pcmHandle)
			{
				// Link input and output
				resA = snd_pcm_link(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle);
				if(resA < 0)
				{
					std::cout << "Warning: snd_pcm_link failed, reason: " << snd_strerror(resA) << std::endl;
				}
				else
				{
					linkSuccess = true;
				}
			}

			// Flush in two audio frames: one for first outplay and followed. This determines the "real" delay
			this->fillBuffersBeforeStart_readwrite();

			// If in async callback mode, setup the async callback handler
			if(runtime.inAction.asyncCallback)
			{
				snd_async_add_pcm_handler(&runtime.ram.hdlCallback, runtime.inAction.output.pcmHandle,
						hdlPcmCallback_rw, this);
			}

			// Start pcm if not done so automatically
			resA = snd_pcm_start(runtime.inAction.output.pcmHandle);
			if(resA < 0)
			{
				std::cout << "ALSA: Unexpected Error #202a: " << snd_strerror(resA) << std::endl;
				exit(0);
			}
			if(!linkSuccess)
			{
				if(runtime.inAction.input.pcmHandle)
				resA = snd_pcm_start(runtime.inAction.input.pcmHandle);
				if(resA < 0)
				{
					std::cout << "ALSA: Unexpected Error #202a: " << snd_strerror(resA) << std::endl;
					exit(0);
				}
			}
		}
		else
		{
			// only input available

			// If callback async, install handler
			if(runtime.inAction.asyncCallback)
			{
				snd_async_add_pcm_handler(&runtime.ram.hdlCallback, runtime.inAction.input.pcmHandle,
						hdlPcmCallback_rw, this);
			}

			// Start it
			resA = snd_pcm_start(runtime.inAction.input.pcmHandle);
			if(resA < 0)
			{
				std::cout << "ALSA: Unexpected Error #203a: " << snd_strerror(resA) << std::endl;
				exit(0);
			}
		}

		// If we do not run in async mode,
		if(runtime.inAction.asyncCallback)
		{
			runtime.inAction.threadHdl = -1;
		}
		else
		{
			runtime.inAction.continueLoop = true;
			if(shareWithUi.psp)
			{
				JVX_CREATE_THREAD(runtime.inAction.threadHdl, read_write_loop_pspon, this, NOT_USED);
			}
			else
			{
				JVX_CREATE_THREAD(runtime.inAction.threadHdl, read_write_loop_pspoff, this, NOT_USED);
			}
			JVX_SET_THREAD_PRIORITY(runtime.inAction.threadHdl, JVX_THREAD_PRIORITY_REALTIME);
		}
	}
	else
	{
		// ***************************************************************
		// Fill output FIFO with samples (if output is there)
		// ***************************************************************
		if(runtime.inAction.output.pcmHandle)
		{
			// If duplex, link input with output
			if(runtime.inAction.input.pcmHandle)
			{
				// Link input and output
				resA = snd_pcm_link(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle);
				if(resA < 0)
				{
					std::cout << "ALSA: Unexpected Error #200a: " << snd_strerror(resA) << std::endl;
					exit(0);
				}
			}

			this->fillBuffersBeforeStart_mmap();

			// If in async callback mode, setup the async callback handler
			if(runtime.inAction.asyncCallback)
			{
				snd_async_add_pcm_handler(&runtime.ram.hdlCallback, runtime.inAction.output.pcmHandle,
						hdlPcmCallback_rw, this);
			}

			// Start pcm if not done so automatically
			resA = snd_pcm_start(runtime.inAction.output.pcmHandle);
			if(resA < 0)
			{
				std::cout << "ALSA: Unexpected Error #202a: " << snd_strerror(resA) << std::endl;
				exit(0);
			}
		}
		else
		{
			// only input available

			// If callback async, install handler
			if(runtime.inAction.asyncCallback)
			{
				snd_async_add_pcm_handler(&runtime.ram.hdlCallback, runtime.inAction.input.pcmHandle,
						hdlPcmCallback_rw, this);
			}

			// Start it
			resA = snd_pcm_start(runtime.inAction.input.pcmHandle);
			if(resA < 0)
			{
				std::cout << "ALSA: Unexpected Error #203a: " << snd_strerror(resA) << std::endl;
				exit(0);
			}
		}

		// If we do not run in async mode,
		if(runtime.inAction.asyncCallback)
		{
			runtime.inAction.threadHdl = -1;
		}
		else
		{
			runtime.inAction.continueLoop = true;
			JVX_CREATE_THREAD(runtime.inAction.threadHdl, mmap_read_write_loop, this, NOT_USED);

			JVX_SET_THREAD_PRIORITY(runtime.inAction.threadHdl, JVX_THREAD_PRIORITY_REALTIME);
		}
	}

	return (res);
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================


jvxErrorType
CjvxAudioAlsaDevice::stopAlsa()
{
	jvxErrorType res = JVX_NO_ERROR;

	int resA = 0;
	int i;
	bool veto = false;

#ifdef WRITE_LOGFILE
	IrtpWriteLogfiles** retHdl = NULL;
	jvxInt64 writtenToMod = 0;
	jvxInt64 writtenToFile = 0;
	std::string err;
#endif

		// Wait for thread to complete
	runtime.inAction.continueLoop = false;
	if (runtime.inAction.threadHdl != -1)
	{
		JVX_WAIT_FOR_THREAD_TERMINATE_INF(runtime.inAction.threadHdl);
	}



	if(runtime.inAction.output.pcmHandle)
	{
		snd_pcm_drop(runtime.inAction.output.pcmHandle);
		if(runtime.inAction.input.pcmHandle)
		{
			snd_pcm_unlink(runtime.inAction.input.pcmHandle);
		}
	}
	else
	{
		if(runtime.inAction.input.pcmHandle)
		{
			snd_pcm_drop(runtime.inAction.input.pcmHandle);
		}
	}

#ifdef WRITE_LOGFILE
	if(this->shareWithUi.logfileAvailable)
	{
		runtime.logfile.wrapper.stopProcessing(&writtenToMod, &writtenToFile, err);
	}
#endif

	JVX_PREPARE_RT_STOP();

	return (res);
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

jvxErrorType
CjvxAudioAlsaDevice::postProcessAlsa()
{

	jvxErrorType res = JVX_NO_ERROR;
	int resA = 0;
	int i;
	bool veto = false;

#ifdef WRITE_LOGFILE
	IrtpWriteLogfiles** retHdl = NULL;
	jvxInt64 writtenToMod = 0;
	jvxInt64 writtenToFile = 0;
	std::string err;
#endif


	if (runtime.inAction.input.params_sw)
	{
        snd_pcm_sw_params_free(runtime.inAction.input.params_sw);
	}
	runtime.inAction.input.params_sw = NULL;

	if (runtime.inAction.output.params_sw)
	{
        snd_pcm_sw_params_free(runtime.inAction.output.params_sw);
	}
	runtime.inAction.output.params_sw = NULL;

	if (runtime.ram.fld_outputi)
	{
		delete[] (runtime.ram.fld_outputi);
		runtime.ram.fld_outputi = NULL;
	}

	if (runtime.ram.fld_inputi)
	{
		delete[] (runtime.ram.fld_inputi);
		runtime.ram.fld_inputi = NULL;
	}

	if (runtime.ram.fld_outputn)
	{
		for (i = 0; i < runtime.inAction.numOutputChansHw; i++)
		{
			delete[] ((char*) runtime.ram.fld_outputn[i]);
		}
		delete[] (runtime.ram.fld_outputn);
		runtime.ram.fld_outputn = NULL;
	}

	if (runtime.ram.fld_inputn)
	{
		for (i = 0; i < runtime.inAction.numInputChansHw; i++)
		{
			delete[] ((char*) runtime.ram.fld_inputn[i]);
		}
		delete[] (runtime.ram.fld_inputn);
		runtime.ram.fld_inputn = NULL;
	}

	if (runtime.theArea_rw_input)
	{
		delete[] (runtime.theArea_rw_input);
		runtime.theArea_rw_input = NULL;
	}

	if (runtime.theArea_rw_output)
	{
		delete[] (runtime.theArea_rw_output);
		runtime.theArea_rw_output = NULL;
	}

	//this->closeHandles(runtime.inAction.input.pcmHandle, runtime.inAction.output.pcmHandle, runtime.inAction.input.params_hw, runtime.inAction.output.params_hw);

#ifdef WRITE_LOGFILE
	if(this->shareWithUi.logfileAvailable)
	{
		runtime.logfile.wrapper.cleanAllLines();
		runtime.logfile.wrapper.getLogFileHandle(retHdl);
		runtime.logfile.wrapper.deactivate();
		runtime.logfile.RPA.fld = NULL;
		runtime.logfile.RPB.fld = NULL;
		runtime.logfile.RPC.fld = NULL;
		runtime.logfile.RPD.fld = NULL;

		this->_hostRefObject->requestInterface((void**)&retHdl, JVX_INTERFACE_WRITELOGFILES);

	}

#endif

#ifdef JVX_ALSA_LOGTOFILE
    theLogger.deactivate_logging();
    theLogger.remove_all_tags();
    theLogger.remove_all_lanes();
#endif

	return(res);
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================


void
CjvxAudioAlsaDevice::openHandles(snd_pcm_t*& input_pcmHandle, snd_pcm_t*& output_pcmHandle, snd_pcm_hw_params_t*& input_params_hw, snd_pcm_hw_params_t*& output_params_hw, bool block, std::string systemNameDevice)
//				 snd_pcm_sw_params_t*& input_params_sw, snd_pcm_sw_params_t*& output_params_sw)
{
	int resA = 0;
	std::string nameDevice;

	nameDevice = systemNameDevice;

	if (thisDevice.input.exists)
	{
		snd_pcm_hw_params_malloc(&input_params_hw);
		//snd_pcm_sw_params_malloc(&input_params_sw);


		resA = snd_pcm_open(&input_pcmHandle, nameDevice.c_str(), SND_PCM_STREAM_CAPTURE, (block ? 0 : SND_PCM_ASYNC));
		if (resA != 0)
		{
			std::cout << "ALSA: Unexpected error, failed to open input PCM " << thisDevice.systemNameDevice << ", reason: " << snd_strerror(resA) << std::endl;
			exit(0);
		}

		// Get the input hw parameters
		resA = snd_pcm_hw_params_any(input_pcmHandle, input_params_hw);
		std::cout << resA << std::endl;
		if (resA < 0)
		{
			std::cout << "ALSA: Error obtaining HW params input for PCM " << thisDevice.systemNameDevice << ", reason: " << snd_strerror(resA) << std::endl;
			exit(0);
		}
		/*
		 // Get the input sw parameters
		 resA = snd_pcm_sw_params_current(input_pcmHandle, input_params_sw);
		 if(resA != 0)
		 {
		 std::cout << "ALSA: Error obtaining SW params input for PCM " <<
		 thisDevice.systemNameDevice << ", reason: " <<  snd_strerror(resA) << std::endl;
		 exit(0);
		 }
		 */
	}

	if (thisDevice.output.exists)
	{
		snd_pcm_hw_params_malloc(&output_params_hw);
		//snd_pcm_sw_params_malloc(&output_params_sw);

		resA = snd_pcm_open(&output_pcmHandle, nameDevice.c_str(), SND_PCM_STREAM_PLAYBACK, (block ? 0 : SND_PCM_ASYNC));
		if (resA != 0)
		{
			std::cout << "ALSA: Unexpected error, failed to open output PCM " << thisDevice.systemNameDevice.c_str() << ", reason: " << snd_strerror(resA) << std::endl;
			exit(0);
		}

		// Get the output hw parameters
		resA = snd_pcm_hw_params_any(output_pcmHandle, output_params_hw);
		if (resA < 0)
		{
			std::cout << "ALSA: Error obtaining HW params output for PCM " << thisDevice.systemNameDevice << ", reason: " << snd_strerror(resA) << std::endl;
			exit(0);
		}

		/*
		 // Get the output sw parameters
		 resA = snd_pcm_sw_params_current(output_pcmHandle, output_params_sw);
		 if(resA != 0)
		 {
		 std::cout << "ALSA: Error obtaining SW params output for PCM " <<
		 thisDevice.systemNameDevice << ", reason: " <<  snd_strerror(resA) << std::endl;
		 exit(0);
		 }
		 */
	}
}

// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================
// =============================================================================================================

void
CjvxAudioAlsaDevice::closeHandles(snd_pcm_t*& input_pcmHandle, snd_pcm_t*& output_pcmHandle, snd_pcm_hw_params_t*& input_params_hw, snd_pcm_hw_params_t*& output_params_hw)
{
	if (input_pcmHandle)
	{
		snd_pcm_hw_params_free(input_params_hw);
		//snd_pcm_sw_params_free(input_params_sw);
		snd_pcm_close(input_pcmHandle);
		snd_config_update_free_global();
		// https://stackoverflow.com/questions/13478861/alsa-mem-leak
	}
	input_pcmHandle = NULL;
	input_params_hw = NULL;
//input_params_sw = NULL;

	if (output_pcmHandle)
	{
		snd_pcm_hw_params_free(output_params_hw);
		//snd_pcm_sw_params_free(output_params_sw);
		snd_pcm_close(output_pcmHandle);
		snd_config_update_free_global();
		// https://stackoverflow.com/questions/13478861/alsa-mem-leak
	}
	output_pcmHandle = NULL;
	output_params_hw = NULL;
//output_params_sw = NULL;
}
