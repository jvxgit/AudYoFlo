#include "jvx.h"
#include "CjvxAuNCodec.h"

CjvxAuNCodec::CjvxAuNCodec(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	jvxSize i;
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);	
	_common_spec_proc_set.thisisme = static_cast<IjvxObject*>(this);

	for (i = 0; i < JVX_NUM_LANES; i++)
	{
		selIdLane[i] = JVX_SIZE_UNSELECTED;
	}
	mainModuleConnected = NULL;
}

CjvxAuNCodec::~CjvxAuNCodec()
{
}

jvxErrorType 
CjvxAuNCodec::activate()
{
	jvxSize num;
	jvxSize i,j;
	jvxApiString descr;
	jvxApiString descror;
	jvxErrorType resL = JVX_NO_ERROR;

	jvxErrorType res = CjvxAudioNode::activate();
	
	if(res == JVX_NO_ERROR)
	{
		genEncoderDecoder_node::init_all();
		genEncoderDecoder_node::allocate_all();
		genEncoderDecoder_node::register_all(static_cast<CjvxProperties*>(this));

		_common_set_properties.reportIfNoChange = true;

		for (i = 0; i < JVX_NUM_LANES; i++)
		{
			selIdLane[i] = JVX_SIZE_UNSELECTED;
		}

		if (_common_set.theToolsHost)
		{
			_common_set.theToolsHost->number_tools(JVX_COMPONENT_AUDIO_CODEC, &num);
			for (i = 0; i < num; i++)
			{
				resL = _common_set.theToolsHost->identification_tool(JVX_COMPONENT_AUDIO_CODEC, i, &descr, &descror, NULL);
				descr.assert_valid();
				if (resL == JVX_NO_ERROR)
				{
					oneEncDecEntry entry;
					entry.description = descr.std_str();
					entry.descriptor = descror.std_str();
					entry.theCodec = NULL; 
					entry.codecObj = NULL;
					entry.id = i;
					resL = _common_set.theToolsHost->reference_tool(JVX_COMPONENT_AUDIO_CODEC, &entry.codecObj, i, NULL);
					assert(resL == JVX_NO_ERROR);
					entry.codecObj->request_specialization(reinterpret_cast<jvxHandle**>(&entry.theCodec), NULL, NULL, NULL);
					entry.theCodec->initialize(_common_set_min.theHostRef);
					entry.theCodec->select();
					entry.theCodec->activate();
					availableCodecs.push_back(entry);
				}
				else
				{
					assert(0);
				}
			}

			for (i = 0; i < availableCodecs.size(); i++)
			{
				oneEncDecLaneEntry newElm;
				newElm.id = i;
				newElm.inputBuffer = NULL;
				
				newElm.theDecoder = NULL;
				newElm.theEncoder = NULL;
				newElm.theEncProps = NULL;
				newElm.theEncObj = NULL;

				for (j = 0; j < JVX_NUM_LANES; j++)
				{
					availableCodecs[i].theCodec->request_encoder(&newElm.theEncoder);
					availableCodecs[i].theCodec->request_decoder(&newElm.theDecoder);
					if (newElm.theEncoder && newElm.theDecoder)
					{
						theLanes[j].push_back(newElm);
					}
				}
			}

			for (i = 0; i < JVX_NUM_LANES; i++)
			{
				if (theLanes[i].size())
				{
					selIdLane[i] = 0;
					activateEncoderDecoder(theLanes[i][selIdLane[i]], i);
				}
			}

			// Setup first lane for editing
			jvx_bitZSet(genEncoderDecoder_node::general.select_codec_lane.value.selection(), 0);

			updateDependentProperties(true);

			for (i = 0; i < JVX_NUM_LANES; i++)
			{
				createMicroConnections(i);
			}

			update_codec_properties();
		}
	}
	return(res);
};

void
CjvxAuNCodec::createMicroConnections(jvxSize id_lane)
{
	IjvxHiddenInterface* interfaces[2];
	const char* icon_names[2];
	const char* ocon_names[2];
	if (connection_lane[id_lane].micro_connection == NULL)
	{
		connection_lane[id_lane].micro_connection = new HjvxMicroConnection(("Micro Connection Lane #" + jvx_size2String(id_lane)).c_str(), 
			false, ("Micro Connection Lane #" + jvx_size2String(id_lane)).c_str(), 
			0, ("local-temp-lane #" + jvx_size2String(id_lane)).c_str(), 
			JVX_COMPONENT_ACCESS_SUB_COMPONENT, JVX_COMPONENT_AUDIO_DEVICE, "", NULL);
		connection_lane[id_lane].micro_connection->set_location_info(jvxComponentIdentification(JVX_SIZE_SLOT_OFF_SYSTEM, JVX_SIZE_SLOT_OFF_SYSTEM));

		connection_lane[id_lane].theEncoder = theLanes[id_lane][selIdLane[id_lane]].theEncoder;
		connection_lane[id_lane].theDecoder = theLanes[id_lane][selIdLane[id_lane]].theDecoder;

		interfaces[0] = connection_lane[id_lane].theEncoder;
		interfaces[1] = connection_lane[id_lane].theDecoder;

		icon_names[0] = "default";
		icon_names[1] = "default";
		ocon_names[0] = "default";
		ocon_names[1] = "default";

		connection_lane[id_lane].micro_connection->activate_connection(_common_set_min.theHostRef,
			interfaces, icon_names, ocon_names, 2, 
			"default", "default",  
			("Micro Connection Lane #" + jvx_size2String(id_lane)).c_str(),
			false, static_cast<IjvxObject*>(this));
		connection_lane[id_lane].stat = JVX_STATE_INIT;

		if (mainModuleConnected)
		{
			jvxErrorType resL = connection_lane[id_lane].micro_connection->connect_connection(
				&connection_lane[id_lane].descr_encoder_in,
				&connection_lane[id_lane].descr_decoder_out,
				nullptr, nullptr,
				false, false);
			assert(resL == JVX_NO_ERROR);
			connection_lane[id_lane].stat = JVX_STATE_ACTIVE;
		}
	}
}

void
CjvxAuNCodec::releaseMicroConnections(jvxSize id_lane)
{
	jvxErrorType resL = JVX_NO_ERROR;
	IjvxHiddenInterface* interfaces[2];
	const char* icon_names[2];
	const char* ocon_names[2];
	if (connection_lane[id_lane].micro_connection)
	{
		if (connection_lane[id_lane].stat == JVX_STATE_ACTIVE)
		{
			resL = connection_lane[id_lane].micro_connection->disconnect_connection();
			assert(resL == JVX_NO_ERROR);
			connection_lane[id_lane].stat = JVX_STATE_INIT;
		}

		if (connection_lane[id_lane].stat == JVX_STATE_INIT)
		{
			resL = connection_lane[id_lane].micro_connection->deactivate_connection();
			delete connection_lane[id_lane].micro_connection;
			connection_lane[id_lane].micro_connection = NULL;
			connection_lane[id_lane].stat = JVX_STATE_NONE;
		}
	}
}

void
CjvxAuNCodec::activateEncoderDecoder(oneEncDecLaneEntry& theEntry, jvxSize id)
{
	theEntry.theEncoder->initialize(_common_set_min.theHostRef);
	theEntry.theEncoder->select();
	theEntry.theEncoder->activate();
	theEntry.theEncoder->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theEntry.theEncProps));
	if (theEntry.theEncProps)
	{
		_register_sub_module(theEntry.theEncProps, (2 * id + 1)*1024, ("lane" + jvx_size2String(id) + "_encoder").c_str(), "Encoder Lane[" + jvx_size2String(id) + "]: ");
	}
	theEntry.theDecoder->initialize(_common_set_min.theHostRef);
	theEntry.theDecoder->select();
	theEntry.theDecoder->activate();
	theEntry.theDecoder->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theEntry.theDecProps));
	if (theEntry.theDecProps)
	{
		_register_sub_module(theEntry.theDecProps, (2 * id + 2)*1024, ("lane" + jvx_size2String(id) + "_decoder").c_str(), "Decoder Lane[" + jvx_size2String(id) + "]: ");
	}
}

void
CjvxAuNCodec::deactivateEncoderDecoder(oneEncDecLaneEntry& theEntry, jvxSize id)
{
	if (theEntry.theEncProps)
	{
		_unregister_sub_module(("lane" + jvx_size2String(id) + "_encoder").c_str());
	}
	theEntry.theEncoder->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theEntry.theEncProps));
	theEntry.theEncProps = NULL;
	theEntry.theEncoder->deactivate();
	theEntry.theEncoder->unselect();
	theEntry.theEncoder->terminate();

	if (theEntry.theDecProps)
	{
		_unregister_sub_module(("lane" + jvx_size2String(id) + "_decoder").c_str());
	}
	theEntry.theDecoder->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theEntry.theDecProps));
	theEntry.theDecoder->deactivate();
	theEntry.theDecoder->unselect();
	theEntry.theDecoder->terminate();
}

void 
CjvxAuNCodec::updateDependentProperties(jvxBool reconstructList)
{
	jvxSize i;
	jvxSize selLane = JVX_SIZE_UNSELECTED;
	jvxSize selId = JVX_SIZE_UNSELECTED;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxInt32 valI32;
	jvxInt16 valI16;

	jvxHandle* myHdl = NULL;
	jvxSize myHdlSz = 0;

	this->_lock_properties_local();

	selLane = jvx_bitfieldSelection2Id(genEncoderDecoder_node::general.select_codec_lane.value.selection(), genEncoderDecoder_node::general.select_codec_lane.value.entries.size());
	assert(selLane != JVX_SIZE_UNSELECTED);

	if (reconstructList)
	{
		genEncoderDecoder_node::general.codec_in_selected_lane.value.entries.clear();
		for (i = 0; i < theLanes[selLane].size(); i++)
		{
			genEncoderDecoder_node::general.codec_in_selected_lane.value.entries.push_back(availableCodecs[theLanes[selLane][i].id].description);			
		}
	}

	if (JVX_CHECK_SIZE_SELECTED(selIdLane[selLane]))
	{
		jvx_bitZSet(genEncoderDecoder_node::general.codec_in_selected_lane.value.selection(), selIdLane[selLane]);
	}

	// Pass parameters of encoder to match the decoder

	/*
	for (i = 0; i < JVX_NUM_LANES; i++)
	{
		myHdl = NULL;
		myHdlSz = 0;
		theLanes[i][selIdLane[i]].theEncoder->request_configure_token(&myHdl, &myHdlSz);
		if (myHdlSz)
		{
			theLanes[i][selIdLane[i]].theDecoder->provide_configure_token(myHdl, myHdlSz);
		}
	}*/
	
	_common_set_node_params_1io.preferred.buffersize.min = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.buffersize.max = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.samplerate.min = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.samplerate.max = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.number_input_channels.min = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.number_input_channels.max = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.number_output_channels.min = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.number_output_channels.max = JVX_SIZE_UNSELECTED;
	_common_set_node_params_1io.preferred.format.min = JVX_DATAFORMAT_NONE;
	_common_set_node_params_1io.preferred.format.max = JVX_DATAFORMAT_NONE;

	_common_set_node_params_1io.settingReadyFailReason = "";
	_common_set_node_params_1io.settingReadyToStart = true;

	/*
	for (i = 0; i < JVX_NUM_LANES; i++)
	{
		updateAudioParameters(&theLanes[i][selIdLane[i]]);
	}
	*/
	this->_unlock_properties_local();
}

void
CjvxAuNCodec::updateAudioParameters(oneEncDecLaneEntry* theEntry)
{
	IjvxProperties* theProps = NULL;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxInt32 valI32;
	jvxInt16 valI16;
	jvxCallManagerProperties callGate;

	theEntry->theEncoder->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
	if (theProps)
	{
		resL = jvx_get_property(theProps, &valI32, 0, 1, JVX_DATAFORMAT_32BIT_LE, true, "/system/framesize", callGate);
		if (resL == JVX_NO_ERROR)
		{
			if (JVX_CHECK_SIZE_UNSELECTED(_common_set_node_params_1io.preferred.buffersize.min))
			{
				_common_set_node_params_1io.preferred.buffersize.min = (jvxSize)valI32;
				_common_set_node_params_1io.preferred.buffersize.max = (jvxSize)valI32;
			}
			else
			{
				if ((valI32 >= 0) && (valI32 != _common_set_node_params_1io.preferred.buffersize.min))
				{
					_common_set_node_params_1io.settingReadyFailReason = "Bufferize mismatch between lanes / encoders / decoders: ";
					_common_set_node_params_1io.settingReadyFailReason += "<" + 
						jvx_size2String(_common_set_node_params_1io.preferred.buffersize.min) + "> vs. <" + jvx_size2String(valI32) + ">";
					_common_set_node_params_1io.settingReadyToStart = false;
				}
			}
		}
		resL = jvx_get_property(theProps, &valI32, 0, 1, JVX_DATAFORMAT_32BIT_LE, true, "/system/rate", callGate);
		if (resL == JVX_NO_ERROR)
		{
			if (JVX_CHECK_SIZE_UNSELECTED(_common_set_node_params_1io.preferred.samplerate.min))
			{
				_common_set_node_params_1io.preferred.samplerate.min = (jvxSize)valI32;
				_common_set_node_params_1io.preferred.samplerate.max = (jvxSize)valI32;
			}
			else
			{
				if ((valI32 >= 0) && (valI32 != _common_set_node_params_1io.preferred.samplerate.min))
				{
					_common_set_node_params_1io.settingReadyFailReason = "Samplerate mismatch between lanes / encoders / decoders: ";
					_common_set_node_params_1io.settingReadyFailReason += "<" + 
						jvx_size2String(_common_set_node_params_1io.preferred.samplerate.min) + "> vs. <" + jvx_size2String(valI32) + ">";
					_common_set_node_params_1io.settingReadyToStart = false;
				}
			}
		}
		resL = jvx_get_property(theProps, &valI16, 0, 1, JVX_DATAFORMAT_16BIT_LE, true, "/system/dataformat", callGate);
		if (resL == JVX_NO_ERROR)
		{
			if (_common_set_node_params_1io.preferred.format.min == JVX_DATAFORMAT_NONE)
			{
				_common_set_node_params_1io.preferred.format.min = (jvxDataFormat)valI16;
				_common_set_node_params_1io.preferred.format.max = (jvxDataFormat)valI16;
			}
			else
			{
				if ((valI16 != JVX_DATAFORMAT_NONE) && (valI16 != _common_set_node_params_1io.preferred.format.min))
				{
					_common_set_node_params_1io.settingReadyFailReason = "Format mismatch between lanes / encoders / decoders: ";
					_common_set_node_params_1io.settingReadyFailReason += "<";
					_common_set_node_params_1io.settingReadyFailReason += jvxDataFormat_txt(_common_set_node_params_1io.preferred.format.min);
					_common_set_node_params_1io.settingReadyFailReason += "> vs. <";
					_common_set_node_params_1io.settingReadyFailReason += jvxDataFormat_txt(valI16);
					_common_set_node_params_1io.settingReadyFailReason += ">";
					_common_set_node_params_1io.settingReadyToStart = false;
				}
			}
		}
		resL = jvx_get_property(theProps, &valI32, 0, 1, JVX_DATAFORMAT_32BIT_LE, true, "/system/num_input_channels", callGate);
		if (resL == JVX_NO_ERROR)
		{
			if (JVX_CHECK_SIZE_UNSELECTED(_common_set_node_params_1io.preferred.number_input_channels.min))
			{
				_common_set_node_params_1io.preferred.number_input_channels.min = valI32;
				_common_set_node_params_1io.preferred.number_input_channels.max = valI32;
			}
			else
			{
				if ((valI32 >= 0) && (valI32 != _common_set_node_params_1io.preferred.number_input_channels.min))
				{
					_common_set_node_params_1io.settingReadyFailReason = "Number input channels mismatch between lanes: ";
					_common_set_node_params_1io.settingReadyFailReason += "<" + 
						jvx_size2String(_common_set_node_params_1io.preferred.number_input_channels.min) + "> vs. <" + jvx_int2String(valI32) + ">";
					_common_set_node_params_1io.settingReadyToStart = false;
				}
			}
		}

		theEntry->theEncoder->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
	}

	theEntry->theDecoder->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
	if (theProps)
	{
		resL = jvx_get_property(theProps, &valI32, 0, 1, JVX_DATAFORMAT_32BIT_LE, true, "/system/framesize", callGate);
		if (resL == JVX_NO_ERROR)
		{
			if (JVX_CHECK_SIZE_UNSELECTED(_common_set_node_params_1io.preferred.buffersize.min))
			{
				_common_set_node_params_1io.preferred.buffersize.min = (jvxSize)valI32;
				_common_set_node_params_1io.preferred.buffersize.max = (jvxSize)valI32;
			}
			else
			{
				if ((valI32 >= 0) && (valI32 != _common_set_node_params_1io.preferred.buffersize.min))
				{
					_common_set_node_params_1io.settingReadyFailReason = "Bufferize mismatch between lanes / encoders / decoders: ";
					_common_set_node_params_1io.settingReadyFailReason += "<" + 
						jvx_size2String(_common_set_node_params_1io.preferred.buffersize.min) + "> vs. <" + jvx_size2String(valI32) + ">";
					_common_set_node_params_1io.settingReadyToStart = false;
				}
			}
		}
		resL = jvx_get_property(theProps, &valI32, 0, 1, JVX_DATAFORMAT_32BIT_LE, true, "/system/rate", callGate);
		if (resL == JVX_NO_ERROR)
		{
			if (JVX_CHECK_SIZE_UNSELECTED(_common_set_node_params_1io.preferred.samplerate.min))
			{
				_common_set_node_params_1io.preferred.samplerate.min = (jvxSize)valI32;
				_common_set_node_params_1io.preferred.samplerate.max = (jvxSize)valI32;
			}
			else
			{
				if ((valI32 >= 0) && (valI32 != _common_set_node_params_1io.preferred.samplerate.min))
				{
					_common_set_node_params_1io.settingReadyFailReason = "Samplerate mismatch between lanes / encoders / decoders: ";
					_common_set_node_params_1io.settingReadyFailReason += "<" + 
						jvx_size2String(_common_set_node_params_1io.preferred.samplerate.min) + "> vs. <" + jvx_size2String(valI32) + ">";
					_common_set_node_params_1io.settingReadyToStart = false;
				}
			}
		}
		resL = jvx_get_property(theProps, &valI16, 0, 1, JVX_DATAFORMAT_16BIT_LE, true, "/system/dataformat", callGate);
		if (resL == JVX_NO_ERROR)
		{
			if (_common_set_node_params_1io.preferred.format.min == JVX_DATAFORMAT_NONE)
			{
				_common_set_node_params_1io.preferred.format.min = (jvxDataFormat)valI16;
				_common_set_node_params_1io.preferred.format.max = (jvxDataFormat)valI16;
			}
			else
			{
				if ((valI16 != JVX_DATAFORMAT_NONE) && (valI16 != _common_set_node_params_1io.preferred.format.min))
				{
					_common_set_node_params_1io.settingReadyFailReason = "Format mismatch between lanes / encoders / decoders: ";
					_common_set_node_params_1io.settingReadyFailReason += "<";
					_common_set_node_params_1io.settingReadyFailReason += jvxDataFormat_txt(_common_set_node_params_1io.preferred.format.min);
					_common_set_node_params_1io.settingReadyFailReason += "> vs. <";
					_common_set_node_params_1io.settingReadyFailReason += jvxDataFormat_txt(valI16);
					_common_set_node_params_1io.settingReadyFailReason += ">";
					_common_set_node_params_1io.settingReadyToStart = false;
				}
			}
		}

		/*
		resL = jvx_get_property(theProps, &valI32, 0, 1, JVX_DATAFORMAT_32BIT_LE, true, "/system/num_input_channels");
		if (resL == JVX_NO_ERROR)
		{
			if (_common_set_node_params_1io.preferred.number_output_channels.min == -1)
			{
				_common_set_node_params_1io.preferred.number_output_channels.min = valI32;
				_common_set_node_params_1io.preferred.number_output_channels.max = valI32;
			}
			else
			{
				if ((valI32 >= 0) && (valI32 != _common_set_node_params_1io.preferred.number_output_channels.min))
				{
					_common_set_node_params_1io.settingReadyFailReason = "Number output channels mismatch between lanes: ";
					_common_set_node_params_1io.settingReadyFailReason += "<" + jvx_int2String(_common_set_node_params_1io.preferred.number_output_channels.min) + "> vs. <" + jvx_int2String(valI32) + ">";
					_common_set_node_params_1io.settingReadyToStart = false;
				}
			}
		}
		*/
		theEntry->theEncoder->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
		//_common_set_node_params_1io.preferred.number_output_channels.max = -1;
		if (JVX_CHECK_SIZE_SELECTED(_common_set_node_params_1io.preferred.number_input_channels.min))
		{
			_common_set_node_params_1io.preferred.number_output_channels.min = _common_set_node_params_1io.preferred.number_input_channels.min * JVX_NUM_LANES;
		}
		_common_set_node_params_1io.preferred.number_output_channels.max = JVX_SIZE_UNSELECTED;
	}
}

jvxErrorType 
CjvxAuNCodec::deactivate()
{
	jvxSize i,j;
	jvxErrorType res = CjvxAudioNode::_pre_check_deactivate();
	jvxErrorType resL = JVX_NO_ERROR;
	if(res == JVX_NO_ERROR)
	{
		if (_common_set.theToolsHost)
		{
			for (i = 0; i < JVX_NUM_LANES; i++)
			{
				releaseMicroConnections(i);
			}

			for (i = 0; i < JVX_NUM_LANES; i++)
			{
				this->deactivateEncoderDecoder(theLanes[i][selIdLane[i]], i);
				//this->deactivateEncoderDecoder(theLanes[i][selIdLane[i]], 1);

				for (j = 0; j < theLanes[i].size(); j++)
				{
					availableCodecs[theLanes[i][j].id].theCodec->return_encoder(theLanes[i][j].theEncoder);
					availableCodecs[theLanes[i][j].id].theCodec->return_decoder(theLanes[i][j].theDecoder);
				}

				theLanes[i].clear();
			}
			for (i = 0; i < availableCodecs.size(); i++)
			{
				availableCodecs[i].theCodec->deactivate();
				availableCodecs[i].theCodec->unselect();
				availableCodecs[i].theCodec->terminate();

				resL = _common_set.theToolsHost->return_reference_tool(JVX_COMPONENT_AUDIO_CODEC, availableCodecs[i].codecObj);

			}
			availableCodecs.clear();

			// theToolsHostRef->return_object_reference(&theToolsHostObj); -> does not exist
		}
		CjvxAudioNode::deactivate();
	}
	return(res);
};

jvxErrorType 
CjvxAuNCodec::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{
	jvxSize i;
	int ic;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool extCallPres = false;
	jvxSize out = jvx_bitfieldSelection2Id(genEncoderDecoder_node::general.select_codec_lane_out.value.selection(),
		genEncoderDecoder_node::general.select_codec_lane_out.value.entries.size());
	jvxSize cnt = 0;
	jvxBool engage[JVX_NUM_LANES] = { true, true };
	if (genEncoderDecoder_node::general.lane0_encoder_suppress_processing.value)
	{
		engage[0] = false;
	}
	if (genEncoderDecoder_node::general.lane1_encoder_suppress_processing.value)
	{
		engage[1] = false;
	}

	for (i = 0; i < JVX_NUM_LANES; i++)
	{
		if (engage[i])
		{
			connection_lane[i].micro_connection->prepare_process_connection(NULL);

			jvxSize idxIn = *connection_lane[i].descr_encoder_in.con_pipeline.idx_stage_ptr;
			jvxSize idxOut = *connection_lane[i].descr_decoder_out.con_pipeline.idx_stage_ptr;
			jvxSize chan_out = theData->con_compat.number_channels - JVX_NUM_LANES + i;
			assert(chan_out >= 0);

			memcpy(connection_lane[i].descr_encoder_in.con_data.buffers[idxIn][0],
				_common_set_ldslave.theData_in->con_data.buffers[idx_sender_to_receiver][0],
				_common_set_ldslave.theData_in->con_params.buffersize * jvxDataFormat_getsize(_common_set_ldslave.theData_in->con_params.format));

			connection_lane[i].micro_connection->process_connection(NULL);

			memcpy(
				_common_set_ldslave.theData_out.con_data.buffers[idx_receiver_to_sender][chan_out],
				connection_lane[i].descr_decoder_out.con_data.buffers[idxOut][0],
				_common_set_ldslave.theData_in->con_params.buffersize * jvxDataFormat_getsize(_common_set_ldslave.theData_in->con_params.format));

			connection_lane[i].micro_connection->postprocess_process_connection();
		}
	}

	// Copy the one selection into the audio output puffers
	jvxSize idLane = jvx_bitfieldSelection2Id(genEncoderDecoder_node::general.select_codec_lane_out.value.selection(), genEncoderDecoder_node::general.select_codec_lane_out.value.entries.size());
	if (JVX_CHECK_SIZE_SELECTED(idLane))
	{
		jvxSize chan_copy = theData->con_compat.number_channels - JVX_NUM_LANES + idLane;
		for (ic = 0; ic < theData->con_compat.number_channels - 2; ic++)
		{
			memcpy(
				_common_set_ldslave.theData_out.con_data.buffers[idx_receiver_to_sender][ic],
				_common_set_ldslave.theData_out.con_data.buffers[idx_receiver_to_sender][chan_copy],
				_common_set_ldslave.theData_in->con_params.buffersize * jvxDataFormat_getsize(_common_set_ldslave.theData_in->con_params.format));
		}
	}



#if 0
	for (i = 0; i < JVX_NUM_LANES; i++)
	{
		for (ic = 0; ic < theData->con_params.number_channels; ic++)
		{
			memcpy(theDataEnc[i].con_data.buffers[0][ic], theData->con_data.buffers[idx_sender_to_receiver][ic],
				jvxDataFormat_size[theData->con_params.format] * theData->con_params.buffersize);
		}
		procLanes[i].theEncDp->process_st(&theDataEnc[i], 0, 0);
		procLanes[i].theDecDp->process_st(&theDataDec[i], 0, 0);

		for (ic = 0; ic < theDataDec[i].con_compat.number_channels; ic++)
		{
			if (cnt < theData->con_compat.number_channels)
			{
				memcpy(theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender][cnt],
					theDataDec[i].con_compat.from_receiver_buffer_allocated_by_sender[0][ic],
					jvxDataFormat_size[theData->con_compat.format] * theData->con_compat.buffersize);
				cnt++;
			}
		}
	}

	// Fill all other output channels
	jvxBool stopthis = false;
	while(!stopthis)
	{

		for (ic = 0; ic < theDataDec[out].con_compat.number_channels; ic++)
		{
			if (cnt < theData->con_compat.number_channels)
			{
				memcpy(theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender][cnt],
					theDataDec[out].con_compat.from_receiver_buffer_allocated_by_sender[0][ic],
					jvxDataFormat_size[theData->con_compat.format] * theData->con_compat.buffersize);
				cnt++;
			}
			else
			{
				stopthis = true;
				break;
			}
		}		
	}
#endif
	return(res);
};

jvxErrorType
CjvxAuNCodec::prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	jvxSize i;
	jvxInt32 valI32 = 0;
	jvxErrorType res = CjvxAudioNode::prepare_sender_to_receiver(theData);
	jvxErrorType resL;
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < JVX_NUM_LANES; i++)
		{
			connection_lane[i].descr_encoder_in.con_data.number_buffers = 1;
			connection_lane[i].micro_connection->prepare_connection(false, false);
			// connection_lane[i].micro_connection->start_connection();
		}

#if 0

		_data_link_init();
		for (i = 0; i < JVX_NUM_LANES; i++)
		{
			// Preset the data structures
			//_prepare_data_link_slot(&theDataEnc[i]);
			//_prepare_data_link_slot(&theDataDec[i]);

			// Return the maximum number of bytes per frame
			resL = jvx_get_property(theLanes[i][selIdLane[i]].theEncProps, &valI32, 0, 1, JVX_DATAFORMAT_32BIT_LE, true, "/system/codec/max_bytes_frame");
			assert(resL == JVX_NO_ERROR);

			// Obtain data processor handles
			assert(0);
			/*
			resL = theLanes[i][selIdLane[i]].theEncoder->request_hidden_interface(JVX_INTERFACE_DATAPROCESSOR, reinterpret_cast<jvxHandle**>(&procLanes[i].theEncDp));
			assert(resL == JVX_NO_ERROR);
			resL = theLanes[i][selIdLane[i]].theDecoder->request_hidden_interface(JVX_INTERFACE_DATAPROCESSOR, reinterpret_cast<jvxHandle**>(&procLanes[i].theDecDp));
			assert(resL == JVX_NO_ERROR);
			*/

			// Prepare allocation
			theDataEnc[i].con_params.buffersize = _common_set_node_params_1io.processing.buffersize;
			theDataEnc[i].con_params.format = _common_set_node_params_1io.processing.format;
			theDataEnc[i].con_params.rate = _common_set_node_params_1io.processing.samplerate;
			theDataEnc[i].con_params.number_channels = _common_set_node_params_1io.processing.number_input_channels;
			theDataEnc[i].con_data.number_buffers = 1;

			// Fill in the values for output direction
			theDataEnc[i].con_compat.buffersize = valI32;
			theDataEnc[i].con_compat.format = JVX_DATAFORMAT_BYTE;
			theDataEnc[i].con_compat.rate = -1;
			theDataEnc[i].con_compat.number_channels = 1;
			theDataEnc[i].con_compat.number_buffers = 1;

			// Allocate data to pass data to encoder
			resL = procLanes[i].theEncDp->prepare_sender_to_receiver(&theDataEnc[i]);
			assert(resL == JVX_NO_ERROR);

			// Prepare decoder input data - should be identical to maximum size of encoder output
			theDataDec[i].con_params.buffersize = valI32;
			theDataDec[i].con_params.format = JVX_DATAFORMAT_BYTE;
			theDataDec[i].con_params.rate = -1;
			theDataDec[i].con_params.number_channels = 1;
			theDataDec[i].con_data.number_buffers = 1;

			// Output side requires a new buffer with audio parameters
			theDataDec[i].con_compat.buffersize = _common_set_node_params_1io.processing.buffersize;
			theDataDec[i].con_compat.format = _common_set_node_params_1io.processing.format;
			theDataDec[i].con_compat.rate = _common_set_node_params_1io.processing.samplerate;
			theDataDec[i].con_compat.number_channels = _common_set_node_params_1io.processing.number_input_channels;
			theDataDec[i].con_compat.number_buffers = 1;

			// Allocate decoder input buffer
			resL = procLanes[i].theDecDp->prepare_sender_to_receiver(&theDataDec[i]);
			assert(resL == JVX_NO_ERROR);

			theDecoderHints_other[i] = NULL;
			theEncoderHints_other[i] = NULL;

			//jvx_hintDesriptor_find(theDataEnc[i].con_data.user_hints, reinterpret_cast<jvxHandle**>(&theEncoderHints_other[i]), JVX_DATAPROCESSOR_HINT_DESCRIPTOR_TYPE_ENTRIES_RESTRICT, theDataEnc[i].receiver.tp);
			//jvx_hintDesriptor_find(theDataDec[i].con_data.user_hints, reinterpret_cast<jvxHandle**>(&theDecoderHints_other[i]), JVX_DATAPROCESSOR_HINT_DESCRIPTOR_TYPE_ENTRIES_RESTRICT, theDataDec[i].receiver.tp);
		}
#endif
	}
	return(res);
}

jvxErrorType
CjvxAuNCodec::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = CjvxAudioNode::start_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < JVX_NUM_LANES; i++)
		{
			connection_lane[i].micro_connection->start_connection();
		}
	}
	return res;

}

jvxErrorType
CjvxAuNCodec::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = CjvxAudioNode::stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < JVX_NUM_LANES; i++)
		{
			connection_lane[i].micro_connection->stop_connection();
		}
	}
	return res;

}

jvxErrorType
CjvxAuNCodec::prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	jvxSize i;
	jvxInt32 valI32 = 0;
	jvxErrorType res = CjvxAudioNode::prepare_complete_receiver_to_sender(theData);
	jvxErrorType resL;
	if (res == JVX_NO_ERROR)
	{
#if 0
		for (i = 0; i < JVX_NUM_LANES; i++)
		{
			// Now allocate decoder output - locally right here
			//_init_data_link(&theDataDec[i], false, false);
			assert(0);

			// Cross link: Decoder input is encoder output
			theDataEnc[i].con_compat.from_receiver_buffer_allocated_by_sender =
				theDataDec[i].con_data.buffers;

			// Set cross references
			//theDataEnc[i].sender.tp = _common_set.theComponentType;
			//theDataEnc[i].sender.source = _common_set.thisisme;

			theEncoderHints[i].num_entries = -1;
			theEncoderHints[i].offset_entries = -1;
			theDataEnc[i].con_compat.user_hints =
				jvx_hintDesriptor_push_front(&theEncoderHints[i],
					JVX_DATAPROCESSOR_HINT_DESCRIPTOR_TYPE_ENTRIES_RESTRICT, JVX_COMPONENT_AUDIO_NODE,
					theDataEnc[i].con_compat.user_hints);

			// Show encoder that allocation is complete
			resL = procLanes[i].theEncDp->prepare_complete_receiver_to_sender(&theDataEnc[i]);
			assert(resL == JVX_NO_ERROR);

			theDecoderHints[i].num_entries = -1;
			theDecoderHints[i].offset_entries = -1;
			theDataDec[i].con_compat.user_hints =
				jvx_hintDesriptor_push_front(&theDecoderHints[i],
					JVX_DATAPROCESSOR_HINT_DESCRIPTOR_TYPE_ENTRIES_RESTRICT, JVX_COMPONENT_AUDIO_NODE,
					theDataDec[i].con_compat.user_hints);

			// Tell the decoder that output is complete
			resL = procLanes[i].theDecDp->prepare_complete_receiver_to_sender(&theDataDec[i]);
			assert(resL == JVX_NO_ERROR);
		}

		// Start everything
		_data_link_start();
#endif
	}
	return res;
}

jvxErrorType 
CjvxAuNCodec::postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)
{
	jvxSize i;
	jvxInt32 valI32 = 0;
	jvxErrorType res = CjvxAudioNode::postprocess_sender_to_receiver(theData);
	jvxErrorType resL;
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < JVX_NUM_LANES; i++)
		{
			connection_lane[i].micro_connection->postprocess_connection();
		}
#if 0
		for (i = 0; i < JVX_NUM_LANES; i++)
		{
			resL = procLanes[i].theDecDp->postprocess_sender_to_receiver(&theDataDec[i]);
			assert(resL == JVX_NO_ERROR);

			jvx_hintDesriptor_pop_front(theDataDec[i].con_compat.user_hints,
				NULL, NULL, NULL);

			resL = procLanes[i].theEncDp->postprocess_sender_to_receiver(&theDataEnc[i]);
			assert(resL == JVX_NO_ERROR);

			jvx_hintDesriptor_pop_front(theDataEnc[i].con_compat.user_hints,
				NULL, NULL, NULL);

			/*
			resL = theLanes[i][selIdLane[i]].theEncoder->return_hidden_interface(JVX_INTERFACE_DATAPROCESSOR, reinterpret_cast<jvxHandle**>(procLanes[i].theEncDp));
			assert(resL == JVX_NO_ERROR);
			procLanes[i].theEncDp = NULL;

			resL = theLanes[i][selIdLane[i]].theDecoder->return_hidden_interface(JVX_INTERFACE_DATAPROCESSOR, reinterpret_cast<jvxHandle**>(procLanes[i].theDecDp));
			assert(resL == JVX_NO_ERROR);
			procLanes[i].theDecDp = NULL;
			*/
		}
		//_postprocess_data_link_slot(NULL);
		_data_link_terminate();
#endif
	}
	return res;
}

jvxErrorType 
CjvxAuNCodec::before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData)
{
	jvxSize i;
	jvxInt32 valI32 = 0;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL;

#if 0
	_data_link_stop();

	for (i = 0; i < JVX_NUM_LANES; i++)
	{

		resL = procLanes[i].theDecDp->before_postprocess_receiver_to_sender(&theDataDec[i]);
		assert(resL == JVX_NO_ERROR);

		resL = procLanes[i].theEncDp->before_postprocess_receiver_to_sender(&theDataEnc[i]);
		assert(resL == JVX_NO_ERROR);

		jvx_deallocateDataLinkDescriptor(theData, true);
		theDataEnc[i].con_compat.from_receiver_buffer_allocated_by_sender = NULL;
	}
#endif
	res =  CjvxAudioNode::before_postprocess_receiver_to_sender(theData);
	return res;
}

jvxErrorType
CjvxAuNCodec::set_property(
	jvxCallManagerProperties& callGate, 
	const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
	const jvx::propertyAddress::IjvxPropertyAddress& ident,
	const jvx::propertyDetail::CjvxTranferDetail& trans)
{
	jvxSize i;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxBool fundamentalChange = false;
	jvxBool anyChangeAtAll = false;
	jvxAccessProtocol accProtLoc = JVX_ACCESS_PROTOCOL_OK;

	jvxErrorType res = CjvxAudioNode::set_property(
		callGate, rawPtr, ident, trans);

	JVX_PROPERTY_CHECK_RESULT(res, callGate)
	{
		JVX_TRANSLATE_PROP_ADDRESS_IDX_CAT(ident, propId, category);
		if (
			(propId == genEncoderDecoder_node::general.select_codec_lane.globalIdx) &&
			(category == genEncoderDecoder_node::general.select_codec_lane.category))
		{
			jvxSize newLaneId = jvx_bitfieldSelection2Id(genEncoderDecoder_node::general.select_codec_lane.value.selection(), genEncoderDecoder_node::general.select_codec_lane.value.entries.size());

			// Update all dependent configurations
			jvx_bitZSet(genEncoderDecoder_node::general.codec_in_selected_lane.value.selection(), this->selIdLane[newLaneId]);
		}

		if (
			(propId == genEncoderDecoder_node::general.codec_in_selected_lane.globalIdx) &&
			(category == genEncoderDecoder_node::general.codec_in_selected_lane.category))
		{
			jvxSize newLaneId = jvx_bitfieldSelection2Id(genEncoderDecoder_node::general.select_codec_lane.value.selection(), genEncoderDecoder_node::general.select_codec_lane.value.entries.size());
			jvxSize newSelId = jvx_bitfieldSelection2Id(genEncoderDecoder_node::general.codec_in_selected_lane.value.selection(), genEncoderDecoder_node::general.codec_in_selected_lane.value.entries.size());
			if (newSelId != this->selIdLane[newLaneId])
			{
				this->releaseMicroConnections(newLaneId);
				this->deactivateEncoderDecoder(this->theLanes[newLaneId][this->selIdLane[newLaneId]], newLaneId);
				this->selIdLane[newLaneId] = newSelId;
				this->activateEncoderDecoder(this->theLanes[newLaneId][this->selIdLane[newLaneId]], newLaneId);
				this->createMicroConnections(newLaneId);
				fundamentalChange = true;
			}
		}

		if (fundamentalChange || (
			(propId == CjvxAudioNode_genpcg::properties_parameters.buffersize.globalIdx) &&
			(category == CjvxAudioNode_genpcg::properties_parameters.buffersize.category)))
		{
			update_codec_properties();
			anyChangeAtAll = true;
		}
		if (fundamentalChange || (
			(propId == CjvxAudioNode_genpcg::properties_parameters.samplerate.globalIdx) &&
			(category == CjvxAudioNode_genpcg::properties_parameters.samplerate.category)))
		{
			update_codec_properties();
			anyChangeAtAll = true;
		}
		if (fundamentalChange || (
			(propId == CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.globalIdx) &&
			(category == CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.category)))
		{
			update_codec_properties();
			anyChangeAtAll = true;
		}
		if (fundamentalChange || (
			(propId == CjvxAudioNode_genpcg::properties_parameters.format.globalIdx) &&
			(category == CjvxAudioNode_genpcg::properties_parameters.format.category)))
		{
			update_codec_properties();
			anyChangeAtAll = true;
		}

		if (anyChangeAtAll)
		{
			updateDependentProperties(true);
			CjvxAudioNode::reportPreferredParameters(JVX_PROPERTY_CATEGORY_UNKNOWN, JVX_SIZE_UNSELECTED);
			_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_PROPERTY_VIEWER_FULL_SHIFT);
		}
	}

	return(res);
}

// ==========================================================================

jvxErrorType 
CjvxAuNCodec::prepare()
{
	jvxSize i;
	jvxErrorType res = CjvxAudioNode::prepare();
	jvxErrorType resL = JVX_NO_ERROR;
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < JVX_NUM_LANES; i++)
		{
			resL = theLanes[i][selIdLane[i]].theEncoder->prepare();
			assert(resL == JVX_NO_ERROR);

			resL = theLanes[i][selIdLane[i]].theDecoder->prepare();
			assert(resL == JVX_NO_ERROR);
		}
	}
	return(res);
}

jvxErrorType 
CjvxAuNCodec::start()
{
	jvxSize i;
	jvxErrorType res = CjvxAudioNode::start();
	jvxErrorType resL = JVX_NO_ERROR;
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < JVX_NUM_LANES; i++)
		{
			resL = theLanes[i][selIdLane[i]].theEncoder->start();
			assert(resL == JVX_NO_ERROR);

			resL = theLanes[i][selIdLane[i]].theDecoder->start();
			assert(resL == JVX_NO_ERROR);
		}
	}
	return(res);
}

jvxErrorType 
CjvxAuNCodec::stop()
{
	jvxSize i;
	jvxErrorType res = CjvxAudioNode::stop();
	jvxErrorType resL = JVX_NO_ERROR;
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < JVX_NUM_LANES; i++)
		{
			resL = theLanes[i][selIdLane[i]].theEncoder->stop();
			assert(resL == JVX_NO_ERROR);

			resL = theLanes[i][selIdLane[i]].theDecoder->stop();
			assert(resL == JVX_NO_ERROR);
		}
	}
	return(res);
}

jvxErrorType 
CjvxAuNCodec::postprocess()
{
	jvxSize i;
	jvxErrorType res = CjvxAudioNode::postprocess();
	jvxErrorType resL = JVX_NO_ERROR;
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < JVX_NUM_LANES; i++)
		{
			resL = theLanes[i][selIdLane[i]].theEncoder->postprocess();
			assert(resL == JVX_NO_ERROR);

			resL = theLanes[i][selIdLane[i]].theDecoder->postprocess();
			assert(resL == JVX_NO_ERROR);
		}
	}
	return(res);
}

// ===========================================================================

jvxErrorType
CjvxAuNCodec::update_codec_properties()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGateLoc;

	callGateLoc.accessFlags = JVX_ACCESS_ROLE_DEFAULT;
	callGateLoc.s_push();

	for (i = 0; i < JVX_NUM_LANES; i++)
	{
		callGateLoc.s_pop();

		jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
			CjvxAudioNode_genpcg::properties_parameters.buffersize.globalIdx, 
			CjvxAudioNode_genpcg::properties_parameters.buffersize.category);
		jvx::propertyDetail::CjvxTranferDetail trans(true);

		res = theLanes[i][selIdLane[i]].theEncProps->set_property(callGateLoc,
			jPRG(&CjvxAudioNode_genpcg::properties_parameters.buffersize.value,
			1, CjvxAudioNode_genpcg::properties_parameters.buffersize.format),
			ident, trans);
		//assert(res == JVX_NO_ERROR);
	}

	for (i = 0; i < JVX_NUM_LANES; i++)
	{
		callGateLoc.s_pop();

		jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
			CjvxAudioNode_genpcg::properties_parameters.samplerate.globalIdx, 
			CjvxAudioNode_genpcg::properties_parameters.samplerate.category);
		jvx::propertyDetail::CjvxTranferDetail trans(true);

		res = theLanes[i][selIdLane[i]].theEncProps->set_property(
			callGateLoc, 
			jPRG(&CjvxAudioNode_genpcg::properties_parameters.samplerate.value,
			1, CjvxAudioNode_genpcg::properties_parameters.samplerate.format),
			ident, trans);
		//assert(res == JVX_NO_ERROR);

	}

	for (i = 0; i < JVX_NUM_LANES; i++)
	{
		callGateLoc.s_pop();

		jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
			CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.globalIdx, 
			CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.category);
		jvx::propertyDetail::CjvxTranferDetail trans(true);

		res = theLanes[i][selIdLane[i]].theEncProps->set_property(
			callGateLoc, 
			jPRG(&CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.value,
			1, CjvxAudioNode_genpcg::properties_parameters.numberinputchannels.format),
			ident, trans);
		//assert(res == JVX_NO_ERROR);

	}
	for (i = 0; i < JVX_NUM_LANES; i++)
	{
		callGateLoc.s_pop();

		jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
			CjvxAudioNode_genpcg::properties_parameters.format.globalIdx, 
			CjvxAudioNode_genpcg::properties_parameters.format.category);
		jvx::propertyDetail::CjvxTranferDetail trans(true);

		res = theLanes[i][selIdLane[i]].theEncProps->set_property(
			callGateLoc, 
			jPRG(&CjvxAudioNode_genpcg::properties_parameters.format.value,
			1, CjvxAudioNode_genpcg::properties_parameters.format.format),
			ident, trans);
		//assert(res == JVX_NO_ERROR);

		for (i = 0; i < JVX_NUM_LANES; i++)
		{
			callGateLoc.s_pop();

			jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(
				CjvxAudioNode_genpcg::properties_parameters.format.globalIdx, 
				CjvxAudioNode_genpcg::properties_parameters.format.category);
			jvx::propertyDetail::CjvxTranferDetail trans(true);

			res = theLanes[i][selIdLane[i]].theDecProps->set_property(
				callGateLoc, 
				jPRG(&CjvxAudioNode_genpcg::properties_parameters.format.value,
				1, CjvxAudioNode_genpcg::properties_parameters.format.format),
				ident, trans);
			//assert(res == JVX_NO_ERROR);
		}
	}
	return JVX_NO_ERROR;
}
