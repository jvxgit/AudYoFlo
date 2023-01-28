#ifndef __CJVXSIGNALPROCESSINGDEVICEDEPLOY_H__
#define __CJVXSIGNALPROCESSINGDEVICEDEPLOY_H__

#include "CjvxSockets.h"

#include "jvxTechnologies/CjvxFullMasterDevice.h"
#include "pcg_CjvxSignalProcessingDeviceSocket.h"
#include "CjvxAudioSocketsStateMachine.h"

#include "CjvxBufferSync.h"

class CjvxSignalProcessingTechnologySocket;

// ======================================================================
// ======================================================================

class CjvxSignalProcessingDeviceSocket : 
	public CjvxFullMasterDevice, 
	//public IjvxSocketsConnection_report,
	public IjvxAudioSocketsStateMachine_common,
	public CjvxSignalProcessingDeviceSocket_genpcg
{
	friend class CjvxSignalProcessingTechnologySocket;
private:
	
	/*
	enum class deviceStatus 
	{
		JVX_SOCKET_DEVICE_NONE,
		JVX_SOCKET_DEVICE_CONNECTED,
		JVX_SOCKET_DEVICE_PARAMETER_PASSED,
		JVX_SOCKET_DEVICE_EXPOSED,
		JVX_SOCKET_DEVICE_PARAMETER_ZOMBI
	};
	*/
	

	CjvxSignalProcessingTechnologySocket* parent = nullptr;

	std::string name_connection = "no connection";
	IjvxSocketsConnection* connection = nullptr;
	// deviceStatus devState = deviceStatus::JVX_SOCKET_DEVICE_NONE;	

	jvxBool shutdowTriggered = false;

	CjvxAudioSocketsStateMachine theSMachine;

	/*
	jvxTick	tstamp_last = JVX_SIZE_UNSELECTED;
	jvxTimeStampData tstamp;
	*/ 

	CjvxBufferSync syncio;
	jvxBufferSync_config config_sync; 
	jvxBufferSync_runtime runtime_sync;
	jvxBufferSync_fillheight fheight_sync;

	jvxTimeStampData tStampRates_extern;
	jvxTick tStampRates_last_out_extern;
	jvxData delta_t_smoothed_extern;

	jvxData**fld_buffers_convert = nullptr;
	jvxSize sz_buffers_convert = 0;
	jvxSize num_buffers_convert = 0;
	JVX_MUTEX_HANDLE safeAccess_convertbufs;
	jvxCBitField flags_channels = 0;

	jvxTimeStampData tStampRates_local;
	jvxTick tStampRates_last_out_local;
	jvxData delta_t_smoothed_local;
public:
	JVX_CALLINGCONVENTION CjvxSignalProcessingDeviceSocket(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxSignalProcessingDeviceSocket();

	jvxErrorType init_connection(IjvxSocketsConnection* conn,
		CjvxSignalProcessingTechnologySocket* parentRef);
	jvxErrorType terminate_connection();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess() override;

	virtual jvxErrorType test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;


	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	jvxErrorType data_slave(jvxAudioSocketParameters* params);
	jvxErrorType accept_slave(jvxAudioSocketParameters* params);

	virtual jvxErrorType report_new_frame_start(
		jvxSize bsize_data,
		jvxDataFormat format_data,
		jvxSize numchannels_data,
		jvxSize seq_id) override;

	virtual jvxErrorType report_channel_buffer(
		jvxHandle* buffer,
		jvxSize chan_id_idx) override;

	virtual jvxErrorType report_frame_complete(jvxBool detectedError, jvxSize seq_id) override;

	virtual jvxErrorType report_state_switch(jvxAudioSocketsState stat) override;

	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION provide_data_and_length(char** fld_use, jvxSize* lenField, jvxSize* offset, IjvxSocketsAdditionalInfoPacket* additionalInfo)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_data_and_read(char* fld_filled, jvxSize lenField, jvxSize offset, IjvxSocketsAdditionalInfoPacket* additionalInfo)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION report_connection_error(jvxSocketsErrorType errCode, const char* errDescription)override;
	*/

	// JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(get_interface_ptr);
public:

	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION setDataInteractReference(IjvxSignalProcessingDeploy_interact* interact) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION setOutputParameters(jvxLinkDataDescriptor_con_params* params) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION setInputParameters(jvxLinkDataDescriptor_con_params* params) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION triggerTest() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION exchangeData(IjvxSignalProcessingDeploy_data* data) override;
	*/
	/*
	*/
};

#endif
