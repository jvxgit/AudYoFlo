#include "CjvxSectionOriginList.h"

CjvxSectionOriginList::CjvxSectionOriginList()
{
}

jvxErrorType 
CjvxSectionOriginList::number_entries_map(jvxSize* num)
{
	if (num)
	{
		*num = sectionList.size();
	}
	return JVX_NO_ERROR; 
}

jvxErrorType 
CjvxSectionOriginList::entry_idx(jvxSize idx, jvxApiString* nmSection, jvxApiString* origin, jvxSize* line_no)
{
	if (idx < sectionList.size())
	{
		auto elm = sectionList.begin();
		std::advance(elm, idx);
		nmSection->assign(elm->second.sectionPath);
		origin->assign(elm->second.origin);
		if (line_no)
		{
			*line_no = elm->second.lineno;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType 
CjvxSectionOriginList::number_entries_duplicate(jvxSize* num)
{
	if (num)
	{
		*num = duplicateList.size();
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSectionOriginList::duplicate_idx(jvxSize idx, jvxApiString* nmSection, jvxApiString* origin, jvxSize* line_no) 
{
	if (idx < duplicateList.size())
	{
		auto elm = duplicateList.begin();
		std::advance(elm, idx);
		nmSection->assign(elm->sectionPath);
		origin->assign(elm->origin);
		if (line_no)
		{
			*line_no = elm->lineno;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}


