#ifndef _CJVXOUTPUTCONNECTOR_H__
#define _CJVXOUTPUTCONNECTOR_H__

#include "jvx.h"

class CjvxOutputConnectorCore
{
public:
	class common_set_ocon_t
	{
	public:
		IjvxConnectorBridge* conn_out = nullptr;
		jvxLinkDataDescriptor theData_out;
		IjvxDataConnectionCommon* theCommon_from = nullptr;

		struct
		{
			jvxDataFormatGroup format_group = JVX_DATAFORMAT_GROUP_NONE;
			jvxDataflow data_flow = JVX_DATAFLOW_NONE;
		} caps_out;
	};

	common_set_ocon_t _common_set_ocon;
public:
	CjvxOutputConnectorCore();
};

#endif
