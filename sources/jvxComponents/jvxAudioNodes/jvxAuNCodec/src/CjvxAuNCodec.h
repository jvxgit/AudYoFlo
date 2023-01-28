#ifndef __CJVXAUNCODEC_H__
#define __CJVXAUNCODEC_H__

#include "jvx.h"
#include "jvxAudioNodes/CjvxAudioNode.h"
#include "pcg_exports_node.h"

#include "CjvxAuNCodec.h"
#include "HjvxMicroConnection.h"

#define JVX_NUM_LANES 2

class CjvxAuNCodec: public CjvxAudioNode, public genEncoderDecoder_node
{
private:

	typedef struct
	{
		IjvxObject* codecObj;
		IjvxAudioCodec* theCodec;
		std::string description;
		std::string descriptor;
		jvxSize id;
	} oneEncDecEntry;

	typedef struct
	{
		jvxData* inputBuffer;
		IjvxAudioEncoder* theEncoder;
		IjvxAudioDecoder* theDecoder;
		IjvxProperties* theEncProps;
		IjvxProperties* theDecProps;
		IjvxObject* theEncObj;
		IjvxObject* theDecObj;
		jvxSize id;
	} oneEncDecLaneEntry;

	typedef struct
	{
		IjvxDataProcessor* theEncDp;
		IjvxDataProcessor* theDecDp;
	} myActiveMods;

	struct oneConnectionLane
	{
		HjvxMicroConnection* micro_connection;
		jvxLinkDataDescriptor descr_encoder_in;
		jvxLinkDataDescriptor descr_decoder_out;
		jvxState stat;
		IjvxAudioEncoder* theEncoder;
		IjvxAudioDecoder* theDecoder;
		oneConnectionLane()
		{
			micro_connection = NULL;
			stat = JVX_STATE_NONE;
			jvx_initDataLinkDescriptor(&descr_encoder_in);
			jvx_initDataLinkDescriptor(&descr_decoder_out);
			theEncoder = NULL;
			theDecoder = NULL;
		};
	};

	std::vector<oneEncDecLaneEntry> theLanes[JVX_NUM_LANES];
	jvxSize selIdLane[JVX_NUM_LANES];
	jvxLinkDataDescriptor theDataEnc[JVX_NUM_LANES];
	jvxLinkDataDescriptor theDataDec[JVX_NUM_LANES];
	myActiveMods procLanes[JVX_NUM_LANES];

	jvxValidEntriesRestrict theDecoderHints[JVX_NUM_LANES];
	jvxValidEntriesRestrict theEncoderHints[JVX_NUM_LANES];

	jvxValidEntriesRestrict* theDecoderHints_other[JVX_NUM_LANES];
	jvxValidEntriesRestrict* theEncoderHints_other[JVX_NUM_LANES];

	std::vector<oneEncDecEntry> availableCodecs;

	// The connection lanes
	oneConnectionLane connection_lane[JVX_NUM_LANES];
	jvxBool mainModuleConnected;

public:

	JVX_CALLINGCONVENTION CjvxAuNCodec(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxAuNCodec();
	
	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_sender_to_receiver(jvxLinkDataDescriptor* theData)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION set_property(
		jvxCallManagerProperties& callGate,
		const jvx::propertyRawPointerType::IjvxRawPointerType& rawPtr,
		const jvx::propertyAddress::IjvxPropertyAddress& ident,
		const jvx::propertyDetail::CjvxTranferDetail& trans)override;

	// ============================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// ============================================================================================

private:
	void updateDependentProperties(jvxBool);
	void updateAudioParameters(oneEncDecLaneEntry* theEntry);
	void activateEncoderDecoder(oneEncDecLaneEntry& theEntry, jvxSize id);
	void deactivateEncoderDecoder(oneEncDecLaneEntry& theEntry, jvxSize id);

	void createMicroConnections(jvxSize id_lane);
	void releaseMicroConnections(jvxSize id_lane);

	jvxErrorType update_codec_properties();
};

#endif
