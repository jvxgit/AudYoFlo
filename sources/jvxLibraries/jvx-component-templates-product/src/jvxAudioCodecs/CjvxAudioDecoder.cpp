#include "jvx.h"
#include "jvxAudioCodecs/CjvxAudioDecoder.h"
#include "jvx-helpers-cpp.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

CjvxAudioDecoder::CjvxAudioDecoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
		CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_DECODER);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxAudioDecoder*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set.theInterfaceFactory = static_cast<IjvxInterfaceFactory*>(this);
}

CjvxAudioDecoder::~CjvxAudioDecoder()
{
}

// ==============================================================================

jvxErrorType
CjvxAudioDecoder::activate()
{
	jvxErrorType res = CjvxObject::_activate();
	if (res == JVX_NO_ERROR)
	{
		JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(
			NULL, static_cast<IjvxObject*>(this), "default", NULL,
			_common_set.theModuleName);		
	}
	return(res);
}

// ============================================================================================

jvxErrorType
CjvxAudioDecoder::deactivate()
{
	jvxErrorType res = CjvxObject::_deactivate();
	if (res == JVX_NO_ERROR)
	{
		JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();
	}
	return(res);
}

// ============================================================================================

jvxErrorType
CjvxAudioDecoder::is_ready(jvxBool* suc, jvxApiString* reasonIfNot)
{
	jvxErrorType res = CjvxObject::_is_ready(suc, reasonIfNot);
	if (res == JVX_NO_ERROR)
	{
	}
	return(res);
}

jvxErrorType
CjvxAudioDecoder::prepare()
{
	jvxErrorType res = CjvxObject::_prepare();
	if (res == JVX_NO_ERROR)
	{
	}
	return(res);
}

jvxErrorType
CjvxAudioDecoder::start()
{
	jvxErrorType res = CjvxObject::_start();
	if (res == JVX_NO_ERROR)
	{
	}
	return(res);
}

jvxErrorType
CjvxAudioDecoder::stop()
{
	jvxErrorType res = CjvxObject::_stop();
	if (res == JVX_NO_ERROR)
	{
	}
	return(res);
}

jvxErrorType
CjvxAudioDecoder::postprocess()
{
	jvxErrorType res = CjvxObject::_postprocess();
	if (res == JVX_NO_ERROR)
	{
	}
	return(res);
}

// ===========================================================================
// ===================================================================

jvxErrorType
CjvxAudioDecoder::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	res = neg_input._negotiate_connect_icon(_common_set_icon.theData_in,
		static_cast<IjvxObject*>(this),
		_common_set_io_common.descriptor.c_str()
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		accept_input_parameters();

		// Here would be the place to make sure that the config token is consistent to setting
		res = _test_connect_icon(true, JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

// ===================================================================

jvxErrorType
CjvxAudioDecoder::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	jvxLinkDataDescriptor* ld_cp = nullptr;
	jvxLinkDataDescriptor forward;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:
		ld_cp = (jvxLinkDataDescriptor*)data;
		if (ld_cp)
		{
			if (
				(ld_cp->con_params.rate == _common_set_ocon.theData_out.con_params.rate) &&
				(ld_cp->con_params.number_channels == _common_set_ocon.theData_out.con_params.number_channels) &&
				(ld_cp->con_params.format == _common_set_ocon.theData_out.con_params.format) &&
				(ld_cp->con_params.format_group == _common_set_ocon.theData_out.con_params.format_group))
			{
				if (
					(ld_cp->con_params.buffersize != _common_set_ocon.theData_out.con_params.buffersize))
				{
					forward = *_common_set_icon.theData_in;

					res = updated_backward_format_spec(forward, ld_cp);
					if (res == JVX_NO_ERROR)
					{
						res = _transfer_backward_ocon(true, tp, &forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
						if (res == JVX_NO_ERROR)
						{
							// This accepts the new input arguments
							set_configure_token(_common_set_icon.theData_in->con_params.format_spec.c_str());

							// This does nothing in most cases
							accept_input_parameters();

							res = neg_input._negotiate_connect_icon(
								_common_set_icon.theData_in, 
								static_cast<IjvxObject*>(this),
								_common_set_io_common.descriptor.c_str()
								JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
							if (res == JVX_NO_ERROR)
							{
								// This modifies the output parameters
								test_set_output_parameters();
							}
						}
					}
				}
				else
				{
					res = JVX_NO_ERROR;
				}
			}
			else
			{
				res = JVX_ERROR_UNSUPPORTED;
			}
		}
		break;
	default:
		res = _transfer_backward_ocon(true, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;

}


#ifdef JVX_PROJECT_NAMESPACE
}
#endif
