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
// #include <Win32-Extensions.h>
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

#endif 

jvxErrorType get_macs(std::string name, char* mac);
void pcap_prepareBufferSend(char* tmp, jvxUInt8* macDest, jvxUInt8* macSrc);

#endif // JVX_WITH_PCAP