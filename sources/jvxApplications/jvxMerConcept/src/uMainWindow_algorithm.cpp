#include "uMainWindow.h"

jvxErrorType 
uMainWindow::checkStatusAlgorithm_engaged(jvxBool& isEngaged)
{
	IjvxProperties* theProps = NULL;
	jvxErrorType res = this->involvedComponents.theHost.hHost->request_hidden_interface_component(JVX_COMPONENT_AUDIO_NODE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
	if((res == JVX_NO_ERROR) && theProps)
	{
		jvxSelectionList fldSelList;

		jvxSize idx = 0;
		jvxDataFormat format = JVX_DATAFORMAT_NONE;
		jvxInt32 num = 0;
		jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
		jvxBool isValid = false;
		jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_READ_ONLY;	
		jvxStringList fldStr;
		fldStr.lStrings = 0;
		fldSelList.strList = &fldStr;

		if(jvx_findPropertyDescriptor("jvx_engage_sellist", &idx, &format, &num, &decHtTp))
		{
			res = theProps->get_property(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST, idx, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, true, &isValid, &accessType);
			if(res == JVX_NO_ERROR)
			{
				if(JVX_EVALUATE_BITFIELD(fldSelList.bitFieldSelected & 0x1))
				{
					isEngaged = true;
				}
				else
				{
					isEngaged = false;
				}
			}
		}
		this->involvedComponents.theHost.hHost->return_hidden_interface_component(JVX_COMPONENT_AUDIO_NODE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
	}
	return(res);
}

jvxErrorType 
uMainWindow::toggleStatusAlgorithm_engaged()
{
	IjvxProperties* theProps = NULL;
	jvxErrorType res = this->involvedComponents.theHost.hHost->request_hidden_interface_component(JVX_COMPONENT_AUDIO_NODE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theProps));
	if((res == JVX_NO_ERROR) && theProps)
	{
		jvxSelectionList fldSelList;

		jvxSize idx = 0;
		jvxDataFormat format = JVX_DATAFORMAT_NONE;
		jvxInt32 num = 0;
		jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
		jvxBool isValid = false;
		jvxPropertyAccessType accessType = JVX_PROPERTY_ACCESS_READ_ONLY;	
		jvxStringList fldStr;
		fldStr.lStrings = 0;
		fldSelList.strList = &fldStr;

		if(jvx_findPropertyDescriptor("jvx_engage_sellist", &idx, &format, &num, &decHtTp))
		{
			res = theProps->get_property(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST, idx, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, true, &isValid, &accessType);
			if(res == JVX_NO_ERROR)
			{
				if(JVX_EVALUATE_BITFIELD(fldSelList.bitFieldSelected & 0x1))
				{
					fldSelList.bitFieldSelected = 0x2;
				}
				else
				{
					fldSelList.bitFieldSelected = 0x1;
				}
				res = theProps->set_property(&fldSelList, 1, JVX_DATAFORMAT_SELECTION_LIST, idx, JVX_PROPERTY_CATEGORY_PREDEFINED, 0, true);
			}
		}
		this->involvedComponents.theHost.hHost->return_hidden_interface_component(JVX_COMPONENT_AUDIO_NODE, JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theProps));
	}
	return(res);
}