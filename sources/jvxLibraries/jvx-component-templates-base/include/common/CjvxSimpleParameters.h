#ifndef _CJVXSIMPLEPARAMETERS_H__
#define _CJVXSIMPLEPARAMETERS_H__

class CjvxSimpleParameters
{
public:
	jvxInt32 buffersize = JVX_SIZE_UNSELECTED_INT32;
	jvxSize number_channels = JVX_SIZE_UNSELECTED_INT32;
	jvxInt32 samplerate = JVX_SIZE_UNSELECTED_INT32;
	jvxInt16 format = JVX_DATAFORMAT_NONE;
	jvxInt16 subformat = JVX_DATAFORMAT_GROUP_NONE;
	jvxInt16 data_flow = jvxDataflow::JVX_DATAFLOW_DONT_CARE;
	std::string format_spec;
	struct
	{
		jvxInt32 x = JVX_SIZE_UNSELECTED_INT32;
		jvxInt32 y = JVX_SIZE_UNSELECTED_INT32;
	} segmentation;

	void reset()
	{
		buffersize = JVX_SIZE_UNSELECTED_INT32;
		number_channels = JVX_SIZE_UNSELECTED;
		samplerate = JVX_SIZE_UNSELECTED_INT32;
		format = JVX_DATAFORMAT_NONE;
		subformat = JVX_DATAFORMAT_GROUP_NONE;
		data_flow = jvxDataflow::JVX_DATAFLOW_DONT_CARE;
		segmentation.x = JVX_SIZE_UNSELECTED_INT32;
		segmentation.y = JVX_SIZE_UNSELECTED_INT32;
		format_spec.clear();
	};

	void derive_buffersize()
	{
		jvxSize szElement = jvxDataFormatGroup_getsize_mult(subformat);
		jvxSize szElementsLine = segmentation.x * szElement;
		jvxSize szElementsField = segmentation.y * szElementsLine;
		buffersize = szElementsField / jvxDataFormatGroup_getsize_div(subformat);
	};
};

#endif
