#include "mexJvxHost.h"
#include "jvxAudioHost_common.h"

#ifdef JVX_MATLAB_RT_AUDIO
#ifdef JVX_OS_WINDOWS
#include "jvxAuTAsio.h"
#endif

#ifdef JVX_OS_LINUX
#include "jvxAuTAlsa.h"
#endif

#ifdef JVX_OS_MACOSX
#include "jvxAuTCoreAudio.h"
#endif

#ifdef JVX_USE_PORTAUDIO
#include "jvxAuTPortAudio.h"
#endif

#include "jvxAuTGenericWrapper.h"
#endif

#ifdef JVX_MATLAB_OFFLINE_AUDIO
#include "jvxAuTOfflineAudio.h"
#endif

jvxErrorType
mexJvxHost::initialize_specific()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	oneAddedStaticComponent comp;
#if 0
	/*
	* Add the configuration lines here
	*/
	IjvxConfigurationLine* theConfigLines = NULL;
	
	res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATIONLINE, reinterpret_cast<jvxHandle**>(&theConfigLines));
	if((res == JVX_NO_ERROR) && theConfigLines)
	{
		theConfigLines->add_line(JVX_COMPONENT_AUDIO_DEVICE, "Use the audio parameters from the audio device in the audio node and the camera");
		theConfigLines->add_slave_line(JVX_COMPONENT_AUDIO_DEVICE, JVX_COMPONENT_AUDIO_NODE);

		for(i = 0; i < JVX_NUM_CONFIGURATIONLINE_PROPERTIES_AUDIO; i++)
		{
			jvxSize id = 0;
			if(jvx_findPropertyDescriptor(jvxProperties_configLine_audio[i], &id, NULL, NULL, NULL))
			{
				theConfigLines->add_configuration_property_line(JVX_COMPONENT_AUDIO_DEVICE, id);
			}
		}

		theConfigLines->add_line(JVX_COMPONENT_VIDEO_DEVICE, "Use the audio parameters from the audio device in the audio node and the camera");
		theConfigLines->add_slave_line(JVX_COMPONENT_VIDEO_DEVICE, JVX_COMPONENT_VIDEO_NODE);

		for (i = 0; i < JVX_NUM_CONFIGURATIONLINE_PROPERTIES_VIDEO; i++)
		{
			jvxSize id = 0;
			if (jvx_findPropertyDescriptor(jvxProperties_configLine_video[i], &id, NULL, NULL, NULL))
			{
				theConfigLines->add_configuration_property_line(JVX_COMPONENT_VIDEO_DEVICE, id);
			}
		}
	}


	/*
	* Add the types to be handled by host
	*/
	IjvxHostTypeHandler* theTypeHandler = NULL;
	res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, reinterpret_cast<jvxHandle**>(&theTypeHandler));
	if((res == JVX_NO_ERROR) && theTypeHandler)
	{
		jvxComponentType tp[2];

		tp[0] = JVX_COMPONENT_AUDIO_TECHNOLOGY;
		tp[1] = JVX_COMPONENT_AUDIO_DEVICE;
		res = theTypeHandler->add_type_host(tp, 2, "Rtproc Audio Technologies", "audio", JVX_COMPONENT_TYPE_TECHNOLOGY, 1, 1);
		assert(res == JVX_NO_ERROR);

		tp[0] = JVX_COMPONENT_APPLICATION_CONTROLLER_TECHNOLOGY;
		tp[1] = JVX_COMPONENT_APPLICATION_CONTROLLER_DEVICE;
		res = theTypeHandler->add_type_host(tp, 2, "Rtproc External Controller Technologies", "ex_control", JVX_COMPONENT_TYPE_TECHNOLOGY, 1, 1);
		assert(res == JVX_NO_ERROR);

		tp[0] = JVX_COMPONENT_AUDIO_NODE;
		tp[1] = JVX_COMPONENT_UNKNOWN;
		res = theTypeHandler->add_type_host(tp, 1, "Rtproc Audio Node", "audio_node", JVX_COMPONENT_TYPE_NODE, 1, 1);
		assert(res == JVX_NO_ERROR);

		/*			theTypeHandler->add_type_host(JVX_COMPONENT_SCANNER_TECHNOLOGY, "", "camera", JVX_COMPONENT_TYPE_TECHNOLOGY);
		theTypeHandler->add_type_host(JVX_COMPONENT_SCANNER_TECHNOLOGY, "", "lamp", JVX_COMPONENT_TYPE_TECHNOLOGY);
		theTypeHandler->add_type_host(JVX_COMPONENT_SCANNER_TECHNOLOGY, "", "spectrum_processor", JVX_COMPONENT_TYPE_TECHNOLOGY);

		theTypeHandler->add_type_host(JVX_COMPONENT_SCANNER_TECHNOLOGY, "", "", JVX_COMPONENT_TYPE_ENDPOINT);
		*/
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, reinterpret_cast<jvxHandle*>(theTypeHandler));
	}
#endif

	// Do not allow that host components are loaded via DLL
#ifdef JVX_MATLAB_OFFLINE_AUDIO
	LOAD_ONE_MODULE_LIB_FULL(jvxAuTOfflineAudio_init, jvxAuTOfflineAudio_terminate, "Offline Audio", involvedComponents.addedStaticObjects, involvedComponents.theHost.hFHost);
#endif

#ifdef JVX_MATLAB_RT_AUDIO
#ifdef JVX_OS_WINDOWS
	LOAD_ONE_MODULE_LIB_FULL(jvxAuTAsio_init, jvxAuTAsio_terminate, "Asio Audio", involvedComponents.addedStaticObjects, involvedComponents.theHost.hFHost);
#else

#ifdef JVX_OS_LINUX
	LOAD_ONE_MODULE_LIB_FULL(jvxAuTAlsa_init, jvxAuTAlsa_terminate, "Alsa Audio", involvedComponents.addedStaticObjects, involvedComponents.theHost.hFHost);
#endif

#ifdef JVX_OS_MACOSX
    LOAD_ONE_MODULE_LIB_FULL(jvxAuTCoreAudio_init, jvxAuTCoreAudio_terminate, "Core Audio", involvedComponents.addedStaticObjects, involvedComponents.theHost.hFHost);
#endif

#endif

#ifdef JVX_USE_PORTAUDIO
	LOAD_ONE_MODULE_LIB_FULL(jvxAuTPortAudio_init, jvxAuTPortAudio_terminate, "PortAudio Audio", involvedComponents.addedStaticObjects, involvedComponents.theHost.hFHost);
#endif

	LOAD_ONE_MODULE_LIB_FULL(jvxAuTGenericWrapper_init, jvxAuTGenericWrapper_terminate, "Generic Wrapper Audio", involvedComponents.addedStaticObjects, involvedComponents.theHost.hFHost);

#endif

	involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_HOST);
#if defined(JVX_MATLAB_RT_AUDIO) || defined(JVX_MATLAB_OFFLINE_AUDIO)
	involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_AUDIO_TECHNOLOGY);
#endif
	return(res);
}

jvxErrorType
mexJvxHost::terminate_specific()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	// Do not allow that host components are loaded via DLL
	involvedComponents.theHost.hFHost->remove_component_load_blacklist(JVX_COMPONENT_HOST);

#if defined(JVX_MATLAB_RT_AUDIO) || defined(JVX_MATLAB_OFFLINE_AUDIO)
	involvedComponents.theHost.hFHost->remove_component_load_blacklist(JVX_COMPONENT_AUDIO_TECHNOLOGY);
#endif

	// Remove all library objects
	for(i = 0; i < involvedComponents.addedStaticObjects.size(); i++)
	{
		UNLOAD_ONE_MODULE_LIB_FULL(involvedComponents.addedStaticObjects[i], involvedComponents.theHost.hFHost);
	}

	involvedComponents.addedStaticObjects.clear();

#if 0
		/*
	* Add the types to be handled by host
	*/
	IjvxHostTypeHandler* theTypeHandler = NULL;
	res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, reinterpret_cast<jvxHandle**>(&theTypeHandler));
	if((res == JVX_NO_ERROR) && theTypeHandler)
	{
		res = theTypeHandler->remove_all_host_types();
		assert(res == JVX_NO_ERROR);
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, reinterpret_cast<jvxHandle*>(theTypeHandler));
	}
#endif
	return(res);
}

jvxErrorType
mexJvxHost::st_process_msg_callback(jvxHandle* theField, jvxSize szField, jvxCBitField elementType, jvxHandle* theContext)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theContext)
	{
		mexJvxHost* this_pointer = (mexJvxHost*)theContext;
		return(this_pointer->ic_process_msg_callback(theField, szField, elementType));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
mexJvxHost::ic_process_msg_callback(jvxHandle* theField, jvxSize szField, jvxCBitField elementType)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxInt32* subId = NULL;

	if(externalCall.fromHost.theHdl)
	{
		std::string command = msg_queue.callback_prefix + "(";
		command = command + jvx_int2String((int)elementType);

		if (jvx_cbitTest(elementType, JVX_REPORT_REQUEST_TEST_CHAIN_MASTER_SHIFT))
		{
			jvxSize procId = (jvxSize)theField;
			command = command + ", " + jvx_size2String(procId);
			command += ", []";
		}
		else if (jvx_cbitTest(elementType, JVX_REPORT_REQUEST_USER_MESSAGE_SHIFT))
		{
			subId = (jvxInt32*)theField;
			if (subId)
			{
				command = command + ", " + jvx_int2String((int)*subId);
			}
			else
			{
				command = command + ", -1";
			}
			command += ", []";
		}
		else
		{
			command += ", -1, []";
		}

		command += ");";

		try
		{
			resL = externalCall.fromHost.theHdl->executeExternalCommand(command.c_str());
			if(resL != JVX_NO_ERROR)
			{
				jvxApiString errDescr;
				externalCall.fromHost.theHdl->getLastErrorReason(&errDescr);
				externalCall.fromHost.theHdl->postMessageExternal(("Warning: Execution of command <" + command + "> failed, error: " + errDescr.std_str() + ".\n").c_str(), false);
				res = JVX_ERROR_CALL_SUB_COMPONENT_FAILED;
			}
		}
		catch(...)
		{
			externalCall.fromHost.theHdl->postMessageExternal(("Command <" + command + "> aborted.\n").c_str(), false);
			res = JVX_ERROR_ABORT;
		}
	}
	else
	{
		res = JVX_ERROR_NOT_READY;
	}
	return(res);
}

jvxErrorType
mexJvxHost::st_remove_msg_callback(jvxHandle* theField, jvxSize szField, jvxCBitField elementType, jvxHandle* theContext)
{
	jvxErrorType res = JVX_NO_ERROR;
	if(theContext)
	{
		mexJvxHost* this_pointer = (mexJvxHost*)theContext;
		return(this_pointer->ic_remove_msg_callback(theField, szField, elementType));
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxErrorType
mexJvxHost::ic_remove_msg_callback(jvxHandle* theField, jvxSize szField, jvxCBitField elementType)
{
	jvxInt32* subId = NULL;
	if(jvx_cbitTest(elementType, JVX_REPORT_REQUEST_USER_MESSAGE_SHIFT))
	{
		subId = (jvxInt32*)theField;
		JVX_SAFE_DELETE_OBJ(subId);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::add_element_to_queue(const mxArray* prhs[], int nrhs, std::string& error)
{
	jvxInt32 subIdMessage = -1;
	jvxInt32* subId = NULL;

	const mxArray* arr = prhs[0];
	if(mxIsDouble(arr))
	{
		subIdMessage = ((int)*((double*)mxGetData(arr)));
	}
	else if(mxIsSingle(arr))
	{
		subIdMessage = ((int)*((float*)mxGetData(arr)));
	}
	else if(mxIsInt32(arr))
	{
		subIdMessage = *((int*)mxGetData(arr));
	}

	if(subIdMessage < 0)
	{
		error = "Invalid argument #1 for message queue element to be added";
		return(JVX_ERROR_INVALID_ARGUMENT);
	}

	JVX_SAFE_NEW_OBJ(subId, jvxInt32);
	*subId = subIdMessage;

	return(add_element_queue((jvxCBitField)1 << JVX_REPORT_REQUEST_USER_MESSAGE_SHIFT, subId, JVX_SIZE_UNSELECTED, st_remove_msg_callback, reinterpret_cast<jvxHandle*>(this)));
}


jvxErrorType
mexJvxHost::lookup_type_id__name_specific(std::vector<std::string>& strLst, std::string& category,
	std::string tp, int nlhs, mxArray* plhs[], bool return_options_lookup, bool return_options_names)
{
	return(JVX_ERROR_UNSUPPORTED);
}

jvxErrorType 
mexJvxHost::lookup_const__name_specific(std::vector<std::string>& strLst, std::string& category, int nlhs, mxArray* plhs[], bool return_options_lookup)
{
	if (return_options_lookup)
	{
		strLst.push_back("jvxPropertyLookaheadSizes");
		return(JVX_NO_ERROR);
	}
	if(category == "jvxPropertyLookaheadSizes")
	{ 
		if (nlhs > 1)
		{
			this->mexReturnInt32List(plhs[1], tableLookaheadBuffers, NUM_LOOKAHEAD_BUFFERS);
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_UNSUPPORTED);
}
