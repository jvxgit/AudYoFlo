#ifndef _MYSOCKETREPORT_H_
#define _MYSOCKETREPORT_H_

#include "jvx_connect_server_st.h"

class mySocketReport
{
public:
	jvx_connect_server_callback_type theCallbacks;
	jvxByte* fld;
	jvxSize szFld;

	mySocketReport();
	~mySocketReport();

	JVX_SERVER_SOCKET_FORWARD_C_CALLBACK_DECLARE_ALL
};

#endif