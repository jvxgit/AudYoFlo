#ifndef __JVX_SYSTEM_COMMON_TYPES_H__
#define __JVX_SYSTEM_COMMON_TYPES_H__

typedef enum
{
	JVX_STATE_NONE = 0x1,
	JVX_STATE_INIT = 0x2,
	JVX_STATE_SELECTED = 0x4,			// Plugin has been opened but has not yet been initialized
	JVX_STATE_ACTIVE = 0x8,		// Plugin has currently been initialized
	JVX_STATE_PREPARED = 0x10,			// Plugin is ready for processing
	JVX_STATE_PROCESSING = 0x20,		// Plugin is in processing state
	JVX_STATE_COMPLETE = 0x40,		// Plugin is in complete state
	JVX_STATE_DONTCARE = -1 //(JVX_STATE_NONE || JVX_STATE_INIT | JVX_STATE_SELECTED|	JVX_STATE_ACTIVE | JVX_STATE_PREPARED | JVX_STATE_PROCESSING | JVX_STATE_COMPLETE)
} jvxState;

#endif

