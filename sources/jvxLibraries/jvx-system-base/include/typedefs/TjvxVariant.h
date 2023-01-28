#ifndef __JVXVARIANT_H__
#define __JVXVARIANT_H__

class jvxVariant
{
private:
	jvxHandle* content;
	jvxDataFormat format;
	jvxSize offset;
	jvxSize numElements;
	std::string propDescr;
	jvxSize propId;
	jvxPropertyCategoryType cat;
	jvxBool memAllocated;
	
	void memoryClean()
	{
		content = NULL;
		format = JVX_DATAFORMAT_NONE;
		offset = 0;
		numElements = 0;
		propId = JVX_SIZE_UNSELECTED;
		cat = JVX_PROPERTY_CATEGORY_UNKNOWN;
		memAllocated = false;
	};

	void memoryCleanDealloc()
	{
		assert(0);
	}
public:

	jvxVariant()
	{
		memoryClean();
	};
	~jvxVariant(){};

	// =====================================================================

	void assign(jvxApiString* str, const char* pDescr = NULL, jvxSize pId = JVX_SIZE_UNSELECTED, jvxPropertyCategoryType catI = JVX_PROPERTY_CATEGORY_UNKNOWN)
	{
		if (memAllocated)
		{
			memoryCleanDealloc();
		}
		else
		{
			memoryClean();
		}
		content = str;
		format = JVX_DATAFORMAT_STRING;
		offset = 0;
		numElements = 1;
		propDescr.clear();
		if (pDescr)
			propDescr = pDescr;
		propId = pId;
		cat = catI;
		memAllocated = false;
	};

	// =====================================================================

	jvxErrorType getApiString(jvxApiString** ptr)
	{
		if (format == JVX_DATAFORMAT_STRING)
		{
			*ptr = (jvxApiString*)content;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_SETTING;
	};
};

#endif
