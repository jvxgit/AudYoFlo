#include "jvxLibHost.h"

jvxErrorType
jvxLibHost::save_config()
{
	jvxCallManagerConfiguration callConf;
	jvxErrorType res = JVX_NO_ERROR;
	if (!cfg_filename_in_use.empty())
	{
		callConf.configModeFlags = JVX_CONFIG_MODE_FULL;
		callConf.accessFlags = JVX_ACCESS_ROLE_DEFAULT;
		res = configureToFile(&callConf, cfg_filename_in_use);
	}
	return res;
}

jvxErrorType 
jvxLibHost::set_operation_flags(bool reportOnSet, bool reportSyncDirect)
{
	oConfig.reportOnSet = reportOnSet;
	oConfig.reportSyncDirect = reportSyncDirect;
	return JVX_NO_ERROR;
}

jvxErrorType 
jvxLibHost::get_compile_flags(jvxDataTypeSpec& dataTpFormat, jvxSize& num32BitWordsSelection)
{
	dataTpFormat = JVX_DATA_DATAFORMAT_ENUM;
	num32BitWordsSelection = JVX_NUMBER_32BITS_BITFIELD;
	return JVX_NO_ERROR;
}

