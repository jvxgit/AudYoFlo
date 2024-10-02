#include "CayfViNCameraConvert.h"
#include "HjvxDataLinkDescriptor.h"

#include <opencv2/imgproc.hpp>

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
CayfViNCameraConvert::activate()
{
	jvxErrorType res = CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>::activate();
	if (res == JVX_NO_ERROR)
	{
		genCameraConvert_node::init_all();
		genCameraConvert_node::allocate_all();
		genCameraConvert_node::register_all(this);

		genCameraConvert_node::register_callbacks(this, set_config, this, nullptr);
	}
	return res;

}

jvxErrorType 
CayfViNCameraConvert::deactivate()
{
	jvxErrorType res = CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>::deactivate();
	if (res == JVX_NO_ERROR)
	{
		genCameraConvert_node::unregister_callbacks(this);
		genCameraConvert_node::unregister_all(this);
		genCameraConvert_node::deallocate_all();
	}
	return res;
}


/*
jvxErrorType
CayfViNCameraConvert::connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>::connect_connect_icon(theData JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		jvx_common_set_ocon.theData_out
	}
}
*/

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
CayfViNCameraConvert::accept_negotiate_output(jvxLinkDataTransferType tp, jvxLinkDataDescriptor* preferredByOutput JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxCBitField modFlags = 0;
	jvxLinkDataDescriptor preferredByOutputCopy = *preferredByOutput;
	res = neg_output._negotiate_transfer_backward_ocon(&preferredByOutputCopy, &_common_set_ocon.theData_out,
		this, &modFlags JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

	if (res == JVX_NO_ERROR)
	{
		if (genCameraConvert_node::config.try_negotiate_input.value == c_true)
		{
			// Here, we can run it with the input format and keep the output format.
			// However, we may find a better setting on the input side
			if (
				(preferredByOutput->con_params.segmentation.x != _common_set_icon.theData_in->con_params.segmentation.x) ||
				(preferredByOutput->con_params.segmentation.y != _common_set_icon.theData_in->con_params.segmentation.y))
			{
				if (_common_set_icon.theData_in->con_link.connect_from)
				{
					jvxLinkDataDescriptor proposeFwd = *_common_set_icon.theData_in;
					proposeFwd.con_params.segmentation.x = preferredByOutput->con_params.segmentation.x;
					proposeFwd.con_params.segmentation.y = preferredByOutput->con_params.segmentation.y;
					proposeFwd.con_params.buffersize = jvx_derive_buffersize(proposeFwd.con_params);
					jvxErrorType resTry = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(tp, &proposeFwd JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					if (resTry == JVX_NO_ERROR)
					{
						// Test the new settings
						resTry = neg_input._negotiate_connect_icon(_common_set_icon.theData_in,
							static_cast<IjvxObject*>(this),
							_common_set_io_common.descriptor.c_str()
							JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
						if (resTry == JVX_NO_ERROR)
						{
							// Accept modified new input arguments
							update_simple_params_from_ldesc();
						}
						else
						{
							res = resTry;
						}
					}
				}
			}
		}

		if (res == JVX_NO_ERROR)
		{
			// Take over the arguments as requested
			_common_set_ocon.theData_out.con_params = preferredByOutput->con_params;
			update_output_params_from_ldesc_on_test();
			res = JVX_ERROR_ABORT; // <- skip internal call to <neg_output._negotiate_transfer_backward_ocon>
		}
	}

	return res;
}
void
CayfViNCameraConvert::from_input_to_output()
{
	std::string reason;

	// Only if the output is not specified, we derive from the input
	if(
		(jvx_check_valid(_common_set_ocon.theData_out.con_params, reason) != JVX_NO_ERROR) ||
		(genCameraConvert_node::config.try_negotiate_input.value == c_true)
		)
	{
		// These constraints need to match in any case!!
		neg_output._update_parameter_fixed(jvxAddressLinkDataEntry::JVX_ADDRESS_NUM_CHANNELS_SHIFT, node_inout._common_set_node_params_a_1io.number_channels);
		neg_output._update_parameter_fixed(jvxAddressLinkDataEntry::JVX_ADDRESS_SAMPLERATE_SHIFT, node_inout._common_set_node_params_a_1io.samplerate);
		neg_output._update_parameter_fixed(jvxAddressLinkDataEntry::JVX_ADDRESS_DATAFLOW_SHIFT, node_inout._common_set_node_params_a_1io.data_flow);
		CjvxVideoNodeTpl<CjvxBareNode1ioRearrange>::update_output_params_from_input_params();
	}
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CayfViNCameraConvert, set_config)
{
	return JVX_NO_ERROR;
}

