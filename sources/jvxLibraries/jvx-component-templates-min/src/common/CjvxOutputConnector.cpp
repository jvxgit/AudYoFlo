#include "common/CjvxOutputConnector.h"

CjvxOutputConnectorCore::CjvxOutputConnectorCore()
{
	_common_set_ocon.conn_out = NULL;
	_common_set_ocon.theCommon_from = NULL;
	jvx_initDataLinkDescriptor(&_common_set_ocon.theData_out);
}
