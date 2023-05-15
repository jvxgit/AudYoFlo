#ifndef __IJVXSECTIONORIGINLIST_H__
#define __IJVXSECTIONORIGINLIST_H__

JVX_INTERFACE IjvxSectionOriginList
{
public:
	virtual ~IjvxSectionOriginList() {};

	virtual jvxErrorType number_entries_map(jvxSize* num) = 0;
	virtual jvxErrorType entry_idx(jvxSize idx, jvxApiString* nmSection, jvxApiString* origin, jvxSize* line_no) = 0;

	virtual jvxErrorType number_entries_duplicate(jvxSize* num) = 0;
	virtual jvxErrorType duplicate_idx(jvxSize idx, jvxApiString* nmSection, jvxApiString* origin, jvxSize* line_no) = 0; 

	virtual jvxErrorType start_collect_file_contents() = 0;
	virtual jvxErrorType stop_collect_file_contents() = 0;

	virtual jvxErrorType number_collected_file_contents(jvxSize* num) = 0;
	virtual jvxErrorType number_collected_file_idx_number_content(jvxSize idx, jvxSize* num) = 0;
	virtual jvxErrorType get_collected_file_idx_content_idx(jvxSize idxFile, jvxApiString* fName, jvxApiString* content, jvxSize idxContent = 0) = 0;
};


#endif
