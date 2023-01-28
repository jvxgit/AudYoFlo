#include <iostream>
#include "CjvxSockets.h"
#include "CjvxAudioSocketsStateMachine.h"

class controlClient : public IjvxSocketsClient_report, public IjvxSocketsConnection_report
{
	JVX_MUTEX_HANDLE safeAccessConnection;
	IjvxSocketsConnection* connection = nullptr;
	jvxGmProtocolDescription resp;
public:
	jvxBool noResponse = true;

	controlClient()
	{
		JVX_INITIALIZE_MUTEX(safeAccessConnection);
		memset(&resp, 0, sizeof(jvxGmProtocolDescription));

	}
	~controlClient()
	{
		JVX_TERMINATE_MUTEX(safeAccessConnection);
	};

	jvxErrorType send_message(IjvxSocketsConnection_transfer* transfer, jvxSize * nums)
	{
		JVX_LOCK_MUTEX(safeAccessConnection);
		if (connection)
		{
			connection->transfer(transfer, nums);
		}
		JVX_UNLOCK_MUTEX(safeAccessConnection);
		return JVX_NO_ERROR;
	}

	jvxErrorType disconnect()
	{
		JVX_LOCK_MUTEX(safeAccessConnection);
		if (connection)
		{
			connection->disconnect();
		}
		JVX_UNLOCK_MUTEX(safeAccessConnection);
		return JVX_NO_ERROR;
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION provide_data_and_length(char** fld_use, jvxSize* lenField, jvxSize* offset, IjvxSocketsAdditionalInfoPacket* additionalInfo) override
	{
		*fld_use = (char*)&resp;
		*lenField = sizeof(resp);
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION report_data_and_read(char* fld_filled, jvxSize lenField, jvxSize offset, IjvxSocketsAdditionalInfoPacket* additionalInfo) override
	{
		if (
			(resp.gm_header.purpose == (JVX_PROTOCOL_GM_REQUEST | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_RESPONSE)) ||
			(resp.gm_header.fam_hdr.proto_family == JVX_PROTOCOL_TYPE_GENERIC_MESSAGE))
		{
			noResponse = false;
		}
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION report_connection_error(jvxSocketsErrorType errCode, const char* errDescription) override
	{
		std::cout << __FUNCTION__ << std::endl;
		return JVX_NO_ERROR;
	};
	

	// ==========================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION report_client_connect(IjvxSocketsConnection* connectionRef) override
	{
		std::cout << __FUNCTION__ << std::endl;
		
		JVX_LOCK_MUTEX(safeAccessConnection);
		connection = connectionRef;
		connection->register_report(static_cast<IjvxSocketsConnection_report*>(this));
		JVX_UNLOCK_MUTEX(safeAccessConnection);
				
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION report_client_disconnect(IjvxSocketsConnection* connection) override
	{
		std::cout << __FUNCTION__ << std::endl;

		JVX_LOCK_MUTEX(safeAccessConnection);
		connection->unregister_report(static_cast<IjvxSocketsConnection_report*>(this));
		connection = NULL;
		JVX_UNLOCK_MUTEX(safeAccessConnection);

		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION report_client_error(jvxSocketsErrorType err, const char* description)override
	{
		std::cout << __FUNCTION__ << std::endl;
		return JVX_NO_ERROR;
	};

	bool is_connected()
	{
		return (connection != NULL);
	}
};

// socketClient myCallbackClass;

int main(int argc, char* argv[])
{
	jvxSize i;
	jvxBool restart = true;

	controlClient theClient;

	CjvxSocketsClientFactory theDeviceSocketFactory;
	CjvxSocketsClientInterfaceUnix* theUnixSocket = nullptr;

	std::string deskPath = JVX_GET_USER_DESKTOP_PATH();
	std::string socketPath = deskPath + JVX_SEPARATOR_DIR + "jvxrt.ctrl.server";

	CjvxSocketsClientInterface* clientIf = NULL;
	theDeviceSocketFactory.initialize(jvxSocketsConnectionType::JVX_SOCKET_TYPE_UNIX);
	theDeviceSocketFactory.request_interface(&clientIf);
	theUnixSocket = static_cast<CjvxSocketsClientInterfaceUnix*>(clientIf);
	theUnixSocket->configure(socketPath);

	jvxErrorType res = JVX_NO_ERROR;
	res = theUnixSocket->start(static_cast<IjvxSocketsClient_report*>(&theClient));
	if (res == JVX_NO_ERROR)
	{
		while (1)
		{
			if (theClient.is_connected())
			{
				break;
			}
			else
			{
				JVX_SLEEP_MS(100);
			}
		}

		std::string message;
		for (i = 1; i < argc; i++)
		{
			message += argv[i];
			if (i != argc - 1)
			{
				message += " ";
			}
		}

		if (message.size())
		{
			jvxSize nums = 0;
			jvxByte* ptrSend = NULL;
			jvxSize szSend = sizeof(jvxGmProtocolDescription);
			szSend += message.size() + 1;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(ptrSend, jvxByte, szSend);
			jvxGmProtocolDescription* ptrSendStruct = (jvxGmProtocolDescription*)ptrSend;

			ptrSend += sizeof(jvxGmProtocolDescription);
			memcpy(ptrSend, message.c_str(), (message.size() + 1));

			ptrSendStruct->gm_header.paketsize = szSend;
			ptrSendStruct->gm_header.purpose = (JVX_PROTOCOL_GM_REQUEST | JVX_PROTOCOL_ADVLINK_MESSAGE_PURPOSE_REQUEST);
			ptrSendStruct->gm_header.fam_hdr.proto_family = JVX_PROTOCOL_TYPE_GENERIC_MESSAGE;
			CjvxSocketsConnection_transfer theTransfer((jvxByte*)ptrSendStruct, szSend);

			theClient.send_message(&theTransfer, &nums);
			std::cout << "Sending of message with <" << nums << "> bytes completed." << std::endl;

			JVX_DSP_SAFE_DELETE_FIELD_TYPE(ptrSendStruct, jvxByte);
			for (i = 0; i < 10; i++)
			{
				if (theClient.noResponse)
				{
					JVX_SLEEP_MS(200);
				}
				else
				{
					break;
				}
			}
			std::cout << "Received response after <" << i << "> loop steps." << std::endl;
		}
		else
		{
			std::cout << "Nothing to send." << std::endl;
		}
		theUnixSocket->stop();
		theDeviceSocketFactory.return_interface(clientIf);
		theDeviceSocketFactory.terminate();
	}
	else
	{
		std::cout << "Starting Socket failed" << std::endl;
	}
}
	
