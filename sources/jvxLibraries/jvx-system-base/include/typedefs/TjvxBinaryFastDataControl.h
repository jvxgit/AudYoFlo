#ifndef __TJVXBINARYFASTDATACONTROL_H__
#define __TJVXBINARYFASTDATACONTROL_H__

typedef enum
{
	JVX_BINARY_DATACONTROLLER_ID_NONE
#ifndef JVX_NO_SYSTEM_EXTENSIONS
#include "typedefs/TpjvxBinaryFastDataControl_ids.h"
#endif
	,JVX_BINARY_DATACONTROLLER_ID_LIMIT
} jvxBinaryDataControlIds;

typedef struct
{
	jvxBinaryDataControlIds purposeId;
	jvxInt32 sizeField;
} jvxBinaryData_header;

#endif
