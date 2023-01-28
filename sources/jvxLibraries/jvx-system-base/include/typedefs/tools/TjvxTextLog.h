#ifndef __TJVXTEXTLOG_H__
#define __TJVXTEXTLOG_H__

typedef enum
{
	JVX_TEXT_LOG_OUTPUT_FILE_SHIFT = 0, 
	JVX_TEXT_LOG_OUTPUT_CONSOLE_SHIFT = 1,
	JVX_TEXT_LOG_OUTPUT_CBUFFER_SHIFT = 2
} jvxTextLogOutputTargetEnum;

typedef struct
{
	jvxSize iter;
	jvxSize idx;
} jvxTextLogOutputPositionCircBuffer;

#endif