#ifndef __HJVXCOMPONENTPRESELECTION_COMMON_H__
#define __HJVXCOMPONENTPRESELECTION_COMMON_H__

/**
 * Property filters
 *///============================================================
typedef enum
{
	JVX_PROPERTIES_FILTER_PLOT_FIELDS,
	JVX_PROPERTIES_FILTER_CIRC_PLOT_FIELDS,
	JVX_PROPERTIES_FILTER_PROPERTIES_RTV
} jvxRealtimeViewerFilterShow;

bool jvx_applyPropertyFilter(jvxDataFormat format, jvxSize num_values, jvxPropertyDecoderHintType decHtTp, jvxRealtimeViewerFilterShow filter);

#endif
