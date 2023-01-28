#ifndef __CJVXSOCKETCONNECTION_H__
#define __CJVXSOCKETCONNECTION_H__

class CjvxSocketsConnection_transfer : public IjvxSocketsConnection_transfer
{
public:
	jvxByte* bufSend = nullptr;
	jvxSize bufSize = 0;

public:
	CjvxSocketsConnection_transfer();
	CjvxSocketsConnection_transfer(jvxByte* bufSendArg, jvxSize bufSizeArg);

	virtual jvxErrorType get_data(jvxByte** buf_send, jvxSize* numElements) override;
	virtual jvxSocketsConnectionTransfer type() override;
	virtual jvxErrorType specialization(jvxHandle** ptr, jvxSocketsConnectionTransfer tp) override;

public:
	virtual void set_buffer(jvxByte* bufSend, jvxSize numSend);

};

class CjvxSocketsConnection: public IjvxSocketsConnection
{
protected:
	IjvxSocketsConnection_report* theReportConnection = nullptr;

public:
	CjvxSocketsConnection() {};	
	virtual void main_loop() = 0;

	virtual jvxErrorType JVX_CALLINGCONVENTION register_report(IjvxSocketsConnection_report* report)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unregister_report(IjvxSocketsConnection_report* report) override;	
};

#endif