#include "uMainWindow.h"

jvxErrorType 
uMainWindow::prepareChannels(jvxStandaloneHostState targetState)
{
	jvxSize i;
	jvxSize numChannels = 0;
	jvxSize numChannelsListen = 0;
	IjvxProperties* theProps = NULL;
	IjvxObject* theObj = NULL;
	jvxErrorType res = this->involvedComponents.theHost.hHost->request_hidden_interface_component(JVX_COMPONENT_AUDIO_DEVICE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
	if((res == JVX_NO_ERROR) && theProps)
	{
		res = this->involvedComponents.theHost.hHost->request_object_component(JVX_COMPONENT_AUDIO_DEVICE, &theObj);
		assert(res == JVX_NO_ERROR);

		jvxSelectionList fldSelList;

		jvxSize idx = 0;
		jvxDataFormat format = JVX_DATAFORMAT_NONE;
		jvxInt32 num = 0;
		jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
		jvxBool isValid = false;
		jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_READ_ONLY;	
		fldSelList.strList = NULL;

		numChannels = 0;
		if(jvx_findPropertyDescriptor("jvx_selection_input_channels_sellist", &idx, &format, &num, &decHtTp))
		{
			res = theProps->get_property(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST, idx, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, false, &isValid, &accessType);
			if(res == JVX_NO_ERROR)
			{
				jvxBitField sel = 0;
				for(i = 0; i < fldSelList.strList->lStrings; i++)
				{
					std::string token = fldSelList.strList->bStrings[i].bString;
					size_t pos = token.find("::");
					if(pos != std::string::npos)
					{
						token = token.substr(0,pos);
					}

					if(numChannels < JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS)
					{
						switch(targetState)
						{
						case JVX_STANDALONE_HOST_STATE_LISTENING:
						case JVX_STANDALONE_HOST_STATE_RECORDING:
							if(token == "hw")
							{
								sel |= ((jvxBitField)1 << i);
								numChannels++;
							}
							break;
						case JVX_STANDALONE_HOST_STATE_PLAYING:
						case JVX_STANDALONE_HOST_STATE_PROCESSING:
							if(token == "e")
							{
								sel |= ((jvxBitField)1 << i);
								numChannels++;
							}
							break;
						}
					}
				}
				theObj->deallocate(fldSelList.strList);
				fldSelList.bitFieldSelected = sel;
				res = theProps->set_property(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST, idx, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, true);
			}			
		}
		if(numChannels != JVX_STANDALONE_HOST_NUMBER_INPUT_CHANNELS)
		{	
			res = JVX_ERROR_INVALID_SETTING;
		}


		if(res == JVX_NO_ERROR)
		{
			numChannels = 0;
			numChannelsListen = 0;
			if(jvx_findPropertyDescriptor("jvx_selection_output_channels_sellist", &idx, &format, &num, &decHtTp))
			{
				res = theProps->get_property(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST, idx, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, false, &isValid, &accessType);
				if(res == JVX_NO_ERROR)
				{
					jvxBitField sel = 0;
					for(i = 0; i < fldSelList.strList->lStrings; i++)
					{
						std::string token = fldSelList.strList->bStrings[i].bString;
						size_t pos = token.find("::");
						if(pos != std::string::npos)
						{
							token = token.substr(0,pos);
						}

						if(systemParams.listen && (targetState == JVX_STANDALONE_HOST_STATE_RECORDING))
						{
							if(numChannelsListen < JVX_STANDALONE_HOST_NUMBER_OUTPUT_CHANNELS)
							{
								if(token == "hw")
								{
									sel |= ((jvxBitField)1 << i);
									numChannelsListen ++;
								}
							}
							if(numChannels < JVX_STANDALONE_HOST_NUMBER_OUTPUT_CHANNELS)
							{
								if(token == "e")
								{
									sel |= ((jvxBitField)1 << i);
									numChannels ++;
								}
							}
						}
						else
						{
							if(numChannels < JVX_STANDALONE_HOST_NUMBER_OUTPUT_CHANNELS)
							{
								switch(targetState)
								{
								case JVX_STANDALONE_HOST_STATE_LISTENING:
								case JVX_STANDALONE_HOST_STATE_PLAYING:
									if(token == "hw")
									{
										sel |= ((jvxBitField)1 << i);
										numChannels ++;
									}
									break;						
								case JVX_STANDALONE_HOST_STATE_PROCESSING:
								case JVX_STANDALONE_HOST_STATE_RECORDING:
									if(token == "e")
									{
										sel |= ((jvxBitField)1 << i);
										numChannels ++;
									}
									break;
								}
							}
						}
					}
					theObj->deallocate(fldSelList.strList);
					fldSelList.bitFieldSelected = sel;
					res = theProps->set_property(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST, idx, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, true);
				}	
				if((numChannels != JVX_STANDALONE_HOST_NUMBER_OUTPUT_CHANNELS) ||
					(systemParams.listen && (targetState == JVX_STANDALONE_HOST_STATE_RECORDING) && (numChannelsListen != JVX_STANDALONE_HOST_NUMBER_OUTPUT_CHANNELS)))
				{	
					res = JVX_ERROR_INVALID_SETTING;
				}
			}
			this->involvedComponents.theHost.hHost->return_hidden_interface_component(JVX_COMPONENT_AUDIO_NODE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));

		}
	}
	return(res);
}

