#ifndef __IJVXPLUGINCOMMON_H__
#define __IJVXPLUGINCOMMON_H__

#include "TjvxPluginCommon.h"

JVX_INTERFACE IjvxPluginParameterAllocator
{
public:
	virtual ~IjvxPluginParameterAllocator() {};
	virtual jvxErrorType allocate_unit(jvxSize id, jvxSize mappedId, const char* nm) = 0;

	virtual jvxErrorType allocate_parameter_normalized(const char* nm, jvxInt32 id,
		jvxCBitField flags, jvxData initval, jvxSize unit_id) = 0;

	virtual jvxErrorType allocate_parameter_norm_so(const char* nm, jvxInt32 id,
		jvxCBitField flags, const char* unit, jvxData offset, jvxData scalefac,
		jvxData initval, jvxSize unit_id, jvxSize num_digits) = 0;

	virtual jvxErrorType allocate_parameter_norm_strlist(
		const char* nm, jvxInt32 id,
		jvxApiStringList* lstEntries,
		jvxCBitField flags, jvxData initval, 
		jvxSize unit_id, const char* empty_arg = nullptr) = 0;
};

JVX_INTERFACE IjvxPluginMessageControl
{
public:
	virtual ~IjvxPluginMessageControl() {};
	virtual jvxErrorType plugin_send_message(jvxAudioPluginBinaryHeader* fld) = 0;
};

JVX_INTERFACE IjvxPluginParamMessage
{
public:
	virtual ~IjvxPluginParamMessage() {};
	virtual jvxErrorType trigger_transfer_param_norm_strlist(
		jvxInt32 id, jvxData normv, jvxErrorType resL, jvxApiStringList * strl, const char* tag, jvxInt16 process_id) = 0;
	virtual jvxErrorType trigger_transfer_param_norm(
		jvxInt32 id, jvxData normv, jvxErrorType resL, const char* tag, jvxInt16 process_id) = 0;
};

#endif
