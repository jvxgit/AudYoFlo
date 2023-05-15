#ifndef __CJVXSECTIONORIGINLIST_H__
#define __CJVXSECTIONORIGINLIST_H__

#include "jvx.h"

class CjvxSectionOriginList : public IjvxSectionOriginList
{
public:
	class CjvxOneOutputContent
	{
	public:
		std::string fName;
		std::list<std::string> content_sections;
	};

	class CjvxOneSectionConfig
	{
	public:
		std::string sectionPath;
		std::string origin;
		int lineno = 0;
	};
	std::map<std::string, CjvxOneSectionConfig> sectionList;
	std::list<CjvxOneSectionConfig> duplicateList;

	std::map<std::string, CjvxOneOutputContent> collections;
public:

	CjvxSectionOriginList();

	virtual jvxErrorType number_entries_map(jvxSize* num) override;
	virtual jvxErrorType entry_idx(jvxSize idx, jvxApiString* nmSection, jvxApiString* origin, jvxSize* line_no) override;

	virtual jvxErrorType number_entries_duplicate(jvxSize* num) override;
	virtual jvxErrorType duplicate_idx(jvxSize idx, jvxApiString* nmSection, jvxApiString* origin, jvxSize* line_no) override;

	virtual jvxErrorType start_collect_file_contents() override;
	virtual jvxErrorType stop_collect_file_contents() override;

	virtual jvxErrorType number_collected_file_contents(jvxSize* num) override;
	virtual jvxErrorType number_collected_file_idx_number_content(jvxSize idx, jvxSize* num) override;
	virtual jvxErrorType get_collected_file_idx_content_idx(jvxSize idxFile, jvxApiString* fName, jvxApiString* content, jvxSize idxContent = 0) override;
};

#endif
