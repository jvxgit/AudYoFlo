#include <iostream>
#include "jvx.h"
#include "mySocketReport.h"

#define str_size 32
static char str[str_size] = { 0 };

int main(int argc, char* argv[])
{
	jvxSize i;
	char oneChar = 0;
	jvxErrorType res = JVX_NO_ERROR;
	jvx_connect_server_family* hdlFamily = NULL;
	jvx_connect_server_if* hdlServer = NULL;
	jvxSize num;
	mySocketReport mySocketReporter;
	
	res = jvx_connect_server_family_initialize_st(&hdlFamily, JVX_CONNECT_SOCKET_TYPE_UDP);
	assert(res == JVX_NO_ERROR);

	res = jvx_connect_server_family_number_interfaces_st(hdlFamily, &num);
	assert(res == JVX_NO_ERROR);

	res = jvx_connect_server_family_name_interfaces_st(hdlFamily, str, str_size, 0);
	assert(res == JVX_NO_ERROR);

	// ===================================
	res = jvx_connect_server_initialize_st(hdlFamily,
		&hdlServer,
		&mySocketReporter.theCallbacks,
		reinterpret_cast<jvxHandle*>(&mySocketReporter),
		1, 0);
	assert(res == JVX_NO_ERROR);

	jvx_connect_spec_udp mySpec;
	mySpec.family = JVX_IP_V4;
	mySpec.localPort = 5004;
	mySpec.connectToPort = 0; // Will be filled on first packets arriving - or not 
	mySpec.targetName = "localhost";
	res = jvx_connect_server_start_listening_st(hdlServer, &mySpec, JVX_CONNECT_PRIVATE_ARG_TYPE_SOCKET_CONFIGURE_UDP);
	assert(res == JVX_NO_ERROR);

	res = jvx_connect_server_whoami_st(hdlServer, str, str_size, JVX_CONNECT_SOCKET_NAME_ALL_IP_ADDR);
	assert(res == JVX_NO_ERROR);

	std::cout << "IP " << str << ", listening on port " << mySpec.localPort << std::endl;
	std::cout << "Want to stop?" << std::flush;
	std::cin >> oneChar;

	res = jvx_connect_server_trigger_shutdown_st(hdlServer);
	assert(res == JVX_NO_ERROR);

	res = jvx_connect_server_optional_wait_for_shutdown_st(hdlServer);
	assert(res == JVX_NO_ERROR);

	res = jvx_connect_server_terminate_st(hdlServer);
	assert(res == JVX_NO_ERROR);

	hdlServer = NULL;

	res = jvx_connect_server_family_terminate_st(hdlFamily);
	hdlFamily = NULL;
}
	
