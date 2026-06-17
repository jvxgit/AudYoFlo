#ifndef __CJVXCURRENTCONVERSIONSCENARIO_H__
#define __CJVXCURRENTCONVERSIONSCENARIO_H__

#include "jvx.h"

class CjvxInputOutputSettings
{
public:

	jvxSize versionId = 0;

	struct procParams
	{
		jvxSize bSize = 0;
		jvxSize rate = 0;
		jvxSize nChans = 0;
		jvxDataFormat form = jvxDataFormat::JVX_DATAFORMAT_NONE;
		jvxDataFormatGroup subForm = jvxDataFormatGroup::JVX_DATAFORMAT_GROUP_NONE;
		jvxCBitField currentCponnectionState = 0;
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
		AYF_NUM_CHANNELS_OK_SHIFT = 0,
		AYF_FORMAT_OK_SHIFT = 1,
		AYF_SUBFORMAT_OK_SHIFT = 2,
		AYF_BSIZE_OK_SHIFT = 3,
		AYF_SRATE_OK_SHIFT = 4
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
};

#endif

