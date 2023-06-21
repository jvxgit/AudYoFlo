#ifndef IJVXREPORT_H__
#define IJVXREPORT_H__

class CjvxReportCommandRequest;

JVX_INTERFACE IjvxReportSystem
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxReportSystem() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION request_command(const CjvxReportCommandRequest& request) = 0;
};

JVX_INTERFACE IjvxSubReport
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxSubReport() {};
	virtual jvxErrorType JVX_CALLINGCONVENTION register_sub_report(IjvxReportSystem* request) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_sub_report(IjvxReportSystem* request) = 0;
};

JVX_INTERFACE IjvxReport: public IjvxReportSystem
{
public:
	virtual JVX_CALLINGCONVENTION ~IjvxReport(){};

	virtual jvxErrorType JVX_CALLINGCONVENTION report_simple_text_message(const char* txt, jvxReportPriority prio) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, jvxSize offset_start, jvxSize numElements,
		const jvxComponentIdentification& tpTo,	jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
		jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_command_request(jvxCBitField request, 
		jvxHandle* caseSpecificData = NULL, 
		jvxSize szSpecificData = 0) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_os_specific(jvxSize commandId, void* context) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION interface_sub_report(IjvxSubReport** subReport) = 0;
};

JVX_INTERFACE IjvxReportSystemForward
{
public:
	virtual ~IjvxReportSystemForward() {};

	virtual void request_command_in_main_thread(CjvxReportCommandRequest* request, jvxBool removeAfterHandle = true) = 0;
};

#endif
