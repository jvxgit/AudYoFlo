#include "CjvxAuNChannelRearrange.h"
#include "jvx-helpers-cpp.h"

CjvxAuNChannelRearrange::CjvxAuNChannelRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1ioRearrange(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_NODE_TYPE_SPECIFIER_TYPE);
	_common_set.theComponentSubTypeDescriptor = JVX_NODE_TYPE_SPECIFIER_DESCRIPTOR;
	outputArgsFromOutputParams = true;
}

CjvxAuNChannelRearrange::~CjvxAuNChannelRearrange()
{
	
}

jvxErrorType
CjvxAuNChannelRearrange::activate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::activate();
	if (res == JVX_NO_ERROR)
	{
		genChannelRearrange_node::init_all();
		genChannelRearrange_node::allocate_all();
		genChannelRearrange_node::register_all(this);

		genChannelRearrange_node::register_callbacks(this, get_level_pre, get_level_post, set_passthru, this);
	}
	return res;
}

jvxErrorType
CjvxAuNChannelRearrange::deactivate()
{
	jvxErrorType res = CjvxBareNode1ioRearrange::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{		
		
		CjvxBareNode1ioRearrange::deactivate();
	}
	return res;
}

// ===================================================================
jvxErrorType
CjvxAuNChannelRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = CjvxBareNode1ioRearrange::test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if(res == JVX_NO_ERROR)
	{
	}
	return res;
}

// ===================================================================

jvxErrorType
CjvxAuNChannelRearrange::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxBareNode1ioRearrange::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CjvxAuNChannelRearrange::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return CjvxBareNode1ioRearrange::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

// ===========================================================================

jvxErrorType
CjvxAuNChannelRearrange::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{

	if (!_common_set_ldslave.setup_for_termination)
	{
		jvxData** fieldInput = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_ldslave.theData_in, idx_stage);
		jvxData** fieldOutput = jvx_process_icon_extract_output_buffers<jvxData>(&_common_set_ldslave.theData_out);
		jvxBool passThrough = (genChannelRearrange_node::passthrough.active.value == c_true);
		// Talkthrough
		jvxSize ii;
		jvxSize inChans = _common_set_ldslave.theData_in->con_params.number_channels;
		jvxSize outChans = _common_set_ldslave.theData_out.con_params.number_channels;
		if (passThrough)
		{
			inChans = JVX_MAX((int)inChans - 1, 0);
			outChans = JVX_MAX((int)outChans - 1, 0);
		}
		jvxSize maxChans = JVX_MAX(inChans, outChans);
		jvxSize minChans = JVX_MIN(inChans, outChans);

		// This default function does not tolerate a lot of unexpected settings
		assert(_common_set_ldslave.theData_in->con_params.format == _common_set_ldslave.theData_out.con_params.format);
		assert(_common_set_ldslave.theData_in->con_params.buffersize == _common_set_ldslave.theData_out.con_params.buffersize);

		if (minChans)
		{
			for (ii = 0; ii < maxChans; ii++)
			{
				jvxSize idxIn = ii % inChans;
				jvxSize idxOut = ii % outChans;

				assert(_common_set_ldslave.theData_in->con_params.format == _common_set_ldslave.theData_out.con_params.format);
				jvx_convertSamples_memcpy(
					_common_set_ldslave.theData_in->con_data.buffers[
						*_common_set_ldslave.theData_in->con_pipeline.idx_stage_ptr][idxIn],
							_common_set_ldslave.theData_out.con_data.buffers[
								*_common_set_ldslave.theData_out.con_pipeline.idx_stage_ptr][idxOut],
									jvxDataFormat_size[_common_set_ldslave.theData_in->con_params.format],
									_common_set_ldslave.theData_in->con_params.buffersize);
			}
		}
		if (passThrough)
		{
			if ((inChans > 0) && (outChans > 0))
			{
				memcpy(fieldOutput[outChans], fieldInput[inChans], jvxDataFormat_getsize(_common_set_ldslave.theData_in->con_params.format) * _common_set_ldslave.theData_in->con_params.buffersize);
			}
		}
		return fwd_process_buffers_icon(mt_mask, idx_stage);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAuNChannelRearrange::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:

		// Forward call to base class
		return CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		break;

	default:
		res = CjvxBareNode1ioRearrange::transfer_backward_ocon(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

// ===================================================================
// ===================================================================

jvxErrorType
CjvxAuNChannelRearrange::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	jvxErrorType res = CjvxBareNode1ioRearrange::put_configuration(callMan,
		processor, sectionToContainAllSubsectionsForMe,
		filename, lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			genChannelRearrange_node::put_configuration_all(callMan, processor, sectionToContainAllSubsectionsForMe);
		}
	}
	return res;
}

jvxErrorType
CjvxAuNChannelRearrange::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	std::vector<std::string> warns;
	jvxErrorType res = CjvxBareNode1ioRearrange::get_configuration(callMan, processor, sectionWhereToAddAllSubsections);;
	if (res == JVX_NO_ERROR)
	{
		genChannelRearrange_node::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);
	}
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNChannelRearrange, get_level_pre)
{
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNChannelRearrange, get_level_post)
{
	return JVX_NO_ERROR;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxAuNChannelRearrange, set_passthru)
{
	if (JVX_PROPERTY_CHECK_ID_CAT(ident.id, ident.cat, genChannelRearrange_node::passthrough.active))
	{
		// Actually, we need not do anything here, this macro is considered only in the procesing function
	}
	return JVX_NO_ERROR;
}
