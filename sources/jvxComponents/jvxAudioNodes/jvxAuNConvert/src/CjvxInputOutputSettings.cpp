#include "CjvxInputOutputSettings.h"

jvxErrorType 
CjvxInputOutputSettings::passFromPredecessor(jvxLinkDataDescriptor* data)
{
	in.currentDeviationState = checkDeviation(data, in);
	if (jvx_bitTest(in.currentDeviationState, (jvxSize)convertCfgFlags::AYF_BSIZE_SHIFT))
	{
		in.bSize = data->con_params.buffersize;
	}
	if (jvx_bitTest(in.currentDeviationState, (jvxSize)convertCfgFlags::AYF_SRATE_SHIFT))
	{
		in.rate = data->con_params.rate;
	}
	if (jvx_bitTest(in.currentDeviationState, (jvxSize)convertCfgFlags::AYF_NUM_CHANNELS_SHIFT))
	{
		in.nChans = data->con_params.number_channels;
	}
	if (jvx_bitTest(in.currentDeviationState, (jvxSize)convertCfgFlags::AYF_FORMAT_SHIFT))
	{
		in.form = data->con_params.format;
	}
	if (jvx_bitTest(in.currentDeviationState, (jvxSize)convertCfgFlags::AYF_SUBFORMAT_SHIFT))
	{
		in.subForm = data->con_params.format_group;
	}

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxInputOutputSettings::passFromSuccessor(jvxLinkDataDescriptor* data)
{
	return JVX_NO_ERROR;
}

jvxCBitField 
CjvxInputOutputSettings::checkDeviation(jvxLinkDataDescriptor* data, CjvxInputOutputSettings::procParams& params)
{
	jvxCBitField retVal = 0;
	if (data->con_params.buffersize != params.bSize)
	{
		jvx_bitSet(retVal, (jvxSize)convertCfgFlags::AYF_BSIZE_SHIFT);
	}
	if (data->con_params.rate != params.rate)
	{
		jvx_bitSet(retVal, (jvxSize)convertCfgFlags::AYF_SRATE_SHIFT);
	}
	if (data->con_params.number_channels != params.nChans)
	{
		jvx_bitSet(retVal, (jvxSize)convertCfgFlags::AYF_NUM_CHANNELS_SHIFT);
	}
	if (data->con_params.number_channels != params.nChans)
	{
		jvx_bitSet(retVal, (jvxSize)convertCfgFlags::AYF_NUM_CHANNELS_SHIFT);
	}
	if (data->con_params.format != params.nChans)
	{
		jvx_bitSet(retVal, (jvxSize)convertCfgFlags::AYF_FORMAT_SHIFT);
	}
	if (data->con_params.format_group != params.subForm)
	{
		jvx_bitSet(retVal, (jvxSize)convertCfgFlags::AYF_SUBFORMAT_SHIFT);
	}
	return retVal;
}