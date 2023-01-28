#include <iostream>
#include "CjvxSockets.h"
#include "CjvxAudioSocketsStateMachine.h"

class socketServer: 
	public CjvxAudioSocketsStateMachine,
	public IjvxAudioSocketsStateMachine_common,
	public IjvxSocketsServer_report
	// public IjvxSocketsConnection_report, public IjvxSocketsClient_report
{
	//JVX_MUTEX_HANDLE safeAccessConnection;
	IjvxSocketsConnection* connection = nullptr;
	//jvxRemoteDeviceConfig rem_cfg;
	CjvxSocketsServer theDeviceSocket;
	// theStateMaschine;

public:

	socketServer()
	{
		//JVX_INITIALIZE_MUTEX(safeAccessConnection);
		//memset(&rem_cfg, 0, sizeof(rem_cfg));

	}
	~socketServer()
	{
		//JVX_TERMINATE_MUTEX(safeAccessConnection);
	};

	jvxErrorType report_new_frame_start(
			jvxSize bsize_data,
			jvxDataFormat format_data,
			jvxSize numchannels_data,
			jvxSize seq_id) override
	{
		return JVX_NO_ERROR;
	}

	jvxErrorType report_channel_buffer(
			jvxHandle* buffer,
			jvxSize chan_id_idx) override
	{
		return JVX_NO_ERROR;
	}

	jvxErrorType report_frame_complete(jvxBool detectedError, jvxSize sequenceId) override
	{
		return JVX_NO_ERROR;
	}

	virtual jvxErrorType report_state_switch(jvxAudioSocketsState stat) override
	{
		switch (stat)
		{
		case jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_INIT:
			std::cout << "Accepted settings from client: " << std::endl;
			std::cout << "bsize = " << this->cfg_remote.bsize << std::endl;
			std::cout << "srate = " << this->cfg_remote.srate << std::endl;
			std::cout << "channelssend = " << this->cfg_remote.channels_send << std::endl;
			std::cout << "channelsrecv = " << this->cfg_remote.channels_recv << std::endl;
			std::cout << "description = " << this->cfg_remote.description << std::endl;
			break;
		}
		return JVX_NO_ERROR;
	}

	virtual jvxErrorType start(const std::string& socketPath)
	{
		return theDeviceSocket.start(static_cast<IjvxSocketsServer_report*>(this), true, socketPath.c_str());
	}

	virtual jvxErrorType start(jvxSize loc_port)
	{
		return theDeviceSocket.start(static_cast<IjvxSocketsServer_report*>(this), true, loc_port);
	}
	virtual jvxErrorType stop()
	{
		return theDeviceSocket.stop();
	}

	// ===================================================================

	virtual jvxErrorType report_server_denied(const char* description)override
	{
		return JVX_NO_ERROR;
	}

	virtual jvxErrorType report_server_connect(const char* description, IjvxSocketsConnection* newConnection)override
	{
		std::cout << "Server connect <" << description << ">." << std::endl;
		connection = newConnection;

		//devState = CjvxSignalProcessingDeviceSocket::deviceStatus::JVX_SOCKET_DEVICE_CONNECTED;
		connection->register_report(static_cast<IjvxSocketsConnection_report*>(this));
		this->initialize_slave(
			static_cast<IjvxAudioSocketsStateMachine_common*>(this),
			connection);
		return JVX_NO_ERROR;
	}

	virtual jvxErrorType report_server_disconnect(const char* description, IjvxSocketsConnection* newConnection) override
	{
		connection->unregister_report(static_cast<IjvxSocketsConnection_report*>(this));
		connection = NULL;
		this->unlink_slave();
		this->terminate();

		std::cout << "Server disconnect <" << description << ">." << std::endl;
		return JVX_NO_ERROR;
	}

	virtual jvxErrorType report_server_error(jvxSocketsErrorType err, const char* description) override
	{
		return JVX_NO_ERROR;
	}

	// ===================================================================

};

// socketClient myCallbackClass;

int main(int argc, char* argv[])
{
	jvxBool restart = true;
	jvxBool startUnix = true;

	socketServer theServer;

	if (argc == 2)
	{
		if (argv[1] == (std::string)"tcp")
		{
			startUnix = false;
		}
	}

	if (startUnix)
	{
		std::string deskPath = JVX_GET_USER_DESKTOP_PATH();
		std::string socketPath = deskPath + JVX_SEPARATOR_DIR + "jvxrt.sock.server";

		// socketPath = " jvxrt.sock.server";
		std::cout << "Starting UNIX Server on file <" << socketPath << ">." << std::endl;
		theServer.start(socketPath.c_str());
	}
	else
	{
		jvxSize locPort = 45657;
		std::cout << "Starting TCP Server on port <" << locPort << ">." << std::endl;
		theServer.start(locPort);
	}

	/*
	jvxAudioSocketParameters params;

	params.bsize = 1024;
	params.srate = 48000;
	params.format = JVX_DATAFORMAT_16BIT_LE;
	params.formGroup = JVX_DATAFORMAT_GROUP_AUDIO_PCM_HOA;
	params.numChannelsSend = 25;
	params.numChannelsReceive = 0;
	params.nameConnection = "UnrealEP";
	*/

	while (1)
	{
		if (theServer.localState == jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_INIT)
		{
			jvxAudioSocketParameters params;			

			theServer.data_slave(&params);

			// modifications may happen here
			theServer.accept_slave(&params);
			break;
		}
		else
		{
			JVX_SLEEP_MS(100);
		}
	}

	char c;
	std::cout << "Server up and running, enter any character to stop." << std::endl;
	std::cin >> c;
	
	theServer.stop();
	// theDeviceSocketFactory.terminate();

}
	
