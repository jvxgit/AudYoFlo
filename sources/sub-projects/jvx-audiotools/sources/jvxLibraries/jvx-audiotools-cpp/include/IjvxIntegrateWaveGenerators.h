#ifndef __IJVXINTEGRATEWAVEGENERATORS_H__
#define __IJVXINTEGRATEWAVEGENERATORS_H__

JVX_INTERFACE IjvxIntegrateWaveGenerators
{

public:
	virtual JVX_CALLINGCONVENTION ~IjvxIntegrateWaveGenerators(){};

	virtual JVX_CALLINGCONVENTION jvxErrorType activate(CjvxProperties* theProps, const char* propertyPrefix) = 0;
	virtual JVX_CALLINGCONVENTION jvxErrorType deactivate() = 0;

	virtual JVX_CALLINGCONVENTION jvxErrorType setParameters(jvxInt32 rate_param, jvxSize bSize_param, jvxInt32 numchans_param, jvxDataFormat format, jvxBool* updateUi ) = 0;

	virtual JVX_CALLINGCONVENTION jvxErrorType set_property_local(
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		jvxSize propId, jvxPropertyCategoryType category,
		jvxSize offsetStart, jvxBool contentOnly, jvxPropertyCallPurpose callPurpose,
		jvxBool* updateUi) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno) = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections) = 0;

	virtual JVX_CALLINGCONVENTION jvxErrorType startAll() = 0;
	
	virtual JVX_CALLINGCONVENTION jvxErrorType stopAll() = 0;
	virtual JVX_CALLINGCONVENTION jvxErrorType process(jvxHandle** fldsFill, jvxSize& numFilled, jvxBool& allStopped) = 0;
};

extern jvxErrorType jvx_factory_allocate_jvxIntegrateWaveGenerators(IjvxIntegrateWaveGenerators** returnedObj);
extern jvxErrorType jvx_factory_deallocate_jvxIntegrateWaveGenerators(IjvxIntegrateWaveGenerators* returnedObj);

#endif