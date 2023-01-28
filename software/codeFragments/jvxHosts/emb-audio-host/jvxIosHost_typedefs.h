#ifndef __JVXIOSHOST_TYPEDEFS_H__
#define __JVXIOSHOST_TYPEDEFS_H__

/*
#define JVX_ASSERT(cond, descr) \
if(!cond) \
{ \
NSLog(descr); \
assert(0); \
}
*/

typedef enum
{
    JVX_IOS_REQUEST_ACTION_NONE
} jvxIosRequestAction;

typedef enum
{
    JVX_IOS_BWD_COMMAND_NONE = 0,
    JVX_IOS_BWD_COMMAND_NEW_ROUTE = 1,
    JVX_IOS_BWD_COMMAND_INTERRUPTION = 2,
    JVX_IOS_BWD_COMMAND_MEDIACENTER_RESET = 3,
    JVX_IOS_BWD_COMMAND_BOOTUP_COMPLETE = 4, // second argument is NULL pointer
    JVX_IOS_BWD_COMMAND_TEXT_MESSAGE = 5, // second argument is NSString*
    JVX_IOS_BWD_COMMAND_START = 6,
    JVX_IOS_BWD_COMMAND_STOP = 7,
    JVX_IOS_BWD_COMMAND_UPDATE_WINDOW = 8
} jvxIosBwdReportEvent;

#endif
