#ifndef CJVXGENERICWRAPPER_FILESELECTION_H__
#define CJVXGENERICWRAPPER_FILESELECTION_H__

//#include "jvxFileReader.h"
//#include "jvxFileWriter.h"
#include "jvx.h"

class CjvxGenericWrapperDevice;

class CjvxGenericWrapperDevice_fileSelection
{
	friend class CjvxGenericWrapperDevice;
	friend class CjvxGenericWrapperTechnology;
	friend class CjvxGenericWrapperDeviceQT;
	friend class CjvxGenericWrapperTechnologyQT;
	friend class uSubDialogFileIO;

	typedef enum
	{
		FILE_INACTIVE,
		FILE_ACCESS,
		FILE_ACCESS_PAUSED,
		FILE_ACCESS_COMPLETED
	} fileAccessMode;

private:

	typedef struct
	{
		int uniqueFileID;
		std::string fName;
//		WORD16 numChannels;
		jvxSize lFile;
//		WORD32 samplerate;
		bool boostPriority;
		int numberBuffersLookahead;
		bool resampling;
		IjvxRtAudioFileReader* fReader;
		CjvxGenericWrapperDevice* reservedBy;

		int bSize_rs;
		jvxData sRate_rs;
		jvxSize numFrames;
		jvxData progressDouble;
		bool oneoftheChannelsSelected;
		char** fieldSamplesSW;
		char** fieldSamplesRS;
		int idCrossReference;
		fileAccessMode stateAccess;
		//jvxInt16 numChannels;
		//IjvxResampler** resampler;
		
		std::vector<jvxBool> channelsSelected;
	} oneFileInput;
	
	typedef struct
	{
		int uniqueFileID;
		std::string fName;
		jvxInt16 numChannels;
		jvxSize lFile;
		jvxInt32 samplerate;
		bool boostPriority;
		int numberBuffersLookahead;
		bool resampling;
		//IjvxRtAudioFileWriter* fWriter;		
		jvxDataFormat enumOpen;
		CjvxGenericWrapperDevice* reservedBy;
		
		int bSize_rs;
		jvxData sRate_rs;
		bool oneoftheChannelsSelected;
		char** fieldSamplesSW;
		char** fieldSamplesRS;
		//IrtpResampler** resampler;
		int idCrossReference;
		fileAccessMode stateAccess;
	

		std::vector<bool> channelsSelected;
	} oneFileOutput;

	struct
	{
		std::vector<oneFileInput> inputFiles;
		std::vector<oneFileOutput> outputFiles;

	} shareWithUI;

	JVX_MUTEX_HANDLE accessInputFileList;
	JVX_MUTEX_HANDLE accessOutputFileList;
	//bool isInputFileListAccessible;
	//bool isOutputFileListAccessible;

	void updateChannelSelection(oneFileInput& ff)
	{
		int i;
		std::vector<bool> newChannelsSelected;
		jvxInt16 numChannels = 0;
		if(ff.fReader)
		{
			ff.fReader->properties(&numChannels, NULL, 
				NULL, NULL,
				NULL, NULL, NULL, 
				NULL, NULL);
		}
		for(i = 0; i < numChannels; i++)
		{
			if(i < (int)ff.channelsSelected.size())
			{
				newChannelsSelected.push_back(ff.channelsSelected[i]);
			}
			else
			{
				newChannelsSelected.push_back(false);
			}
		}
		ff.channelsSelected = newChannelsSelected;
	};

	void updateChannelSelection(oneFileOutput& ff)
	{
		int i;
		std::vector<bool> newChannelsSelected;
		jvxInt16 numChannels = ff.numChannels;
		for(i = 0; i < numChannels; i++)
		{
			if(i < (int) ff.channelsSelected.size())
			{
				newChannelsSelected.push_back(ff.channelsSelected[i]);
			}
			else
			{
				newChannelsSelected.push_back(false);
			}
		}
		ff.channelsSelected = newChannelsSelected;
	};

	// Todo: Lock machanism in case multiple devices access the same file handle
};

#endif