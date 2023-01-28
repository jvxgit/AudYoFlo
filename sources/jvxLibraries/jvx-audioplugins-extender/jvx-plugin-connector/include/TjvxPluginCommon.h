#ifndef __TJVXPLUGINCONNECTOR_H__
#define __TJVXPLUGINCONNECTOR_H__

typedef enum
{
	PARAM_FLAGS_NOFLAGS = 0,
	PARAM_FLAGS_AUTOMATE = 1,
	PARAM_FLAGS_READONLY = 2,
	PARAM_FLAGS_PROGRAM_ID = 1 << 15
} jvxAudioPluginParamFlags;

#ifdef JVX_OS_WINDOWS
#pragma pack(push, 1)
#else
#pragma pack(push)
#pragma pack(1)
#endif

typedef enum
{
	JVX_AUDIOPLUGIN_TRIGGER_SYNC_PARAM = 1,
	JVX_AUDIOPLUGIN_SEND_NORM_PARAM = 2,
	JVX_AUDIOPLUGIN_TRIGGER_SYNC_STATE = 3,
	JVX_AUDIOPLUGIN_REPORT_CURRENT_STATE = 4,
	JVX_AUDIOPLUGIN_REPORT_STATE_CHANGED = 5,
	JVX_AUDIOPLUGIN_TRIGGER_REQ_LIN_FIELD = 6,
	JVX_AUDIOPLUGIN_SEND_LIN_FIELD = 7,
	JVX_AUDIOPLUGIN_REPORT_PROCESS_COMPLETE = 31
} jvxAudioPluginBinaryReqType;

typedef enum
{
	JVX_AUDIOPLUGIN_PARAM_NORMALIZED= 1,
	JVX_AUDIOPLUGIN_PARAM_NORM_STRLIST = 2
} jvxAudioPluginBinarySyncParamType;

typedef enum
{
	JVX_AUDIOPLUGIN_STATE_STARTED = 1,
	JVX_AUDIOPLUGIN_STATE_STOPPED = 2
} jvxAudioPluginBinaryStateChange;

typedef struct
{
	jvxUInt32 command;
	jvxUInt32 sz;
	jvxInt16 rep_process_id;
	jvxInt16 ass_process_id;
} jvxAudioPluginBinaryHeader;

typedef struct
{
	jvxAudioPluginBinaryHeader hdr;
	jvxUInt16 stateId;
} jvxAudioPluginBinaryStateChangeHeader;

typedef struct
{
	jvxAudioPluginBinaryHeader hdr;

	jvxUInt16 numChar;
	jvxUInt16 numFields;	

} jvxAudioPluginBinaryParamHeader;

typedef struct
{
	jvxAudioPluginBinaryHeader hdr;

	jvxUInt16 numChar;
	jvxInt32 id; 
	jvxInt32 subid;
	jvxUInt32 offset;
	jvxUInt32 num;
} jvxAudioPluginBinaryLinFieldHeader;

typedef struct
{
	jvxInt32 id;
} jvxAudioPluginBinaryField_trigger;

typedef struct
{
	jvxInt32 id;
	jvxUInt16 type_entry;
	jvxUInt16 err_entry;
} jvxAudioPluginBinaryField_sync;

typedef struct
{
	jvxAudioPluginBinaryField_sync hdr;
	jvxData val;
} jvxAudioPluginBinaryField_sync_norm;

typedef struct
{
	jvxAudioPluginBinaryField_sync_norm norm;
	jvxSize num;
} jvxAudioPluginBinaryField_sync_norm_strlist;

typedef struct
{
	jvxSize numchar;

	// Followed by byte buffer for text
} jvxAudioPluginBinaryField_sync_norm_strlist_oneentry;

#ifdef JVX_OS_WINDOWS
#pragma pack(pop)
#else
#pragma pack(pop)
#endif 

#endif
