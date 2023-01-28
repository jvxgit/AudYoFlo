#ifndef __JVX_CONSOLE_TOOLS_H__
#define __JVX_CONSOLE_TOOLS_H__

#include "jvx.h"
#include "jvxSpecialCharacterDefines.h"

// ===============================================
class jvxConsoleHdl
{
public:
	INPUT_RECORD irInBuf[128] = { 0 };
	DWORD numMax = 128;
	jvxSize posi_read = 0;
	jvxSize posi_avail = 0;
	jvxSize in_sequence_count = 0;
	JVX_FILE_DESCRIPTOR_HANDLE hstdin = JVX_INVALID_FILE_DESCRIPTOR_VALUE;
	JVX_FILE_DESCRIPTOR_HANDLE hstdend = JVX_INVALID_FILE_DESCRIPTOR_VALUE;
	JVX_FILE_DESCRIPTOR_HANDLE handles[2] = { JVX_INVALID_HANDLE_VALUE, JVX_INVALID_HANDLE_VALUE };

public:
	void prepare(JVX_FILE_DESCRIPTOR_HANDLE& hdlStop);
	void postprocess();
	int get_single_console_character();
};




#endif
