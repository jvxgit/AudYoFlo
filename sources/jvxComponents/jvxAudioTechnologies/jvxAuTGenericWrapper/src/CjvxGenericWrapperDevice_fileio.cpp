#include "CjvxGenericWrapperTechnology.h"
#include "CjvxGenericWrapperDevice.h"

// ==========================================================================
// ==========================================================================


jvxErrorType
CjvxGenericWrapperDevice::removeInputFile_nolock(jvxSize idx)
{
	jvxSize i;
	jvxInt32 uniqueId = 0;
	jvxErrorType res = this->onInit.parentRef->removeInputFile_nolock(idx, uniqueId);
	if(res == JVX_NO_ERROR)
	{
		std::vector<oneEntryChannelMapper> newList;
		for(i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
		{
			switch(runtime.channelMappings.inputChannelMapper[i].theChannelType)
			{
			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE:
				if(runtime.channelMappings.inputChannelMapper[i].refHint != uniqueId)
				{
					newList.push_back(runtime.channelMappings.inputChannelMapper[i]);
				}

				break;
			default:
				newList.push_back(runtime.channelMappings.inputChannelMapper[i]);
				break;
			}
		}
		runtime.channelMappings.inputChannelMapper = newList;
	}
	return(res);
}

// =================================================================================

jvxErrorType
CjvxGenericWrapperDevice::removeOutputFile_nolock(jvxSize idx)
{
	jvxSize i;
	jvxInt32 uniqueId = 0;
	jvxErrorType res = this->onInit.parentRef->removeOutputFile_nolock(idx, uniqueId);
	if(res == JVX_NO_ERROR)
	{
		std::vector<oneEntryChannelMapper> newList;
		for(i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
		{
			switch(runtime.channelMappings.outputChannelMapper[i].theChannelType)
			{
			case JVX_GENERIC_WRAPPER_CHANNEL_TYPE_FILE:
				if(runtime.channelMappings.outputChannelMapper[i].refHint != uniqueId)
				{
					newList.push_back(runtime.channelMappings.outputChannelMapper[i]);
				}

				break;
			default:
				newList.push_back(runtime.channelMappings.outputChannelMapper[i]);
				break;
			}
		}
		runtime.channelMappings.outputChannelMapper = newList;
	}
	return(res);
}

// =====================================================================================

jvxErrorType
CjvxGenericWrapperDevice::addChannel_output_nolock(jvxSize idAdd, jvxInt32 uniqueId, std::string& descr, std::string& fName, jvxGenericWrapperChannelType tp)
{
	oneEntryChannelMapper newElm;
	newElm.description = descr;
	newElm.friendlyName = fName;
	newElm.isSelected = true;
	newElm.toHwFile.pointsToIdAllThisDevice = idAdd;
	newElm.toHwFile.pointsToIdSelectionThisDevice = idAdd;
	newElm.refHint = uniqueId;
	newElm.theChannelType = tp;
	runtime.channelMappings.outputChannelMapper.push_back(newElm);
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperDevice::remChannel_output_nolock(jvxSize idRemove, jvxInt32 uniqueId, jvxGenericWrapperChannelType tp)
{
	jvxSize i;
	std::vector<oneEntryChannelMapper> newList;
	for(i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
	{
		if(runtime.channelMappings.outputChannelMapper[i].theChannelType == tp)
		{
			if(!
				(
				(runtime.channelMappings.outputChannelMapper[i].refHint == uniqueId) &&
				(runtime.channelMappings.outputChannelMapper[i].toHwFile.pointsToIdAllThisDevice == idRemove)))
			{
				newList.push_back(runtime.channelMappings.outputChannelMapper[i]);
			}
		}
		else
		{
			newList.push_back(runtime.channelMappings.outputChannelMapper[i]);
		}
	}
	runtime.channelMappings.outputChannelMapper = newList;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperDevice::remChannel_output_id_nolock(jvxInt32 uniqueId, jvxGenericWrapperChannelType tp)
{
	jvxSize i;
	std::vector<oneEntryChannelMapper> newList;
	for(i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
	{
		if(runtime.channelMappings.outputChannelMapper[i].theChannelType == tp)
		{
			if(!
				(
				(runtime.channelMappings.outputChannelMapper[i].refHint == uniqueId)))
			{
				newList.push_back(runtime.channelMappings.outputChannelMapper[i]);
			}
		}
		else
		{
			newList.push_back(runtime.channelMappings.outputChannelMapper[i]);
		}
	}
	runtime.channelMappings.outputChannelMapper = newList;
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxGenericWrapperDevice::renameChannel_output_name_nolock(jvxInt32 uniqueId, std::string newName, jvxGenericWrapperChannelType tp)
{
	jvxSize i;
	std::vector<oneEntryChannelMapper> newList;
	for(i = 0; i < runtime.channelMappings.outputChannelMapper.size(); i++)
	{
		if(runtime.channelMappings.outputChannelMapper[i].theChannelType == tp)
		{
			if(runtime.channelMappings.outputChannelMapper[i].refHint == uniqueId)
			{
				runtime.channelMappings.outputChannelMapper[i].friendlyName = newName;
			}
		}
	}
	return(JVX_NO_ERROR);
}

// =====================================================================================

jvxErrorType
CjvxGenericWrapperDevice::addChannel_input_nolock(jvxSize idAdd, jvxInt32 uniqueId, std::string& descr, std::string& fName, jvxGenericWrapperChannelType tp)
{
	oneEntryChannelMapper newElm;
	newElm.description = descr;
	newElm.friendlyName = fName;
	newElm.isSelected = true;
	newElm.toHwFile.pointsToIdAllThisDevice = idAdd;
	newElm.toHwFile.pointsToIdSelectionThisDevice = idAdd;
	newElm.refHint = uniqueId;
	newElm.theChannelType = tp;
	runtime.channelMappings.inputChannelMapper.push_back(newElm);
	return(JVX_NO_ERROR);
}



jvxErrorType
CjvxGenericWrapperDevice::remChannel_input_nolock(jvxSize idRemove, jvxInt32 uniqueId, jvxGenericWrapperChannelType tp)
{
	jvxSize i;
	std::vector<oneEntryChannelMapper> newList;
	for(i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
	{
		if(runtime.channelMappings.inputChannelMapper[i].theChannelType == tp)
		{
			if(!
				(
				(runtime.channelMappings.inputChannelMapper[i].refHint == uniqueId) &&
				(runtime.channelMappings.inputChannelMapper[i].toHwFile.pointsToIdAllThisDevice == idRemove)))
			{
				newList.push_back(runtime.channelMappings.inputChannelMapper[i]);
			}
		}
		else
		{
			newList.push_back(runtime.channelMappings.inputChannelMapper[i]);
		}
	}
	runtime.channelMappings.inputChannelMapper = newList;
	return(JVX_NO_ERROR);
}


jvxErrorType
CjvxGenericWrapperDevice::remChannel_input_id_nolock(jvxInt32 uniqueId, jvxGenericWrapperChannelType tp)
{
	jvxSize i;
	std::vector<oneEntryChannelMapper> newList;
	for(i = 0; i < runtime.channelMappings.inputChannelMapper.size(); i++)
	{
		if(runtime.channelMappings.inputChannelMapper[i].theChannelType == tp)
		{
			if(!
				(
				(runtime.channelMappings.inputChannelMapper[i].refHint == uniqueId)))
			{
				newList.push_back(runtime.channelMappings.inputChannelMapper[i]);
			}
		}
		else
		{
			newList.push_back(runtime.channelMappings.inputChannelMapper[i]);
		}
	}
	runtime.channelMappings.inputChannelMapper = newList;
	return(JVX_NO_ERROR);
}

