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
};


#endif
