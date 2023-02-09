#include "CjvxAuNCodec.h"

jvxErrorType
CjvxAuNCodec::connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	res = CjvxAudioNode::connect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		mainModuleConnected = true;

		if (connection_lane[0].stat == JVX_STATE_INIT)
		{
			resL = connection_lane[0].micro_connection->connect_connection(
				&connection_lane[0].descr_encoder_in,
				&connection_lane[0].descr_decoder_out);
			assert(resL == JVX_NO_ERROR);
			connection_lane[0].stat = JVX_STATE_ACTIVE;
		}

		if (connection_lane[1].stat == JVX_STATE_INIT)
		{
			resL = connection_lane[1].micro_connection->connect_connection(
				&connection_lane[1].descr_encoder_in,
				&connection_lane[1].descr_decoder_out);
			assert(resL == JVX_NO_ERROR);
			connection_lane[1].stat = JVX_STATE_ACTIVE;
		}

	}
	return res;
};

jvxErrorType
CjvxAuNCodec::disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	res = CjvxAudioNode::disconnect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		if (connection_lane[0].stat == JVX_STATE_ACTIVE)
		{
			resL = connection_lane[0].micro_connection->disconnect_connection();
			assert(resL == JVX_NO_ERROR);
			connection_lane[0].stat = JVX_STATE_INIT;
		}

		if (connection_lane[1].stat == JVX_STATE_ACTIVE)
		{
			resL = connection_lane[1].micro_connection->disconnect_connection();
			assert(resL == JVX_NO_ERROR);
			connection_lane[1].stat = JVX_STATE_INIT;
		}
		mainModuleConnected = false;
	}
	return res;
};

// ===============================================================================
jvxErrorType
CjvxAuNCodec::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* from_encoder_to_decoder = NULL;
	jvxSize szElm = 0;

	// Check the input
	if (
		(connection_lane[0].stat == JVX_STATE_ACTIVE) && (connection_lane[1].stat == JVX_STATE_ACTIVE))
	{
		JVX_CONNECTION_FEEDBACK_TYPE_DEFINE_CLASS(fdbLoc0);
		JVX_CONNECTION_FEEDBACK_TYPE_DEFINE_CLASS_INIT(fdbLoc0);

		connection_lane[0].descr_encoder_in.con_params = _common_set_ldslave.theData_in->con_params;
		jvxErrorType resL0 = connection_lane[0].micro_connection->test_connection(JVX_CONNECTION_FEEDBACK_CALL(fdbLoc0));

		JVX_CONNECTION_FEEDBACK_TYPE_DEFINE_CLASS(fdbLoc1);
		JVX_CONNECTION_FEEDBACK_TYPE_DEFINE_CLASS_INIT(fdbLoc1);

		connection_lane[1].descr_encoder_in.con_params = _common_set_ldslave.theData_in->con_params;
		jvxErrorType resL1 = connection_lane[1].micro_connection->test_connection(JVX_CONNECTION_FEEDBACK_CALL(var));

		if (
			(resL0 == JVX_NO_ERROR) &&
			(resL1 == JVX_NO_ERROR))
		{
			jvxLinkDataDescriptor lp;
			lp.con_params = connection_lane[0].descr_encoder_in.con_params;
			jvxErrorType resL01 = _common_set_ldslave.theData_in->con_link.connect_from->transfer_backward_ocon(
				JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS, reinterpret_cast<jvxHandle*>(&lp)
				JVX_CONNECTION_FEEDBACK_CALL_A(var));
			if (resL01 != JVX_NO_ERROR)
			{
				return resL01;
			}
			/*
			if (
				(connection_lane[0].descr_encoder_in.con_params.buffersize == connection_lane[0].descr_encoder_in.con_params.buffersize))
			{
				std::cout << "Match!" << std::endl;
			}
			*/
		}
		this->_common_set_node_params_1io.preferred.buffersize.min = connection_lane[0].descr_encoder_in.con_params.buffersize;
		this->_common_set_node_params_1io.preferred.buffersize.max = connection_lane[0].descr_encoder_in.con_params.buffersize;
		this->_common_set_node_params_1io.preferred.samplerate.min = connection_lane[0].descr_encoder_in.con_params.rate;
		this->_common_set_node_params_1io.preferred.samplerate.max = connection_lane[0].descr_encoder_in.con_params.rate;
		this->_common_set_node_params_1io.preferred.format.min = connection_lane[0].descr_encoder_in.con_params.format;
		this->_common_set_node_params_1io.preferred.format.max = connection_lane[0].descr_encoder_in.con_params.format;
		this->_common_set_node_params_1io.preferred.number_input_channels.min = connection_lane[0].descr_encoder_in.con_params.number_channels;
		this->_common_set_node_params_1io.preferred.number_input_channels.max = connection_lane[0].descr_encoder_in.con_params.number_channels;


		this->_common_set_node_params_1io.preferred.number_output_channels.min = 2;
		this->_common_set_node_params_1io.preferred.number_output_channels.max = JVX_SIZE_UNSELECTED;
	}
		
	// This function negotiates the input settings and the output settings (number channels)
	res = CjvxAudioNode::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(var));

	CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value = _common_set_ldslave.theData_out.con_params.number_channels;
	CjvxAudioNode_genpcg::properties_parameters.buffersize.value = _common_set_ldslave.theData_in->con_params.buffersize;
	CjvxAudioNode_genpcg::properties_parameters.samplerate.value = _common_set_ldslave.theData_in->con_params.rate;
	CjvxAudioNode_genpcg::properties_parameters.format.value = _common_set_ldslave.theData_in->con_params.format;

	/*
	CjvxAudioNode_genpcg::properties_parameters.numberoutputchannels.value = 2;
	CjvxAudioNode_genpcg::properties_parameters.buffersize.value = connection_lane[0].descr_decoder_out.con_params.buffersize;
	CjvxAudioNode_genpcg::properties_parameters.samplerate.value = connection_lane[0].descr_decoder_out.con_params.rate;
	CjvxAudioNode_genpcg::properties_parameters.format.value = connection_lane[0].descr_decoder_out.con_params.format;
	*/
	return res;

}

/*
jvxErrorType 
CjvxAuNCodec::test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(var))
{
	jvxErrorType res = JVX_NO_ERROR;
	_common_set_ldslave.theData_out.sender_to_receiver = connection_lane[0].descr_decoder_out.sender_to_receiver;
	_common_set_ldslave.theData_out.con_params.number_channels = 2;
	res = CjvxAudioNode::test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(var));
	return res;
}
*/
