#include "jvx.h"

class HjvxPropertyStreamUdpSocket;

JVX_INTERFACE HjvxPropertyStreamUdpSocket_report
{
public:
    virtual ~HjvxPropertyStreamUdpSocket_report(){};

	virtual jvxErrorType report_incoming_packet(jvxByte* fld, jvxSize sz, HjvxPropertyStreamUdpSocket* resp_socket) = 0;
	virtual jvxErrorType report_connection_error(const char* errorText) = 0;
};

class HjvxPropertyStreamUdpSocket: public IjvxConnection_report
{
private:
	typedef enum
	{
		JVX_PROP_UDP_CLIENT, 
		JVX_PROP_UDP_SERVER
	} jvxUdpOperationType;

	jvxInt32 port;
	std::string target;
	IjvxConnection* theSocket;
	IjvxObject* obj;
	std::list<jvxSize> prop_ids;
	IjvxToolsHost* tools;
	IjvxHiddenInterface* host;
	jvxState theState;
	jvxUdpOperationType oper_tp;

	jvxSize buffer_receive_sz;
	jvxByte* buffer_receive;

	HjvxPropertyStreamUdpSocket_report* report;
private:

	void deallocate_buffer();
	void allocate_buffer(jvxSize sz);

public:
	HjvxPropertyStreamUdpSocket();
	
	jvxErrorType initialize(HjvxPropertyStreamUdpSocket_report* reportRef, IjvxToolsHost* toolsRef, IjvxHiddenInterface* theHostRef, jvxBool init_client = true);
	
	jvxErrorType terminate();

	jvxErrorType register_property(jvxSize uId, jvxInt32 port, const std::string& target = "");
	
	jvxErrorType unregister_property(jvxSize uId, jvxInt32& nolongerinuse);

	jvxErrorType send_packet(const char* packet, jvxSize szPacket, const char* target = NULL, int port = 0);

	jvxErrorType open_socket(jvxBool startasclient = true, jvxSize buffer_receive_sz_ref = sizeof(jvxPropertyPropertyObserveHeader_response), jvxSize timeoutConnectRef = 1000	);

	jvxErrorType close_socket(jvxBool stopasclient = true);

	// =================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION provide_data_and_length(jvxByte**ptr, jvxSize* maxNumCopy, jvxSize* offset, jvxSize id_port, jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_data_and_read(jvxByte* ptr, jvxSize numRead, jvxSize offset, jvxSize id_port, jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_event(jvxBitField eventMask, jvxSize id_port, jvxHandle* addInfo, jvxConnectionPrivateTypeEnum whatsthis) override;

};
