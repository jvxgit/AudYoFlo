#ifndef __IJVXMIXMATRIX_H__
#define __IJVXMIXMATRIX_H__

JVX_INTERFACE IjvxMixMatrix
{

public:
	virtual JVX_CALLINGCONVENTION ~IjvxMixMatrix(){};

	virtual JVX_CALLINGCONVENTION jvxErrorType activate(CjvxProperties* theProps, const char* propertyPrefix) = 0;
	virtual JVX_CALLINGCONVENTION jvxErrorType deactivate() = 0;

	virtual JVX_CALLINGCONVENTION jvxErrorType setParameters(jvxSize bSize_param, jvxSize numchansin_param, jvxSize numchansout_param, jvxBool* updateUi ) = 0;

	virtual JVX_CALLINGCONVENTION jvxErrorType set_property_local(
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		jvxSize propId, jvxPropertyCategoryType category,
		jvxSize offsetStart, jvxBool contentOnly, jvxPropertyCallPurpose callPurpose,
		jvxBool* updateUi) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
								     jvxHandle* sectionToContainAllSubsectionsForMe, 
								     const char* filename,
								     jvxInt32 lineno) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
								     jvxHandle* sectionWhereToAddAllSubsections) = 0;

	virtual JVX_CALLINGCONVENTION jvxErrorType start() = 0;
	
	virtual JVX_CALLINGCONVENTION jvxErrorType stop() = 0;

	virtual JVX_CALLINGCONVENTION jvxErrorType process(jvxHandle** inputs, jvxHandle** outputs) = 0;
};

extern jvxErrorType jvx_factory_allocate_jvxMixMatrix(IjvxMixMatrix** returnedObj);
extern jvxErrorType jvx_factory_deallocate_jvxMixMatrix(IjvxMixMatrix* returnedObj);

#endif