#include "CayfViNCameraConvert.h"
#include "HjvxDataLinkDescriptor.h"

// class CayfViNCameraConvert

CayfViNCameraConvert::CayfViNCameraConvert(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	neg_output.negBehavior = CjvxNegotiate_common::negBehaviorType::JVX_BEHAVIOR_VIDEO;
}

CayfViNCameraConvert::~CayfViNCameraConvert()
{
}

jvxErrorType 
CayfViNCameraConvert::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var))
{
	jvxErrorType res = CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(var));
	neg_output._pop_constraints();
	return res;
}

jvxErrorType 
CayfViNCameraConvert::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));	
	return res;
}

jvxErrorType 
CayfViNCameraConvert::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	// This module decides itself if it is operated in zero copy or not!
	// If not, allocate the required number of buffers
	_common_set_ldslave.num_min_buffers_out = _common_set_icon.theData_in->con_data.number_buffers;

	if (jvx_check_in_out_params_match_test(_common_set_icon.theData_in, &_common_set_ocon.theData_out, JVX_CHECK_PARAM_ALL))
	{
		_common_set_ldslave.zeroCopyBuffering_cfg = true;
	}
	else
	{
		switch (_common_set_icon.theData_in->con_params.format_group)
		{
		case JVX_DATAFORMAT_GROUP_VIDEO_NV12:
		{
			// Allocate the proper conversion buffer!!
			jvxSize szElement = _common_set_icon.theData_in->con_params.segmentation.x *
				_common_set_icon.theData_in->con_params.segmentation.y;
			jvxSize szElementField = szElement * jvxDataFormatGroup_getsize_mult(_common_set_icon.theData_in->con_params.format_group);
			szElementField /= jvxDataFormatGroup_getsize_div(_common_set_icon.theData_in->con_params.format_group);
			jvxSize szField = szElementField * jvxDataFormat_getsize(_common_set_icon.theData_in->con_params.format);

			convert.NV12.plane1_Sz = _common_set_icon.theData_in->con_params.segmentation.x * _common_set_icon.theData_in->con_params.segmentation.y;
			convert.NV12.plane2_Sz = szField - convert.NV12.plane1_Sz;
			break;
		}
		case JVX_DATAFORMAT_GROUP_VIDEO_RGB24:
			break;
		default:
			assert(0);
		}
		
	}
	return CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CayfViNCameraConvert::accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCBitField modFlags = 0;
	jvxLinkDataDescriptor preferredByOutputCopy = *preferredByOutput;
	res = neg_output._negotiate_transfer_backward_ocon(&preferredByOutputCopy, &_common_set_ocon.theData_out,
		this, &modFlags JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

	if (res != JVX_NO_ERROR)
	{
		jvxCBitField checkFlags = 0;
		jvx_bitSet(checkFlags, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_BUFFERSIZE_SHIFT);
		jvx_bitSet(checkFlags, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_SEGX_SHIFT);
		jvx_bitSet(checkFlags, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_SEGY_SHIFT);

		if (modFlags & checkFlags)
		{
			jvx_bitInvert(checkFlags);
			if (!(modFlags & checkFlags))
			{
				jvxLinkDataDescriptor proposeFwd = *_common_set_icon.theData_in;
				proposeFwd.con_params.buffersize = preferredByOutput->con_params.buffersize;
				proposeFwd.con_params.segmentation.x = preferredByOutput->con_params.segmentation.x;
				proposeFwd.con_params.segmentation.y = preferredByOutput->con_params.segmentation.y;
				if (_common_set_icon.theData_in->con_link.connect_from)
				{
					res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(tp, &proposeFwd JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				}
				if (res == JVX_NO_ERROR)
				{
					neg_output._update_parameters_fixed(JVX_SIZE_UNSELECTED,
						proposeFwd.con_params.buffersize,
						JVX_SIZE_UNSELECTED, JVX_DATAFORMAT_NONE, JVX_DATAFORMAT_GROUP_NONE,
						JVX_DATAFLOW_DONT_CARE, &_common_set_ocon.theData_out,
						proposeFwd.con_params.segmentation.x, proposeFwd.con_params.segmentation.y);
					output_params_from_ldesc_on_test();
					test_set_output_parameters();
					res = JVX_ERROR_ABORT;
				}
			}
		}
	}
	return res;
}
void
CayfViNCameraConvert::from_input_to_output()
{
	neg_output._push_constraints();
	neg_output._update_parameters_fixed(
		node_inout._common_set_node_params_a_1io.number_channels, 
		node_inout._common_set_node_params_a_1io.buffersize,
		node_inout._common_set_node_params_a_1io.samplerate,
		
		// Leave these two open since we will convert right here!!
		JVX_DATAFORMAT_NONE, 
		JVX_DATAFORMAT_GROUP_NONE,

		(jvxDataflow)node_inout._common_set_node_params_a_1io.data_flow, 
		&_common_set_ocon.theData_out,
		node_inout._common_set_node_params_a_1io.segmentation.x,
		node_inout._common_set_node_params_a_1io.segmentation.y);

	CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>::update_output_params_from_input_params();
}
