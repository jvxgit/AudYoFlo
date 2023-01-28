#ifndef _IJVXPROPERTYEXTENDERSTREAM_H__
#define _IJVXPROPERTYEXTENDERSTREAM_H__

JVX_INTERFACE IjvxAudioPluginReport;

JVX_INTERFACE IjvxAudioPluginRuntime
{
public:
	virtual ~IjvxAudioPluginRuntime() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION disengage_processing() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION engage_processing()  = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION try_lock_processing() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION lock_processing() = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION unlock_processing() = 0; 
};

JVX_INTERFACE IjvxAudioPluginDetailsParameter
{

public:
	virtual JVX_CALLINGCONVENTION ~IjvxAudioPluginDetailsParameter() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION get_number_fine_structure(jvxSize* num) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION get_entry_fine_structure(jvxSize idx, jvxSize* sample_offset, jvxData* norm_val) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION create_entry_fine_structure(IjvxAudioPluginReport* rep, jvxSize idx, jvxHandle** association) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION add_entry_fine_structure(jvxSize idx, jvxSize position_offset, jvxData norm_val, jvxHandle* associaton) = 0;
};

JVX_INTERFACE IjvxAudioPluginReport
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxAudioPluginReport() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION report_input_parameter(jvxInt32 id, jvxData normVal, IjvxAudioPluginDetailsParameter* detail) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION request_output_parameters(IjvxAudioPluginDetailsParameter* detail) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION post_single_parameter_norm(jvxUInt32 id, jvxData norm_value) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION post_single_parameter_norm_strlist(jvxUInt32 id, jvxData norm_value, jvxApiStringList* lst) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION request_single_parameter_norm(jvxUInt32 id, jvxData* norm_value) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION request_single_parameter_norm_strlist(jvxUInt32 id, jvxData* norm_value, jvxApiStringList* lst) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_field_context(jvxData* fld_ptr, jvxSize fld_sz, 
		jvxInt32 id, jvxInt32 subid, jvxSize offset, jvxSize num) = 0;
};

JVX_INTERFACE IjvxPropertyExtenderStreamAudioPlugin: public IjvxPropertyExtender
{
	
public:
	virtual JVX_CALLINGCONVENTION ~IjvxPropertyExtenderStreamAudioPlugin(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION register_report(IjvxAudioPluginReport* rep, const char* tag) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION register_id_parameter(IjvxAudioPluginReport* rep, jvxSize id, const char* descr) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_report(IjvxAudioPluginReport* rep) = 0;
	
	virtual jvxErrorType JVX_CALLINGCONVENTION request_report_handle(IjvxAudioPluginReport** rep, const char* tag) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_report_handle(IjvxAudioPluginReport* rep) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_runtime_handle(IjvxAudioPluginRuntime** rep) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_runtime_handle(IjvxAudioPluginRuntime* rep) = 0;
};

#endif
