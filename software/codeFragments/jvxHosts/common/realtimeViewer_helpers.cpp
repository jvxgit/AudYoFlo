#include "realtimeViewer_helpers.h"

void setInit(jvxRealtimeViewerPropertyDescriptor* theDescr)
{
	theDescr->reset();
};

jvxErrorType static_initData(jvxHandle* data, jvxSize sz, jvxRealtimePrivateMemoryDecoderEnum decodeHint)
{
	jvxRealtimeViewerPropertyPlotGroup* theLinearPlotPropGroup = NULL;
	jvxRealtimeViewerPropertyPlotItem* theLinearPlotPropItem = NULL;
	jvxRealtimeViewerPropertyCircPlotItem* theLinearCircPlotPropItem = NULL;
	jvxRealtimeViewerPropertyItem* thePropertyPropItem = NULL;
	jvxRealtimeViewerPropertySection* thePropertySection = NULL;

	// After returning, the field pointed to by privateData will be copied and afterwards never used again. Therefore using a static 
	// object is the best solution.
	
	switch(decodeHint)
	{
	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_GROUP:
		
		theLinearPlotPropGroup = (jvxRealtimeViewerPropertyPlotGroup*) data;
		setInit(&theLinearPlotPropGroup->descriptor);
		theLinearPlotPropGroup->autox = true;
		theLinearPlotPropGroup->autoy = true;
		theLinearPlotPropGroup->xmax = 1024;
		theLinearPlotPropGroup->xmin = 0;
		theLinearPlotPropGroup->ymax = 1.0;
		theLinearPlotPropGroup->ymin = 0.0;
		theLinearPlotPropGroup->xaxistxt = "Index";
		theLinearPlotPropGroup->xaxistxt = "Amplitude";
		break;
	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_ITEM:

		theLinearPlotPropItem = (jvxRealtimeViewerPropertyPlotItem*)data;
		setInit(&theLinearPlotPropItem->descriptorx);
		setInit(&theLinearPlotPropItem->descriptory);
		theLinearPlotPropItem->color = JVX_REALTIME_PLOT_COLOR_BLACK;
		theLinearPlotPropItem->width = JVX_REALTIME_PLOT_1PIXEL;
		theLinearPlotPropItem->linestyle = JVX_REALTIME_PLOT_LINE_SOLID;
		theLinearPlotPropItem->show = true;
		theLinearPlotPropItem->idPropX = JVX_SIZE_UNSELECTED;
		theLinearPlotPropItem->thePlotCurve = NULL;
		theLinearPlotPropItem->dataX = NULL;
		theLinearPlotPropItem->dataY = NULL;
		theLinearPlotPropItem->numValues = 0;
		theLinearPlotPropItem->maxX = JVX_DATA_MAX_NEG;
		theLinearPlotPropItem->minX = JVX_DATA_MAX_POS;
		theLinearPlotPropItem->maxY = JVX_DATA_MAX_NEG;
		theLinearPlotPropItem->minY = JVX_DATA_MAX_POS;
		theLinearPlotPropItem->minmaxValid = false;
		break;

	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_SPLOT_SINGLE_DIAG_ITEM:

		theLinearCircPlotPropItem = (jvxRealtimeViewerPropertyCircPlotItem*)data;
		theLinearCircPlotPropItem->length = 0;
		theLinearCircPlotPropItem->num_chans = 0;
		theLinearCircPlotPropItem->selection = 0;
		theLinearCircPlotPropItem->szCircBuffer = 0;
		theLinearCircPlotPropItem->numElmsCircBuffer = 0;
		theLinearCircPlotPropItem->ptrCircBuffer = NULL;
		theLinearCircPlotPropItem->thePlotCurves = NULL;
		theLinearCircPlotPropItem->dataInc = NULL;
		theLinearCircPlotPropItem->dataCpy = NULL;
		theLinearCircPlotPropItem->lineWidth = 1;
		theLinearCircPlotPropItem->isValid = false;
		theLinearCircPlotPropItem->idChannelX = JVX_SIZE_UNSELECTED;
		theLinearCircPlotPropItem->safeAccessHdl = NULL;
		theLinearCircPlotPropItem->maxX = JVX_DATA_MAX_NEG;
		theLinearCircPlotPropItem->minX = JVX_DATA_MAX_POS;
		theLinearCircPlotPropItem->maxY = JVX_DATA_MAX_NEG;
		theLinearCircPlotPropItem->minY = JVX_DATA_MAX_POS;
		theLinearCircPlotPropItem->minmaxValid = false;
		theLinearCircPlotPropItem->expectRescaleX = true;

		break;
	case JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_ITEM:
			
		thePropertyPropItem = (jvxRealtimeViewerPropertyItem*)data;
		setInit(&thePropertyPropItem->descriptor);
		thePropertyPropItem->setup_complete = false;
		thePropertyPropItem->line_edit = NULL;
		thePropertyPropItem->sel_access = 0; // read
		//std::vector<QLabel*> label_flags;
		//std::vector<QLabel*> label_entries;
		thePropertyPropItem->combo_box = NULL;
		thePropertyPropItem->label_min = NULL;
		thePropertyPropItem->slider = NULL;
		thePropertyPropItem->label_max = NULL;
		thePropertyPropItem->label_valid = NULL;
		thePropertyPropItem->button_1 = NULL;
		thePropertyPropItem->button_2 = NULL;
		thePropertyPropItem->button_3 = NULL;
		thePropertyPropItem->button_4 = NULL;
		thePropertyPropItem->combo_box_access_rwcd = NULL;
		thePropertyPropItem->combo_box_access_what = NULL;
		thePropertyPropItem->combo_box_config = NULL;
		thePropertyPropItem->frame_tags = NULL;
		thePropertyPropItem->checkbox_onoff = NULL;
		// thePropertyPropItem->ctxt
		//thePropertyPropItem->checkbox_1 = NULL;
		thePropertyPropItem->selection = JVX_SIZE_UNSELECTED;
		break;
	case JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_SECTION:
		thePropertySection = (jvxRealtimeViewerPropertySection*)data;
		thePropertySection->scrollX = 0;
		thePropertySection->scrollY = 0;
		break;

	default:
		break;
	}

	return(JVX_NO_ERROR);
}

jvxErrorType static_allocateData(jvxHandle** privateData, jvxSize* sz, jvxRealtimePrivateMemoryDecoderEnum decodeHint)
{
	jvxRealtimeViewerPropertyPlotGroup* theLinearPlotPropGroup = NULL;
	jvxRealtimeViewerPropertyPlotItem* theLinearPlotPropItem = NULL;
	jvxRealtimeViewerPropertyCircPlotItem* theLinearCircPlotPropItem = NULL;
	jvxRealtimeViewerPropertyItem* thePropertyPropItem = NULL;
	jvxRealtimeViewerPropertySection* thePropertySection = NULL;

	// After returning, the field pointed to by privateData will be copied and afterwards never used again. Therefore using a static 
	// object is the best solution.
	
	*privateData = NULL;
	*sz = 0;

	switch(decodeHint)
	{
	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_GROUP:
		
		theLinearPlotPropGroup = new jvxRealtimeViewerPropertyPlotGroup;
		*privateData = theLinearPlotPropGroup;
		*sz = sizeof(jvxRealtimeViewerPropertyPlotGroup);
		break;
	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_ITEM:

		theLinearPlotPropItem = new jvxRealtimeViewerPropertyPlotItem;
		*privateData = theLinearPlotPropItem;
		*sz = sizeof(jvxRealtimeViewerPropertyPlotItem);
		break;
	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_SPLOT_SINGLE_DIAG_ITEM:

		theLinearCircPlotPropItem = new jvxRealtimeViewerPropertyCircPlotItem;
		*privateData = theLinearCircPlotPropItem;
		*sz = sizeof(jvxRealtimeViewerPropertyCircPlotItem);
		break;
	case JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_ITEM:
			
		thePropertyPropItem = new jvxRealtimeViewerPropertyItem;
		*privateData = thePropertyPropItem;
		*sz = sizeof(jvxRealtimeViewerPropertyItem);
		break;
	case JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_SECTION:
		thePropertySection = new jvxRealtimeViewerPropertySection;
		*privateData = thePropertySection;
		*sz = sizeof(jvxRealtimeViewerPropertySection);
		break;
	default:
		break;
	}
	if(*privateData)
	{
		static_initData(*privateData, *sz, decodeHint);
	}
	return(JVX_NO_ERROR);
}
	
jvxErrorType static_putConfiguration(IjvxConfigProcessor* theReader, jvxConfigData* readEntriesFrom, jvxHandle* privateDataItem, jvxSize sz, jvxRealtimePrivateMemoryDecoderEnum decodeId, 
									 jvxInt16 itemId)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string entry;
	jvxConfigData* datTmp = NULL;
	jvxRealtimeViewerPropertyPlotItem* thePropLinPlotItem = NULL;
	jvxRealtimeViewerPropertyCircPlotItem* thePropCircLinPlotItem = NULL;
	jvxRealtimeViewerPropertyPlotGroup* thePropLinPlotGroup = NULL;
	jvxRealtimeViewerPropertySection* thePropSection = NULL;

	switch(decodeId)
	{
	case JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_ITEM:
		// Nothing to do but return positive
		res = JVX_NO_ERROR;
		break;

	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_GROUP:
	
		if(sz == sizeof(jvxRealtimeViewerPropertyPlotGroup))
		{
			entry = JVX_RTV_ONE_LINEAR_PLOT_GROUP;
			datTmp = NULL;
			thePropLinPlotGroup = (jvxRealtimeViewerPropertyPlotGroup*) privateDataItem;

			res = theReader->getReferenceSubsectionCurrentSection_name(readEntriesFrom, &datTmp, entry.c_str());
			if((res == JVX_NO_ERROR) && datTmp)
			{
				HjvxConfigProcessor_readEntry_assignment<jvxData>(theReader, datTmp, JVX_RTV_LINPLOT_XMAX, &thePropLinPlotGroup->xmax);
				HjvxConfigProcessor_readEntry_assignment<jvxData>(theReader, datTmp, JVX_RTV_LINPLOT_XMIN, &thePropLinPlotGroup->xmin);
				HjvxConfigProcessor_readEntry_assignment<jvxData>(theReader, datTmp, JVX_RTV_LINPLOT_YMAX, &thePropLinPlotGroup->ymax);
				HjvxConfigProcessor_readEntry_assignment<jvxData>(theReader, datTmp, JVX_RTV_LINPLOT_YMIN, &thePropLinPlotGroup->ymin);
				HjvxConfigProcessor_readEntry_assignment<jvxInt16>(theReader, datTmp, JVX_RTV_LINPLOT_AUTOX, &thePropLinPlotGroup->autox);
				HjvxConfigProcessor_readEntry_assignment<jvxInt16>(theReader, datTmp, JVX_RTV_LINPLOT_AUTOY, &thePropLinPlotGroup->autoy);
				HjvxConfigProcessor_readEntry_assignmentString(theReader, datTmp, JVX_RTV_LINPLOT_XAXIS, &thePropLinPlotGroup->xaxistxt);
				HjvxConfigProcessor_readEntry_assignmentString(theReader, datTmp, JVX_RTV_LINPLOT_YAXIS, &thePropLinPlotGroup->yaxistxt);
			}
			else
			{
				res = JVX_ERROR_UNEXPECTED;
			}
		}
		else
		{
			res = JVX_ERROR_SIZE_MISMATCH;
		}
		break;
	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_ITEM:
	
		if(sz == sizeof(jvxRealtimeViewerPropertyPlotItem))
		{
			entry = JVX_RTV_ONE_LINEAR_PLOT_ITEM_PREFIX + jvx_int2String(itemId);
			datTmp = NULL;
			thePropLinPlotItem = (jvxRealtimeViewerPropertyPlotItem*) privateDataItem;

			res = theReader->getReferenceSubsectionCurrentSection_name(readEntriesFrom, &datTmp, entry.c_str());
			if((res == JVX_NO_ERROR) && datTmp)
			{
				HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_LINPLOT_COLOR_ID, &thePropLinPlotItem->color);
				HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_LINPLOT_WIDTH_ID, &thePropLinPlotItem->width);
				HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_LINPLOT_STYLE_ID, &thePropLinPlotItem->linestyle);
				HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_LINPLOT_SHOW, &thePropLinPlotItem->show);
			}
			else
			{
				res = JVX_ERROR_UNEXPECTED;
			}
		}
		else
		{
			res = JVX_ERROR_SIZE_MISMATCH;
		}
		break;
	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_SPLOT_SINGLE_DIAG_ITEM:
		
		if(sz == sizeof(jvxRealtimeViewerPropertyCircPlotItem))
		{
			entry = JVX_RTV_ONE_LINEAR_CIRC_PLOT_ITEM_PREFIX + jvx_int2String(itemId);
			datTmp = NULL;
			thePropCircLinPlotItem = (jvxRealtimeViewerPropertyCircPlotItem*) privateDataItem;

			res = theReader->getReferenceSubsectionCurrentSection_name(readEntriesFrom, &datTmp, entry.c_str());
			if((res == JVX_NO_ERROR) && datTmp)
			{
				HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_CIRC_SPLOT_LINE_WIDTH, &thePropCircLinPlotItem->lineWidth);
				HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_CIRC_SPLOT_XCHANNEL_SELECTION, &thePropCircLinPlotItem->idChannelX);
				HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_CIRC_SPLOT_BUFFER_LENGTH, &thePropCircLinPlotItem->length);
				HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_CIRC_SPLOT_NUM_CHANNELS, &thePropCircLinPlotItem->num_chans);
				HjvxConfigProcessor_readEntry_assignmentBitField(theReader, datTmp, JVX_RTV_LINPLOT_CIRCPLOT_SELECTION_Y,&thePropCircLinPlotItem->selection);
			}
			else
			{
				res = JVX_ERROR_UNEXPECTED;
			}
		}
		else
		{
			res = JVX_ERROR_SIZE_MISMATCH;
		}
		break;
	case JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_SECTION:

		if(sz == sizeof(jvxRealtimeViewerPropertySection))
		{
			entry = JVX_RTV_ONE_PROP_SECTION;
			datTmp = NULL;
			thePropSection = (jvxRealtimeViewerPropertySection*) privateDataItem;
			res = theReader->getReferenceSubsectionCurrentSection_name(readEntriesFrom, &datTmp, entry.c_str());
			if((res == JVX_NO_ERROR) && datTmp)
			{
				HjvxConfigProcessor_readEntry_assignment<jvxInt16>(theReader, datTmp, JVX_RTV_PROPERTY_SECTION_SCROLL_X, &thePropSection->scrollX);
				HjvxConfigProcessor_readEntry_assignment<jvxInt16>(theReader, datTmp, JVX_RTV_PROPERTY_SECTION_SCROLL_Y, &thePropSection->scrollY);
			}
			else
			{
				res = JVX_ERROR_UNEXPECTED;
			}
		}
		else
		{
			res = JVX_ERROR_SIZE_MISMATCH;
		}
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
		break;
	}
	return(res);
}

jvxErrorType static_getConfiguration(IjvxConfigProcessor* theReader, jvxConfigData* writeEntriesTo, jvxHandle* privateDataItem, jvxSize sz, jvxRealtimePrivateMemoryDecoderEnum decodeId, jvxInt16 itemId)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string entry;
	jvxConfigData* datTmp = NULL;
	jvxRealtimeViewerPropertyPlotItem* thePropLinPlotItem = NULL;
	jvxRealtimeViewerPropertyCircPlotItem* thePropCircLinPlotItem = NULL;
	jvxRealtimeViewerPropertyPlotGroup* thePropLinPlotGroup = NULL;
	jvxRealtimeViewerPropertySection* thePropSection = NULL;

	switch(decodeId)
	{
	case JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_ITEM:
		// Nothing to do but return positive
		res = JVX_NO_ERROR;
		break;
	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_GROUP:

		if(sz == sizeof(jvxRealtimeViewerPropertyPlotGroup))
		{
			entry = JVX_RTV_ONE_LINEAR_PLOT_GROUP;
			datTmp = NULL;
			thePropLinPlotGroup = (jvxRealtimeViewerPropertyPlotGroup*) privateDataItem;

			res = theReader->createEmptySection(&datTmp, entry.c_str());
			if((res == JVX_NO_ERROR) && datTmp)
			{
				HjvxConfigProcessor_writeEntry_assignment<jvxData>(theReader, datTmp, JVX_RTV_LINPLOT_XMAX, &thePropLinPlotGroup->xmax);
				HjvxConfigProcessor_writeEntry_assignment<jvxData>(theReader, datTmp, JVX_RTV_LINPLOT_XMIN, &thePropLinPlotGroup->xmin);
				HjvxConfigProcessor_writeEntry_assignment<jvxData>(theReader, datTmp, JVX_RTV_LINPLOT_YMAX, &thePropLinPlotGroup->ymax);
				HjvxConfigProcessor_writeEntry_assignment<jvxData>(theReader, datTmp, JVX_RTV_LINPLOT_YMIN, &thePropLinPlotGroup->ymin);
				HjvxConfigProcessor_writeEntry_assignment<jvxInt16>(theReader, datTmp, JVX_RTV_LINPLOT_AUTOX, &thePropLinPlotGroup->autox);
				HjvxConfigProcessor_writeEntry_assignment<jvxInt16>(theReader, datTmp, JVX_RTV_LINPLOT_AUTOY, &thePropLinPlotGroup->autoy);
				HjvxConfigProcessor_writeEntry_assignmentString(theReader, datTmp, JVX_RTV_LINPLOT_XAXIS, &thePropLinPlotGroup->xaxistxt);
				HjvxConfigProcessor_writeEntry_assignmentString(theReader, datTmp, JVX_RTV_LINPLOT_YAXIS, &thePropLinPlotGroup->yaxistxt);
				theReader->addSubsectionToSection(writeEntriesTo, datTmp);
			}
			else
			{
				res = JVX_ERROR_UNEXPECTED;
			}
		}
		else
		{
			res = JVX_ERROR_SIZE_MISMATCH;
		}
		break;
			
	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_ITEM:

		if(sz == sizeof(jvxRealtimeViewerPropertyPlotItem))
		{
			entry = JVX_RTV_ONE_LINEAR_PLOT_ITEM_PREFIX + jvx_int2String(itemId);
			datTmp = NULL;
			thePropLinPlotItem = (jvxRealtimeViewerPropertyPlotItem*) privateDataItem;

			res = theReader->createEmptySection(&datTmp, entry.c_str());
			if((res == JVX_NO_ERROR) && datTmp)
			{
				HjvxConfigProcessor_writeEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_LINPLOT_COLOR_ID, &thePropLinPlotItem->color);
				HjvxConfigProcessor_writeEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_LINPLOT_WIDTH_ID, &thePropLinPlotItem->width);
				HjvxConfigProcessor_writeEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_LINPLOT_STYLE_ID, &thePropLinPlotItem->linestyle);
				HjvxConfigProcessor_writeEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_LINPLOT_SHOW, &thePropLinPlotItem->show);
				theReader->addSubsectionToSection(writeEntriesTo, datTmp);
			}
			else
			{
				res = JVX_ERROR_UNEXPECTED;
			}
		}
		else
		{
			res = JVX_ERROR_SIZE_MISMATCH;
		}
		break;
			
	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_SPLOT_SINGLE_DIAG_ITEM:

		if(sz == sizeof(jvxRealtimeViewerPropertyCircPlotItem))
		{
			entry = JVX_RTV_ONE_LINEAR_CIRC_PLOT_ITEM_PREFIX + jvx_int2String(itemId);
			datTmp = NULL;
			thePropCircLinPlotItem = (jvxRealtimeViewerPropertyCircPlotItem*) privateDataItem;

			res = theReader->createEmptySection(&datTmp, entry.c_str());
			if((res == JVX_NO_ERROR) && datTmp)
			{
				HjvxConfigProcessor_writeEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_CIRC_SPLOT_LINE_WIDTH, &thePropCircLinPlotItem->lineWidth);
				HjvxConfigProcessor_writeEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_CIRC_SPLOT_XCHANNEL_SELECTION, &thePropCircLinPlotItem->idChannelX);
				HjvxConfigProcessor_writeEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_CIRC_SPLOT_BUFFER_LENGTH, &thePropCircLinPlotItem->length);
				HjvxConfigProcessor_writeEntry_assignment<jvxSize>(theReader, datTmp, JVX_RTV_CIRC_SPLOT_NUM_CHANNELS, &thePropCircLinPlotItem->num_chans);
				HjvxConfigProcessor_writeEntry_assignmentBitField(theReader, datTmp, JVX_RTV_LINPLOT_CIRCPLOT_SELECTION_Y,&thePropCircLinPlotItem->selection);

				theReader->addSubsectionToSection(writeEntriesTo, datTmp);
			}
			else
			{
				res = JVX_ERROR_UNEXPECTED;
			}
		}
		else
		{
			res = JVX_ERROR_SIZE_MISMATCH;
		}
		break;
	case JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_SECTION:
		if(sz == sizeof(jvxRealtimeViewerPropertySection))
		{
			entry = JVX_RTV_ONE_PROP_SECTION;
			datTmp = NULL;
			thePropSection = (jvxRealtimeViewerPropertySection*) privateDataItem;

			res = theReader->createEmptySection(&datTmp, entry.c_str());
			if((res == JVX_NO_ERROR) && datTmp)
			{			
				HjvxConfigProcessor_writeEntry_assignment<jvxInt16>(theReader, datTmp, JVX_RTV_PROPERTY_SECTION_SCROLL_X, &thePropSection->scrollX);
				HjvxConfigProcessor_writeEntry_assignment<jvxInt16>(theReader, datTmp, JVX_RTV_PROPERTY_SECTION_SCROLL_Y, &thePropSection->scrollY);
				theReader->addSubsectionToSection(writeEntriesTo, datTmp);
			}
			else
			{
				res = JVX_ERROR_UNEXPECTED;
			}
		}
		else
		{
			res = JVX_ERROR_SIZE_MISMATCH;
		}
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
		break;
	}
	return(res);
}

jvxErrorType static_copyData(jvxHandle* copyTo, jvxHandle* copyFrom, jvxSize szFld, jvxRealtimePrivateMemoryDecoderEnum decodeId)
{
	jvxErrorType res = JVX_NO_ERROR;
	switch(decodeId)
	{
	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_GROUP:

		if(szFld == sizeof(jvxRealtimeViewerPropertyPlotGroup))
		{
			jvxRealtimeViewerPropertyPlotGroup* thePropLinPlotGroup_to = (jvxRealtimeViewerPropertyPlotGroup*) copyTo;
			jvxRealtimeViewerPropertyPlotGroup* thePropLinPlotGroup_from = (jvxRealtimeViewerPropertyPlotGroup*) copyFrom;

			*thePropLinPlotGroup_to = *thePropLinPlotGroup_from;

		}
		else
		{
			res = JVX_ERROR_SIZE_MISMATCH;
		}
		break;
			
	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_ITEM:

		if(szFld == sizeof(jvxRealtimeViewerPropertyPlotItem))
		{
			jvxRealtimeViewerPropertyPlotItem* thePropLinPlotItem_to = (jvxRealtimeViewerPropertyPlotItem*) copyTo;
			jvxRealtimeViewerPropertyPlotItem* thePropLinPlotItem_from = (jvxRealtimeViewerPropertyPlotItem*) copyFrom;

			*thePropLinPlotItem_to = *thePropLinPlotItem_from;
		}
		else
		{
			res = JVX_ERROR_SIZE_MISMATCH;
		}
		break;
	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_SPLOT_SINGLE_DIAG_ITEM:

		if(szFld == sizeof(jvxRealtimeViewerPropertyCircPlotItem))
		{
			jvxRealtimeViewerPropertyCircPlotItem* thePropLinCircPlotItem_to = (jvxRealtimeViewerPropertyCircPlotItem*) copyTo;
			jvxRealtimeViewerPropertyCircPlotItem* thePropLinCircPlotItem_from = (jvxRealtimeViewerPropertyCircPlotItem*) copyFrom;

			*thePropLinCircPlotItem_to = *thePropLinCircPlotItem_from;
		}
		else
		{
			res = JVX_ERROR_SIZE_MISMATCH;
		}
		break;

	case JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_ITEM:

		if(szFld == sizeof(jvxRealtimeViewerPropertyItem))
		{
			jvxRealtimeViewerPropertyItem* thePropItem_to = (jvxRealtimeViewerPropertyItem*) copyTo;
			jvxRealtimeViewerPropertyItem* thePropItem_from = (jvxRealtimeViewerPropertyItem*) copyFrom;

			*thePropItem_to = *thePropItem_from;
		}
		else
		{
			res = JVX_ERROR_SIZE_MISMATCH;
		}
		break;
	
	case JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_SECTION:

		if(szFld == sizeof(jvxRealtimeViewerPropertySection))
		{
			jvxRealtimeViewerPropertySection* thePropSection_to = (jvxRealtimeViewerPropertySection*) copyTo;
			jvxRealtimeViewerPropertySection* thePropSection_from = (jvxRealtimeViewerPropertySection*) copyFrom;

			*thePropSection_to = *thePropSection_from;

		}
		else
		{
			res = JVX_ERROR_SIZE_MISMATCH;
		}
		break;
	default:
		res = JVX_ERROR_UNSUPPORTED;
		break;
	}
	return(res);
}

jvxErrorType static_deallocateData(jvxHandle* privateData, jvxSize sz, jvxRealtimePrivateMemoryDecoderEnum decodeHint)
{
	jvxRealtimeViewerPropertyPlotGroup* theLinearPlotPropGroup = NULL;
	jvxRealtimeViewerPropertyPlotItem* theLinearPlotPropItem = NULL;
	jvxRealtimeViewerPropertyCircPlotItem* theLinearCircPlotPropItem = NULL;
	jvxRealtimeViewerPropertyItem* theLinearPropertyItem = NULL;
	jvxRealtimeViewerPropertySection* thePropSection = NULL;

	// After returning, the field pointed to by privateData will be copied and afterwards never used again. Therefore using a static 
	// object is the best solution.
		
	switch(decodeHint)
	{
	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_GROUP:
		
		theLinearPlotPropGroup = (jvxRealtimeViewerPropertyPlotGroup*) privateData;
		delete(theLinearPlotPropGroup);
		break;
	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_PLOT_ITEM:
		theLinearPlotPropItem = (jvxRealtimeViewerPropertyPlotItem*)privateData;

		assert(theLinearPlotPropItem->thePlotCurve == NULL);
		assert(theLinearPlotPropItem->dataY == NULL);
		assert(theLinearPlotPropItem->dataX == NULL);
		//JVX_SAFE_DELETE_FLD(theLinearPlotPropItem->dataY, jvxData);
		//JVX_SAFE_DELETE_FLD(theLinearPlotPropItem->dataX, jvxData);
		theLinearPlotPropItem->numValues = 0;

		delete(theLinearPlotPropItem);
		break;

	case JVX_REALTIME_PRIVATE_MEMORY_LINEAR_SPLOT_SINGLE_DIAG_ITEM:
		theLinearCircPlotPropItem = (jvxRealtimeViewerPropertyCircPlotItem*)privateData;
		assert(theLinearCircPlotPropItem->thePlotCurves == NULL);
		assert(theLinearCircPlotPropItem->dataCpy == NULL);
		assert(theLinearCircPlotPropItem->dataInc == NULL);
		//JVX_SAFE_DELETE_FLD(theLinearPlotPropItem->dataY, jvxData);
		//JVX_SAFE_DELETE_FLD(theLinearPlotPropItem->dataX, jvxData);

		delete(theLinearPlotPropItem);
		break;

	case JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_ITEM:
		
		theLinearPropertyItem = (jvxRealtimeViewerPropertyItem*) privateData;
		delete(theLinearPropertyItem);
		break;
	
	case JVX_REALTIME_PRIVATE_MEMORY_PROPERTY_SECTION:
		
		thePropSection = (jvxRealtimeViewerPropertySection*) privateData;
		delete(thePropSection);
		break;
	default:
		break;
	}

	return(JVX_NO_ERROR);
}
