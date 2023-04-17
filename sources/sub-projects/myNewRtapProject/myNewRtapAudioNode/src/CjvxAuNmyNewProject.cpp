#include "jvx.h"
#include "CjvxAuNmyNewProject.h"
#include "mcg_export_project.h"

CjvxAuNmyNewProject::CjvxAuNmyNewProject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxAudioNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set_node_params_1io.preferred.format.min = JVX_DATAFORMAT_DATA;
	_common_set_node_params_1io.preferred.format.max = JVX_DATAFORMAT_32BIT_LE;
	_theExtCallObjectName = "";

	_common_set_ldslave.num_min_buffers_in = 1;
	_common_set_ldslave.num_min_buffers_out = 1;
	_common_set_ldslave.num_additional_pipleline_stages = 0;
}

CjvxAuNmyNewProject::~CjvxAuNmyNewProject()
{
}

#ifdef JVX_EXTERNAL_CALL_ENABLED
JVX_MEXCALL_SELECT_REGISTER(CjvxAuNmyNewProject, CjvxAudioNode, _theExtCallObjectName)
JVX_MEXCALL_UNSELECT_UNREGISTER(CjvxAuNmyNewProject, CjvxAudioNode, _theExtCallObjectName)
JVX_MEXCALL_PREPARE(CjvxAuNmyNewProject, CjvxAudioNode)
JVX_MEXCALL_POSTPROCESS(CjvxAuNmyNewProject, CjvxAudioNode)
JVX_MEXCALL_PREPARE_SENDER_TO_RECEIVER(CjvxAuNmyNewProject, CjvxAudioNode)
JVX_MEXCALL_PREPARE_COMPLETE_RECEIVER_TO_SENDER(CjvxAuNmyNewProject, CjvxAudioNode, &_common_set_ocon.theData_out)
JVX_MEXCALL_POSTPROCESS_SENDER_TO_RECEIVER(CjvxAuNmyNewProject, CjvxAudioNode)
JVX_MEXCALL_BEFORE_POSTPROCESS_RECEIVER_TO_SENDER(CjvxAuNmyNewProject, CjvxAudioNode)

jvxErrorType
CjvxAuNmyNewProject::put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, 
	jvxInt32 lineno)
{
	std::vector<std::string> warnings;
	jvxErrorType res = CjvxAudioNode::put_configuration(callConf, processor, sectionToContainAllSubsectionsForMe, filename, lineno);
	if (res != JVX_NO_ERROR)
		return res;
	res = CjvxMexCalls::put_configuration(callConf, processor, this->_common_set_min.theState, sectionToContainAllSubsectionsForMe, filename, lineno);

	put_configuration__properties_active(callConf, processor, sectionToContainAllSubsectionsForMe, &warnings);

	this->_report_command_request((jvxCBitField)1 << JVX_REPORT_REQUEST_UPDATE_WINDOW_SHIFT);
	return res;
}

jvxErrorType
CjvxAuNmyNewProject::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxAudioNode::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	if (res != JVX_NO_ERROR)
		return res;
	res = CjvxMexCalls::get_configuration(callConf, processor, sectionWhereToAddAllSubsections);
	get_configuration__properties_active(callConf, processor, sectionWhereToAddAllSubsections);
	return res;
}
#else

jvxErrorType
CjvxAuNmyNewProject::put_configuration(IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	std::vector<std::string> warnings_properties;
	jvxErrorType res = CjvxAudioNode::put_configuration(processor,
		sectionToContainAllSubsectionsForMe,
		filename,
		lineno);
	if (res == JVX_NO_ERROR)
	{
		put_configuration__properties_active(processor, sectionToContainAllSubsectionsForMe, warnings_properties);
	}
	return(res);
}

jvxErrorType
CjvxAuNmyNewProject::get_configuration(IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = CjvxAudioNode::get_configuration(processor,
		sectionWhereToAddAllSubsections);

	if (res == JVX_NO_ERROR)
	{
		get_configuration__properties_active(processor, sectionWhereToAddAllSubsections);
	}
	return(res);
}

#endif

jvxErrorType 
CjvxAuNmyNewProject::activate()
{
	jvxErrorType res = CjvxAudioNode::activate();
	if(res == JVX_NO_ERROR)
	{
		this->myNewProject = new jvx_myNewProject;

#ifdef JVX_EXTERNAL_CALL_ENABLED
		res = CjvxMexCalls::activate();
#endif
		genSimple_node::init__properties_active();
		genSimple_node::allocate__properties_active();
		genSimple_node::register__properties_active(static_cast<CjvxProperties*>(this)); 

		associate__properties_active(static_cast<CjvxProperties*>(this), &(this->myNewProject->myVolume), 1);

		jvxrtst << "Hello World, console" << std::endl;
	}
	return(res);
};

jvxErrorType 
CjvxAuNmyNewProject::deactivate()
{
	jvxErrorType res = CjvxAudioNode::deactivate();
	if(res == JVX_NO_ERROR)
	{
		genSimple_node::unregister__properties_active(static_cast<CjvxProperties*>(this));
		genSimple_node::deallocate__properties_active();
#ifdef JVX_EXTERNAL_CALL_ENABLED
		res = CjvxMexCalls::deactivate();
#endif
		deassociate__properties_active(static_cast<CjvxProperties*>(this));
		delete this->myNewProject;

	}
	return(res);
};

jvxErrorType 
CjvxAuNmyNewProject::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)
{

	jvxErrorType res = JVX_NO_ERROR;
	jvxBool extCallPres = false;
	jvxBool matlabEngaged = false;
	
#ifdef JVX_EXTERNAL_CALL_ENABLED

	res = CjvxMexCalls::is_extcall_reference_present(&extCallPres);
	if (extCallPres)
	{
		CjvxMexCalls::is_matlab_processing_engaged(&matlabEngaged);
	}

	if (matlabEngaged)
	{
		// Pass control to Matlab processing
		res = CjvxMexCalls::process_st(theData, idx_sender_to_receiver, idx_receiver_to_sender);
	}
	else
	{
#endif
		// This default function does not tolerate a lot of unexpected settings
		assert(theData->con_params.format == theData->con_compat.format);
		assert(theData->con_params.buffersize == theData->con_compat.buffersize);

		if (theData->con_params.format == JVX_DATAFORMAT_DATA)
		{
			res = process_buffer(
				theData->con_data.buffers[idx_sender_to_receiver],
				theData->con_compat.from_receiver_buffer_allocated_by_sender[idx_receiver_to_sender],
				theData->con_params.buffersize,
				theData->con_params.number_channels,
				theData->con_compat.number_channels,
				theData->con_params.format);
		}
		else
		{
			res = CjvxAudioNode::process_st(theData, idx_sender_to_receiver, idx_receiver_to_sender);
		}
#ifdef JVX_EXTERNAL_CALL_ENABLED
	}
#endif

	return(res);
};

jvxErrorType 
CjvxAuNmyNewProject::process_buffer(jvxHandle** buffers_input, jvxHandle** buffers_output, jvxSize bSize,
		jvxSize numChans_input,jvxSize numChans_output,jvxDataFormat format)
{
	jvxSize i, j;
	jvxSize chanMax = 0;
	jvxData gain = properties_active.output_gain.value.val();
	jvxDspBaseErrorType resD = myNewProject->process((jvxData**) buffers_input, (jvxData**) buffers_output, numChans_input, numChans_output, bSize);
	if (properties_active.output_mute.value)
	{
		gain = 0.0;
	}

	if (resD == JVX_NO_ERROR)
	{
		switch (format)
		{
		case JVX_DATAFORMAT_DATA:
			for (i = 0; i < numChans_output; i++)
			{
				jvxData* outbuf = (jvxData*)buffers_output[i];

				for (j = 0; j < bSize; j++)
				{
					outbuf[j] *= gain;
				}
			}
			break;
		}
	}
	else
	{
		return JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
	}
	return JVX_NO_ERROR;
}

#ifdef JVX_EXTERNAL_CALL_ENABLED

jvxErrorType 
CjvxAuNmyNewProject::myFirstMatlabEntry()
{
	return(JVX_NO_ERROR);
}

#endif



