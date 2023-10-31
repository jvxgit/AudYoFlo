#ifndef __TJVXPROPERTYSTREAMEXTENDER_H__
#define __TJVXPROPERTYSTREAMEXTENDER_H__

// TjvxPropertyStreamExtender.h

#include "typedefs/property-extender/TpjvxPropertyExtender.h"

enum class jvxPropertyExtenderType
{
	JVX_PROPERTY_EXTENDER_NONE
#if defined JVX_PRODUCT_PROPERTY_EXTENDER_TYPES
	JVX_PRODUCT_PROPERTY_EXTENDER_TYPES
#endif
	, JVX_PROPERTY_EXTENDER_LIMIT
};

#endif
