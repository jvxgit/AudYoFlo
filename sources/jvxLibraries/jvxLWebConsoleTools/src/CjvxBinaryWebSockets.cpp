#include "CjvxBinaryWebSockets.h"
#include "CjvxWebControl_fe.h"
#include "CjvxWebControl_fe_private.h"

CjvxBinaryWebSockets::CjvxBinaryWebSockets()
{
	wsUniqueId = 1;
	JVX_GET_TICKCOUNT_US_SETREF(&tStamp);
}

void
CjvxBinaryWebSockets::register_binary_socket_main_loop(jvxWebContext* ctxt)

{
	jvxApiString uri;
	jvxApiString query;
	jvxApiString local_uri;
	jvxApiString url_origin;
	jvxApiString user;

	// Allow a single binary socket - the old version
	if (webSocketPeriodic.theCtxt.context_conn == NULL)
	{
		webSocketPeriodic.theCtxt = *ctxt;

		// Get the originating IP address 
		hostRef->request_event_information(webSocketPeriodic.theCtxt.context_server, webSocketPeriodic.theCtxt.context_conn,
			&uri, &query, &local_uri, &url_origin, &user);
		webSocketPeriodic.theCtxt.uri = uri.std_str();
		webSocketPeriodic.query = query.std_str();
		webSocketPeriodic.local_uri = local_uri.std_str();
		webSocketPeriodic.url_origin = url_origin.std_str();
		webSocketPeriodic.user = user.std_str();
		JVX_START_LOCK_LOG_REF(hostRef, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
		log << "Web socket connection from " << url_origin.std_str() << ":" << user.std_str() << std::endl;
		JVX_STOP_LOCK_LOG_REF(hostRef);		
	}
	else
	{
		std::cout << "Failed to activate web socket connection: binary web socket already in use!" << std::endl;
		std::cout << "Sending immediate terminate signal!" << std::endl;
		hostRef->myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, 0x8, NULL, 0);
	}
}

jvxErrorType 
CjvxBinaryWebSockets::unregister_binary_socket_main_loop(jvxHandle* refCtxt)
{
	// ========================================================
	// Web socket disconnect
	// ========================================================
	std::cout << "Web socket disconnect" << std::endl;
	if (webSocketPeriodic.theCtxt.context_conn)
	{
		if (webSocketPeriodic.theCtxt.context_conn == refCtxt)
		{
			webSocketPeriodic.theCtxt.reset();
			webSocketPeriodic.query = "";
			webSocketPeriodic.local_uri = "";
			webSocketPeriodic.url_origin = "";
			webSocketPeriodic.user = "";

			// Cancel timout message
			jvxErrorType res = hostRef->evLop->event_clear(webSocketPeriodic.wsMessId, NULL, NULL);
			assert(res == JVX_NO_ERROR);
			webSocketPeriodic.wsMessId = JVX_SIZE_UNSELECTED;

			auto elm = lstUpdateProperties.begin();
			for (; elm != lstUpdateProperties.end(); elm++)
			{
				deactivate_property(elm->second);

				if (elm->second.prio == JVX_PROP_CONNECTION_TYPE_HIGH_PRIO)
				{
					disconnect_udp_port(elm->second);
				}
			}
			lstUpdateProperties.clear();
			webSocketPeriodic.ping_cnt_close = 0;
			webSocketPeriodic.ping_cnt_close_failed = 0;
			webSocketPeriodic.ping_cnt_trigger = 0;
			webSocketPeriodic.current_ping_count = 0;
			return JVX_NO_ERROR;
		}
		std::cout << "Failed to deactivate web socket connection: web socket is not in use!" << std::endl;
		return JVX_ERROR_ELEMENT_NOT_FOUND;	
	}
	return JVX_ERROR_INVALID_ARGUMENT;	
}

void 
CjvxBinaryWebSockets::force_stop_properties(jvxHandle* hdl)
{
	// We may end up here if the web console app stops itself:
	// The message queue is already down but the last websocket message arrived. 
	// In that case we need to remove all properties from the observer list "by hand"
	// This only affects the additional udp update routines.
	if (hdl == webSocketPeriodic.theCtxt.context_conn)
	{
		auto elm = lstUpdateProperties.begin();
		for (; elm != lstUpdateProperties.end(); elm++)
		{
			deactivate_property(elm->second);

			if (elm->second.prio == JVX_PROP_CONNECTION_TYPE_HIGH_PRIO)
			{
				disconnect_udp_port(elm->second);
			}
		}
		lstUpdateProperties.clear();
	}
}

jvxErrorType
CjvxBinaryWebSockets::deactivate_property(onePropertyWebSocketDefinition& defOneProperty)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;
	jvxPropertyStreamHeader* preparedHeader = NULL;

	if (defOneProperty.runtime.transferState == JVX_STATE_ACTIVE)
	{
		// We need to add a little bit of protection since the high prio properties are transfered
		// in a high speed thread but only if active
		defOneProperty.runtime.transferState = JVX_STATE_NONE;

		// Deactivate the property
		if (defOneProperty.runtime.theTriple.theProps)
		{

			switch (defOneProperty.cond_update)
			{
			case JVX_PROP_STREAM_UPDATE_TIMEOUT:
			case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
			case JVX_PROP_STREAM_UPDATE_ON_REQUEST:
				switch (defOneProperty.runtime.descr.decTp)
				{
				case JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER:

					res = deallocateMultichannelCircBuf_propstream(defOneProperty);
					break;
				case JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER:
					// todo: allocate and install switch buffer
					assert(0);
					break;
				default:
					deallocateLinearBuffer_propstream(defOneProperty);
					break;
				}
				break;
			case JVX_PROP_STREAM_UPDATE_ON_CHANGE_REPORT_ORIGIN:
				JVX_DSP_SAFE_DELETE_FIELD(defOneProperty.runtime.allocatedRawBuffer);
				defOneProperty.runtime.allocatedRawBuffer = NULL;
				break;
			default:
				assert(0);
			}

			jvx_returnReferencePropertiesObject(hostRef->myHostRef, &defOneProperty.runtime.theTriple, defOneProperty.cpId);
		}
		jvx_initPropertyReferenceTriple(&defOneProperty.runtime.theTriple);
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return res;
}

jvxErrorType
CjvxBinaryWebSockets::allocateLinearBuffer_propstream(onePropertyWebSocketDefinition& defOneProperty)
{
	jvxPropertyStreamHeader* preparedHeader = NULL;
	if (defOneProperty.runtime.descr.num == 0)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	switch (defOneProperty.runtime.descr.format)
	{
	case JVX_DATAFORMAT_DATA:
	case JVX_DATAFORMAT_8BIT:
	case JVX_DATAFORMAT_16BIT_LE:
	case JVX_DATAFORMAT_32BIT_LE:
	case JVX_DATAFORMAT_64BIT_LE:
	case JVX_DATAFORMAT_U16BIT_LE:
	case JVX_DATAFORMAT_U32BIT_LE:
	case JVX_DATAFORMAT_U64BIT_LE:
		break;
	default:
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	std::cout << "-- Property with descriptor <" << defOneProperty.runtime.descr.descriptor.std_str() << "> --" << std::flush;
	switch (defOneProperty.cond_update)
	{
	case JVX_PROP_STREAM_UPDATE_TIMEOUT:
		std::cout << "Update on timeout, update period = " << defOneProperty.param_cond_update << "." << std::endl;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
		std::cout << "Update on change." << std::flush;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_REQUEST:
		std::cout << "Update on request." << std::flush;
		break;
	}

	defOneProperty.runtime.szRawBuffer = jvxDataFormat_getsize(defOneProperty.runtime.descr.format) * defOneProperty.runtime.descr.num;
	defOneProperty.runtime.szRawBuffer += sizeof(jvxPropertyStreamHeader);

	std::cout << "--> Allocating linear buffer, length = " << defOneProperty.runtime.descr.num <<
		", format = " << jvxDataFormat_txt(defOneProperty.runtime.descr.format) <<
		"." << std::endl;

	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(defOneProperty.runtime.allocatedRawBuffer, jvxByte, defOneProperty.runtime.szRawBuffer);
	defOneProperty.runtime.referencedPayloadBuffer = defOneProperty.runtime.allocatedRawBuffer + sizeof(jvxPropertyStreamHeader);
	preparedHeader = (jvxPropertyStreamHeader*)defOneProperty.runtime.allocatedRawBuffer;

	/*
		Set streaming parameters
	*/
	preparedHeader->raw_header.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
	preparedHeader->raw_header.loc_header.purpose = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER | JVX_PS_SEND_LINEARFIELD;
	preparedHeader->raw_header.loc_header.paketsize = JVX_SIZE_INT32(defOneProperty.runtime.szRawBuffer);
	preparedHeader->raw_header.stream_id = JVX_SIZE_UINT16(defOneProperty.uniqueId);
	preparedHeader->raw_header.packet_tstamp = (jvxUInt16)-1;
	preparedHeader->property_num_elements = JVX_SIZE_UINT32(defOneProperty.runtime.descr.num);
	preparedHeader->property_offset = JVX_SIZE_UINT16(defOneProperty.offset);
	preparedHeader->property_format = defOneProperty.runtime.descr.format;
	preparedHeader->property_type = JVX_SIZE_UINT8(defOneProperty.htTp);
	preparedHeader->param0 = JVX_SIZE_UINT32(defOneProperty.param0);
	preparedHeader->param1 = JVX_SIZE_UINT32(defOneProperty.param1);
	preparedHeader->requires_flow_response = 1;
	preparedHeader->reserved0 = 0;
	preparedHeader->reserved1 = 0;

	defOneProperty.runtime.cntTicks = 0;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxBinaryWebSockets::deallocateLinearBuffer_propstream(onePropertyWebSocketDefinition& defOneProperty)
{
	std::cout << "-- Property with descriptor <" << defOneProperty.runtime.descr.descriptor.std_str() << "> --" << std::flush;
	switch (defOneProperty.cond_update)
	{
	case JVX_PROP_STREAM_UPDATE_TIMEOUT:
		std::cout << "Update on timeout, update period = " << defOneProperty.param_cond_update << "." << std::endl;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
		std::cout << "Update on change." << std::flush;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_REQUEST:
		std::cout << "Update on request." << std::flush;
		break;
	}
	std::cout << "--> Deallocating linear buffer, length = " << defOneProperty.runtime.descr.num <<
		", format = " << jvxDataFormat_txt(defOneProperty.runtime.descr.format) <<
		"." << std::endl;

	JVX_DSP_SAFE_DELETE_FIELD(defOneProperty.runtime.allocatedRawBuffer);
	defOneProperty.runtime.referencedPayloadBuffer = NULL;
	defOneProperty.runtime.szRawBuffer = 0;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxBinaryWebSockets::allocateMultichannelCircBuf_propstream(onePropertyWebSocketDefinition& defOneProperty)
{
	jvxCallManagerProperties callGate;
	jvxPropertyStreamHeader* preparedHeader = NULL;
	if (defOneProperty.runtime.descr.num == 0)
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	switch (defOneProperty.runtime.descr.format)
	{
	case JVX_DATAFORMAT_DATA:
	case JVX_DATAFORMAT_16BIT_LE:
	case JVX_DATAFORMAT_32BIT_LE:
	case JVX_DATAFORMAT_64BIT_LE:
	case JVX_DATAFORMAT_8BIT:
	case JVX_DATAFORMAT_U16BIT_LE:
	case JVX_DATAFORMAT_U32BIT_LE:
	case JVX_DATAFORMAT_U64BIT_LE:
	case JVX_DATAFORMAT_U8BIT:
		break;
	default:
		return JVX_ERROR_INVALID_ARGUMENT;
	}

	/*
	if (
		!(
		(defOneProperty.runtime.descr.format >= ) &&
			(defOneProperty.runtime.descr.format <= JVX_DATAFORMAT_8BIT)))
	{
		return JVX_ERROR_INVALID_ARGUMENT;
	}
	*/

	jvxSize szFld_y = JVX_SIZE_UNSELECTED;
	jvxSize szFld_x = JVX_SIZE_UNSELECTED;

	std::cout << "-- Property with descriptor <" << defOneProperty.runtime.descr.descriptor.std_str() << "> --" << std::flush;
	switch (defOneProperty.cond_update)
	{
	case JVX_PROP_STREAM_UPDATE_TIMEOUT:
		std::cout << "Update on timeout, update period = " << defOneProperty.param_cond_update << "." << std::endl;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
		std::cout << "Update on change." << std::flush;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_REQUEST:
		std::cout << "Update on request." << std::flush;
		break;
	}

	if (JVX_CHECK_SIZE_UNSELECTED(defOneProperty.param1))
	{
		defOneProperty.runtime.theTriple.theProps->get_property_extended_info(callGate,
			&szFld_y, JVX_PROPERTY_INFO_RECOMMENDED_SIZE_Y,
			jPAGID(defOneProperty.runtime.descr.globalIdx, defOneProperty.runtime.descr.category));
	}
	if (JVX_CHECK_SIZE_UNSELECTED(defOneProperty.param0))
	{
		defOneProperty.runtime.theTriple.theProps->get_property_extended_info(callGate,
			&szFld_x, JVX_PROPERTY_INFO_RECOMMENDED_SIZE_X,
			jPAGID(defOneProperty.runtime.descr.globalIdx, defOneProperty.runtime.descr.category));
	}

	if (JVX_CHECK_SIZE_SELECTED(defOneProperty.param1) && JVX_CHECK_SIZE_SELECTED(defOneProperty.param0))
	{
		std::cout << "--> Allocating multichannel circular buffer, length = " << defOneProperty.param1 <<
			", number channels = " << defOneProperty.param0 <<
			", format = " << jvxDataFormat_txt(defOneProperty.runtime.descr.format) <<
			", subformat = " << jvxDataFormatGroup_txt(JVX_DATAFORMAT_GROUP_GENERIC_NON_INTERLEAVED) << std::flush;
		if (defOneProperty.num_emit_min)
		{
			std::cout << ", minimum length constraint = " << defOneProperty.num_emit_min << std::flush;
		}
		else
		{
			std::cout << ", maximum length constraint = " << defOneProperty.num_emit_max << std::flush;

		}
		std::cout << "." << std::endl;

		defOneProperty.runtime.specbuffer = jvx_allocate1DCircExternalBuffer(
			defOneProperty.param1, defOneProperty.param0,
			defOneProperty.runtime.descr.format,
			JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED,
			jvx_static_lock, jvx_static_try_lock, jvx_static_unlock,
			&defOneProperty.runtime.specbuffer_sz);
		defOneProperty.runtime.specbuffer_valid = true;

		// Specify the property parameters for association
		jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(defOneProperty.runtime.specbuffer,
			&defOneProperty.runtime.specbuffer_valid,
			defOneProperty.runtime.descr.format);
		jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(defOneProperty.runtime.descr.globalIdx,
			defOneProperty.runtime.descr.category);

		defOneProperty.runtime.theTriple.theProps->install_property_reference(callGate,
			ptrRaw, ident);

		defOneProperty.runtime.szRawBuffer = jvxDataFormat_getsize(defOneProperty.runtime.descr.format) * defOneProperty.param0 * defOneProperty.param1;
		defOneProperty.runtime.szRawBuffer += sizeof(jvxPropertyStreamHeader);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(defOneProperty.runtime.allocatedRawBuffer, jvxByte, defOneProperty.runtime.szRawBuffer);
		defOneProperty.runtime.referencedPayloadBuffer = defOneProperty.runtime.allocatedRawBuffer + sizeof(jvxPropertyStreamHeader);
		preparedHeader = (jvxPropertyStreamHeader*)defOneProperty.runtime.allocatedRawBuffer;

		/*
			Set streaming parameters
		*/
		preparedHeader->raw_header.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
		preparedHeader->raw_header.loc_header.purpose = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER | JVX_PS_SEND_LINEARFIELD;
		preparedHeader->raw_header.loc_header.paketsize = JVX_SIZE_INT32(defOneProperty.runtime.szRawBuffer);
		preparedHeader->raw_header.stream_id = JVX_SIZE_UINT16(defOneProperty.uniqueId);
		preparedHeader->raw_header.packet_tstamp = (jvxUInt16)-1;
		preparedHeader->property_num_elements = JVX_SIZE_UINT32(defOneProperty.runtime.descr.num);
		preparedHeader->property_offset = JVX_SIZE_UINT16(defOneProperty.offset);
		preparedHeader->property_format = defOneProperty.runtime.descr.format;
		preparedHeader->property_type = JVX_SIZE_UINT8(defOneProperty.htTp);
		preparedHeader->param0 = JVX_SIZE_UINT32(defOneProperty.param0);
		preparedHeader->param1 = JVX_SIZE_UINT32(defOneProperty.param1);
		preparedHeader->requires_flow_response = 1;
		preparedHeader->reserved0 = 0;
		preparedHeader->reserved1 = 0;

		defOneProperty.runtime.cntTicks = 0;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxErrorType
CjvxBinaryWebSockets::deallocateMultichannelCircBuf_propstream(onePropertyWebSocketDefinition& defOneProperty)
{
	std::cout << "-- Property with descriptor <" << defOneProperty.runtime.descr.descriptor.std_str() << "> --" << std::flush;
	switch (defOneProperty.cond_update)
	{
	case JVX_PROP_STREAM_UPDATE_TIMEOUT:
		std::cout << "Update on timeout, update period = " << defOneProperty.param_cond_update << "." << std::endl;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
		std::cout << "Update on change." << std::flush;
		break;
	case JVX_PROP_STREAM_UPDATE_ON_REQUEST:
		std::cout << "Update on request." << std::flush;
		break;
	}
	std::cout << "--> Deallocating multichannel circular buffer, length = " << defOneProperty.param1 <<
		", number channels = " << defOneProperty.param0 <<
		", format = " << jvxDataFormat_txt(defOneProperty.runtime.descr.format) <<
		", subformat = " << jvxDataFormatGroup_txt(JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED) <<
		"." << std::endl;

	jvxCallManagerProperties callGate;
	assert(defOneProperty.runtime.specbuffer);

	jvx::propertyRawPointerType::CjvxRawPointerTypeExternal<jvxExternalBuffer> ptrRaw(defOneProperty.runtime.specbuffer,
		&defOneProperty.runtime.specbuffer_valid,
		defOneProperty.runtime.descr.format);
	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(defOneProperty.runtime.descr.globalIdx,
		defOneProperty.runtime.descr.category);

	defOneProperty.runtime.theTriple.theProps->uninstall_property_reference(callGate, ptrRaw, ident);
	jvx_deallocateExternalBuffer(defOneProperty.runtime.specbuffer);
	defOneProperty.runtime.specbuffer = NULL;
	defOneProperty.runtime.specbuffer_sz = 0;
	defOneProperty.runtime.specbuffer_valid = false;
	return deallocateLinearBuffer_propstream(defOneProperty);
}

jvxErrorType
CjvxBinaryWebSockets::try_activate_property(onePropertyWebSocketDefinition& defOneProperty)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxState stat = JVX_STATE_NONE;
	std::string collectStr;
	jvxPropertyStreamHeader* preparedHeader = NULL;
	jvxCallManagerProperties callGate;
	if (defOneProperty.runtime.transferState == JVX_STATE_NONE)
	{
		if (!defOneProperty.runtime.serious_fail)
		{
			hostRef->myHostRef->state_selected_component(defOneProperty.cpId, &stat);
			if (stat & defOneProperty.state_active)
			{
				// Activate the property
				jvx_initPropertyReferenceTriple(&defOneProperty.runtime.theTriple);
				jvx_getReferencePropertiesObject(hostRef->myHostRef, &defOneProperty.runtime.theTriple, defOneProperty.cpId);
				if (defOneProperty.runtime.theTriple.theProps)
				{
					jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(defOneProperty.propertyName.c_str());

					res = jvx_getPropertyDescription(defOneProperty.runtime.theTriple.theProps, defOneProperty.runtime.descr, ident, callGate);
					if (res == JVX_NO_ERROR)
					{
						switch (defOneProperty.cond_update)
						{
						case JVX_PROP_STREAM_UPDATE_TIMEOUT:
						case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
						case JVX_PROP_STREAM_UPDATE_ON_REQUEST:
							if (
								((defOneProperty.runtime.descr.num == defOneProperty.numElms_spec) || (JVX_CHECK_SIZE_UNSELECTED(defOneProperty.numElms_spec))) &&
								((defOneProperty.runtime.descr.format == defOneProperty.format_spec) || (defOneProperty.format_spec == JVX_DATAFORMAT_NONE)) &&
								(defOneProperty.runtime.descr.decTp == defOneProperty.htTp))
							{
								switch (defOneProperty.runtime.descr.decTp)
								{
								case JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER:
									res = allocateMultichannelCircBuf_propstream(defOneProperty);
									break;

								case JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER:
									// todo: allocate and install switch buffer
									res = JVX_ERROR_UNSUPPORTED;
									break;
								default:

									res = allocateLinearBuffer_propstream(defOneProperty);
									break;
								}
							}
							else
							{
								defOneProperty.runtime.serious_fail = true;
								if (JVX_CHECK_SIZE_SELECTED(defOneProperty.numElms_spec))
								{
									if (defOneProperty.runtime.descr.num != defOneProperty.numElms_spec)
									{
										std::cout << "Failed to activate property " << defOneProperty.propertyName << " in component " << jvxComponentIdentification_txt(defOneProperty.cpId)
											<< ": number of element mismatch, local property provides  <" << defOneProperty.runtime.descr.num
											<< "> elements but external request asked for <" << defOneProperty.numElms_spec << "> elements." << std::endl;
									}
								}
								if (defOneProperty.format_spec != JVX_DATAFORMAT_NONE)
								{
									if (defOneProperty.runtime.descr.format != defOneProperty.format_spec)
									{
										std::cout << "Failed to activate property " << defOneProperty.propertyName << " in component " << jvxComponentIdentification_txt(defOneProperty.cpId)
											<< ": format of element mismatch, local property provides <" << jvxDataFormat_txt(defOneProperty.runtime.descr.format)
											<< "> but external request asked for <" << jvxDataFormat_txt(defOneProperty.format_spec) << ">" << std::endl;
									}
								}
								if (defOneProperty.runtime.descr.decTp != defOneProperty.htTp)
								{
									std::cout << "Failed to activate property " << defOneProperty.propertyName << " in component " << jvxComponentIdentification_txt(defOneProperty.cpId)
										<< ": decoder hint type mismatch, local property provides <" << jvxPropertyDecoderHintType_txt(defOneProperty.runtime.descr.decTp)
										<< "> but external request asked for <" << jvxPropertyDecoderHintType_txt(defOneProperty.htTp) << ">" << std::endl;
								}

								res = JVX_ERROR_INVALID_SETTING;
							}
							break;
						case JVX_PROP_STREAM_UPDATE_ON_CHANGE_REPORT_ORIGIN:
							collectStr = jvxComponentIdentification_txt(defOneProperty.cpId);
							collectStr += defOneProperty.propertyName;
							defOneProperty.runtime.szRawBuffer = collectStr.size();
							defOneProperty.runtime.szRawBuffer += sizeof(jvxPropertyStreamHeader);
							JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(defOneProperty.runtime.allocatedRawBuffer, jvxByte, defOneProperty.runtime.szRawBuffer);
							defOneProperty.runtime.referencedPayloadBuffer = defOneProperty.runtime.allocatedRawBuffer + sizeof(jvxPropertyStreamHeader);
							memcpy(defOneProperty.runtime.referencedPayloadBuffer, collectStr.c_str(), collectStr.size());
							preparedHeader = (jvxPropertyStreamHeader*)defOneProperty.runtime.allocatedRawBuffer;

							/*
								Set streaming parameters
							 */
							preparedHeader->raw_header.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
							preparedHeader->raw_header.loc_header.purpose = JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_TRIGGER | JVX_PS_SEND_INFORM_CHANGE;
							preparedHeader->raw_header.loc_header.paketsize = JVX_SIZE_INT32(defOneProperty.runtime.szRawBuffer);
							preparedHeader->raw_header.stream_id = JVX_SIZE_UINT16(defOneProperty.uniqueId);
							preparedHeader->raw_header.packet_tstamp = (jvxUInt16)-1;
							preparedHeader->property_num_elements = JVX_SIZE_UINT32(collectStr.size());
							preparedHeader->property_offset = JVX_SIZE_UINT16(0);
							preparedHeader->property_format = JVX_DATAFORMAT_STRING;
							preparedHeader->property_type = JVX_SIZE_UINT8(JVX_PROPERTY_DECODER_NONE);
							preparedHeader->param0 = JVX_SIZE_UINT32(defOneProperty.param0);
							preparedHeader->param1 = JVX_SIZE_UINT32(defOneProperty.param1);
							preparedHeader->requires_flow_response = 1;
							preparedHeader->reserved0 = 0;
							preparedHeader->reserved1 = 0;

							defOneProperty.runtime.cntTicks = 0;
							break;
						default:
							assert(0);
						}

						if (res == JVX_NO_ERROR)
						{
							if (defOneProperty.prio == JVX_PROP_CONNECTION_TYPE_HIGH_PRIO)
							{
								defOneProperty.cond_update = JVX_PROP_STREAM_UPDATE_ON_CHANGE; // high priority only on change!
							}
						}

						if (res != JVX_NO_ERROR)
						{
							jvx_returnReferencePropertiesObject(hostRef->myHostRef, &defOneProperty.runtime.theTriple, defOneProperty.cpId);
							jvx_initPropertyDescription(defOneProperty.runtime.descr);
							jvx_initPropertyReferenceTriple(&defOneProperty.runtime.theTriple);
						}
						else
						{
							// We need to add a little bit of protection since the high prio properties are transfered
							// in a high speed thread but only if active
							defOneProperty.runtime.transferState = JVX_STATE_ACTIVE;
						}
					} // if (res == JVX_NO_ERROR)
					else
					{
						defOneProperty.runtime.serious_fail = true;
						std::cout << "Failed to activate property " << defOneProperty.propertyName << " in component " << jvxComponentIdentification_txt(defOneProperty.cpId)
							<< ": failed to access property within component, error description: " << jvxErrorType_descr(res) << std::endl;
					}
				} // if (defOneProperty.runtime.theTriple.theProps)
				else
				{
					defOneProperty.runtime.serious_fail = true;
					std::cout << "Failed to activate property " << defOneProperty.propertyName << " in component " << jvxComponentIdentification_txt(defOneProperty.cpId)
						<< ": failed to obtain property handle reference." << std::endl;
				}
			}// if (stat & defOneProperty.state_active)
		}//if (!defOneProperty.runtime.serious_fail)
	}// if (defOneProperty.runtime.transferState == JVX_STATE_NONE)
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return res;
}

jvxPropertyTransferType
CjvxBinaryWebSockets::check_transfer_property(onePropertyWebSocketDefinition& defOneProperty, jvxPropertyStreamCondUpdate theReason,
	const jvxComponentIdentification& idCp, jvxPropertyCategoryType cat, jvxSize propId)
{
	jvxPropertyTransferType doTransfer = JVX_PROP_TRANSFER_NONE;

	if (defOneProperty.requiresFlowControl)
	{
		if (defOneProperty.state_transfer != JVX_PROPERTY_WEBSOCKET_STATE_NONE)
		{
			jvxBool fail = true;
			if (defOneProperty.cond_update == JVX_PROP_STREAM_UPDATE_ON_CHANGE)
			{
				if (defOneProperty.param_cond_update > 0)
				{
					jvxTick tick = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
					jvxData tickNow = (jvxData)tick * 0.001;
					if (tickNow - defOneProperty.runtime.last_send_msec > defOneProperty.param_cond_update)
					{
						defOneProperty.state_transfer = JVX_PROPERTY_WEBSOCKET_STATE_NONE;
						fail = false;
						std::cout << "Restarting flow control for property <" << defOneProperty.propertyName << "> due to expired response timeout, " <<
							" missing response for id = " << defOneProperty.runtime.latest_tstamp << "." << std::endl;
					}
				}
			}
			if (fail)
			{
#ifdef JVX_VERBOSE_FLOW_CONTROL
				std::cout << "F" << std::endl;
#endif
				return doTransfer;
			}
		}
	}

	switch (theReason)
	{
	case JVX_PROP_STREAM_UPDATE_TIMEOUT:
		if (defOneProperty.cond_update == theReason)
		{
			if (defOneProperty.runtime.cntTicks == 0)
			{
				doTransfer = JVX_PROP_TRANSFER_CONTENT;
			}
			defOneProperty.runtime.cntTicks = (defOneProperty.runtime.cntTicks + 1) % defOneProperty.param_cond_update;
		}
		break;
	case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
		if (defOneProperty.cond_update == theReason)
		{
			if (
				(defOneProperty.cpId == idCp) &&
				(defOneProperty.runtime.descr.globalIdx == propId) &&
				(defOneProperty.runtime.descr.category == cat))
			{
				doTransfer = JVX_PROP_TRANSFER_CONTENT;
			}
		}
		break;
	case JVX_PROP_STREAM_UPDATE_ON_CHANGE_REPORT_ORIGIN:
		if (defOneProperty.cond_update == theReason)
		{
			if (
				(defOneProperty.cpId == idCp) &&
				(defOneProperty.runtime.descr.globalIdx == propId) &&
				(defOneProperty.runtime.descr.category == cat))
			{
				doTransfer = JVX_PROP_TRANSFER_SELF;
			}
		}
		break;
	case JVX_PROP_STREAM_UPDATE_ON_REQUEST:
		break;
	}
	return doTransfer;
}

jvxErrorType
CjvxBinaryWebSockets::transfer_activated_property(onePropertyWebSocketDefinition& defOneProperty,
	jvxSize& numBytesTransferred)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxPropertyStreamHeader* preparedHeader = NULL;
	jvxSize num_avail = 0;
	jvxSize idxread = 0;

	jvxByte* copyFrom = NULL, * copyFrom0 = NULL;
	jvxByte* copyTo = NULL, * copyTo0 = NULL;
	jvxSize i;
	jvxSize szOneSample = 0;
	jvxSize szOneChannel_fh = 0;
	jvxSize szOneChannel_full = 0;
	jvxSize szAllChannels_fh = 0;
	jvxSize szAllChannels_full = 0;
	numBytesTransferred = 0;
	jvxCallManagerProperties callGate;

	jvxPropertyStreamHeader* theHeader = NULL;
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE resM = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;

	// We may only transfer data via websocket if normal priority.
	// 
	if (defOneProperty.runtime.transferState == JVX_STATE_ACTIVE)
	{
		// Deactivate the property
		if (defOneProperty.runtime.theTriple.theProps)
		{
			switch (defOneProperty.cond_update)
			{
			case JVX_PROP_STREAM_UPDATE_TIMEOUT:
			case JVX_PROP_STREAM_UPDATE_ON_CHANGE:
				switch (defOneProperty.runtime.descr.decTp)
				{
				case JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER:
					// todo: allocate and install circular buffer
					preparedHeader = (jvxPropertyStreamHeader*)defOneProperty.runtime.allocatedRawBuffer;

					// std::cout << "------" << "Circular buffer pointer on ws send: " << defOneProperty.runtime.specbuffer << std::endl;
					defOneProperty.runtime.specbuffer->safe_access.lockf(defOneProperty.runtime.specbuffer->safe_access.priv);
					num_avail = defOneProperty.runtime.specbuffer->fill_height;
					idxread = defOneProperty.runtime.specbuffer->idx_read;
					defOneProperty.runtime.specbuffer->safe_access.unlockf(defOneProperty.runtime.specbuffer->safe_access.priv);

					// std::cout << __FUNCTION__ << "Circular buffer property via socket: " << defOneProperty.runtime.specbuffer << std::endl;

					if (num_avail > defOneProperty.num_emit_min)
					{
						jvxBool is_last_emit = false;
						while (!is_last_emit)
						{
							jvxSize num_transfer = num_avail;

							if (defOneProperty.num_emit_max > 0)
							{
								num_transfer = JVX_MIN(num_transfer, defOneProperty.num_emit_max);
							}

							num_avail -= num_transfer;
							if (num_avail <= defOneProperty.num_emit_min)
							{
								is_last_emit = true;
							}

							/*
							std::cout << "------" << "Sending " << num_transfer << " values" << std::flush;
							if (is_last_emit)
							{
								std::cout << " (last emit)." << std::flush;
							}
							else
							{
								std::cout << "." << std::flush;
							}
							std::cout << std::endl;
							*/

							szOneSample = jvxDataFormat_getsize(defOneProperty.runtime.descr.format);
							szOneChannel_fh = szOneSample * num_transfer;
							szOneChannel_full = szOneSample * defOneProperty.param1;
							szAllChannels_fh = szOneChannel_fh * defOneProperty.param0;
							szAllChannels_full = szOneChannel_full * defOneProperty.param0;

							preparedHeader->raw_header.loc_header.paketsize = JVX_SIZE_UINT32(sizeof(jvxPropertyStreamHeader) + szAllChannels_fh);
							preparedHeader->property_num_elements = JVX_SIZE_UINT32(num_transfer);
							preparedHeader->requires_flow_response = 1;

							jvxSize ll1 = defOneProperty.runtime.specbuffer->length - idxread;
							ll1 = JVX_MIN(ll1, num_transfer);
							jvxSize ll2 = num_transfer - ll1;
							// jvxUInt32* cpt = (jvxUInt32*)(defOneProperty.runtime.allocatedRawBuffer + sizeof(jvxPropertyStreamHeader));

							copyTo0 = defOneProperty.runtime.allocatedRawBuffer + sizeof(jvxPropertyStreamHeader);

							for (i = 0; i < defOneProperty.param0; i++)
							{
								// Compute pointer in buffers

								// Target buffers will be of length fHeight
								copyTo = copyTo0 + i * szOneChannel_fh;

								// Source buffers are of full size
								copyFrom0 = defOneProperty.runtime.specbuffer->ptrFld + i * szOneChannel_full;

								if (ll1)
								{
									copyFrom = copyFrom0 + szOneSample * idxread;
									memcpy(copyTo, copyFrom, szOneSample * ll1);
									copyTo += szOneSample * ll1;
								}
								if (ll2)
								{
									copyFrom = copyFrom0;
									memcpy(copyTo, copyFrom, szOneSample * ll2);
									copyTo += szOneSample * ll2;
								}
							}
							/*
							std::cout << defOneProperty.propertyName << " -- First value = " << *cpt << std::endl;
							cpt = (jvxUInt32*)copyTo - 8;
							std::cout << defOneProperty.propertyName << " -- Last value = " << *cpt << std::endl;
							*/
							// Update circular buffer state
							defOneProperty.runtime.specbuffer->safe_access.lockf(defOneProperty.runtime.specbuffer->safe_access.priv);
							defOneProperty.runtime.specbuffer->fill_height -= num_transfer;
							defOneProperty.runtime.specbuffer->idx_read = (defOneProperty.runtime.specbuffer->idx_read +
								num_transfer) % defOneProperty.runtime.specbuffer->length;
							idxread = defOneProperty.runtime.specbuffer->idx_read; // Local update of read index. Do NOT update fill height to prevent endless send at very high speeds
							defOneProperty.runtime.specbuffer->safe_access.unlockf(defOneProperty.runtime.specbuffer->safe_access.priv);

							jvxPropertyStreamHeader* theHeader = (jvxPropertyStreamHeader*)defOneProperty.runtime.allocatedRawBuffer;
							theHeader->raw_header.packet_tstamp = defOneProperty.tStamp++;
							defOneProperty.streamPropertyInTransferState = 0;
							defOneProperty.state_transfer = JVX_PROPERTY_WEBSOCKET_STATE_IN_TRANSFER;

							jvxTick tick = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
							defOneProperty.runtime.last_send_msec = (jvxData)tick * 0.001;
							defOneProperty.runtime.latest_tstamp = theHeader->raw_header.packet_tstamp;

							if (is_last_emit)
							{
								preparedHeader->requires_flow_response = 1;
							}
							else
							{
								preparedHeader->requires_flow_response = 0;
							}

#ifdef JVX_VERBOSE_FLOW_CONTROL
							std::cout << "S[" << theHeader->raw_header.stream_id << "--" << theHeader->raw_header.packet_tstamp << "]" << std::endl;
#endif
							if (defOneProperty.prio == JVX_PROP_CONNECTION_TYPE_NORMAL_PRIO)
							{
								resL = hostRef->myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
									(const char*)theHeader, theHeader->raw_header.loc_header.paketsize);
							}
							else
							{
								resL = defOneProperty.runtime.prio_high.udpSocket->send_packet((const char*)theHeader, theHeader->raw_header.loc_header.paketsize);
								if (resL != JVX_NO_ERROR)
								{
									std::cout << __FUNCTION__ << ": Error: UDP Send Error, error reason: " << jvxErrorType_txt(resL) << "." << std::endl;
								}
							}
							numBytesTransferred = theHeader->raw_header.loc_header.paketsize;
						}

					}

					break;
				case JVX_PROPERTY_DECODER_MULTI_CHANNEL_SWITCH_BUFFER:
					// todo: allocate and install switch buffer
					res = JVX_ERROR_UNSUPPORTED;
					break;
				default:

					if (JVX_CHECK_SIZE_UNSELECTED(defOneProperty.numElms_spec))
					{
						jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
						jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(defOneProperty.propertyName.c_str());
						if (defOneProperty.runtime.theTriple.theProps)
						{
							resL = jvx_getPropertyDescription(defOneProperty.runtime.theTriple.theProps, theDescr, ident, callGate);
						}
						else
						{
							resL = JVX_ERROR_UNSUPPORTED;
						}
						if (resL != JVX_NO_ERROR)
						{
							res = JVX_ERROR_INTERNAL;
						}
						if (
							(defOneProperty.runtime.descr.num != theDescr.num) ||
							(defOneProperty.runtime.descr.format != theDescr.format))
						{
							defOneProperty.runtime.descr = theDescr;
							resL = deallocateLinearBuffer_propstream(defOneProperty);
							resL = allocateLinearBuffer_propstream(defOneProperty);
						}
					}
					resL = jvx_get_property(defOneProperty.runtime.theTriple.theProps, defOneProperty.runtime.referencedPayloadBuffer,
						defOneProperty.offset, defOneProperty.runtime.descr.num, defOneProperty.runtime.descr.format, true,
						defOneProperty.propertyName.c_str(),
						callGate);
					if (resL == JVX_NO_ERROR)
					{
						jvxPropertyStreamHeader* theHeader = (jvxPropertyStreamHeader*)defOneProperty.runtime.allocatedRawBuffer;

						theHeader->raw_header.packet_tstamp = defOneProperty.tStamp++;
						theHeader->requires_flow_response = 1;

						defOneProperty.streamPropertyInTransferState = 0;
						defOneProperty.state_transfer = JVX_PROPERTY_WEBSOCKET_STATE_IN_TRANSFER;

						jvxTick tick = JVX_GET_TICKCOUNT_US_GET_CURRENT(&tStamp);
						defOneProperty.runtime.last_send_msec = (jvxData)tick * 0.001;
						defOneProperty.runtime.latest_tstamp = theHeader->raw_header.packet_tstamp;

#ifdef JVX_VERBOSE_FLOW_CONTROL
						std::cout << "S[" << theHeader->raw_header.stream_id << "--" << theHeader->raw_header.packet_tstamp << "]" << std::endl;
#endif
						if (defOneProperty.prio == JVX_PROP_CONNECTION_TYPE_NORMAL_PRIO)
						{
							resL = hostRef->myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
								(const char*)theHeader, theHeader->raw_header.loc_header.paketsize);
						}
						else
						{
							resL = defOneProperty.runtime.prio_high.udpSocket->send_packet((const char*)theHeader, theHeader->raw_header.loc_header.paketsize);
							if (resL != JVX_NO_ERROR)
							{
								std::cout << __FUNCTION__ << ": Error: UDP Send Error, error reason: " << jvxErrorType_txt(resL) << "." << std::endl;
							}
						}
						numBytesTransferred = theHeader->raw_header.loc_header.paketsize;
					}

					break;
				}
				break;
			case JVX_PROP_STREAM_UPDATE_ON_CHANGE_REPORT_ORIGIN:

				if (defOneProperty.prio == JVX_PROP_CONNECTION_TYPE_HIGH_PRIO)
				{
					theHeader = (jvxPropertyStreamHeader*)defOneProperty.runtime.allocatedRawBuffer;

					theHeader->requires_flow_response = 1;
					resL = hostRef->myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
						(const char*)theHeader, theHeader->raw_header.loc_header.paketsize);
					numBytesTransferred = theHeader->raw_header.loc_header.paketsize;
				}
				break;
			default:
				assert(0);
			} // switch (defOneProperty.cond_update)
		}
		else
		{
			res = JVX_ERROR_INTERNAL;
		}
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}

	return res;
}

void
CjvxBinaryWebSockets::step_update_properties_websocket(jvxPropertyStreamCondUpdate theReason,
	jvxBool* has_disconnected, const jvxComponentIdentification& idCp, jvxPropertyCategoryType cat, jvxSize propId)
{
	jvxErrorType resL = JVX_NO_ERROR;
	std::map<jvxSize, onePropertyWebSocketDefinition>::iterator elm;
	jvxSize cnt = 0;
	jvxSize numBytes = 0;

	*has_disconnected = false;

	if (theReason == JVX_PROP_STREAM_UPDATE_TIMEOUT)
	{
		// std::cout << __FUNCTION__ << " Running property callback - " << webSocketPeriodic.current_ping_count << std::endl;
		webSocketPeriodic.current_ping_count++;
		if (webSocketPeriodic.current_ping_count > webSocketPeriodic.ping_cnt_trigger)
		{
			// resL = myWebServer.hdl->in_connect_drop_connection((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn);
			hostRef->myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, 0x9, NULL, 0);
		}
		if (webSocketPeriodic.current_ping_count > webSocketPeriodic.ping_cnt_close)
		{
			// resL = myWebServer.hdl->in_connect_drop_connection((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn);
			hostRef->myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, 0x8, NULL, 0);
		}
		if (webSocketPeriodic.current_ping_count > webSocketPeriodic.ping_cnt_close_failed)
		{
			// Client is not responding - assuming it has gone!
			unregister_binary_socket_main_loop(webSocketPeriodic.theCtxt.context_conn);
			*has_disconnected = true;
		}
	}
	// Do not need to protect list since only read access in main thread
	elm = lstUpdateProperties.begin();

	JVX_START_LOCK_LOG_REF(hostRef, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
	log << "Update properties via websocket, reason " << jvxPropertyStreamCondUpdate_txt(theReason) << std::endl;
	if (!hostRef->config.silent_mode)
	{
		std::cout << "Update properties via websocket, reason " << jvxPropertyStreamCondUpdate_txt(theReason) << std::endl;
	}
	JVX_STOP_LOCK_LOG_REF(hostRef);

	cnt = 0;
	for (; elm != lstUpdateProperties.end(); elm++)
	{
		JVX_START_LOCK_LOG_REF(hostRef, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
		log << "Check #" << cnt << ": " << jvxComponentIdentification_txt(elm->second.cpId) << "::" << elm->second.propertyName << "(" << elm->second.uniqueId << ")" << "--" << "Status: " << jvxState_txt(elm->second.runtime.transferState)
			<< "--Activate on: " << jvxState_txt(elm->second.state_active) << "--" << jvxPropertyStreamCondUpdate_txt(elm->second.cond_update) << "::" << elm->second.param_cond_update << std::endl;
		if (!hostRef->config.silent_mode)
		{
			log << "Check #" << cnt << ": " << jvxComponentIdentification_txt(elm->second.cpId) << "::" << elm->second.propertyName << "(" << elm->second.uniqueId << ")" << "--" << "Status: " << jvxState_txt(elm->second.runtime.transferState)
				<< "--Activate on: " << jvxState_txt(elm->second.state_active) << "--" << jvxPropertyStreamCondUpdate_txt(elm->second.cond_update) << "::" << elm->second.param_cond_update << std::endl;
		}
		JVX_STOP_LOCK_LOG_REF(hostRef);

		if (elm->second.runtime.transferState == JVX_STATE_NONE)
		{
			resL = try_activate_property(elm->second);
		}

		if (elm->second.runtime.transferState == JVX_STATE_ACTIVE)
		{
			jvxPropertyTransferType doTransfer = check_transfer_property(elm->second, theReason, idCp, cat, propId);
			if (doTransfer != JVX_PROP_TRANSFER_NONE)
			{
				JVX_START_LOCK_LOG_REF(hostRef, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
				log << "--> Property " << elm->second.propertyName << " is requested to be transfered." << std::endl;
				if (!hostRef->config.silent_mode)
				{
					std::cout << "--> Property " << elm->second.propertyName << " is requested to be transfered." << std::endl;
				}
				JVX_STOP_LOCK_LOG_REF(hostRef);

				resL = transfer_activated_property(elm->second, numBytes);
				if (resL != JVX_NO_ERROR)
				{
					JVX_START_LOCK_LOG_REF(hostRef, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
					log << "Failed to transfer property " << elm->second.propertyName << ", deactivating" << std::endl;
					if (!hostRef->config.silent_mode)
					{
						std::cout << "Failed to transfer property" << elm->second.propertyName << ", deactivating" << std::endl;
					}
					JVX_STOP_LOCK_LOG_REF(hostRef);

					resL = deactivate_property(elm->second);
				}
				else
				{
					JVX_START_LOCK_LOG_REF(hostRef, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
					log << "--> For property " << elm->second.propertyName << ", " << numBytes << " bytes were successfully transfered." << std::endl;
					if (!hostRef->config.silent_mode)
					{
						std::cout << "--> For property " << elm->second.propertyName << ", " << numBytes << " bytes were successfully transfered." << std::endl;
					}
					JVX_STOP_LOCK_LOG_REF(hostRef);
					
				}
			}
		}

		cnt++;
	}
}

void 
CjvxBinaryWebSockets::add_property_observer_list(jvxHandle* param, jvxSize paramType)
{
	onePropertyWebSocketDefinition newProp;
	jvxPropertyPropertyObserveHeader_response theRespA;

	assert(paramType == JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 2);
	jvxPropertyPropertyObserveHeader* paddheader = (jvxPropertyPropertyObserveHeader*)param;

	JVX_START_LOCK_LOG_REF(hostRef, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
	log << "Incoming request web socket property add:" << std::endl;
	log << "Component identification = " << jvxComponentIdentification_txt(jvxComponentIdentification((jvxComponentType)paddheader->component_type, paddheader->component_slot, paddheader->component_subslot)) << std::endl;
	log << "Update Condition = " << jvxPropertyStreamCondUpdate_txt(paddheader->cond_update) << std::endl;
	log << "Update Condition Param = " << paddheader->param_cond_update << std::endl;
	log << "Property format = " << jvxDataFormat_txt(paddheader->property_format) << std::endl;
	log << "Property number elements = " << paddheader->property_num_elements << std::endl;
	log << "Property offset = " << paddheader->property_offset << std::endl;
	log << "Property state active = " << jvxState_dec(paddheader->state_active) << std::endl;
	log << "Property decoder hint type = " << jvxPropertyDecoderHintType_txt(paddheader->property_dec_hint_tp) << std::endl;
	log << "Param0 = " << paddheader->param0 << std::endl;
	log << "Param1 = " << paddheader->param1 << std::endl;
	log << "User [int32] = " << paddheader->user_specific << std::endl;
	log << "Priority [uint16] = " << paddheader->priority << std::endl;
	log << "Port [uint32] = " << paddheader->port << std::endl;
	log << "Number emit min [uint32] = " << paddheader->num_emit_min << std::endl;
	log << "Number emit max [uint32] = " << paddheader->num_emit_max << std::endl;
	JVX_STOP_LOCK_LOG_REF(hostRef);

	assert(paddheader->component_type < JVX_COMPONENT_ALL_LIMIT);
	newProp.cpId = jvxComponentIdentification((jvxComponentType)paddheader->component_type, paddheader->component_slot, paddheader->component_subslot);
	newProp.format_spec = (jvxDataFormat)paddheader->property_format;
	newProp.htTp = (jvxPropertyDecoderHintType)paddheader->property_dec_hint_tp;
	newProp.numElms_spec = JVX_UINT32_SIZE(paddheader->property_num_elements);
	newProp.offset = JVX_UINT16_SIZE(paddheader->property_offset);
	newProp.param0 = paddheader->param0;
	newProp.param1 = paddheader->param1;
	newProp.state_active = (jvxState)paddheader->state_active;
	newProp.cond_update = (jvxPropertyStreamCondUpdate)paddheader->cond_update;
	newProp.param_cond_update = paddheader->param_cond_update;
	//newProp.streamPropertyInTransferStateMax = paddheader->cnt_report_disconnect;
	newProp.requiresFlowControl = (paddheader->requires_flow_control != 0);
	newProp.prio = (jvxPropertyTransferPriority)paddheader->priority;
	newProp.high_prio.port = paddheader->port;
	newProp.num_emit_min = JVX_UINT32_SIZE(paddheader->num_emit_min);
	newProp.num_emit_max = JVX_UINT32_SIZE(paddheader->num_emit_max);
	newProp.propertyName = std::string((char*)paddheader + sizeof(jvxPropertyPropertyObserveHeader), (paddheader->loc_header.paketsize - sizeof(jvxPropertyPropertyObserveHeader)));
	newProp.uniqueId = wsUniqueId++;

	newProp.tStamp = 0;
	newProp.state_transfer = JVX_PROPERTY_WEBSOCKET_STATE_NONE;
	newProp.streamPropertyInTransferState = 0;
	newProp.runtime.allocatedRawBuffer = NULL;
	jvx_initPropertyDescription(newProp.runtime.descr);
	newProp.runtime.referencedPayloadBuffer = NULL;
	newProp.runtime.szRawBuffer = 0;
	jvx_initPropertyReferenceTriple(&newProp.runtime.theTriple);
	newProp.runtime.transferState = JVX_STATE_NONE;
	newProp.runtime.cntTicks = 0;
	newProp.runtime.serious_fail = false;
	newProp.runtime.last_send_msec = 0;

	if (newProp.prio == JVX_PROP_CONNECTION_TYPE_HIGH_PRIO)
	{
		jvxApiString astr;
		hostRef->myWebServer.hdl->in_connect_get_ws_ip_addr((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, &astr);
		newProp.high_prio.target = astr.std_str();

		// Create a UDP socket and open!
		auto elm = high_prio_transfer.theUdpSockets.find(newProp.high_prio.port);
		if (elm == high_prio_transfer.theUdpSockets.end())
		{
			// Allocate new socket
			HjvxPropertyStreamUdpSocket* theNewUdpSocket = NULL;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(theNewUdpSocket, HjvxPropertyStreamUdpSocket);
			theNewUdpSocket->initialize(
				static_cast<HjvxPropertyStreamUdpSocket_report*>(this),
				hostRef->myToolsHost, hostRef->myHostRef);
			theNewUdpSocket->register_property(newProp.uniqueId, newProp.high_prio.port, newProp.high_prio.target);
			high_prio_transfer.theUdpSockets[newProp.high_prio.port] = theNewUdpSocket;
			newProp.runtime.prio_high.udpSocket = theNewUdpSocket;
		}
		else
		{
			(elm->second)->register_property(newProp.uniqueId, newProp.high_prio.port, astr.std_str());
			newProp.runtime.prio_high.udpSocket = elm->second;
		}
	}

	lstUpdateProperties[newProp.uniqueId] = newProp;

	JVX_START_LOCK_LOG_REF(hostRef, jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
	log << "Registered property via web socket <" << newProp.uniqueId << ">: " << jvxComponentIdentification_txt(newProp.cpId) << "--" << newProp.propertyName << "||" <<
		jvxDataFormat_txt(newProp.format_spec) << "--" << jvxPropertyDecoderHintType_txt(newProp.htTp) << "--" << newProp.offset << newProp.numElms_spec << std::flush;
	switch (newProp.prio)
	{
	case JVX_PROP_CONNECTION_TYPE_NORMAL_PRIO:
		log << "--" << "prio_normal." << std::flush;
		break;
	case JVX_PROP_CONNECTION_TYPE_HIGH_PRIO:
		log << "--" << "prio_high, udp target: <" << newProp.high_prio.target << ":" << newProp.high_prio.port << ">." << std::flush;
		break;
	}
	log << std::endl;
	JVX_STOP_LOCK_LOG_REF(hostRef);

	// Send response
	theRespA.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
	theRespA.loc_header.purpose = JVX_PS_ADD_PROPERTY_TO_OBSERVE | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
	theRespA.loc_header.paketsize = sizeof(jvxPropertyPropertyObserveHeader_response);
	theRespA.errcode = JVX_NO_ERROR;
	theRespA.component_type = paddheader->component_type;
	theRespA.component_slot = paddheader->component_slot;
	theRespA.component_subslot = paddheader->component_subslot;
	theRespA.stream_id = JVX_SIZE_INT16(newProp.uniqueId);
	theRespA.user_specific = paddheader->user_specific;

	hostRef->myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
		(const char*)&theRespA, theRespA.loc_header.paketsize);
}

void
CjvxBinaryWebSockets::rem_property_observer_list(jvxHandle* param, jvxSize paramType)
{
	jvxPropertyPropertyObserveHeader_response theRespA;
	assert(paramType == JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 2);
	jvxPropertyPropertyObserveHeader* premheader = (jvxPropertyPropertyObserveHeader*)param;

	while (1)
	{
		std::map<jvxSize, onePropertyWebSocketDefinition>::iterator elm = lstUpdateProperties.begin();
		for (; elm != lstUpdateProperties.end(); elm++)
		{
			if (
				(elm->second.cpId.tp == premheader->component_type) &&
				(elm->second.cpId.slotid == premheader->component_slot) &&
				(elm->second.cpId.slotsubid == premheader->component_subslot))
			{
				if (elm->second.runtime.transferState == JVX_STATE_ACTIVE)
				{
					deactivate_property(elm->second);
				}
				break;
			}
		}
		if (elm == lstUpdateProperties.end())
		{
			// No more found
			break;
		}
		else
		{
			if (elm->second.prio == JVX_PROP_CONNECTION_TYPE_HIGH_PRIO)
			{
				disconnect_udp_port(elm->second);
			}
			lstUpdateProperties.erase(elm);
		}
	}

	theRespA.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
	theRespA.loc_header.purpose = JVX_PS_REMOVE_PROPERTY_TO_OBSERVE | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
	theRespA.loc_header.paketsize = sizeof(jvxPropertyPropertyObserveHeader_response);
	theRespA.errcode = JVX_NO_ERROR;
	theRespA.component_type = premheader->component_type;
	theRespA.component_slot = premheader->component_slot;
	theRespA.component_subslot = premheader->component_subslot;

	theRespA.stream_id = JVX_SIZE_INT16(0);
	theRespA.user_specific = premheader->user_specific;

	hostRef->myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
		(const char*)&theRespA, theRespA.loc_header.paketsize);
}

void
CjvxBinaryWebSockets::process_flow_event(jvxHandle* param, jvxSize paramType)
{
	assert(paramType == JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 4);
	jvxRawStreamHeader_response* raw_pheader_resp = (jvxRawStreamHeader_response*)param;
	{
		// No protection required here since only reading and within main thread
		auto elm = lstUpdateProperties.find(raw_pheader_resp->raw_header.stream_id);
		if (elm != lstUpdateProperties.end())
		{
			if (elm->second.requiresFlowControl)
			{
				if (elm->second.state_transfer == JVX_PROPERTY_WEBSOCKET_STATE_IN_TRANSFER)
				{
#ifdef JVX_VERBOSE_FLOW_CONTROL
					std::cout << "R[" << raw_pheader_resp->raw_header.stream_id << "--" << raw_pheader_resp->raw_header.packet_tstamp << "]" << std::endl;
#endif
					elm->second.state_transfer = JVX_PROPERTY_WEBSOCKET_STATE_NONE;
				}
			}
		}
	}
}

jvxErrorType
CjvxBinaryWebSockets::disconnect_udp_port(const onePropertyWebSocketDefinition& prop_elm)
{
	if (prop_elm.runtime.prio_high.udpSocket)
	{
		jvxInt32 port = 0;
		prop_elm.runtime.prio_high.udpSocket->unregister_property(prop_elm.uniqueId, port);
		if (port != 0)
		{
			auto elms = high_prio_transfer.theUdpSockets.find(port);
			if (elms != high_prio_transfer.theUdpSockets.end())
			{
				HjvxPropertyStreamUdpSocket* to_remove = elms->second;
				high_prio_transfer.theUdpSockets.erase(elms);
				to_remove->terminate();
				JVX_DSP_SAFE_DELETE_OBJECT(to_remove);
			}
			else
			{
				assert(0);
			}
		}
	}
	else
	{
		assert(0);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxBinaryWebSockets::report_incoming_packet(jvxByte* fld, jvxSize sz, HjvxPropertyStreamUdpSocket* resp_socket)
{
	jvxErrorType res = JVX_NO_ERROR;

	assert(sz >= sizeof(jvxRawStreamHeader_response));
	TjvxEventLoopElement elm;
	//jvxPropertyPropertyObserveHeader_response* paddheader = (jvxPropertyPropertyObserveHeader_response*)fld;
	elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(hostRef);
	elm.priv_fe = NULL;
	elm.target_be = static_cast<IjvxEventLoop_backend*>(hostRef);
	elm.priv_be = NULL;

	elm.param = (jvxHandle*)fld;
	elm.paramType = JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 4;

	elm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 7;
	elm.eventClass = JVX_EVENTLOOP_REQUEST_CALL_BLOCKING;
	elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
	elm.delta_t = JVX_TIMOUT_BLOCKING_CALL_MSEC;
	elm.autoDeleteOnProcess = c_false;
	res = hostRef->evLop->event_schedule(&elm, NULL, NULL);
	switch (res)
	{
	case JVX_ERROR_END_OF_FILE:
		std::cout << "Disconnected websocket not reported since host shutdown deadline was missed!" << std::endl;
		break;
	case JVX_ERROR_ABORT:
		std::cout << "Disconnected websocket not reported since host does not accept events from this frontend currently!" << std::endl;
		break;
	case JVX_NO_ERROR:
		break;
	default:
		assert(0);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxBinaryWebSockets::report_connection_error(const char* errorText)
{
	TjvxEventLoopElement elm;
	std::string err = errorText;

	elm.origin_fe = static_cast<IjvxEventLoop_frontend*>(hostRef);
	elm.priv_fe = NULL;
	elm.target_be = static_cast<IjvxEventLoop_backend*>(hostRef);
	elm.priv_be = NULL;

	elm.param = (jvxHandle*)&err;
	elm.paramType = JVX_EVENTLOOP_DATAFORMAT_STDSTRING;

	elm.eventType = JVX_EVENTLOOP_EVENT_SPECIFIC + 8;
	elm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
	elm.eventPriority = JVX_EVENTLOOP_PRIORITY_NORMAL;
	elm.delta_t = JVX_TIMOUT_BLOCKING_CALL_MSEC;
	elm.autoDeleteOnProcess = c_true;
	jvxErrorType res = hostRef->evLop->event_schedule(&elm, NULL, NULL);
	return JVX_NO_ERROR;
}

void
CjvxBinaryWebSockets::timeout_reconfigure(jvxHandle* param, jvxSize paramType)
{
	jvxErrorType res = JVX_NO_ERROR;
	TjvxEventLoopElement evLElm;
	jvxPropertyConfigurePropertySend_response theRespC;

	memset(&theRespC, 0, sizeof(jvxPropertyConfigurePropertySend_response));

	assert(paramType == JVX_EVENTLOOP_DATAFORMAT_OFF_SPECIFIC + 3);
	jvxPropertyConfigurePropertySend* passed = (jvxPropertyConfigurePropertySend*)param;
	webSocketPeriodic.timeout_msec = passed->tick_msec;
	webSocketPeriodic.ping_cnt_trigger = passed->ping_count;
	webSocketPeriodic.ping_cnt_close = webSocketPeriodic.ping_cnt_trigger * 2;
	webSocketPeriodic.ping_cnt_close_failed = webSocketPeriodic.ping_cnt_trigger * 3;
	webSocketPeriodic.current_ping_count = 0;

	// Restart timer with a different timeout
	if (JVX_CHECK_SIZE_SELECTED(webSocketPeriodic.wsMessId))
	{
		res = hostRef->evLop->event_clear(webSocketPeriodic.wsMessId, NULL, NULL);
		webSocketPeriodic.wsMessId = JVX_SIZE_UNSELECTED;
	}

	// Reschedule timer event
	evLElm.origin_fe = static_cast<IjvxEventLoop_frontend*>(hostRef);
	evLElm.priv_fe = NULL;
	evLElm.target_be = static_cast<IjvxEventLoop_backend*>(hostRef);
	evLElm.priv_be = NULL;

	evLElm.param = NULL;
	evLElm.paramType = JVX_EVENTLOOP_DATAFORMAT_NONE;

	evLElm.eventType = JVX_EVENT_LOOP_EVENT_WS_TIMEOUT;
	evLElm.eventClass = JVX_EVENTLOOP_REQUEST_TRIGGER;
	evLElm.eventPriority = JVX_EVENTLOOP_PRIORITY_TIMER;
	evLElm.delta_t = webSocketPeriodic.timeout_msec;
	evLElm.autoDeleteOnProcess = c_false;

	res = hostRef->evLop->event_schedule(&evLElm, NULL, NULL);

	webSocketPeriodic.wsMessId = evLElm.message_id;

	switch (res)
	{
	case JVX_ERROR_END_OF_FILE:
		std::cout << "Disconnected websocket not reported since host shutdown deadline was missed!" << std::endl;
		break;
	case JVX_ERROR_ABORT:
		std::cout << "Disconnected websocket not reported since host does not accept events from this frontend currently!" << std::endl;
		break;
	case JVX_NO_ERROR:
		break;
	default:
		assert(0);
	}

	// Positive response
	theRespC.loc_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_PROPERTY_STREAM;
	theRespC.loc_header.purpose = JVX_PS_CONFIGURE_PROPERTY_OBSERVATION | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE;
	theRespC.loc_header.paketsize = sizeof(jvxPropertyConfigurePropertySend_response);
	theRespC.errcode = JVX_NO_ERROR;
	theRespC.user_specific = passed->user_specific;
	hostRef->myWebServer.hdl->in_connect_send_ws_packet((struct mg_connection*)webSocketPeriodic.theCtxt.context_conn, JVX_WEBSOCKET_OPCODE_BINARY,
		(const char*)&theRespC, theRespC.loc_header.paketsize);
}