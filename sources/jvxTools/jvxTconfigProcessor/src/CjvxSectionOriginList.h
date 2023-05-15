#ifndef __CJVXSECTIONORIGINLIST_H__
#define __CJVXSECTIONORIGINLIST_H__

#include "jvx.h"

class CjvxSectionOriginList : public IjvxSectionOriginList
{
public:
	class CjvxOneSectionConfig
	{
	public:
		std::string sectionPath;
		std::string origin;
		int lineno = 0;
	};
	std::map<std::string, CjvxOneSectionConfig> sectionList;
	std::list<CjvxOneSectionConfig> duplicateList;

public:

	CjvxSectionOriginList();
	virtual jvxErrorType number_entries_map(jvxSize* num) override;
	virtual jvxErrorType entry_idx(jvxSize idx, jvxApiString* nmSection, jvxApiString* origin, jvxSize* line_no) override;

	virtual jvxErrorType number_entries_duplicate(jvxSize* num) override;
	virtual jvxErrorType duplicate_idx(jvxSize idx, jvxApiString* nmSection, jvxApiString* origin, jvxSize* line_no) override;
};

#endif
