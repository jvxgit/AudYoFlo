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

#include "CjvxSocketsClientFactory.h"
#include "CjvxSocketsClientInterface.h"
#include "CjvxSocketsClientInterfaceUnix.h"
#include "CjvxSocketsClientInterfaceTcp.h"
#include "CjvxSocketsClientInterfaceTcpUdp.h"

#ifdef OS_SUPPORTS_BTH
#include <initguid.h>
#endif

#ifdef JVX_WITH_PCAP
#include "CjvxSocketsPcap.h"
#include "CjvxSocketsClientInterfacePcap.h"
#endif

CjvxSocketsClientFactory::CjvxSocketsClientFactory()
{
}

jvxErrorType
CjvxSocketsClientFactory::initialize(jvxSocketsConnectionType tp)
{
#ifdef JVX_WITH_PCAP
	jvxSize i, cnt;
	pcap_if_t* alldevs, * d;
	char errbuf[PCAP_ERRBUF_SIZE];
#endif

	oneInterfaceReference ifaceRef;
	switch (tp)
	{

	case jvxSocketsConnectionType::JVX_SOCKET_TYPE_TCP:
	{
		CjvxSocketsClientInterfaceTcp* iface = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(iface, CjvxSocketsClientInterfaceTcp);
		iface->name = "TCP Client";
		iface->id = 0;
		iface->numRefsMax = 1;
		iface->socketType = tp;
		ifaceRef.ptr = iface;
		interfaces.push_back(ifaceRef);
	}
	break;

	case jvxSocketsConnectionType::JVX_SOCKET_TYPE_UDP:
	{
		CjvxSocketsClientInterfaceTcpUdp* iface = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(iface, CjvxSocketsClientInterfaceTcp);
		iface->name = "UDP Client";
		iface->id = 0;
		iface->numRefsMax = 1;
		iface->socketType = tp;
		ifaceRef.ptr = iface;
		interfaces.push_back(ifaceRef);
	}
	break;

	case jvxSocketsConnectionType::JVX_SOCKET_TYPE_UNIX:
	{
		CjvxSocketsClientInterfaceUnix* iface = NULL;
		JVX_DSP_SAFE_ALLOCATE_OBJECT(iface, CjvxSocketsClientInterfaceUnix);
		iface->name = "Unix Client";
		iface->id = 0;
		iface->numRefsMax = 1;
		iface->socketType = tp;
		ifaceRef.ptr = iface;
		interfaces.push_back(ifaceRef);
	}
	break;

	case jvxSocketsConnectionType::JVX_SOCKET_TYPE_PCAP:

#ifdef JVX_WITH_PCAP
		// retreive the device list from the local machine
		if (pcap_findalldevs(&alldevs, errbuf) == -1)
		{
			fprintf(stderr, "Error in pcap_findalldevs_ex: %s\n", errbuf);
			assert(0);
		}

		cnt = 0;
		// print the list
		for (d = alldevs; d != NULL; d = d->next)
		{
			CjvxSocketsClientInterfacePcap* iface = NULL;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(iface, CjvxSocketsClientInterfacePcap);
			iface->name = d->name;

			get_macs(iface->name, iface->mac);
			iface->dev_description = d->name;
			if (d->description)
			{
				iface->dev_description = d->description;
			}
			iface->dev_description += "<";
			for (i = 0; i < 6; i++)
			{
				iface->dev_description += jvx_intx2String((jvxUInt8)iface->mac[i]);
				if (i != 5)
				{
					iface->dev_description += ":";
				}
			}
			iface->dev_description += ">";

			iface->id = cnt;
			iface->socketType = tp;

#ifndef JVX_PCAP_DIRECT_SEND
			iface->my_send_queue = NULL;
#endif

			ifaceRef.ptr = iface;
			interfaces.push_back(ifaceRef);
			cnt++;
		}

		pcap_freealldevs(alldevs);
#else
		std::cout << __FUNCTION__ << ": Error: Requesting PCAP socket but PCAP is not active at build time." << std::endl;
#endif
		break;

	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSocketsClientFactory::number_interfaces(jvxSize* num)
{
	if (num)
	{
		*num = interfaces.size();
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxSocketsClientFactory::request_interface(
	CjvxSocketsClientInterface** ifptr, 
	jvxSocketsConnectionType* tp,
	jvxSize id)
{
	if (id < interfaces.size())
	{
		auto elm = interfaces.begin();
		std::advance(elm, id);

		if (ifptr)
		{
			if (!elm->inUse)
			{
				*ifptr = elm->ptr;
				elm->inUse = true;
			}
		}
		if (tp)
		{
			*tp = elm->socketType;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType 
CjvxSocketsClientFactory::return_interface(CjvxSocketsClientInterface* ifptr)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	auto elm = interfaces.begin();
	while (elm != interfaces.end())
	{
		if (elm->ptr == ifptr)
		{
			if (elm->inUse)
			{
				elm->inUse = false;
			}
			else
			{
				res = JVX_ERROR_WRONG_STATE;
			}
			break;
		}
		elm++;
	}
	return res;
}

jvxErrorType
CjvxSocketsClientFactory::terminate()
{
	auto elm = interfaces.begin();
	for (; elm != interfaces.end(); elm++)
	{
		JVX_DSP_SAFE_DELETE_OBJECT(elm->ptr);
	}
	interfaces.clear();
	return JVX_NO_ERROR;
}
