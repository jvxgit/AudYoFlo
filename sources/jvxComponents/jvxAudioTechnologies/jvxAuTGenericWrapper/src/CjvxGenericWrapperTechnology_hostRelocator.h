#ifndef __CJVXGENERICWRAPPERTECHNOLOGY_HOSTRELOCATOR_H__
#define __CJVXGENERICWRAPPERTECHNOLOGY_HOSTRELOCATOR_H__

#include "jvx.h"

class CjvxGenericWrapperTechnology;

class CjvxGenericWrapperTechnology_hostRelocator :
	public IjvxHiddenInterface, public IjvxReport
{
	CjvxGenericWrapperTechnology* connectedTech;
	IjvxHiddenInterface* theHostRef;
	IjvxReport* theReport;

public:

	CjvxGenericWrapperTechnology_hostRelocator();
	~CjvxGenericWrapperTechnology_hostRelocator();

	void init(CjvxGenericWrapperTechnology* ref, IjvxHiddenInterface* hostRef);
	void terminate();

	virtual jvxErrorType JVX_CALLINGCONVENTION report_simple_text_message(const char* txt, jvxReportPriority prio) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, jvxSize offs, jvxSize num, const jvxComponentIdentification& tpTo,
		jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
		jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo, jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_command_request(
		jvxCBitField request, 
		jvxHandle* caseSpecificData = NULL, 
		jvxSize szSpecificData = 0) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_command(const CjvxReportCommandRequest& request) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_os_specific(jvxSize commandId, void* context) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION interface_sub_report(IjvxSubReport** subReport) override;

	// ========================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType, jvxHandle**) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType, jvxHandle*) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION object_hidden_interface(IjvxObject** objRef)override;
};

#endif