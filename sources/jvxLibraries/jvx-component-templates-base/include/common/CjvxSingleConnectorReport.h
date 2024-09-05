#ifndef __CJVXSINGLECONNECTOR_REPORT_H__
#define __CJVXSINGLECONNECTOR_REPORT_H__

template <class T>
JVX_INTERFACE CjvxSingleConnector_report
{
public:
	virtual ~CjvxSingleConnector_report() {};

	virtual jvxErrorType report_selected_connector(T* ioconn) = 0;
	virtual jvxErrorType report_test_connector(T* ioconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	virtual void request_unique_id_start(T* ioconn, jvxSize* uId) = 0;
	virtual jvxErrorType report_started_connector(T* ioconn) = 0;

	virtual jvxErrorType report_stopped_connector(T* ioconn) = 0;
	virtual void release_unique_id_stop(T* ioconn, jvxSize uId) = 0;
	virtual jvxErrorType report_unselected_connector(T* ioconn) = 0;

	virtual void report_process_buffers(T* ioconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage) = 0;
};

template <class T>
JVX_INTERFACE CjvxSingleConnectorTransfer_report
{
public:
	virtual ~CjvxSingleConnectorTransfer_report() {};

	// This transfer endpoint always is on the side of the chain, hence: 
	// 1) transfer_forward on the input connector and 
	// 2) transfer_backward on the output connector and
	virtual jvxErrorType report_transfer_connector(T* ioconn, jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
};


#endif
