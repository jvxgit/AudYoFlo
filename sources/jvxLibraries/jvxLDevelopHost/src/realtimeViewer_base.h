#ifndef REALTIMEVIEWER_BASE_H__
#define REALTIMEVIEWER_BASE_H__

#include "jvx.h"
#include "common/CjvxRealtimeViewer.h"

class realtimeViewer_base
{
public:
	typedef enum
	{
		JVX_RTV_MAIN_CLASS_NONE = 0,
		JVX_RTV_MAIN_CLASS_RTV_PROPERTIES = 1,
		JVX_RTV_MAIN_CLASS_RTV_PLOTS = 2,
		JVX_RTV_MAIN_CLASS_RTV_ONE_PROPERTY = 3,
		JVX_RTV_MAIN_CLASS_RTV_ONE_LINEAR_PLOT = 4,
		JVX_RTV_MAIN_CLASS_RTV_ONE_LINEAR_TSP_PLOT = 5
	} jvxRtvMainClassType;

protected:
		
	struct
	{
		std::string description;
		IjvxHost* theHostRef;
		CjvxRealtimeViewer* theRef;
		IjvxReport* report;

		jvxSize sectionId;
		jvxSize groupId;
		jvxSize itemId;

	} references;

public:
	realtimeViewer_base(std::string description, IjvxHost* theHostRef, CjvxRealtimeViewer* theRef, IjvxReport* report)
	{
		references.description = description;
		references.theHostRef = theHostRef;
		references.theRef = theRef;
		references.report = report;
		
		references.sectionId = JVX_SIZE_UNSELECTED;
		references.groupId = JVX_SIZE_UNSELECTED;
		references.itemId = JVX_SIZE_UNSELECTED;
	};
	
	~realtimeViewer_base()
	{
	};

	virtual void cleanBeforeDelete() = 0;

	virtual void init() = 0;

	virtual void get_pointer_main_class(jvxHandle** theMainClass, jvxRtvMainClassType* theType)
	{
		if(theMainClass)
		{
			*theMainClass = NULL;
		}
		if(theType)
		{
			*theType = JVX_RTV_MAIN_CLASS_NONE;
		}
	};

};

#endif
