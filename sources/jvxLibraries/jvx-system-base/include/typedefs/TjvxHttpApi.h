#ifndef __TJVXHTTPAPI_H__
#define __TJVXHTTPAPI_H__

typedef enum
{
	JVX_PROP_STREAM_UPDATE_TIMEOUT = 0,
	JVX_PROP_STREAM_UPDATE_ON_CHANGE = 1,
	JVX_PROP_STREAM_UPDATE_ON_REQUEST = 2,
	JVX_PROP_STREAM_UPDATE_ON_CHANGE_REPORT_ORIGIN = 3,
	JVX_PROP_STREAM_UPDATE_LIMIT
}jvxPropertyStreamCondUpdate;

typedef enum
{
	JVX_PROP_TRANSFER_NONE = 0,
	JVX_PROP_TRANSFER_CONTENT = 1,
	JVX_PROP_TRANSFER_SELF = 2,
	JVX_PROP_TRANSFER_LIMIT
}jvxPropertyTransferType;

typedef enum
{
	JVX_PROP_CONNECTION_TYPE_NORMAL_PRIO,
	JVX_PROP_CONNECTION_TYPE_HIGH_PRIO
} jvxPropertyTransferPriority;

struct jvxOneRemotePropertySpecification
{
	/*
		Name of the property to address
	 */
	std::string nmProp;

	/*
		component identification to address right component
	 */
	jvxComponentIdentification cpTp;

	/*
		Data format to address
	 */
	jvxDataFormat format;

	/*
		Number of elements. This parameter can be UNSELECTED to use 
		the size as it is or a specific value to match with property specs
	 */
	jvxSize numElms;

	/*
		Decoder hint type to match
	 */
	jvxPropertyDecoderHintType htTp;

	/*
		Offset
	 */
	jvxSize offset;

	/*
		Generic parameter nr 1
		In case of circular buffer:
			- contains number of channels, should not be UNSELECTED
			- can be overridden by property extended info (not yet fully supported)
	 */
	jvxSize param0;

	/*
		Generic parameter nr 2.
		In case of circular buffer: 
			- contains size of circular buffer, should not be UNSELECTED
			- can be overridden by property extended info (not yet fully supported)
		In case of linear buffer:
			- requests size from addressed property
	 */
	jvxSize param1;

	/*
		Mask of states in which to
	 */
	jvxUInt32 state_in_which_active;

	jvxPropertyStreamCondUpdate cond_update;

	/*
		Type of update, can be 
		JVX_PROP_STREAM_UPDATE_TIMEOUT = 0,
		JVX_PROP_STREAM_UPDATE_ON_CHANGE = 1,
		JVX_PROP_STREAM_UPDATE_ON_REQUEST = 2,
		JVX_PROP_STREAM_UPDATE_ON_CHANGE_REPORT_ORIGIN
	 */
	jvxSize param_cond_update;

	/*
		This parameter is used to identify incoming packets - the user id will be part of each packet
	 */
	jvxSize user_data;

	/*
		Number of accepted fails in property access until this property is disconnected.
		It seems that this is not used.
	 */
	jvxSize cnt_report_disconnect;

	/*
		Activates flow control - that is: always only one packet is on the way and 
		a confirmation is required to 
	 */
	jvxBool requires_flow_control;

	/*
		I do not know what this parameter is used for...
	 */
	jvxBool registeredOnOtherSide; 

	/*
		Currently not used
	 */
	jvxSize id_user;

	/* 
		Specification of streamig priority. NORMAL or HIGH
	 */
	jvxPropertyTransferPriority prio;
	
	/*
		In case the priority is high, the specifies the port to which to stream the data
	 */
	jvxInt32 port;


	/*
		Unique id to identify this property within all UDP sockets
	 */
	jvxSize port_uid; 
	// ===============================================================================================

	/*
		Better control of transmitted packages: do not emit if fewer number of values are available and limit number
		to max_num_emit. If flow control is active only on the last package the confirmation is requested
	 */
	jvxSize min_num_emit;
	jvxSize max_num_emit;

	jvxOneRemotePropertySpecification(
		const std::string& nmProp_arg = "", /* Name of the property on the remote side*/
		jvxComponentIdentification cpTp_arg = JVX_COMPONENT_UNKNOWN, /* component identification on the remote side*/
		jvxDataFormat format_arg = JVX_DATAFORMAT_NONE,	/* Data format on the remote side */
		jvxSize numElms_arg = 0, /* Number element, JVX_SIZE_UNSELECTED if unknown*/
		jvxPropertyDecoderHintType htTp_arg = JVX_PROPERTY_DECODER_NONE, /* Decoder hint type */
		jvxSize offset_arg = 0, /* Offset in property*/
		jvxSize param0_arg = 0, /* param 0, typically used for number of channels*/
		jvxSize param1_arg = 0, /* ??? */
		jvxUInt32 state_in_which_active_arg = JVX_STATE_NONE, /* State mask in which properties may be "activate" */
		jvxPropertyStreamCondUpdate cond_update_arg = JVX_PROP_STREAM_UPDATE_TIMEOUT, /* When to update the property by emitting stream fragment */
		jvxSize param_cond_update_arg = 0, /* Argument to control update of property */
		jvxSize user_data_arg = JVX_SIZE_UNSELECTED, /* user data argument */
		jvxSize cnt_report_disconnect_arg = 10, /* Number of attempts before disconnecting */
		jvxBool requires_flow_control_arg = false, /* Requires flow control to wait for completion of previous transfers. */
		jvxBool registeredOnOtherSide_arg = false, /* ??? */
		jvxSize id_user_arg = 0, /* Additional user argument ***/
		jvxPropertyTransferPriority prio_arg = JVX_PROP_CONNECTION_TYPE_NORMAL_PRIO, /* Priority of data transfers to run websocket or udp channel */
		jvxInt32 port_arg = 0,
		jvxSize min_num_emit_arg = 0,
		jvxSize max_num_emit_arg = 0) /* In case of USP, this is the port. */
	{
		nmProp = nmProp_arg;
		cpTp = cpTp_arg;
		format = format_arg;
		numElms = numElms_arg;
		htTp = htTp_arg;
		offset = offset_arg;
		param0 = param0_arg;
		param1 = param1_arg;
		state_in_which_active = state_in_which_active_arg;
		cond_update = cond_update_arg;
		param_cond_update = param_cond_update_arg;
		user_data = user_data_arg;
		cnt_report_disconnect = cnt_report_disconnect_arg;
		requires_flow_control = requires_flow_control_arg;
		registeredOnOtherSide = registeredOnOtherSide_arg;
		id_user = id_user_arg;
		prio = prio_arg;
		port = port_arg;
		min_num_emit = min_num_emit_arg;
		max_num_emit = max_num_emit_arg;
	};
} ;

#endif
