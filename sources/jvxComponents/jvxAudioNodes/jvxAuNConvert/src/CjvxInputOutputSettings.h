#ifndef __CJVXCURRENTCONVERSIONSCENARIO_H__
#define __CJVXCURRENTCONVERSIONSCENARIO_H__

#include "jvx.h"

class CjvxInputOutputSettings
{
public:

	jvxSize versionId = 0;

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
	};

	procParams in;
	procParams out;

	struct
	{
		jvxSize Up = 1;
		jvxSize Down = 1;
	} resampling;

	enum class convertCfgFlags
	{
		AYF_NUM_CHANNELS_SHIFT = 0,
		AYF_FORMAT_SHIFT = 1,
		AYF_SUBFORMAT_SHIFT = 2,
		AYF_BSIZE_SHIFT = 3,
		AYF_SRATE_SHIFT = 4
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

	jvxErrorType passFromPredecessor(jvxLinkDataDescriptor* data);
	jvxErrorType passFromSuccessor(jvxLinkDataDescriptor* data);

	jvxCBitField checkDeviation(jvxLinkDataDescriptor* data, CjvxInputOutputSettings::procParams& params);
};

#endif

