#ifndef __CJVXVSTPLUGINEXTENDER_H__
#define __CJVXVSTPLUGINEXTENDER_H__

#include "jvx.h"
#include "public.sdk/source/vst/vstaudioeffect.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

class CjvxVstPluginExtender : 
	public IjvxPropertyExtenderStreamAudioPlugin,
	public IjvxAudioPluginDetailsParameter
{
protected:
	Steinberg::Vst::ProcessData* temp_store = nullptr;
	
	class oneListener
	{
	public:
		std::string nm = "not set";
		IjvxAudioPluginReport* rep = nullptr;
		class oneEntry
		{
		public:
			IjvxAudioPluginReport* ref = nullptr;
			jvxInt32 id = -1;
			std::string expr;
		};
		std::map<jvxInt32,oneEntry> registered_ids;
	};

	std::map<IjvxAudioPluginReport*, oneListener> registered_listeners;
	Steinberg::Vst::IParamValueQueue* ext_access_input = nullptr;
	Steinberg::Vst::IParameterChanges* ext_access_output = nullptr;
	IjvxAudioPluginRuntime* rt_ext = nullptr;
public:
	CjvxVstPluginExtender() {};

	virtual jvxErrorType set_runtime_reference(IjvxAudioPluginRuntime* refRt);
	virtual jvxErrorType unset_runtime_reference();

	virtual jvxErrorType associate_processing_data(Steinberg::Vst::ProcessData* data);
	virtual jvxErrorType deassociate_processing_data(Steinberg::Vst::ProcessData* data);

	virtual jvxErrorType process_input_parameters();
	virtual jvxErrorType process_input_events();
	virtual jvxErrorType process_output_parameters();

	virtual jvxErrorType JVX_CALLINGCONVENTION supports_prop_extender_type(jvxPropertyExtenderType tp) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prop_extender_specialization(jvxHandle** prop_extender, jvxPropertyExtenderType tp) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION register_report(IjvxAudioPluginReport* rep, const char* tag) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION register_id_parameter(IjvxAudioPluginReport* rep, jvxSize id, const char* descr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_report(IjvxAudioPluginReport* rep) override;

	// ===========================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION request_runtime_handle(IjvxAudioPluginRuntime** rep) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_runtime_handle(IjvxAudioPluginRuntime* rep) override;

	// ===========================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION request_report_handle(IjvxAudioPluginReport** rep, const char* tag) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_report_handle(IjvxAudioPluginReport* rep) override;

	// ===========================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION get_number_fine_structure(jvxSize* num)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_entry_fine_structure(jvxSize idx, jvxSize* sample_offset, jvxData* norm_val) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION create_entry_fine_structure(IjvxAudioPluginReport* rep, jvxSize idx, jvxHandle** association) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION add_entry_fine_structure(jvxSize idx, jvxSize position_offset, jvxData norm_val, jvxHandle* associaton)override;

};

#endif
