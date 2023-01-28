#include "jvx-helpers.h"

bool jvx_applyPropertyFilter(jvxDataFormat format, jvxSize num_values, jvxPropertyDecoderHintType decHtTp, jvxRealtimeViewerFilterShow filter)
{
	bool filterFulfilled = false;
	switch(filter)
	{
	case JVX_PROPERTIES_FILTER_PLOT_FIELDS:
		if(
			(format == JVX_DATAFORMAT_DATA) && (decHtTp == JVX_PROPERTY_DECODER_NONE)
			)
		{
			if(num_values >= 1)
			{
				filterFulfilled = true;
			}
		}
		break;
	case JVX_PROPERTIES_FILTER_CIRC_PLOT_FIELDS:
		if(
			(format == JVX_DATAFORMAT_DATA) &&
			(decHtTp == JVX_PROPERTY_DECODER_MULTI_CHANNEL_CIRCULAR_BUFFER)
			)
		{
			filterFulfilled = true;
		}
		break;
	case JVX_PROPERTIES_FILTER_PROPERTIES_RTV:
		if(num_values == 1)
		{
			switch(format)
			{
			case JVX_DATAFORMAT_8BIT:
			case JVX_DATAFORMAT_16BIT_LE:
			case JVX_DATAFORMAT_32BIT_LE:
			case JVX_DATAFORMAT_64BIT_LE:
			case JVX_DATAFORMAT_DATA:
			case JVX_DATAFORMAT_SIZE:
			case JVX_DATAFORMAT_STRING:
			case JVX_DATAFORMAT_STRING_LIST:
			case JVX_DATAFORMAT_VALUE_IN_RANGE:
				filterFulfilled = true;
				break;
			case JVX_DATAFORMAT_SELECTION_LIST:
				switch(decHtTp)
				{
				case JVX_PROPERTY_DECODER_BITFIELD:
				case JVX_PROPERTY_DECODER_SINGLE_SELECTION:
				case JVX_PROPERTY_DECODER_MULTI_SELECTION:
				case JVX_PROPERTY_DECODER_INPUT_FILE_LIST:
				case JVX_PROPERTY_DECODER_OUTPUT_FILE_LIST:
				case JVX_PROPERTY_DECODER_MULTI_SELECTION_CHANGE_ORDER:
				case JVX_PROPERTY_DECODER_NONE:
					filterFulfilled = true;
					break;
				}
				break;
			}
		}
		break;
	}
	return(filterFulfilled);
}


