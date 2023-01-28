#ifndef __JVX_CONSOLE_TOOLS_H__
#define __JVX_CONSOLE_TOOLS_H__

#include "jvx.h"
#include "jvxSpecialCharacterDefines.h"

// ===============================================
class jvxConsoleHdl
{
public:
	jvxSize posi_read = 0;
	jvxSize posi_avail = 0;
	JVX_FILE_DESCRIPTOR_HANDLE hstdin = JVX_INVALID_FILE_DESCRIPTOR_VALUE;
	JVX_FILE_DESCRIPTOR_HANDLE  hstdend = JVX_INVALID_FILE_DESCRIPTOR_VALUE;
	struct kevent events[2] = {0};
	JVX_FILE_DESCRIPTOR_HANDLE  kqueuefd = JVX_INVALID_FILE_DESCRIPTOR_VALUE;
	JVX_UNBUFFERED_CHAR_STATE;

public:
	void prepare(JVX_FILE_DESCRIPTOR_HANDLE& hdlStop);
	void postprocess();
	int get_single_console_character();
};


#endif