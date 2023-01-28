#ifndef __CJVXGENERICWRAPPER_COMMON_H__
#define __CJVXGENERICWRAPPER_COMMON_H__

#define JVX_GW_ALPHA_SMOOTH_DELTAT 0.995
#define JVX_GW_ALPHA_RELEASE_DELTAT_MAX 0.999
#define JVX_GW_TIMEOUT_UI 500
#define JVX_GW_WIND_SECONDS 5
#define JVX_GW_DIVISION_EPS 1e-5
#include "jvx.h"

typedef struct
{
	IjvxObject* theObj;
	IjvxResampler* theHdl;
} oneResampler;

typedef enum
{
	FILE_INACTIVE,
	FILE_ACCESS,
	FILE_ACCESS_PAUSED,
	FILE_ACCESS_COMPLETED,
	FILE_ACCESS_DROPOUT,
	FILE_BROKEN
} jvxFileStatus;

class oneEntryChannelMapper
{
public:
	jvxGenericWrapperChannelType theChannelType;
	jvxInt32 refHint;
	std::string description;
	std::string friendlyName;
	jvxSize lastModCnt;
	struct
	{
		//jvxSize pointsToIdInLocalEntity;
		jvxSize pointsToIdSelectionThisDevice;
		jvxSize pointsToIdAllThisDevice;
	} toHwFile;

	struct
	{
		jvxSize pointsToId_inclFiles;
		jvxSize pointsToId_exclFiles;
	} toSw;

	jvxBool isSelected;
	jvxData gain = 1.0;
	jvxBool directIn = false;
	struct
	{
		jvxInt32 idInChannelStruct;
	} proc;

	struct
	{
		jvxBool check;
	} tmp;

	oneEntryChannelMapper()
	{
		theChannelType = JVX_GENERIC_WRAPPER_CHANNEL_TYPE_NONE;
		refHint = -1;
		//description;
		//friendlyName;
		lastModCnt = 0; // in operations starts at 1!
		toHwFile.pointsToIdSelectionThisDevice = JVX_SIZE_UNSELECTED;
		toHwFile.pointsToIdAllThisDevice = JVX_SIZE_UNSELECTED;
		
		toSw.pointsToId_inclFiles = JVX_SIZE_UNSELECTED;
		toSw.pointsToId_exclFiles = JVX_SIZE_UNSELECTED;
		
		isSelected = false;
		proc.idInChannelStruct = -1;
		tmp.check = false;
	};
};

typedef struct
{
	std::string name;
	std::string friendlyName;
	jvxBool genericName;
	IjvxObject* theObj;
	jvxBool boostPrio;
	jvxBool pauseOnStart;
	jvxInt32 samplerate;
	jvxSize number_channels;
	jvxSize length;
	jvxInt16 idxNumberBuffersLookahead;
	jvxInt32 uniqueIdHandles;
	jvxFileFormat fform_file;
	jvxSize numBitsSample;
	jvxSize subformat;

	struct
	{
		jvxInt32 bSize_rs;
		jvxData sRate_rs;
		jvxBool resampling;
		jvxBool selected;
		jvxFileStatus theStatus;

		jvxByte*** fieldSamplesSW;
		jvxByte*** fieldSamplesSW_orig;
		jvxByte** fieldSamplesRS;
		oneResampler* resampler;

		jvxSize lostFrames;

		jvxData deltaT_avrg;
	} processing;
} oneAudioFile;

typedef struct
{
	oneAudioFile common;
	jvxFileContinueType ctTp;
	jvxCBool flag_bwd;
	jvxCBool flag_fwd;
	jvxCBool flag_play;
	jvxCBool flag_pause;
	jvxCBool flag_restart;
	jvxData progress;

	IjvxRtAudioFileReader* theReader;
	//jvxBool rewind;
	//jvxBool forward;
	//jvxBool restart;
} oneInputAudioFile;

typedef struct
{
	oneAudioFile common;
	IjvxRtAudioFileWriter* theWriter;
} oneOutputAudioFile;

typedef struct
{
	std::vector<oneInputAudioFile> theInputFiles;
	std::vector<oneOutputAudioFile> theOutputFiles;
	jvxInt32 uniqueIdHandles;
	jvxBool allowModification;
} CjvxGenericWrapperFiles;

typedef struct
{
	jvxInt32 uniqueId;
	jvxByte** buf;
	jvxBool wasStarted;
} oneDummyChan;

typedef struct
{
	std::vector<oneDummyChan> inputs;
	std::vector<oneDummyChan> outputs;
	jvxInt32 idUsed;
} CjvxGenericWrapperDummyChannels;

// =============================================================================
typedef struct
{
	jvxInt32 samplerate;
	jvxInt16 number_channels;
	IjvxExternalAudioChannels_data* theCallback;
	jvxInt32 uniqueIdHandles;
	std::string description;
	struct
	{
		jvxInt32 bSize_rs;
		jvxData sRate_rs;
		jvxBool resampling;
		jvxBool selected;

		jvxByte*** fieldSamplesSW;
		jvxByte*** fieldSamplesSW_orig; // Buffers which may be linked to become active later
		jvxByte** fieldSamplesRS;
		oneResampler* resampler;
		jvxFileStatus theStatus;
		jvxSize numberLostFrames;
	} processing;
} oneExternalAudioReference;

typedef struct
{
	std::vector<oneExternalAudioReference> theInputReferences;
	std::vector<oneExternalAudioReference> theOutputReferences;
	jvxInt32 uniqueIdHandles;
	jvxBool allowModification;
} CjvxGenericWrapperExtReferences;


/*
inline std::string JVX_GENERATE_CHANNEL_TOKEN(oneEntryChannelMapper& entry)
{
	std::string txt;
	jvxAssignedFileTags theTags;

	jvxInt32 idxSel = (jvxInt32)entry.toHwFile.pointsToIdAllThisDevice;
	switch(entry.theChannelType)
	{
	case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_DEVICE:
		txt = "hw::" + jvx_int2String(idxSel) + "::" + entry.description + "::" + jvx_int2String(entry.refHint);
		break;
	case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE:
		txt = "f::" + jvx_int2String(idxSel) + "::" + entry.friendlyName + "::" + jvx_int2String(entry.refHint);
		break;
	case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_EXTERNAL:
		txt = "e::" + jvx_int2String(idxSel) + "::" + entry.description + "::" + jvx_int2String(entry.refHint);
	}
	return(txt);
}
*/

#define SAMPLERATE_OUTFILE_START 48000
#define FFORMAT_OUTFILE_START JVX_FILEFORMAT_PCM_FIXED
#define JVX_GENERIC_WRAPPER_CONFIG_SUBSECTION_DEVICE "jvxGenericWrapper_subsectionDevice"
#define JVX_GENERIC_WRAPPER_CONFIG_SUBSECTION_TECHNOLOGY "jvxGenericWrapper_subsectionTechnology"

#endif