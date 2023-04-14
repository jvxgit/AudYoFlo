#ifndef __CJVXOBJECTLOG__H__
#define __CJVXOBJECTLOG__H__

#ifdef JVX_OBJECTS_WITH_TEXTLOG

class CjvxObjectLog
{
public:
	JVX_DEFINE_RT_ST_INSTANCES
	CjvxObjectLog() : JVX_INIT_RT_ST_INSTANCES{};
};

#endif

#endif
