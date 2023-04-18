#include "jvx.h"
#include "jvxAudioCodecs/CjvxAudioEncoder.h"
#include "jvx-helpers-cpp.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

CjvxAudioEncoder::CjvxAudioEncoder(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_ENCODER);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxAudioEncoder*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set.theInterfaceFactory = static_cast<IjvxInterfaceFactory*>(this);	
}

CjvxAudioEncoder::~CjvxAudioEncoder()
{
}

// ==============================================================================
jvxErrorType 
CjvxAudioEncoder::activate()
{
	jvxErrorType res = CjvxObject::_activate();
	if (res == JVX_NO_ERROR)
	{
		JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(NULL, static_cast<IjvxObject*>(this), "default", NULL, _common_set.theModuleName);

		// Activate by calling specific function
		this->activate_encoder();

		// Set the codec parameters based on the properties
		// Default: accept JVX_DATAFORMAT_DATA with JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED
		neg_input._set_parameters_fixed(
			JVX_SIZE_UNSELECTED, 
			JVX_SIZE_UNSELECTED,
			JVX_SIZE_UNSELECTED,
			JVX_DATAFORMAT_DATA,
			JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED);

		// Output side: pin to bytes and CODED_GENERIC"
		neg_output._set_parameters_fixed(
			JVX_SIZE_UNSELECTED,
			JVX_SIZE_UNSELECTED,
			JVX_SIZE_UNSELECTED,
			JVX_DATAFORMAT_BYTE, JVX_DATAFORMAT_GROUP_AUDIO_CODED_GENERIC);
	}
	return(res);
}

jvxErrorType 
CjvxAudioEncoder::activate_encoder()
{
	return JVX_NO_ERROR;
}

// ======================================================================================

jvxErrorType
CjvxAudioEncoder::deactivate()
{
	jvxErrorType res = CjvxObject::_deactivate();
	if (res == JVX_NO_ERROR)
	{
		this->deactivate_encoder();

		JVX_DEACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS();
	}
	return(res);
}

jvxErrorType
CjvxAudioEncoder::deactivate_encoder()
{
	return JVX_NO_ERROR;
}

// ==========================================================================================

jvxErrorType 
CjvxAudioEncoder::is_ready(jvxBool* suc, jvxApiString* reasonIfNot)
{
	jvxErrorType res = CjvxObject::_is_ready(suc, reasonIfNot);
	return(res);
}

jvxErrorType
CjvxAudioEncoder::prepare()
{
	jvxErrorType res = CjvxObject::_prepare();
	return(res);
}

jvxErrorType
CjvxAudioEncoder::start()
{
	jvxErrorType res = CjvxObject::_start();
	return(res);
}

jvxErrorType
CjvxAudioEncoder::stop()
{
	jvxErrorType res = CjvxObject::_stop();
	return(res);
}

jvxErrorType
CjvxAudioEncoder::postprocess()
{
	jvxErrorType res = CjvxObject::_postprocess();
	return(res);
}

// ===================================================================

jvxErrorType
CjvxAudioEncoder::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	res = neg_input._negotiate_connect_icon(_common_set_icon.theData_in,
		static_cast<IjvxObject*>(this),
		_common_set_io_common.descriptor.c_str()
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		accept_input_parameters();

		// Forward the call
		res = _test_connect_icon(true, JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res == JVX_NO_ERROR)
		{
		}
	}
	return res;
}

// ===================================================================
jvxErrorType
CjvxAudioEncoder::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	jvxLinkDataDescriptor* ld_cp = nullptr;
	jvxApiString famToken;
	switch (tp)
	{
	case JVX_LINKDATA_TRANSFER_COMPLAIN_DATA_SETTINGS:
		ld_cp = (jvxLinkDataDescriptor*)data;
		if (ld_cp)
		{
			jvxLinkDataDescriptor forward;
			jvxBool forwardRequest = false;
			res = check_backward_parameters(ld_cp, forward, forwardRequest);

			if (forwardRequest)
			{
				res = _common_set_icon.theData_in->con_link.connect_from->transfer_backward_ocon(
					tp, &forward JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
				if (res == JVX_NO_ERROR)
				{
					// Here, we need to refresh all connection parameters.
					// We do take a shortcut here and do not run the negotiation					
					res = neg_input._negotiate_connect_icon(_common_set_icon.theData_in,
						static_cast<IjvxObject*>(this),
						_common_set_io_common.descriptor.c_str()
						JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
					if (res == JVX_NO_ERROR)
					{
						// Large version of the update: input and output update
						accept_input_parameters();

					}
				}
			}

			if(res == JVX_NO_ERROR)
			{
				// This function is called if parameters could be adaptwed locally (res == JVX_NO_ERROR)
				// or forwardRequest is true and res is then corrected from ERROR to JVX_NO_ERROR
				test_set_output_parameters();
			}
		}
		break;
	default:
		res = _transfer_backward_ocon(true, tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	return res;
}

// ===================================================================

#ifdef JVX_PROJECT_NAMESPACE
}
#endif
