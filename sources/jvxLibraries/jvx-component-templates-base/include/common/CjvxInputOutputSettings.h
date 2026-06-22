#ifndef __CJVXCURRENTCONVERSIONSCENARIO_H__
#define __CJVXCURRENTCONVERSIONSCENARIO_H__

#include "jvx.h"
#include "jvx_resampler/jvx_fixed_resampler.h"

#include "common/CjvxSimpleParameters.h"

// #define AYF_IOSETTINGS_PROTOCOL_CREATE

enum class jvxRateLocationMode
{
	// In this case the source to drive the converter must request the information about required sample number 
	// within the process_start_input function. Then, it delivers exactly this number to guarantee a fixed output 
	// buffersize!!
	// Examples for an application:
	// a) Writing to an outout file with an intermediate foward buffer 
	// The name comes from the fact that the samplerate on the input side is forwarded to the output side
	// by demanding variable numbers of samples - to make sure no resampling with fixed buffersize relations is involved
	JVX_FIXED_RATE_LOCATION_INPUT,

	// In this case, the converter stores samples on the input side depending on the number of samples passed.
	// Then, it computes the output samples from the current number of stored samples. This mode allows for 
	// a variable buffersize on the output side and even if there is a fixed relation in buffersize for input 
	// and output the buffersize may be variable if the internal resampling requires so.
	// Examples for an application: 
	// a) Reading audio from file in buffers, passing forward the frames to enter a forward buffer to collect data. 
	//    Here, a fixed buffersize on the output is not required.
	//he name comes from the fact that 

	JVX_FIXED_RATE_LOCATION_OUTPUT
};

class CjvxInputOutputSettings
{
public:

	enum class testProtocolStatus
	{
		AYF_CONVERT_START,
		AYF_CONVERT_INPUT_DEVIATION_ANALYZED,
		AYF_CONVERT_INPUT_ACCEPTED,
		AYF_CONVERT_INPUT_TO_OUTPUT,
		AYF_CONVERT_OUTPUT_PREPARED,
		AYF_CONVERT_TRANSFER_BACK_TRY_UNALTERED,
		AYF_CONVERT_TRANSFER_FORWARD_TRY_COMPROMISE,
		AYF_CONVERT_LEAVE_ON_AGREE,
		AYF_CONVERT_TRIED_CHANGE_FORMAT_FULL,
		AYF_CONVERT_ACCEPTED_CHANGE_FORMAT,
		AYF_CONVERT_TRIED_CHANGE_NCHANS_FULL,
		AYF_CONVERT_ACCEPTED_CHANGE_NCHANS,
		AYF_CONVERT_TRIED_CHANGE_RATE_BUFFERSIZE_FULL,
		AYF_CONVERT_ACCEPTED_CHANGE_RATE_BUFFERSIZE,
		AYF_CONVERT_TRIED_CHANGE_DATAFLOW_FULL,
		AYF_CONVERT_ACCEPTED_CHANGE_DATAFLOW,
		AYF_CONVERT_LIMIT
	};

	jvxSize versionId = 0;
	testProtocolStatus protoState = testProtocolStatus::AYF_CONVERT_START;
	int stepsTaken[(jvxSize)testProtocolStatus::AYF_CONVERT_LIMIT] = { 0 };
	jvxBool requiresUpdateOutput = false;
	jvxLinkDataDescriptor tryLast;
	jvxBool forwardCompromise = true;

	jvxRateLocationMode fixedLocationMode = jvxRateLocationMode::JVX_FIXED_RATE_LOCATION_INPUT;
	class procParams
	{
	public:
		jvxSize bSize = JVX_SIZE_UNSELECTED;
		jvxSize rate = JVX_SIZE_UNSELECTED;
		jvxSize nChans = JVX_SIZE_UNSELECTED;
		jvxDataFormat form = jvxDataFormat::JVX_DATAFORMAT_NONE;
		jvxDataFormatGroup subForm = jvxDataFormatGroup::JVX_DATAFORMAT_GROUP_NONE;
		jvxCBitField currentConnectionState = 0;
		jvxCBitField currentDeviationState = 0;
		jvxDataflow datFlow = JVX_DATAFLOW_DONT_CARE;
	};

	procParams in;
	procParams out;

	struct
	{
		jvxSize bSizeInMin = 0;
		jvxSize bSizeInMax = 0;			
		jvxSize bSizeOutMin = 0;
		jvxSize bSizeOutMax = 0;

		jvx_fixed_resampler_conversion cc;
	} resampling;

	enum class convertCfgFlags
	{
		AYF_NUM_CHANNELS_SHIFT = 0,
		AYF_FORMAT_SHIFT = 1,
		AYF_SUBFORMAT_SHIFT = 2,
		AYF_BSIZE_SHIFT = 3,
		AYF_SRATE_SHIFT = 4,
		AYF_DATAFLOW_SHIFT = 5
	};

	enum convertCfgStatus
	{
		JVX_CONVERT_NOT_OK = 0,
		JVX_CONVERT_OK_INPUT = 0x1,
		JVX_CONVERT_OK_OUTPUT = 0x2
	};

	enum leaveCfgStatus
	{
		JVX_CONVERT_CFG_STATE_INIT = 0,
		JVX_CONVERT_CFG_STATE_LEFT_OCONN_COMPROMISE = 1
	} ;

	convertCfgStatus convStat = convertCfgStatus::JVX_CONVERT_NOT_OK;
	leaveCfgStatus leavStat = leaveCfgStatus::JVX_CONVERT_CFG_STATE_INIT;

#ifdef AYF_IOSETTINGS_PROTOCOL_CREATE
	std::string protoNegotiate;
#endif

	void resetOneTest(jvxRateLocationMode fixedLocationModeArg);

	jvxErrorType passFromPredecessor(jvxLinkDataDescriptor* data);
	jvxErrorType passFromSuccessor(jvxLinkDataDescriptor* desired, jvxLinkDataDescriptor& given, jvxLinkDataDescriptor* fromPrevious JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	jvxCBitField checkDeviation(jvxLinkDataDescriptor* data, CjvxInputOutputSettings::procParams& params);
	void acceptInputParams(jvxLinkDataDescriptor* data, procParams* inTo);

	void computeBufferRelations(jvxBool fromInputToOutput, jvxSize* bsizeArgOpt);
	void inputToOutput();
	void copyOutFromIn(const procParams& inArg);

	void prepareForForward(jvxLinkDataDescriptor& data);

	jvxBool checkIdentical(jvxLinkDataDescriptor& theOne, jvxLinkDataDescriptor& theOther);
	
	void copyInputArgs(CjvxSimpleParameters& args);
	void copyOutputArgs(CjvxSimpleParameters& args);
};

#endif

