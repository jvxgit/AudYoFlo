#include <iostream>
#include "CjvxSockets.h"
#include "CjvxAudioSocketsStateMachine.h"

class socketClient: public CjvxAudioSocketsStateMachine,
	public IjvxAudioSocketsStateMachine_common
	// public IjvxSocketsConnection_report, public IjvxSocketsClient_report
{
	//JVX_MUTEX_HANDLE safeAccessConnection;
	//IjvxSocketsConnection* connection = nullptr;
	//jvxRemoteDeviceConfig rem_cfg;
public:

	socketClient()
	{
		//JVX_INITIALIZE_MUTEX(safeAccessConnection);
		//memset(&rem_cfg, 0, sizeof(rem_cfg));

	}
	~socketClient() 
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
		return JVX_NO_ERROR;
	}
};

// socketClient myCallbackClass;

int main(int argc, char* argv[])
{
	jvxBool restart = true;
	jvxBool startUnix = true;

	socketClient theClient;
	jvxAudioSocketParameters params;

	params.bsize = 1024;
	params.srate = 48000;
	params.format = JVX_DATAFORMAT_16BIT_LE;
	params.formGroup = JVX_DATAFORMAT_GROUP_AUDIO_PCM_HOA;
	params.numChannelsSend = 25;
	params.numChannelsReceive = 0;
	params.nameConnection = "UnrealEP";

	CjvxSocketsClientFactory theDeviceSocketFactory;
	CjvxSocketsClientInterface* clientIf = NULL;

	if (argc == 2)
	{
		if (argv[1] == (std::string)"tcp")
		{
			startUnix = false;
		}
	}

	if (startUnix)
	{
		CjvxSocketsClientInterfaceUnix* theUnixSocket = nullptr;

		std::string deskPath = JVX_GET_USER_DESKTOP_PATH();
		std::string socketPath = deskPath + JVX_SEPARATOR_DIR + "jvxrt.sock.server";

		std::cout << "Initializing socket client on UNIX socket <" << socketPath << ">." << std::endl;

		theDeviceSocketFactory.initialize(jvxSocketsConnectionType::JVX_SOCKET_TYPE_UNIX);
		theDeviceSocketFactory.request_interface(&clientIf);
		theUnixSocket = static_cast<CjvxSocketsClientInterfaceUnix*>(clientIf);
		theUnixSocket->configure(socketPath);
	}
	else
	{
		CjvxSocketsClientInterfaceTcp* theTcpSocket = nullptr;
		jvxSize locPort = 45657;

		std::cout << "Initializing socket client on localhost TCP socket port <" << locPort << ">." << std::endl;
		theDeviceSocketFactory.initialize(jvxSocketsConnectionType::JVX_SOCKET_TYPE_TCP);
		theDeviceSocketFactory.request_interface(&clientIf);
		theTcpSocket = static_cast<CjvxSocketsClientInterfaceTcp*>(clientIf);
		theTcpSocket->configure("localhost", locPort);
	}

	while (restart)
	{
		jvxErrorType res = JVX_NO_ERROR;
		res = theClient.initialize_master(&params, static_cast<IjvxAudioSocketsStateMachine_common*>(&theClient));
		res = clientIf->start(static_cast<IjvxSocketsClient_report*>(&theClient));
		if (res == JVX_NO_ERROR)
		{

			while (1)
			{
				if (theClient.localState == jvxAudioSocketsState::JVX_AUDIOSTATE_CONNECTED)
				{
					theClient.procede_state_machine();
					break;
				}
				else
				{
					JVX_SLEEP_MS(100);
				}
			}

			while (1)
			{
				if (theClient.localState == jvxAudioSocketsState::JVX_AUDIOSTATE_XCHANGE_ESTABLISHED)
				{
					std::cout << "Connected with confirmed parameters:" << std::endl;
					std::cout << "Buffersize = " << theClient.params.bsize << std::endl;
					std::cout << "Samplerate = " << theClient.params.srate << std::endl;
					std::cout << "Format = " << jvxDataFormat_txt(theClient.params.format) << std::endl;
					std::cout << "FormatGroup = " << jvxDataFormatGroup_txt(theClient.params.formGroup) << std::endl;
					std::cout << "Num Channels Send = " << theClient.params.numChannelsSend << std::endl;
					std::cout << "Num Channels Receive = " << theClient.params.numChannelsReceive << std::endl;
					std::cout << "Name = " << theClient.params.nameConnection << std::endl;
					break;
				}
				else
				{
					JVX_SLEEP_MS(100);
				}
			}

			while (1)
			{
				std::cout << "Specify what to do next [c: disconnect single; q: stop full; s: shutdown]:" << std::endl;
				char txt;
				std::cin >> txt;
				/*
				if (txt == 'd')
				{
					myCallbackClass.send_message("123456");
				}
				*/
				if (txt == 'c')
				{
					theClient.disconnect();
					break;
				}
				if (txt == 'q')
				{
					break;
				}
				if (txt == 's')
				{
					restart = false;
					break;
				}
			}
			clientIf->stop();
			theClient.terminate();
		}
		else
		{
			
			std::cout << "Failed to connect to server: " << clientIf->last_error << std::endl;
			restart = false;
		}
	}
	theDeviceSocketFactory.return_interface(clientIf);
	// theDeviceSocketFactory.terminate();

}
	
