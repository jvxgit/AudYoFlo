#ifndef __CJVXMIXMATRIX_H__
#define __CJVXMIXMATRIX_H__

#include "jvx.h"
#include "common/CjvxProperties.h"
#include "pcg_CjvxMixMatrix.h"
#include "IjvxMixMatrix.h"

extern "C"
{
#include "jvx_mixmatrix.h"
};

class CjvxMixMatrix: public IjvxMixMatrix, public MixMatrix
{
public:
	
private:
	
	struct
	{
		jvxSize bsize;
	} params;

	jvxState theState;

	CjvxProperties* propRef;
	std::string propPrefix;

	typedef struct
	{
		jvxHandle* privData;
		jvx_mix_matrix_process_samples theCallback;
	} oneIntersectCallback;

	typedef struct
	{
		jvxData factor;
		jvxCBool mute;
		oneIntersectCallback cbHandle;
	} jvxOneMixIntersection;

	jvxSize numInChannels;
	jvxSize numOutChannels;

	std::vector<jvxOneMixIntersection> theInterSections;

	jvxMixMatrix myMixMatrixHandle;

	JVX_MUTEX_HANDLE safeAccessState;

public: 
	CjvxMixMatrix();
	~CjvxMixMatrix();

	virtual JVX_CALLINGCONVENTION jvxErrorType activate(CjvxProperties* theProps, const char* propertyPrefix)override;
	virtual JVX_CALLINGCONVENTION jvxErrorType deactivate()override;

	virtual JVX_CALLINGCONVENTION jvxErrorType setParameters(jvxSize bSize_param, jvxSize numchansin_param, jvxSize numchansout_param, jvxBool* updateUi)override;

	virtual JVX_CALLINGCONVENTION jvxErrorType set_property_local(
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr, 
		jvxSize propId, jvxPropertyCategoryType category,
		jvxSize offsetStart, jvxBool contentOnly, jvxPropertyCallPurpose callPurpose,
		jvxBool* updateUi)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual JVX_CALLINGCONVENTION jvxErrorType start()override;

	virtual JVX_CALLINGCONVENTION jvxErrorType stop()override;

	virtual JVX_CALLINGCONVENTION jvxErrorType process(jvxHandle** inputs, jvxHandle** outputs)override;

};

#endif