#include "CjvxAudioAlsaTechnology.h"

static const char *ignore[] =
{
		"hw",
		"plughw",
		"plug",
		"dsnoop",
		"tee",
		"file",
		"null",
		"shm",
		"cards",
		"rate_convert",
		NULL
};

static bool IgnoreConfiguration( std::string config )
{
    int i = 0;
    while( ignore[i] )
    {
        if(config == ignore[i])
        {
            return true;
        }
        i++;
    }

    return false;
}

// =========================================================================================

CjvxAudioAlsaTechnology::CjvxAudioAlsaTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): CjvxAudioTechnology(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxTechnology*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
}

jvxErrorType
CjvxAudioAlsaTechnology::activate()
{
	jvxErrorType res = CjvxAudioTechnology::activate();

	if(res == JVX_NO_ERROR)
	{
		initializeAlsa();
	}
	return(res);
}


jvxErrorType
CjvxAudioAlsaTechnology::deactivate()
{
	jvxSize i;
	jvxErrorType res = _deactivate();
	if(res == JVX_NO_ERROR)
	{
	  auto elm = _common_tech_set.lstDevices.begin();
		for(; elm != _common_tech_set.lstDevices.end(); elm++)
		{
			delete(elm->hdlDev);
		}
		_common_tech_set.lstDevices.clear();
	}
	return(res);
}

bool
CjvxAudioAlsaTechnology::initializeAlsa()
{

	snd_ctl_t* cardCtlHandle = NULL;
	snd_ctl_card_info_t* cardInfo = NULL;
	snd_pcm_info_t *pcminfo = NULL;

	//propertiesSet setup;
	//propertiesCanSet setupCanDo;
	int i, j, k;
	int res = 0;
	//int cnt = 0;

	//      int usePeriodMin = period_default;
	//      snd_pcm_uframes_t useBuffersizeMin = buffersize_default;

	// Update configuration files
	if(snd_config == NULL)
	{
		snd_config_update();
	}

	snd_ctl_card_info_malloc(&cardInfo);
	snd_pcm_info_malloc(&pcminfo);

	// std::cout << "Test run for ALSA based soundcards" << std::endl;

	int cardId = -1;
	if (snd_card_next(&cardId) == 0)
	{
		while (cardId >= 0)
		{

			// Reserve one object
			CjvxAudioAlsaDevice::oneCard cc;
			cc.friendlyNameSoundcard = "";
			cc.systemNameSoundcard = "";
			cc.descriptionSoundcard = "";
			cc.idSoundcard = -1;

			// Start processing on card level
			cc.systemNameSoundcard = "hw:" + jvx_int2String(cardId);
			//cc.systemNameSoundcard = "plughw:" + jvx_int2String(cardId);

			// Open card handle
			res = snd_ctl_open(&cardCtlHandle, cc.systemNameSoundcard.c_str(),
					0);
			if (res == 0)
			{
				// Obtain card info
				snd_ctl_card_info(cardCtlHandle, cardInfo);

				// Derive names from card info handle
				cc.friendlyNameSoundcard = snd_ctl_card_info_get_name(cardInfo);
				cc.descriptionSoundcard = snd_ctl_card_info_get_longname(
						cardInfo);
				cc.idSoundcard = cardId;

				// Browse through all devices
				int device = -1;
				res = snd_ctl_pcm_next_device(cardCtlHandle, &device);
				if (res == 0)
				{
					while (device >= 0)
					{
						CjvxAudioAlsaDevice::oneDevice cd;
						cd.systemNameDevice = "";

						// Input side
						cd.input.friendlyNameDevice = "";
						cd.input.exists = false;
						cd.input.numSubdevices = 0;
						memset(&cd.input.syncId, 0, sizeof(int) * 4);

						// Output side
						cd.output.friendlyNameDevice = "";
						cd.output.exists = false;
						cd.output.numSubdevices = 0;
						memset(&cd.output.syncId, 0, sizeof(int) * 4);

						// Set device id for further processing
						snd_pcm_info_set_device(pcminfo, device);

						//snd_pcm_info_set_subdevice(pcminfo, 0);

						cd.systemNameDevice = cc.systemNameSoundcard + ","
								+ jvx_int2String(device);
						cd.isPlugConnection = false;

						// First case: Record
						snd_pcm_info_set_stream(pcminfo,
								SND_PCM_STREAM_CAPTURE);
						res = snd_ctl_pcm_info(cardCtlHandle, pcminfo);
						if (res == 0)
						{
							cd.input.syncId = snd_pcm_info_get_sync(pcminfo);
							cd.input.friendlyNameDevice = snd_pcm_info_get_name(
									pcminfo);
							cd.input.exists = true;
							cd.input.numSubdevices =
									snd_pcm_info_get_subdevices_count(pcminfo);
							for (k = 0; k < cd.input.numSubdevices; k++)
							{
								CjvxAudioAlsaDevice::oneSubDevice cs;
								cs.name = snd_pcm_info_get_subdevice_name(
										pcminfo);
								cd.input.subdevices.push_back(cs);
							}
						}

						// Second case: Playback
						snd_pcm_info_set_stream(pcminfo,
								SND_PCM_STREAM_PLAYBACK);
						res = snd_ctl_pcm_info(cardCtlHandle, pcminfo);
						if (res == 0)
						{
							cd.output.syncId = snd_pcm_info_get_sync(pcminfo);
							cd.output.friendlyNameDevice =
									snd_pcm_info_get_name(pcminfo);
							cd.output.exists = true;
							cd.output.numSubdevices =
									snd_pcm_info_get_subdevices_count(pcminfo);
							for (k = 0; k < cd.output.numSubdevices; k++)
							{
								CjvxAudioAlsaDevice::oneSubDevice cs;
								cs.name = snd_pcm_info_get_subdevice_name(
										pcminfo);
								cd.output.subdevices.push_back(cs);
							}
						}

						cc.lstDevices.push_back(cd);

						cd.systemNameDevice = cd.systemNameDevice;
						cd.isPlugConnection = true;

						cc.lstDevices.push_back(cd);

						res = snd_ctl_pcm_next_device(cardCtlHandle, &device);
						if (res != 0)
						{
							break;
						}

					}
				}


				for (j = 0; j < cc.lstDevices.size(); j++)
				{
					CjvxAudioAlsaDevice* ptrDev = NULL;

					//cnt++;
					if(cc.lstDevices[j].isPlugConnection)
					{
						ptrDev =
						  new CjvxAudioAlsaDevice(
									  (cc.friendlyNameSoundcard + "("
									   + cc.descriptionSoundcard + ","
									   + cc.lstDevices[j].systemNameDevice
									   + ", plug)").c_str(), false,
									  _common_set.theDescriptor.c_str(),
									  _common_set.theFeatureClass,
									  _common_set.theModuleName.c_str(),
									  JVX_COMPONENT_ACCESS_SUB_COMPONENT,
									  JVX_COMPONENT_AUDIO_DEVICE, "", NULL
									  );
						/*device_name.c_str(), false, 
						  _common_set.theDescriptor.c_str(), 
						  _common_set.theFeatureClass, 
						  _common_set.theModuleName.c_str(), 
						  JVX_COMPONENT_ACCESS_SUB_COMPONENT*/
					}
					else
					{
						ptrDev =
						  new CjvxAudioAlsaDevice(
									  (cc.friendlyNameSoundcard + "("
									   + cc.descriptionSoundcard + ","
									   + cc.lstDevices[j].systemNameDevice
									   + ")").c_str(), false,
									  _common_set.theDescriptor.c_str(),
									  _common_set.theFeatureClass,
									  _common_set.theModuleName.c_str(),
									  JVX_COMPONENT_ACCESS_SUB_COMPONENT,
									  JVX_COMPONENT_AUDIO_DEVICE, "", NULL
									  );
						/*device_name.c_str(), false, 
						  _common_set.theDescriptor.c_str(), 
						  _common_set.theFeatureClass, 
						  _common_set.theModuleName.c_str(), 
						  JVX_COMPONENT_ACCESS_SUB_COMPONENT*/
					}

					ptrDev->props_device(cc.lstDevices[j]);
					CjvxTechnology::oneDeviceWrapper elm;
					elm.hdlDev = ptrDev;
					_common_tech_set.lstDevices.push_back(elm);
				}
				snd_ctl_close(cardCtlHandle);
			}      // if (res == 0)
			res = snd_card_next(&cardId);
			if (res != 0)
			{
				break;
			}
		}
	}
	snd_ctl_card_info_free(cardInfo);
	snd_pcm_info_free(pcminfo);

	assert(snd_config);
	snd_config_t* topPcmNode = NULL;

	res = snd_config_search(snd_config, "pcm", &topPcmNode);
	assert(res == 0);

	snd_config_iterator_t it, next;

	snd_config_for_each( it, next, topPcmNode )
	{
		// Reserve one object
		CjvxAudioAlsaDevice::oneCard cc;
		cc.friendlyNameSoundcard = "";
		cc.systemNameSoundcard = "";
		cc.descriptionSoundcard = "";
		cc.idSoundcard = -1;

		const char *tpStr = "unknown", *idStr = NULL;
		int err = 0;

		char *alsaDeviceName, *deviceName;

		//const HwDevInfo *predefined = NULL;
		snd_config_t *n = snd_config_iterator_entry( it ), * tp = NULL;;

		res = snd_config_search( n, "type", &tp );
	    if(res >= 0 )
	    {
	    	res = snd_config_get_string( tp, &tpStr );
	    	assert(res == 0);
	    	res = snd_config_get_id( n, &idStr );
	    	assert(res == 0);
	    	if(!IgnoreConfiguration(idStr))
	    	{
	    		cc.friendlyNameSoundcard = idStr;
	    		cc.systemNameSoundcard = idStr;
	    		cc.descriptionSoundcard = idStr;

	    		CjvxAudioAlsaDevice::oneDevice cd;
	    		cd.systemNameDevice = idStr;
	    		cd.isPlugConnection = false;

	    		// Input side
	    		cd.input.friendlyNameDevice = idStr;
	    		cd.input.exists = true;
	    		cd.input.numSubdevices = 0;
	    		memset(&cd.input.syncId, 0, sizeof(int) * 4);

	    		// Output side
	    		cd.output.friendlyNameDevice = idStr;
	    		cd.output.exists = true;
	    		cd.output.numSubdevices = 0;
	    		memset(&cd.output.syncId, 0, sizeof(int) * 4);

	    		cc.lstDevices.push_back(cd);
	    		CjvxAudioAlsaDevice* ptrDev = NULL;

	    		//cnt++;
	    		ptrDev =
	    				new CjvxAudioAlsaDevice(
								(cc.friendlyNameSoundcard).c_str(), false,
								_common_set.theDescriptor.c_str(),
								_common_set.theFeatureClass,
								_common_set.theModuleName.c_str(),
								JVX_COMPONENT_ACCESS_SUB_COMPONENT,
								JVX_COMPONENT_AUDIO_DEVICE, "", NULL);
			/*device_name.c_str(), false, _common_set.theDescriptor.c_str(), 
			  _common_set.theFeatureClass, _common_set.theModuleName.c_str(), 
			  JVX_COMPONENT_ACCESS_SUB_COMPONENT*/

			ptrDev->props_device(cc.lstDevices[0]);
	    		CjvxTechnology::oneDeviceWrapper elm;
	    		elm.hdlDev = ptrDev;
	    		_common_tech_set.lstDevices.push_back(elm);
	    	}
	    }
	}
	return(true);
}
