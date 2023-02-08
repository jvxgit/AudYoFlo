
// ===================================================================
// Some use case specific definitions
// ===================================================================
#ifdef JVX_OS_WINDWOS
// New socket stuff is supported in VS 2013 and higher
#if (_MSC_VER <= 1700)
#define JVX_DEPRECATED_SOCKET_API
#endif
#endif

static std::string sockAddrToHostName(jvxHandle* saddrv, jvxConnectionIpType family)
{
	std::string tmpStr;
#ifdef JVX_DEPRECATED_SOCKET_API
	struct in_addr* saddr = (struct in_addr*)saddrv;
	tmpStr = inet_ntoa(*saddr);
#else
	char ip_addr[INET6_ADDRSTRLEN];
	const char* retVal = NULL;
	switch (family)
	{
	case JVX_IP_V4:

		retVal = inet_ntop(AF_INET, (struct in_addr*)saddrv, (char*)ip_addr, INET6_ADDRSTRLEN);
		break;
	case JVX_IP_V6:

		retVal = inet_ntop(AF_INET6, (struct in6_addr*)saddrv, (char*)ip_addr, INET6_ADDRSTRLEN);
		break;
	default:
		assert(0);
	}

	if (retVal)
	{
		tmpStr = ip_addr;
	}
	else
	{
		assert(0);
	}
#endif
	return tmpStr;
}

static std::string sockAddrRecvToHostName(jvxHandle* saddrv, jvxConnectionIpType family, int& port)
{

#ifdef JVX_DEPRECATED_SOCKET_API
	return(sockAddrToHostName((jvxHandle*)&((sockaddr_in*)saddrv)->sin_addr, family));
#else

	struct sockaddr_in* saddrv4 = NULL;
	struct sockaddr_in6* saddrv6 = NULL;

	switch (family)
	{
	case JVX_IP_V4:
		saddrv4 = (struct sockaddr_in*)saddrv;
		port = htons(saddrv4->sin_port);
		return(sockAddrToHostName(&saddrv4->sin_addr, family));
	case JVX_IP_V6:
		saddrv6 = (struct sockaddr_in6*)saddrv;
		port = htons(saddrv6->sin6_port);
		return(sockAddrToHostName(&saddrv6->sin6_addr, family));
	default:
		assert(0);
	}
#endif
	return("error");
}

static std::string myNameToIpAddresses(jvxConnectionIpType family)
{
	std::string ip;
	char tt[JVX_MAXSTRING];
	memset(tt, 0, JVX_MAXSTRING);
	int errCode = gethostname(tt, JVX_SIZE_INT(JVX_MAXSTRING));
	assert(errCode == 0);

#ifdef JVX_DEPRECATED_SOCKET_API

	hostent* addr = gethostbyname(tt);

	int cnt = 0;
	struct in_addr** addr_list = (struct in_addr **)addr->h_addr_list;

	assert(addr != NULL);

	// Everything is good
	while (1)
	{
		if (addr_list[cnt])
		{
			if (!ip.empty())
				ip += "|";
			ip += sockAddrToHostName(addr_list[cnt], JVX_IP_V4);
			cnt++;
		}
		else
		{
			break;
		}
	}
#else

	int status;
	struct addrinfo hints, *res;
	struct sockaddr_in* tmpv4;
	struct sockaddr_in6* tmpv6;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;

	switch (family)
	{
	case JVX_IP_V4:
		hints.ai_family = AF_INET;
		if ((status = getaddrinfo(tt, NULL, &hints, &res)) == 0)
		{
			struct addrinfo *lp = res;
			while (lp)
			{
				tmpv4 = ((struct sockaddr_in *)(lp->ai_addr));
				//memcpy((char *)&fillme->sin_addr.s_addr, (char *)res->ai_addr->h_addr, target->h_length);
				if (!ip.empty())
					ip += "|";
				ip += sockAddrToHostName(&tmpv4->sin_addr, family);
				lp = lp->ai_next;
			}
			freeaddrinfo(res);
		}
		break;
	case JVX_IP_V6:
		hints.ai_family = AF_INET6;
		if ((status = getaddrinfo(tt, NULL, &hints, &res)) == 0)
		{
			struct addrinfo *lp = res;
			while (lp)
			{
				tmpv6 = ((struct sockaddr_in6 *)(lp->ai_addr));
				//memcpy((char *)&fillme->sin_addr.s_addr, (char *)res->ai_addr->h_addr, target->h_length);
				if (!ip.empty())
					ip += "|";
				ip += sockAddrToHostName(&tmpv6->sin6_addr, family);
				lp = lp->ai_next;
			}
			freeaddrinfo(res);
		}
		break;
	default:
		assert(0);
	}


#endif

	return(ip);
}


static std::string myNameToFirstIpAddress(jvxConnectionIpType family)
{
	std::string ip;
	char tt[JVX_MAXSTRING];
	memset(tt, 0, JVX_MAXSTRING);
	int errCode = gethostname(tt, JVX_SIZE_INT(JVX_MAXSTRING));
	assert(errCode == 0);

#ifdef JVX_DEPRECATED_SOCKET_API

	hostent* addr = gethostbyname(tt);

	int cnt = 0;
	struct in_addr** addr_list = (struct in_addr **)addr->h_addr_list;

	assert(addr != NULL);

	// Everything is good
	while (1)
	{
		if (addr_list[cnt])
		{
			ip = sockAddrToHostName(addr_list[cnt], JVX_IP_V4);
			break;
		}
		else
		{
			break;
		}
	}
#else

	int status;
	struct addrinfo hints, *res;
	struct sockaddr_in* tmpv4;
	struct sockaddr_in6* tmpv6;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;

	switch (family)
	{
	case JVX_IP_V4:
		hints.ai_family = AF_INET;
		if ((status = getaddrinfo(tt, NULL, &hints, &res)) == 0)
		{
			tmpv4 = ((struct sockaddr_in *)(res->ai_addr));
			ip = sockAddrToHostName(&tmpv4->sin_addr, family);
			freeaddrinfo(res);
		}
		break;
	case JVX_IP_V6:
		hints.ai_family = AF_INET6;
		if ((status = getaddrinfo(tt, NULL, &hints, &res)) == 0)
		{
			tmpv6 = ((struct sockaddr_in6 *)(res->ai_addr));
			ip = sockAddrToHostName(&tmpv6->sin6_addr, family);
			freeaddrinfo(res);
		}
		break;
	default:
		assert(0);
	}


#endif

	return(ip);
}



static jvxErrorType hostNameToSockAddr(jvxHandle* fillmev, const char* hostName, jvxConnectionIpType family, int port)
{

#ifdef JVX_DEPRECATED_SOCKET_API
	struct sockaddr_in* fillme = (struct sockaddr_in*)fillmev;
	hostent* target;
	target = gethostbyname(hostName);
	if (target)
	{
		memset(fillme, 0, sizeof(sockaddr_in));

		// Transform server into an IPV4 address
		fillme->sin_family = AF_INET;
		memcpy((char *)&fillme->sin_addr.s_addr, (char *)target->h_addr, target->h_length);
		fillme->sin_port = htons(port);
		return JVX_NO_ERROR;
	}

#else
	int status;
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;

	switch (family)
	{
	case JVX_IP_V4:
		hints.ai_family = AF_INET;
		if ((status = getaddrinfo(hostName, NULL, &hints, &res)) == 0)
		{
			struct sockaddr_in* fillme = (struct sockaddr_in*)fillmev;
			memset(fillme, 0, sizeof(sockaddr_in));
			*fillme = *((struct sockaddr_in *)(res->ai_addr));
			//memcpy((char *)&fillme->sin_addr.s_addr, (char *)res->ai_addr->h_addr, target->h_length);
			fillme->sin_family = AF_INET;
			fillme->sin_port = htons(port);
			freeaddrinfo(res);
			return JVX_NO_ERROR;
		}
		break;
	case JVX_IP_V6:
		hints.ai_family = AF_INET6;
		if ((status = getaddrinfo(hostName, NULL, &hints, &res)) == 0)
		{
			struct sockaddr_in6* fillme = (struct sockaddr_in6*)fillmev;
			memset(fillme, 0, sizeof(sockaddr_in6));
			*fillme = *((struct sockaddr_in6 *)(res->ai_addr));
			//memcpy((char *)&fillme->sin_addr.s_addr, (char *)res->ai_addr->h_addr, target->h_length);
			fillme->sin6_family = AF_INET6;
			fillme->sin6_port = htons(port);
			freeaddrinfo(res);
			return JVX_NO_ERROR;
		}
		break;
	default:
		assert(0);
	}
#endif
	return(JVX_ERROR_INTERNAL);
}

static std::string ipAddressToHostName(const char* whoisthat, int port, jvxConnectionIpType family)
{
	std::string ip;

#ifdef JVX_DEPRECATED_SOCKET_API

	struct hostent* origHost = NULL;
	struct hostent *he = NULL;
	origHost = gethostbyname(whoisthat);
	he = gethostbyaddr((char*)origHost->h_addr_list[0], 4, AF_INET);
	if (he)
	{
		ip = he->h_name;
	}
	else
	{
		ip = "-";
		ip += whoisthat;
		ip += " not found-";
	}
#else
	struct sockaddr_in theSAddrv4;
	struct sockaddr_in6 theSAddrv6;
	char pp[JVX_MAXSTRING];
	char si[JVX_MAXSTRING];
	int err;
	switch (family)
	{
	case JVX_IP_V4:
		hostNameToSockAddr(&theSAddrv4, whoisthat, family, port);
		err = getnameinfo((struct sockaddr *)&theSAddrv4, sizeof(struct sockaddr_in), pp, JVX_MAXSTRING, si, JVX_MAXSTRING, NI_NAMEREQD);
		break;
	case JVX_IP_V6:
		hostNameToSockAddr(&theSAddrv6, whoisthat, family, port);
		err = getnameinfo((struct sockaddr *)&theSAddrv6, sizeof(struct sockaddr_in6), pp, JVX_MAXSTRING, si, JVX_MAXSTRING, NI_NAMEREQD);
		break;
	default:
		assert(0);
	}

	if (err == 0)
	{
		ip = pp;
	}
	else
	{
		ip = "-";
		ip += whoisthat;
		ip += " not found-";
}
#endif
	return(ip);
}


static std::string hostNameToIpAddr(const char* hostName, jvxConnectionIpType family)
{
#ifdef JVX_DEPRECATED_SOCKET_API
	std::string tmpStr;
	hostent* host = gethostbyname(hostName);

	int cnt = 0;
	std::string ip;
	struct in_addr** addr_list = (struct in_addr **)host->h_addr_list;

	// Everything is good
	while (1)
	{
		if (addr_list[cnt])
		{
			char* ipaddr = inet_ntoa(*addr_list[cnt]);
			if (!ip.empty())
				ip += ":";
			ip += ipaddr;
			cnt++;
		}
		else
		{
			break;
		}
	}

	return(tmpStr);

#else

	std::string tmpStr;
	int status;
	char ip_addr[INET6_ADDRSTRLEN];
	struct sockaddr_in6 *remotev6;
	struct sockaddr_in *remotev4;
	struct addrinfo hints, *res;
	const char* retVal = NULL;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;

	switch (family)
	{
	case JVX_IP_V4:
		hints.ai_family = AF_INET;

		if ((status = getaddrinfo(hostName, NULL, &hints, &res)) != 0)
		{
			assert(0);
		}

		remotev4 = (struct sockaddr_in *)(res->ai_addr);
		retVal = inet_ntop(AF_INET, &remotev4->sin_addr, (char*)ip_addr, INET6_ADDRSTRLEN);
		freeaddrinfo(res);

		break;
	case JVX_IP_V6:
		hints.ai_family = AF_INET6;

		if ((status = getaddrinfo(hostName, NULL, &hints, &res)) != 0)
		{
			assert(0);
		}

		remotev6 = (struct sockaddr_in6 *)(res->ai_addr);
		retVal = inet_ntop(AF_INET6, &remotev6->sin6_addr, (char*)ip_addr, INET6_ADDRSTRLEN);
		freeaddrinfo(res);

		break;
	default:
		assert(0);
	}

	if (retVal)
	{
		tmpStr = ip_addr;
	}
	else
	{
		assert(0);
	}
	return tmpStr;

#endif
}

#ifdef OS_SUPPORTS_BTH

// Bluetooth specific stuff
#define CXN_MAX_INQUIRY_RETRY             1
#define CXN_DELAY_NEXT_INQUIRY            2

DEFINE_GUID(g_guidServiceClass, 0x5dd9a71c, 0x3c63, 0x41c6, 0xa3, 0x57, 0x29, 0x29, 0xb4, 0xda, 0x78, 0xb1);
// List all connected Bluetooth devices
static ULONG listNamesBth(std::vector<std::string>& namesBluetoothDevs)
{
  INT             iResult = 0;
  BOOL            bContinueLookup = FALSE, bRemoteDeviceFound = FALSE;
  ULONG           ulFlags = 0, ulPQSSize = sizeof(WSAQUERYSETA);
  HANDLE          hLookup = NULL;
  PWSAQUERYSETA    pWSAQuerySetA = NULL;

  pWSAQuerySetA = (PWSAQUERYSETA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ulPQSSize);
  if ( NULL == pWSAQuerySetA )
  {
	  iResult = STATUS_NO_MEMORY;
	  wprintf(L"!ERROR! | Unable to allocate memory for WSAQUERYSET\n");
  }

  //
  // Search for the device with the correct name
  //
  if ( 0 == iResult)
  {

	  for ( INT iRetryCount = 0;!bRemoteDeviceFound && (iRetryCount < CXN_MAX_INQUIRY_RETRY); iRetryCount++ )
	  {
		  //
		  // WSALookupService is used for both service search and device inquiry
		  // LUP_CONTAINERS is the flag which signals that we're doing a device inquiry.
		  //
		  ulFlags = LUP_CONTAINERS;

		  //
		  // Friendly device name (if available) will be returned in lpszServiceInstanceName
		  //
		  ulFlags |= LUP_RETURN_NAME;

		  //
		  // BTH_ADDR will be returned in lpcsaBuffer member of WSAQUERYSET
		  //
		  ulFlags |= LUP_RETURN_ADDR;

		  if ( 0 == iRetryCount )
		  {
			  wprintf(L"*INFO* | Inquiring device from cache...\n");
		  }
		  else
		  {
			  //
			  // Flush the device cache for all inquiries, except for the first inquiry
			  //
			  // By setting LUP_FLUSHCACHE flag, we're asking the lookup service to do
			  // a fresh lookup instead of pulling the information from device cache.
			  //
			  ulFlags |= LUP_FLUSHCACHE;

			  //
			  // Pause for some time before all the inquiries after the first inquiry
			  //
			  // Remote Name requests will arrive after device inquiry has
			  // completed.  Without a window to receive IN_RANGE notifications,
			  // we don't have a direct mechanism to determine when remote
			  // name requests have completed.
			  //
			  wprintf(L"*INFO* | Unable to find device.  Waiting for %d seconds before re-inquiry...\n", CXN_DELAY_NEXT_INQUIRY);
			  Sleep(CXN_DELAY_NEXT_INQUIRY * 1000);

			  wprintf(L"*INFO* | Inquiring device ...\n");
		  }

		  //
		  // Start the lookup service
		  //
		  iResult = 0;
		  hLookup = 0;
		  bContinueLookup = FALSE;
		  ZeroMemory(pWSAQuerySetA, ulPQSSize);
		  pWSAQuerySetA->dwNameSpace = NS_BTH;
		  pWSAQuerySetA->dwSize = sizeof(WSAQUERYSETA);
		  iResult = WSALookupServiceBeginA(pWSAQuerySetA, ulFlags, &hLookup);

		  //
		  // Even if we have an error, we want to continue until we
		  // reach the CXN_MAX_INQUIRY_RETRY
		  //
		  if ( (NO_ERROR == iResult) && (NULL != hLookup) )
		  {
			  bContinueLookup = TRUE;
		  }
		  else if ( 0 < iRetryCount )
		  {
			  wprintf(L"=CRITICAL= | WSALookupServiceBegin() failed with error code %d, WSAGetLastError = %d\n", iResult, WSAGetLastError());
			  break;
		  }

		  while ( bContinueLookup )
		  {
			  //
			  // Get information about next bluetooth device
			  //
			  // Note you may pass the same WSAQUERYSET from LookupBegin
			  // as long as you don't need to modify any of the pointer
			  // members of the structure, etc.
			  //
			  // ZeroMemory(pWSAQuerySet, ulPQSSize);
			  // pWSAQuerySet->dwNameSpace = NS_BTH;
			  // pWSAQuerySet->dwSize = sizeof(WSAQUERYSET);
			  if ( NO_ERROR == WSALookupServiceNextA(hLookup,
				  ulFlags,
				  &ulPQSSize,
				  pWSAQuerySetA) ) {

					  //
					  // Compare the name to see if this is the device we are looking for.
					  //
					  if ( pWSAQuerySetA->lpszServiceInstanceName != NULL )
					  {
						  namesBluetoothDevs.push_back(pWSAQuerySetA->lpszServiceInstanceName);
					  }
			  }
			  else
			  {
				  iResult = WSAGetLastError();
				  if ( WSA_E_NO_MORE == iResult )
				  { //No more data
					  //
					  // No more devices found.  Exit the lookup.
					  //
					  bContinueLookup = FALSE;
				  }
				  else if ( WSAEFAULT == iResult )
				  {
					  //
					  // The buffer for QUERYSET was insufficient.
					  // In such case 3rd parameter "ulPQSSize" of function "WSALookupServiceNext()" receives
					  // the required size.  So we can use this parameter to reallocate memory for QUERYSET.
					  //
					  HeapFree(GetProcessHeap(), 0, pWSAQuerySetA);
					  pWSAQuerySetA = (PWSAQUERYSETA) HeapAlloc(GetProcessHeap(),
						  HEAP_ZERO_MEMORY,
						  ulPQSSize);
					  if ( NULL == pWSAQuerySetA ) {
						  wprintf(L"!ERROR! | Unable to allocate memory for WSAQERYSET\n");
						  iResult = STATUS_NO_MEMORY;
						  bContinueLookup = FALSE;
					  }
				  }
				  else
				  {
					  wprintf(L"=CRITICAL= | WSALookupServiceNext() failed with error code %d\n", iResult);
					  bContinueLookup = FALSE;
				  }
			  }
		  }

		  //
		  // End the lookup service
		  //
		  WSALookupServiceEnd(hLookup);

		  if ( STATUS_NO_MEMORY == iResult ) {
			  break;
		  }
	  }
  }

  if ( NULL != pWSAQuerySetA )
  {
	  HeapFree(GetProcessHeap(), 0, pWSAQuerySetA);
	  pWSAQuerySetA = NULL;
  }

  if ( bRemoteDeviceFound )
  {
	  iResult = 0;
  }
  else
  {
	  iResult = 1;
  }

  return iResult;
}

//
// NameToBthAddr converts a bluetooth device name to a bluetooth address,
// if required by performing inquiry with remote name requests.
// This function demonstrates device inquiry, with optional LUP flags.
//
static ULONG NameToBthAddr(_In_ const LPSTR pszRemoteName, _Out_ PSOCKADDR_BTH pRemoteBtAddr)
{
	INT             iResult = 0;
	BOOL            bContinueLookup = FALSE, bRemoteDeviceFound = FALSE;
	ULONG           ulFlags = 0, ulPQSSize = sizeof(WSAQUERYSETA);
	HANDLE          hLookup = NULL;
	PWSAQUERYSETA    pWSAQuerySetA = NULL;

	ZeroMemory(pRemoteBtAddr, sizeof(*pRemoteBtAddr));

	pWSAQuerySetA = (PWSAQUERYSETA) HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,ulPQSSize);
	if ( NULL == pWSAQuerySetA )
	{
		iResult = STATUS_NO_MEMORY;
		wprintf(L"!ERROR! | Unable to allocate memory for WSAQUERYSET\n");
	}

	//
	// Search for the device with the correct name
	//
	if ( 0 == iResult)
	{

		for ( INT iRetryCount = 0;!bRemoteDeviceFound && (iRetryCount < CXN_MAX_INQUIRY_RETRY);	iRetryCount++ )
		{
			//
			// WSALookupService is used for both service search and device inquiry
			// LUP_CONTAINERS is the flag which signals that we're doing a device inquiry.
			//
			ulFlags = LUP_CONTAINERS;

			//
			// Friendly device name (if available) will be returned in lpszServiceInstanceName
			//
			ulFlags |= LUP_RETURN_NAME;

			//
			// BTH_ADDR will be returned in lpcsaBuffer member of WSAQUERYSET
			//
			ulFlags |= LUP_RETURN_ADDR;

			if ( 0 == iRetryCount )
			{
				wprintf(L"*INFO* | Inquiring device from cache...\n");
			}
			else
			{
				//
				// Flush the device cache for all inquiries, except for the first inquiry
				//
				// By setting LUP_FLUSHCACHE flag, we're asking the lookup service to do
				// a fresh lookup instead of pulling the information from device cache.
				//
				ulFlags |= LUP_FLUSHCACHE;

				//
				// Pause for some time before all the inquiries after the first inquiry
				//
				// Remote Name requests will arrive after device inquiry has
				// completed.  Without a window to receive IN_RANGE notifications,
				// we don't have a direct mechanism to determine when remote
				// name requests have completed.
				//
				wprintf(L"*INFO* | Unable to find device.  Waiting for %d seconds before re-inquiry...\n", CXN_DELAY_NEXT_INQUIRY);
				Sleep(CXN_DELAY_NEXT_INQUIRY * 1000);

				wprintf(L"*INFO* | Inquiring device ...\n");
			}

			//
			// Start the lookup service
			//
			iResult = 0;
			hLookup = 0;
			bContinueLookup = FALSE;
			ZeroMemory(pWSAQuerySetA, ulPQSSize);
			pWSAQuerySetA->dwNameSpace = NS_BTH;
			pWSAQuerySetA->dwSize = sizeof(WSAQUERYSETA);
			iResult = WSALookupServiceBeginA(pWSAQuerySetA, ulFlags, &hLookup);

			//
			// Even if we have an error, we want to continue until we
			// reach the CXN_MAX_INQUIRY_RETRY
			//
			if ( (NO_ERROR == iResult) && (NULL != hLookup) )
			{
				bContinueLookup = TRUE;
			}
			else if ( 0 < iRetryCount )
			{
				wprintf(L"=CRITICAL= | WSALookupServiceBegin() failed with error code %d, WSAGetLastError = %d\n", iResult, WSAGetLastError());
				break;
			}

			while ( bContinueLookup )
			{
				//
				// Get information about next bluetooth device
				//
				// Note you may pass the same WSAQUERYSET from LookupBegin
				// as long as you don't need to modify any of the pointer
				// members of the structure, etc.
				//
				// ZeroMemory(pWSAQuerySet, ulPQSSize);
				// pWSAQuerySet->dwNameSpace = NS_BTH;
				// pWSAQuerySet->dwSize = sizeof(WSAQUERYSET);
				if ( NO_ERROR == WSALookupServiceNextA(hLookup, ulFlags,&ulPQSSize,pWSAQuerySetA) )
				{

					//
					// Compare the name to see if this is the device we are looking for.
					//
					if ( ( pWSAQuerySetA->lpszServiceInstanceName != NULL ) &&( 0 == _stricmp(pWSAQuerySetA->lpszServiceInstanceName, pszRemoteName) ) )
					{
						//
						// Found a remote bluetooth device with matching name.
						// Get the address of the device and exit the lookup.
						//
						CopyMemory(pRemoteBtAddr,
							(PSOCKADDR_BTH) pWSAQuerySetA->lpcsaBuffer->RemoteAddr.lpSockaddr,
							sizeof(*pRemoteBtAddr));
						bRemoteDeviceFound = TRUE;
						bContinueLookup = FALSE;
					}
				}
				else
				{
					iResult = WSAGetLastError();
					if ( WSA_E_NO_MORE == iResult )
					{ //No more data
						//
						// No more devices found.  Exit the lookup.
						//
						bContinueLookup = FALSE;
					}
					else if ( WSAEFAULT == iResult )
					{
						//
						// The buffer for QUERYSET was insufficient.
						// In such case 3rd parameter "ulPQSSize" of function "WSALookupServiceNext()" receives
						// the required size.  So we can use this parameter to reallocate memory for QUERYSET.
						//
						HeapFree(GetProcessHeap(), 0, pWSAQuerySetA);
						pWSAQuerySetA = (PWSAQUERYSETA) HeapAlloc(GetProcessHeap(),
							HEAP_ZERO_MEMORY,
							ulPQSSize);
						if ( NULL == pWSAQuerySetA ) {
							wprintf(L"!ERROR! | Unable to allocate memory for WSAQERYSET\n");
							iResult = STATUS_NO_MEMORY;
							bContinueLookup = FALSE;
						}
					} else {
						wprintf(L"=CRITICAL= | WSALookupServiceNext() failed with error code %d\n", iResult);
						bContinueLookup = FALSE;
					}
				}
			}

			//
			// End the lookup service
			//
			WSALookupServiceEnd(hLookup);

			if ( STATUS_NO_MEMORY == iResult )
			{
				break;
			}
		}
	}

	if ( NULL != pWSAQuerySetA )
	{
		HeapFree(GetProcessHeap(), 0, pWSAQuerySetA);
		pWSAQuerySetA = NULL;
	}

	if ( bRemoteDeviceFound )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	return iResult;
}

#endif

#ifdef JVX_WITH_PCAP
#define HAVE_REMOTE

// For the latest version of NPCAP SDK, we need to define 
// PCAP_DONT_INCLUDE_PCAP_BPF_H and exclude Win32-Extensions.h
//
// https://stackoverflow.com/questions/30583626/pcap-program-error-when-compiling
// 

#define PCAP_DONT_INCLUDE_PCAP_BPF_H
#include <pcap.h>
#ifdef JVX_OS_WINDOWS
// #include <Win32-Extensions.h> <- no longer required in current version of npcap
#endif
#define PCAP_READ_TIMEOUT 0
#define PCAP_SNAP_LEN 65536
#ifdef JVX_OS_WINDOWS
#define PCAP_OPEN_FLAGS_NORMAL (PCAP_OPENFLAG_PROMISCUOUS | PCAP_OPENFLAG_MAX_RESPONSIVENESS )
#define PCAP_OPEN_FLAGS_NO_SELF_CAPTURE (PCAP_OPENFLAG_PROMISCUOUS | PCAP_OPENFLAG_MAX_RESPONSIVENESS | PCAP_OPENFLAG_NOCAPTURE_LOCAL)
// PCAP_OPENFLAG_NOCAPTURE_LOCAL: do not capture those packets which I introduced myself
// PCAP_OPENFLAG_PROMISCUOUS means: capture only those packets with the right mac address
#else // JVX_OS_WINDOWS
#define PCAP_OPEN_FLAGS_NORMAL 1 //PCAP_OPENFLAG_PROMISCUOUS
#define PCAP_OPEN_FLAGS_NO_SELF_CAPTURE PCAP_OPEN_FLAGS_NORMAL
#endif // JVX_OS_WINDOWS



#ifdef JVX_OS_WINDOWS
#include <stdio.h>
#include <conio.h>
#include "packet32.h"
#include <ntddndis.h>


static jvxErrorType
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
#else // JVX_OS_WINDOWS

#include <ifaddrs.h>

#ifdef JVX_OS_LINUX
#include <netpacket/packet.h>
#endif

#ifdef JVX_OS_MACOSX
#include <net/if_dl.h>
#define AF_PACKET AF_LINK
#define sockaddr_ll sockaddr_dl
#define sll_halen sdl_alen
#define sll_addr sdl_data
#endif

// What to do in case of other operating systems?
static jvxErrorType get_macs(std::string name, char* mac)
{
	int i;
	jvxSize maxMac = 0;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct ifaddrs* ifPtr = NULL, *ifPtrR = NULL;
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
					struct sockaddr_ll *s = (struct sockaddr_ll*)ifPtrR->ifa_addr;
					maxMac = JVX_MIN(6, s->sll_halen);
					for (i = 0; i <maxMac; i++)
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

static void
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

#endif // JVX_WITH_PCAP


