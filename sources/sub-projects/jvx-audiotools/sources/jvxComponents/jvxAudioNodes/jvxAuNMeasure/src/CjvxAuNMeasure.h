#ifndef __CJVXAUNMEASURE_H__
#define __CJVXAUNMEASURE_H__

#include "jvx.h"
#include "jvxAudioNodes/CjvxAudioNode_logfile.h"
#include "pcg_exports_node.h"
#include "IjvxIntegrateWaveGenerators.h"

#include "CjvxMaStatAnalysis.h"
#include "CjvxMultiChannelCircBufferPlotResampler.h"

extern "C"
{
#include "jvx_measure_ir.h"
}

#define JVX_AUN_LANES_CONFIG "JVX_AUN_LANES_CONFIG"
#define JVX_AUN_ACTIVE_LANE_CONFIG_PREFIX "JVX_AUN_ACTIVE_LANE_CONFIG_"
#define TIMEOUT_MASTAT_MS 1000
#define TIMEPERIOD_SHOW_SIGNAL 10

// =========================================================================
#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxAuNMeasure: public CjvxAudioNode_logfile, public genMeasure_node, public CjvxMaStatAnalysis_report
{
	typedef struct
	{
		jvxData* theIr;
		jvxSize lIr;
		std::string tag;		
	} jvxAuNOneMeasurement;

	typedef enum 
	{
		JVX_AUMEASURE_NONE = 0,
		JVX_AUMEASURE_INPUT = 1,
		JVX_AUMEASURE_OUTPUT = 2
	} inoutChannelSelection;

	typedef enum
	{
		JVX_AUNMEASURE_OPERATION_MODE_NONE = 0,
		JVX_AUNMEASURE_OPERATION_MODE_MEASURE_IR,
		JVX_AUNMEASURE_OPERATION_MODE_MEASURE_TIMING
	} jvxAuNMeasure_operationMode;

	typedef struct
	{
		inoutChannelSelection typeChannelTest;
		jvxSize idChannelTest;

		inoutChannelSelection typeChannelMeasure;
		jvxSize idChannelMeasure;

		std::string description;
		struct
		{
			jvxSize cntSamples;
			jvxMeasureIr coreMeasure;
		} runtime;

		struct
		{
			jvxData* measureIr;
		} results;
	} oneEntryLanes;

	typedef struct
	{
		std::vector<oneEntryLanes> theLanes;
		jvxSize selectionId;
	} allLanes;

	allLanes myLanes;

	jvxSize uniqueId;

	IjvxIntegrateWaveGenerators* myWaveGenerators;
	struct
	{
		jvxBool endProcessingTriggered;
		jvxSize sampleCntCountdownMax;
		jvxSize sampleCntCountdown;
		jvxSize iterationCount;
	} runtime;

	//CjvxProperties myProperties;
	jvxAuNMeasure_operationMode currentMode;

	struct
	{
		std::vector<jvxAuNOneMeasurement> theMeasurements;
		jvxSize selectionId;
	} myMeasurements;

	struct
	{
		CjvxMaStatAnalysis theObj;
		jvxSize statAna_lineidx;
		jvxInt32 bsize;
		jvxInt32 rate;
		jvxData deltatoneframe;
		jvxInt32 numberObservations;
		jvxUInt64 previousTstamp_us;
		jvxTimeStampData tStamp;
	} statAnalysis;

	struct
	{
		jvxBool circBufferAttached;
		jvxExternalBuffer* theCircBufferHeader;
		CjvxMultiChannelCircBufferPlotResampler plotter;
		jvxData resamplingFactor;
	} viewStruct_channel;

	struct
	{
		jvxBool circBufferAttached;
		jvxExternalBuffer* theCircBufferHeader;
		CjvxMultiChannelCircBufferPlotResampler plotter;
		jvxData resamplingFactor;
	} viewStruct_timing;

	struct
	{
		inoutChannelSelection typeChannel;
		jvxSize idChannel;
	} observationChannel;

public:

	JVX_CALLINGCONVENTION CjvxAuNMeasure(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxAuNMeasure();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& transy)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
								     jvxHandle* sectionWhereToAddAllSubsections)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_sender_to_receiver(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

private:

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_buffer_post);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_buffer_pre);

	// We may have to modify this behavior here if we see dropouts..
	void updateParamsLanes_toInside();
	void updateParamsLanes_toOutside();
	void checkupdateParams_lanes();
	void checkupdateParams_channels();
	void updateParamsMeasurements_toOutside();
	void updateParamsObservationTiming_toOutside();

	void reportUpdate(jvxSize line)override;

	JVX_OS_REACT_INTERFACE_PARAMETERS_DECLARE
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
