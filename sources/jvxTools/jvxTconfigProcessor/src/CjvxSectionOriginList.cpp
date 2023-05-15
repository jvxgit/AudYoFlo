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

jvxErrorType 
CjvxSectionOriginList::start_collect_file_contents()
{
	if (collections.empty())
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_COMPONENT_BUSY;
}

jvxErrorType 
CjvxSectionOriginList::stop_collect_file_contents()
{
	collections.clear();
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSectionOriginList::number_collected_file_contents(jvxSize* num)
{
	if (num)
	{
		*num = collections.size();
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSectionOriginList::number_collected_file_idx_number_content(jvxSize idx, jvxSize* num)
{
	if (idx < collections.size())
	{
		auto elm = collections.begin();
		std::advance(elm, idx);
		if(num)
		{
			*num = elm->second.content_sections.size();
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType 
CjvxSectionOriginList::get_collected_file_idx_content_idx(jvxSize idxFile, jvxApiString* fName, jvxApiString* content, jvxSize idxContent)
{
	if (idxFile < collections.size())
	{
		auto elm = collections.begin();
		std::advance(elm, idxFile);

		if (idxContent < elm->second.content_sections.size())
		{
			auto elml = elm->second.content_sections.begin();
			std::advance(elml, idxContent);
			if (fName)
			{
				*fName = elm->second.fName;
			}
			if (content)
			{
				content->assign(*elml);
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}


