#include "CjvxSpNLevelMeter.h"
#include "jvx-helpers-cpp.h"

CjvxSpNLevelMeter::CjvxSpNLevelMeter(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareNode1io(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	forward_complain = true;
}

CjvxSpNLevelMeter::~CjvxSpNLevelMeter()
{	
}

jvxErrorType
CjvxSpNLevelMeter::activate()
{
	jvxErrorType res = CjvxBareNode1io::activate();
	if (res == JVX_NO_ERROR)
	{					
		genLevelMeter_node::init_all();
		genLevelMeter_node::allocate_all();
		genLevelMeter_node::register_all(static_cast<CjvxProperties*>(this));
	}
	return res;
}

jvxErrorType
CjvxSpNLevelMeter::deactivate()
{
	jvxErrorType res = CjvxBareNode1io::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		genLevelMeter_node::unregister_all(static_cast<CjvxProperties*>(this));
		genLevelMeter_node::deallocate_all();

		CjvxBareNode1io::deactivate();
	}
	return res;
}

// ===================================================================
// ===================================================================

jvxErrorType
CjvxSpNLevelMeter::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = CjvxBareNode1io::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		// Attach the level meter buffer
		numChannels = _common_set_ldslave.theData_in->con_params.number_channels;
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrLevel, jvxData, numChannels);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrMute, jvxCBool, numChannels);
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrGain, jvxData, numChannels);

		for (i = 0; i < numChannels; i++)
		{
			ptrMute[i] = c_false;
			ptrGain[i] = 1.0;
		}
		genLevelMeter_node::associate__monitor(
			static_cast<CjvxProperties*>(this),
			ptrLevel, numChannels);

		genLevelMeter_node::associate__control(
			static_cast<CjvxProperties*>(this),
			ptrMute, numChannels, 
			ptrGain, numChannels);

	}
	return res;
}

// void copy_measurement(jvxSize cnt, jvxData* ptr);

jvxErrorType
CjvxSpNLevelMeter::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxSize i;
	jvxErrorType res = CjvxBareNode1io::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (res == JVX_NO_ERROR)
	{
		// Detach the level meter
		genLevelMeter_node::deassociate__monitor(static_cast<CjvxProperties*>(this));
		genLevelMeter_node::deassociate__control(static_cast<CjvxProperties*>(this));
		JVX_DSP_SAFE_DELETE_FIELD(ptrLevel);
		JVX_DSP_SAFE_DELETE_FIELD(ptrMute);
		numChannels = 0;
	}
	return res;
}

jvxErrorType
CjvxSpNLevelMeter::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (!genLevelMeter_node::config.bypass.value)
	{
		jvxSize i, j;
		jvxData alpha = genLevelMeter_node::config.smoothing_alpha.value;
		jvxData m1alpha = 1.0 - alpha;
		jvxData** bufsIn = jvx_process_icon_extract_input_buffers<jvxData>(_common_set_ldslave.theData_in, idx_stage);
		for (i = 0; i < numChannels; i++)
		{
			jvxData gain = ptrGain[i];
			if (ptrMute[i])
			{
				memset(bufsIn[i], 0, sizeof(jvxData) * _common_set_ldslave.theData_in->con_params.buffersize);
			}
			for (j = 0; j < _common_set_ldslave.theData_in->con_params.buffersize; j++)
			{
				jvxData tmp = 0;
				bufsIn[i][j] *= gain;
				tmp = bufsIn[i][j];
				tmp *= tmp;
				ptrLevel[i] = alpha * ptrLevel[i] + m1alpha * tmp;
			}
		}
	}

	// Forward the processed data to next chain element
	res = _process_buffers_icon(mt_mask, idx_stage);
	return res;
}

// =====================================================================================

jvxErrorType 
CjvxSpNLevelMeter::put_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxErrorType res = CjvxBareNode1io::put_configuration(
		callMan,
		processor,
		sectionToContainAllSubsectionsForMe,
		filename,
		lineno);
	if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{
			std::vector<std::string> warns;
			genLevelMeter_node::put_configuration_all(
				callMan,
				processor,
				sectionToContainAllSubsectionsForMe,
				&warns);
			JVX_PROPERTY_PUT_CONFIGRATION_WARNINGS_STD_COUT(warns);			
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSpNLevelMeter::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxBareNode1io::get_configuration(
		callMan,
		processor,
		sectionWhereToAddAllSubsections);
	if (res == JVX_NO_ERROR)
	{
		genLevelMeter_node::get_configuration_all(
			callMan,
			processor,
			sectionWhereToAddAllSubsections);
	}
	return JVX_NO_ERROR;
}

