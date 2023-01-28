#include "CjvxVstPluginExtender.h"

jvxErrorType 
CjvxVstPluginExtender::set_runtime_reference(IjvxAudioPluginRuntime* refRt)
{
	if (rt_ext == nullptr)
	{
		rt_ext = refRt;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}

jvxErrorType 
CjvxVstPluginExtender::unset_runtime_reference()
{
	rt_ext = nullptr;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxVstPluginExtender::associate_processing_data(Steinberg::Vst::ProcessData* data)
{
	if (temp_store == nullptr)
	{
		temp_store = data;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxVstPluginExtender::deassociate_processing_data(Steinberg::Vst::ProcessData* data)
{
	if (temp_store == data)
	{
		temp_store = nullptr;
	}
	return JVX_NO_ERROR;
}

// =====================================================================

jvxErrorType 
CjvxVstPluginExtender::process_input_parameters()
{
	Steinberg::Vst::IParameterChanges* paramChanges = temp_store->inputParameterChanges;
	if (paramChanges)
	{
		jvxInt32 numParamsChanged = paramChanges->getParameterCount();
		// for each parameter which are some changes in this audio block:
		for (jvxSize i = 0; i < numParamsChanged; i++)
		{
			ext_access_input = paramChanges->getParameterData((Steinberg::int32)i);
			if (ext_access_input)
			{
				Steinberg::int32 sampleOffset = 0;
				Steinberg::Vst::ParamID pId = ext_access_input->getParameterId();
				auto elm = registered_listeners.begin();
				for (; elm != registered_listeners.end(); elm++)
				{
					auto elmId = elm->second.registered_ids.find(pId);
					if (elmId != elm->second.registered_ids.end())
					{
						jvxData normVal = 0;
						Steinberg::Vst::ParamValue value;
						jvxInt32 numPoints = ext_access_input->getPointCount();
						if (ext_access_input->getPoint(numPoints - 1, sampleOffset, value) ==
							Steinberg::kResultTrue)
						{
							normVal = value;
							elmId->second.ref->report_input_parameter(pId, normVal, 
								static_cast<IjvxAudioPluginDetailsParameter*>(this));
						}
					}
				}
			}
			ext_access_input = nullptr;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxVstPluginExtender::process_input_events()
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxVstPluginExtender::process_output_parameters()
{
	//---3) Write outputs parameter changes-----------
	if (ext_access_output == nullptr)
	{
		ext_access_output = temp_store->outputParameterChanges;

		auto elm = registered_listeners.begin();
		for (; elm != registered_listeners.end(); elm++)
		{
			elm->first->request_output_parameters(static_cast<IjvxAudioPluginDetailsParameter*>(this));
		}

		ext_access_output = nullptr;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_SETTING;
}

// =====================================================================

jvxErrorType 
CjvxVstPluginExtender::prop_extender_type(jvxPropertyExtenderType* tp) 
{
	if (tp)
	{
		*tp = jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_PROPERTY_STREAM_AUDIO_PLUGIN;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxVstPluginExtender::prop_extender_specialization(jvxHandle** prop_extender, jvxPropertyExtenderType tp) 
{
	if (tp == jvxPropertyExtenderType::JVX_PROPERTY_EXTENDER_PROPERTY_STREAM_AUDIO_PLUGIN)
	{
		if (prop_extender)
		{
			*prop_extender = static_cast<IjvxPropertyExtenderStreamAudioPlugin*>(this);
			return JVX_NO_ERROR;
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxVstPluginExtender::register_report(IjvxAudioPluginReport* rep, const char* descr)
{
	jvxErrorType res = JVX_NO_ERROR;
	auto elm = registered_listeners.find(rep);
	if (elm != registered_listeners.end())
	{
		return JVX_ERROR_DUPLICATE_ENTRY;
	}
	oneListener newElm;
	newElm.nm = descr;
	newElm.rep = rep;
	registered_listeners[rep] = newElm;
	return res;
}

jvxErrorType
CjvxVstPluginExtender::register_id_parameter(IjvxAudioPluginReport* rep, jvxSize id, const char* descr)
{
	jvxErrorType res = JVX_NO_ERROR;
	auto elm = registered_listeners.find(rep);
	if (elm != registered_listeners.end())
	{
		auto elmi = elm->second.registered_ids.find((jvxInt32)id);
		if (elmi != elm->second.registered_ids.end())
		{
			return JVX_ERROR_DUPLICATE_ENTRY;
		}
		oneListener::oneEntry newEntry;
		newEntry.expr = descr;
		newEntry.id = (jvxInt32)id;
		newEntry.ref = rep;
		elm->second.registered_ids[newEntry.id] = newEntry;
	}
	return res;
}

jvxErrorType
CjvxVstPluginExtender::unregister_report(IjvxAudioPluginReport* rep) 
{
	jvxErrorType res = JVX_NO_ERROR;
	auto elm = registered_listeners.find(rep);
	if (elm == registered_listeners.end())
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	registered_listeners.erase(rep);
	return res;
}

jvxErrorType 
CjvxVstPluginExtender::request_report_handle(IjvxAudioPluginReport** rep, const char* tag)
{
	auto elm = registered_listeners.begin();
	for (; elm != registered_listeners.end(); elm++)
	{
		if (elm->second.nm == tag)
		{
			*rep = elm->second.rep;
			return JVX_NO_ERROR;
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxVstPluginExtender::return_report_handle(IjvxAudioPluginReport* rep)
{
	auto elm = registered_listeners.begin();
	for (; elm != registered_listeners.end(); elm++)
	{
		if (elm->second.rep == rep)
		{
			return JVX_NO_ERROR;
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxVstPluginExtender::request_runtime_handle(IjvxAudioPluginRuntime** rep)
{
	if (rep)
		*rep = rt_ext;
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxVstPluginExtender::return_runtime_handle(IjvxAudioPluginRuntime* rep)
{
	if (rep == rt_ext)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
CjvxVstPluginExtender::get_number_fine_structure(jvxSize* num)
{
	if (ext_access_input)
	{
		if (num)
		{
			*num = ext_access_input->getPointCount();
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_INVALID_SETTING;
}

jvxErrorType
CjvxVstPluginExtender::get_entry_fine_structure(jvxSize idx, jvxSize* sample_offset, jvxData* norm_val)
{
	if (ext_access_input)
	{
		Steinberg::int32 sampleOffset;
		Steinberg::Vst::ParamValue value;
		if (ext_access_input->getPoint((Steinberg::int32)idx, sampleOffset, value) == Steinberg::kResultTrue)
		{
			if (sample_offset)
				*sample_offset = sampleOffset;
			if (norm_val)
				*norm_val = value;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INTERNAL;
	}
	return JVX_ERROR_INVALID_SETTING;
}

jvxErrorType
CjvxVstPluginExtender::create_entry_fine_structure(IjvxAudioPluginReport* rep, jvxSize idx, jvxHandle** association)
{
	if (ext_access_output)
	{
		auto elm = registered_listeners.find(rep);
		if (elm != registered_listeners.end())
		{
			Steinberg::int32 idxs = (Steinberg::int32)idx;
			Steinberg::Vst::IParamValueQueue* paramQueue = ext_access_output->addParameterData((Steinberg::Vst::ParamID)idx, idxs);
			if (association)
				*association = paramQueue;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return JVX_ERROR_INVALID_SETTING;
}

jvxErrorType
CjvxVstPluginExtender::add_entry_fine_structure(jvxSize idx, jvxSize position_offset, jvxData norm_val, jvxHandle* ref)
{
	// a new value of VuMeter will be send to the host
	// (the host will send it back in sync to our controller for updating our editor)
	Steinberg::Vst::IParamValueQueue* paramQueue = (Steinberg::Vst::IParamValueQueue*) ref;
	if (paramQueue)
	{
		Steinberg::int32 idxs = (Steinberg::int32)idx;
		paramQueue->addPoint((Steinberg::int32)position_offset, norm_val, idxs);
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}
