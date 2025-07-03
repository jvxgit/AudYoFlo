#include "HjvxNetworkDevices.h"

// Resources:
// https://learn.microsoft.com/en-us/windows/win32/iphlp/packet-timestamping

#include <iphlpapi.h>
#pragma comment(lib, "Iphlpapi")

BOOL
IsPTPv2HardwareTimestampingSupportedForIPv4(PINTERFACE_TIMESTAMP_CAPABILITIES timestampCapabilities)
{
	// Supported if both receive and transmit side support is present
	if (((timestampCapabilities->HardwareCapabilities.PtpV2OverUdpIPv4EventMessageReceive) ||
		(timestampCapabilities->HardwareCapabilities.PtpV2OverUdpIPv4AllMessageReceive) ||
		(timestampCapabilities->HardwareCapabilities.AllReceive))
		&&
		((timestampCapabilities->HardwareCapabilities.PtpV2OverUdpIPv4EventMessageTransmit) ||
			(timestampCapabilities->HardwareCapabilities.PtpV2OverUdpIPv4AllMessageTransmit) ||
			(timestampCapabilities->HardwareCapabilities.TaggedTransmit) ||
			(timestampCapabilities->HardwareCapabilities.AllTransmit)))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL
IsPTPv2HardwareTimestampingSupportedForIPv6(PINTERFACE_TIMESTAMP_CAPABILITIES timestampCapabilities)
{
	// Supported if both receive and transmit side support is present
	if (((timestampCapabilities->HardwareCapabilities.PtpV2OverUdpIPv6EventMessageReceive) ||
		(timestampCapabilities->HardwareCapabilities.PtpV2OverUdpIPv6AllMessageReceive) ||
		(timestampCapabilities->HardwareCapabilities.AllReceive))
		&&
		((timestampCapabilities->HardwareCapabilities.PtpV2OverUdpIPv6EventMessageTransmit) ||
			(timestampCapabilities->HardwareCapabilities.PtpV2OverUdpIPv6AllMessageTransmit) ||
			(timestampCapabilities->HardwareCapabilities.TaggedTransmit) ||
			(timestampCapabilities->HardwareCapabilities.AllTransmit)))
	{
		return TRUE;
	}

	return FALSE;
}

// This function checks and returns the supported timestamp capabilities for an interface for
// a PTPv2 application
jvxSupportedTimestampType
CheckActiveTimestampCapabilitiesForPtpv2(NET_LUID interfaceLuid)
{
	DWORD result = NO_ERROR;
	INTERFACE_TIMESTAMP_CAPABILITIES timestampCapabilities;
	jvxSupportedTimestampType supportedType = TimestampTypeNone;

	result = GetInterfaceActiveTimestampCapabilities(
		&interfaceLuid,
		&timestampCapabilities);
	if (result != NO_ERROR)
	{
		printf("Error retrieving hardware timestamp capabilities: %d\n", result);
		goto Exit;
	}

	if (IsPTPv2HardwareTimestampingSupportedForIPv4(&timestampCapabilities) &&
		IsPTPv2HardwareTimestampingSupportedForIPv6(&timestampCapabilities))
	{
		supportedType = TimestampTypeHardware;
		goto Exit;
	}
	else
	{
		if ((timestampCapabilities.SoftwareCapabilities.AllReceive) &&
			((timestampCapabilities.SoftwareCapabilities.AllTransmit) ||
				(timestampCapabilities.SoftwareCapabilities.TaggedTransmit)))
		{
			supportedType = TimestampTypeSoftware;
		}
	}

Exit:
	return supportedType;
}

// Helper function which does the correlation between hardware and system clock
// using mathematical techniques
void ComputeCorrelationOfHardwareAndSystemTimestamps(INTERFACE_HARDWARE_CROSSTIMESTAMP* crossTimestamp)
{
}

// An application would call this function periodically to gather a set 
// of matching timestamps for use in converting hardware timestamps to
// system timestamps
DWORD
RetrieveAndProcessCrossTimestamp(NET_LUID interfaceLuid)
{
	DWORD result = NO_ERROR;
	INTERFACE_HARDWARE_CROSSTIMESTAMP crossTimestamp;

	result = CaptureInterfaceHardwareCrossTimestamp(
		&interfaceLuid,
		&crossTimestamp);
	if (result != NO_ERROR)
	{
		printf("Error retrieving cross timestamp for the interface: %d\n", result);
		goto Exit;
	}

	// Process crossTimestamp further to create a relation between the hardware clock
	// of the NIC and the QPC values using appropriate mathematical techniques
	ComputeCorrelationOfHardwareAndSystemTimestamps(&crossTimestamp);

Exit:
	return result;
}
std::string pwchar_to_string(PWCHAR pwchar) 
{
	if (!pwchar) return {};

	// Länge des Ziel-Strings bestimmen
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, pwchar, -1, nullptr, 0, nullptr, nullptr);
	if (size_needed <= 0) return {};

	std::string result(size_needed - 1, 0); // -1 wegen Nullterminator
	WideCharToMultiByte(CP_UTF8, 0, pwchar, -1, &result[0], size_needed, nullptr, nullptr);
	return result;
}


jvxErrorType 
HjvxNetworkDevices::scanDevices()
{
	ULONG outBufLen = 0;
	DWORD dwRetVal = 0;
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;

	// Erster Aufruf, um die benötigte Puffergröße zu ermitteln
	dwRetVal = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, NULL, &outBufLen);
	if (dwRetVal != ERROR_BUFFER_OVERFLOW) {
		printf("GetAdaptersAddresses failed (size query): %u\n", dwRetVal);
		return JVX_ERROR_INTERNAL;
	}

	pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
	if (!pAddresses) {
		printf("Memory allocation failed\n");
		return JVX_ERROR_INTERNAL;
	}

	// Zweiter Aufruf, um die Adapterdaten zu erhalten
	dwRetVal = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen);
	if (dwRetVal != NO_ERROR) {
		printf("GetAdaptersAddresses failed: %u\n", dwRetVal);
		goto LOCAL_ERROR;		
	}

	for (PIP_ADAPTER_ADDRESSES pCurr = pAddresses; pCurr != NULL; pCurr = pCurr->Next) {

		oneNetDevice newDev;
		newDev.friendlyName = pwchar_to_string(pCurr->FriendlyName);
		newDev.id = pCurr->IfIndex;

		// InterfaceLuid ermitteln
		NET_LUID luid;
		if (ConvertInterfaceIndexToLuid(pCurr->IfIndex, &newDev.nDevHdl) == NO_ERROR) 
		{			
			devices.push_back(newDev);
		}
		else 
		{
			printf("  InterfaceLuid: Fehler bei der Umwandlung\n");
			goto LOCAL_ERROR;
		}
	}

	free(pAddresses);
	return JVX_NO_ERROR;

LOCAL_ERROR:
	free(pAddresses);
	return JVX_ERROR_INTERNAL;
}

jvxSupportedTimestampType
HjvxNetworkDevices::supportedTimestampType(jvxNetDeviceHandle* devIdHandle)
{
	return CheckActiveTimestampCapabilitiesForPtpv2(*devIdHandle);
}