#ifndef __CJVXINTEGRATEWAVEGENERATORS_H__
#define __CJVXINTEGRATEWAVEGENERATORS_H__

#include "jvx.h"
#include "common/CjvxProperties.h"
#include "pcg_CjvxIntegrateWaveGenerators.h"
#include "IjvxIntegrateWaveGenerators.h"

extern "C"
{
#include "jvx_generator_wave.h"
};


class CjvxIntegrateWaveGenerators: public IjvxIntegrateWaveGenerators, public IntegrateWaveGenerators
{
protected:
	typedef struct
	{
		jvx_generatorWave theGenerator;
		std::string description;
		jvxBitField outChannelMask;
		jvxData* oneBuf;
	}oneGenerator;

	typedef struct
	{
		std::vector<oneGenerator> lstGenerators;
		jvxSize selectionId;
	} theGenerators;

	theGenerators myGenerators;

	struct
	{
		jvxInt32 rate;
		jvxInt32 numchannels;
		jvxSize bsize;
		jvxDataFormat format;
	} params;

	jvxState theState;

	std::vector<jvxInt32> lstPendingUpdates;

	jvxBool updateSyncPending;

	jvxSize uniqueId;

	CjvxProperties* propRef;
	std::string propPrefix;

public:
	CjvxIntegrateWaveGenerators();

	~CjvxIntegrateWaveGenerators();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate(CjvxProperties* theProps, const char* propertyPrefix)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION  deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION  setParameters(jvxInt32 rate_param, jvxSize bSize_param, jvxInt32 numchans_param,
			 jvxDataFormat format, jvxBool* updateUi)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION  set_property_local(
							const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
							jvxSize propId,jvxPropertyCategoryType category,
							jvxSize offsetStart, jvxBool contentOnly, jvxPropertyCallPurpose callPurpose,
							jvxBool* updateUi)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION  startAll()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION  stopAll()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION  process(jvxHandle** fldsFill, jvxSize& numFilled, jvxBool& allStopped)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
								     jvxHandle* sectionWhereToAddAllSubsections)override;
	// ==========================================================

	jvxErrorType takeOverParametersAndUpdateExposure(jvxBool takeOverParamsFromExtern, jvxBool* updateUi, jvxSize propId, jvxPropertyCategoryType category);
	
	jvxErrorType updateParams_toInside(jvxBool* updateUi);
	jvxErrorType updateParams_toOutside(jvxBool* updateUi);

	jvxErrorType classifyPropertySetType(jvxBool& setAsync, jvxBool& triggerSetSync, jvxSize propId, jvxPropertyCategoryType category);
};

#endif
