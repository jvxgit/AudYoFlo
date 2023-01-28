#include "jvx.h"
#include "jvxAudioNodes/CjvxAuN2AudioMixer.h"



CjvxAuN2AudioMixer::CjvxAuN2AudioMixer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxNVTasks(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_AUDIO_NODE);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);	
	// mixBufferChannels = JVX_SIZE_UNSELECTED;
	
	// Add the primary mixbuffer - open minded for all parameters
	oneBufferDefinition bufDef;
	bufDef.id = 0; 
	bufDef.description = "Primary Buffer";
	bufDef.format = JVX_DATAFORMAT_DATA;
	bufDef.numChannels_fixed = 2; // <- Default: stereo output buffers
	mixBuffers[bufDef.id] = bufDef;
}

CjvxAuN2AudioMixer::~CjvxAuN2AudioMixer()
{
	
}

jvxErrorType 
CjvxAuN2AudioMixer::activate()
{
	jvxErrorType res = CjvxNVTasks::activate();
	if(res == JVX_NO_ERROR)
	{
		gen2AudioMixer_node::init_all();
		gen2AudioMixer_node::allocate_all();
		gen2AudioMixer_node::register_all(static_cast<CjvxProperties*>(this));

		gen2AudioMixer_node::register_callbacks(static_cast<CjvxProperties*>(this),
			set_extend_ifx_reference, 
			update_level_get, update_level_set,
			update_level_get, update_level_set,
			clear_storage, address_profiles,
			reinterpret_cast<jvxHandle*>(this),
			nullptr);

	}
	return(res);
};

jvxErrorType 
CjvxAuN2AudioMixer::deactivate()
{
	jvxErrorType res = CjvxNVTasks::deactivate();
	if(res == JVX_NO_ERROR)
	{
		gen2AudioMixer_node::unregister_callbacks(static_cast<CjvxProperties*>(this), nullptr);
		gen2AudioMixer_node::unregister_all(static_cast<CjvxProperties*>(this));
		gen2AudioMixer_node::deallocate_all();

	}
	return(res);
};

jvxErrorType
CjvxAuN2AudioMixer::terminate()
{
	for (auto& elmI : inputChannelsInStorage)
	{
		for (auto& elmIc : elmI.second)
		{
			/*
			if (extender) extender->deallocateAttachChannelSpecific(elmIc.attSpecificPtr, true);
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
			elmIc.attSpecificPtr = nullptr;
#endif
			*/
		}
	}

	for (auto& elmO : outputChannelsInStorage)
	{
		for (auto& elmOc : elmO.second)
		{
			/*
			if (extender) extender->deallocateAttachChannelSpecific(elmOc.attSpecificPtr, false);
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
			elmOc.attSpecificPtr = nullptr;
#endif
			*/
		}
	}

	inputChannelsInStorage.clear();
	outputChannelsInStorage.clear();
	profileList.clear();

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAuN2AudioMixer::connect_connect_ntask(
	jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out,
	jvxSize idTask)
{
	jvxErrorType res = CjvxNVTasks::connect_connect_ntask(theData_in,
		theData_out, idTask);
	if (res == JVX_NO_ERROR)
	{
		jvxSize uId = theData_in->con_link.uIdConn;
		auto elmI = registeredChannelListInput.find(uId);
		if (elmI == registeredChannelListInput.end())
		{
			oneEntryProcessChannelList elmNew;
			elmNew.uIdConn = uId;
			registeredChannelListInput[uId] = elmNew;
		}
		auto elmO = registeredChannelListOutput.find(uId);
		if (elmO == registeredChannelListOutput.end())
		{
			oneEntryProcessChannelList elmNew;
			elmNew.uIdConn = uId;
			registeredChannelListOutput[uId] = elmNew;
		}
	}
	return res;
}

jvxErrorType
CjvxAuN2AudioMixer::disconnect_connect_ntask(
	jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out,
	jvxSize idTask)
{
	jvxErrorType res = CjvxNVTasks::disconnect_connect_ntask(theData_in,
		theData_out, idTask);
	if (res == JVX_NO_ERROR)
	{
		jvxSize uId = theData_in->con_link.uIdConn;
		auto elmI = registeredChannelListInput.find(uId);
		if (elmI != registeredChannelListInput.end())
		{
			for (auto& elmIc : elmI->second.channels)
			{
				/*
				if (extender) extender->deallocateAttachChannelSpecific(elmIc.attSpecificPtr, true);
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
				elmIc.attSpecificPtr = nullptr;
#endif
				*/
			}
			registeredChannelListInput.erase(uId);
		}

		auto elmO = registeredChannelListOutput.find(uId);
		if (elmO != registeredChannelListOutput.end())
		{
			for (auto& elmOc : elmO->second.channels)
			{
				/*
				if (extender) extender->deallocateAttachChannelSpecific(elmOc.attSpecificPtr, false);
#ifdef JVX_AUDIOMIXER_DEBUG_SPECIFIC
				elmOc.attSpecificPtr = nullptr;
#endif
				*/
			}
			registeredChannelListOutput.erase(uId);
		}
		new_setup_to_properties(true, true);
	}
	return res;
}

// ====================================================================================================
// =====================================================================================================

jvxErrorType
CjvxAuN2AudioMixer::connect_connect_icon_vtask(
	jvxSize ctxtId,
	jvxSize ctxtSubId
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = CjvxNVTasks::connect_connect_icon_vtask(ctxtId,
		ctxtSubId
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

		std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* > ocon;
		oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* icon = NULL;

		res = find_i_map(ctxtId, ctxtSubId, icon, ocon);
		if (res == JVX_NO_ERROR)
		{
			jvxLinkDataDescriptor* datIn = icon->con->_common_set_icon_nvtask.theData_in;
			jvxLinkDataDescriptor* datOut = nullptr;
			if (ocon.size())
			{
				auto elmCon = ocon.begin();
				if ((*elmCon)->con)
				{
					datOut = &(*elmCon)->con->_common_set_ocon_nvtask.theData_out;
				}
			}

			jvxSize uId = datIn->con_link.uIdConn;
			auto elmI = registeredChannelListInput.find(uId);
			if (elmI == registeredChannelListInput.end())
			{
				oneEntryProcessChannelList elmNew;
				elmNew.uIdConn = uId;
				registeredChannelListInput[uId] = elmNew;
			}

			if (datOut)
			{
				uId = datOut->con_link.uIdConn;
			}

			auto elmO = registeredChannelListOutput.find(uId);
			if (elmO == registeredChannelListOutput.end())
			{
				oneEntryProcessChannelList elmNew;
				elmNew.uIdConn = uId;
				registeredChannelListOutput[uId] = elmNew;
			}
		}
	}
	return res;
}

jvxErrorType
CjvxAuN2AudioMixer::disconnect_connect_icon_vtask(
	jvxSize ctxtId,
	jvxSize ctxtSubId
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = CjvxNVTasks::disconnect_connect_icon_vtask(
		ctxtId,
		ctxtSubId
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res == JVX_NO_ERROR)
	{
		jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

		std::list<oneConnectorVTask_use<CjvxOutputConnectorVtask, CjvxNegotiate_output>* > ocon;
		oneConnectorVTask_use<CjvxInputConnectorVtask, CjvxNegotiate_input>* icon = NULL;

		res = find_i_map(ctxtId, ctxtSubId, icon, ocon);
		if (res == JVX_NO_ERROR)
		{
			jvxLinkDataDescriptor* datIn = icon->con->_common_set_icon_nvtask.theData_in;
			jvxLinkDataDescriptor* datOut = nullptr;
			if (ocon.size())
			{
				auto elmCon = ocon.begin();
				datOut = &(*elmCon)->con->_common_set_ocon_nvtask.theData_out;
			}

			jvxSize uId = datIn->con_link.uIdConn;
			auto elmI = registeredChannelListInput.find(uId);
			if (elmI != registeredChannelListInput.end())
			{
				registeredChannelListInput.erase(uId);
			}

			// Here, we use the uId from the input side. The uId from the output side 
			// is reset on <CjvxAuN2AudioMixer::disconnect_connect_icon_vtask> since the 
			// connected branch is already disconnected.
			// Alternatively, we could also store the uId before the call - but we would need more code
			// to split up input and output side.
			// uId = datOut->con_link.uIdConn;

			auto elmO = registeredChannelListOutput.find(uId);
			if (elmO != registeredChannelListOutput.end())
			{
				registeredChannelListOutput.erase(uId);
			}

			// once we have disconnected a device, we need to update the exposed channel list
			new_setup_to_properties(true, true);
		}
	}
	return res;
}
// ===========================================================================================

