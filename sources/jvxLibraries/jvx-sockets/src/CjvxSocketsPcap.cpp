#ifdef _MSC_VER
extern "C"
{
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2bth.h>
#include <afunix.h>
}
#include <stdio.h>
#endif

#include "CjvxSockets.h"
#include "CjvxSocketsPcap.h"

#ifdef JVX_OS_WINDOWS
jvxErrorType
get_macs(std::string name, char* mac)
{
	LPADAPTER	lpAdapter = 0;
	PPACKET_OID_DATA  OidData;
	BOOLEAN		Status;

	//
	// Obtain the name of the adapters installed on this machine
	//

	//
	// Open the selected adapter
	//

	lpAdapter = PacketOpenAdapter((PCHAR)name.c_str());

	if (!lpAdapter || (lpAdapter->hFile == INVALID_HANDLE_VALUE))
	{
		return(JVX_ERROR_UNSUPPORTED);
	}

	//
	// Allocate a buffer to get the MAC adress
	//

	OidData = (PPACKET_OID_DATA)malloc(6 + sizeof(PACKET_OID_DATA));
	if (OidData == NULL)
	{
		PacketCloseAdapter(lpAdapter);
		return(JVX_ERROR_UNSUPPORTED);
	}

	//
	// Retrieve the adapter MAC querying the NIC driver
	//

	OidData->Oid = OID_802_3_CURRENT_ADDRESS;

	OidData->Length = 6;
	ZeroMemory(OidData->Data, 6);

	Status = PacketRequest(lpAdapter, FALSE, OidData);
	if (Status)
	{
		mac[0] = (OidData->Data)[0];
		mac[1] = (OidData->Data)[1];
		mac[2] = (OidData->Data)[2];
		mac[3] = (OidData->Data)[3];
		mac[4] = (OidData->Data)[4];
		mac[5] = (OidData->Data)[5];
	}
	else
	{
		printf("error retrieving the MAC address of the adapter!\n");
	}

	free(OidData);
	PacketCloseAdapter(lpAdapter);
	return JVX_NO_ERROR;
}

#else

// What to do in case of other operating systems?
jvxErrorType get_macs(std::string name, char* mac)
{
	int i;
	jvxSize maxMac = 0;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct ifaddrs* ifPtr = NULL, * ifPtrR = NULL;
	int returnVal = 0;
	returnVal = getifaddrs(&ifPtr);
	if (returnVal != 0)
	{
		printf("Failed to list devices for mac address.");
	}
	else
	{
		ifPtrR = ifPtr;
		// Browse all available adapters
		while (ifPtrR)
		{
			if ((ifPtrR->ifa_addr) && (ifPtrR->ifa_addr->sa_family == AF_PACKET))
			{
				if (ifPtrR->ifa_name == name)
				{
					struct sockaddr_ll* s = (struct sockaddr_ll*)ifPtrR->ifa_addr;
					maxMac = JVX_MIN(6, s->sll_halen);
					for (i = 0; i < maxMac; i++)
					{
						mac[i] = s->sll_addr[i];
					}
					break;
				}
			}
			ifPtrR = ifPtrR->ifa_next;
		}
	}
	freeifaddrs(ifPtr);

	return JVX_ERROR_UNSUPPORTED;
}
#endif // JVX_OS_WINDOWS

void
pcap_prepareBufferSend(char* tmp, jvxUInt8* macDest, jvxUInt8* macSrc)
{
	memcpy(tmp, macDest, 6);
	tmp += 6;
	memcpy(tmp, macSrc, 6);
	tmp += 6;
	*tmp = 'J';
	tmp++;
	*tmp = 'X';
}