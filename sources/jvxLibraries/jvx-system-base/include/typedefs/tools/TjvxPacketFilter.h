#ifndef __TJVXPACKTEFILTERRULE_H__
#define __TJVXPACKTEFILTERRULE_H__

typedef struct
{
	jvxHandle* theBuf;
	jvxSize sz;
	jvxHandle* ipEntry;
	jvxCBitField ipVersion;
	jvxHandle* transportEntry; 
	jvxCBitField transportVersion;
	jvxHandle* dataEntry;
	jvxBool* isFullfilled;
	jvxByte* srcMacEntry;
	jvxByte* destMacEntry;
	jvxByte etherProt; 
} jvxCheckPacketType;
		
typedef enum
{
	JVX_PACKET_FILTER_IPV4_SHIFT = 0,
	JVX_PACKET_FILTER_IPV6_SHIFT = 1
} jvxIpVersionFlags;

typedef enum
{
	JVX_PACKET_FILTER_TCP_SHIFT = 0,
	JVX_PACKET_FILTER_UDP_SHIFT = 1
} jvxTransportFlags;

typedef enum
{
	// taken from here:
	// https://en.wikipedia.org/wiki/EtherType
	JVX_ETHERNET_HEADER_TYPE_IPV4 = 0x8000,
	JVX_ETHERNET_HEADER_TYPE_ARP = 0x0806,
	JVX_ETHERNET_HEADER_TYPE_WAKE_ON_LAN = 0x0842,
	JVX_ETHERNET_HEADER_TYPE_TRILL = 0x22F3,
	JVX_ETHERNET_HEADER_TYPE_DECNET = 0x6003,
	JVX_ETHERNET_HEADER_TYPE_REVERSE_ADDR_RESOL = 0x8035,
	JVX_ETHERNET_HEADER_TYPE_APPLETALK = 0x809B,
	JVX_ETHERNET_HEADER_TYPE_AARP = 0x80F3,
	JVX_ETHERNET_HEADER_TYPE_VLAN = 0x8100,
	JVX_ETHERNET_HEADER_TYPE_IPX = 0x8137,
	JVX_ETHERNET_HEADER_TYPE_QNX = 0x8204,
	JVX_ETHERNET_HEADER_TYPE_IPV6 = 0x86DD,
	JVX_ETHERNET_HEADER_TYPE_ETHERNET_HW_FLOW = 0x8808,
	JVX_ETHERNET_HEADER_TYPE_COBRA_NET = 0x8819,
	JVX_ETHERNET_HEADER_TYPE_MPLS_MCAST = 0x8847,
	//0x8848 	MPLS multicast
	//0x8863 	PPPoE Discovery Stage
	//0x8864 	PPPoE Session Stage
	//0x887B 	HomePlug 1.0 MME
	//0x888E 	EAP over LAN(IEEE 802.1X)
	JVX_ETHERNET_HEADER_TYPE_PROFINET = 0x8892,
	//0x889A 	HyperSCSI(SCSI over Ethernet)
	JVX_ETHERNET_HEADER_TYPE_ATA = 0x88A2,
	JVX_ETHERNET_HEADER_TYPE_ETHERCAT = 0x88A4,
	//0x88A8 	Provider Bridging(IEEE 802.1ad) & Shortest Path Bridging IEEE 802.1aq[5]
	//0x88AB 	Ethernet Powerlink[citation needed]
	//0x88B8 	GOOSE(Generic Object Oriented Substation event)
	//0x88B9 	GSE(Generic Substation Events) Management Services
	//0x88CC 	Link Layer Discovery Protocol(LLDP)
	//0x88CD 	SERCOS III
	//0x88E1 	HomePlug AV MME[citation needed]
	//0x88E3 	Media Redundancy Protocol(IEC62439 - 2)
	JVX_ETHERNET_HEADER_TYPE_MAC = 0x88E5,
	//0x88E7 	Provider Backbone Bridges(PBB) (IEEE 802.1ah)
	JVX_ETHERNET_HEADER_TYPE_PTP_IEEE1588 = 0x88F7
	//0x88FB 	Parallel Redundancy Protocol(PRP)
	//0x8902 	IEEE 802.1ag Connectivity Fault Management(CFM) Protocol / ITU - T Recommendation Y.1731 (OAM)
	//0x8906 	Fibre Channel over Ethernet(FCoE)
	//0x8914 	FCoE Initialization Protocol
	//0x8915 	RDMA over Converged Ethernet(RoCE)
	//0x891D 	TTEthernet Protocol Control Frame(TTE)
	//0x892F 	High - availability Seamless Redundancy(HSR)
	//0x9000 	Ethernet Configuration Testing Protocol[6]
	//0x9100 	VLAN - tagged(IEEE 802.1Q) frame with double tagging
} jvxImplementationEthernetType;

/*
typedef enum
{
	// taken from here:
	// https://en.wikipedia.org/wiki/EtherType
	JVX_ETHERNET_HEADER_TYPE_IPV4 = 0x8000,
	JVX_ETHERNET_HEADER_TYPE_ARP = 0x0806,
	JVX_ETHERNET_HEADER_TYPE_WAKE_ON_LAN = 0x084,
	JVX_ETHERNET_HEADER_TYPE_TRILL = 0x22F3,
	JVX_ETHERNET_HEADER_TYPE_DECNET = 0x6003,
	JVX_ETHERNET_HEADER_TYPE_REVERSE_ADDR_RESOL = 0x8035,
	JVX_ETHERNET_HEADER_TYPE_APPLETALK = 0x809B,
	JVX_ETHERNET_HEADER_TYPE_AARP = 0x80F3,
	JVX_ETHERNET_HEADER_TYPE_VLAN = 0x8100,
	JVX_ETHERNET_HEADER_TYPE_IPX = 0x8137,
	JVX_ETHERNET_HEADER_TYPE_QNX = 0x8204,
	JVX_ETHERNET_HEADER_TYPE_IPV6 = 0x86DD,
	JVX_ETHERNET_HEADER_TYPE_ETHERNET_HW_FLOW = 0x8808,
	JVX_ETHERNET_HEADER_TYPE_COBRA_NET = 0x8819,
	JVX_ETHERNET_HEADER_TYPE_MPLS_MCAST = 0x8847,
	//0x8848 	MPLS multicast
	//0x8863 	PPPoE Discovery Stage
	//0x8864 	PPPoE Session Stage
	//0x887B 	HomePlug 1.0 MME
	//0x888E 	EAP over LAN(IEEE 802.1X)
	JVX_ETHERNET_HEADER_TYPE_PROFINET = 0x8892,
	//0x889A 	HyperSCSI(SCSI over Ethernet)
	JVX_ETHERNET_HEADER_TYPE_ATA = 0x88A2,
	JVX_ETHERNET_HEADER_TYPE_ETHERCAT = 0x88A4,
	//0x88A8 	Provider Bridging(IEEE 802.1ad) & Shortest Path Bridging IEEE 802.1aq[5]
	//0x88AB 	Ethernet Powerlink[citation needed]
	//0x88B8 	GOOSE(Generic Object Oriented Substation event)
	//0x88B9 	GSE(Generic Substation Events) Management Services
	//0x88CC 	Link Layer Discovery Protocol(LLDP)
	//0x88CD 	SERCOS III
	//0x88E1 	HomePlug AV MME[citation needed]
	//0x88E3 	Media Redundancy Protocol(IEC62439 - 2)
	JVX_ETHERNET_HEADER_TYPE_MAC = 0x88E5,
	//0x88E7 	Provider Backbone Bridges(PBB) (IEEE 802.1ah)
	JVX_ETHERNET_HEADER_TYPE_PTP_IEEE1588 = 0x88F7
	//0x88FB 	Parallel Redundancy Protocol(PRP)
	//0x8902 	IEEE 802.1ag Connectivity Fault Management(CFM) Protocol / ITU - T Recommendation Y.1731 (OAM)
	//0x8906 	Fibre Channel over Ethernet(FCoE)
	//0x8914 	FCoE Initialization Protocol
	//0x8915 	RDMA over Converged Ethernet(RoCE)
	//0x891D 	TTEthernet Protocol Control Frame(TTE)
	//0x892F 	High - availability Seamless Redundancy(HSR)
	//0x9000 	Ethernet Configuration Testing Protocol[6]
	//0x9100 	VLAN - tagged(IEEE 802.1Q) frame with double tagging
} jvxImplementationIpType;
*/

typedef enum
{
	// https://en.wikipedia.org/wiki/List_of_IP_protocol_numbers
	JVX_IP_TYPE_HOPOPT = 0x00,
	JVX_IP_TYPE_ICMP = 0x01,
	JVX_IP_TYPE_IGMP = 0x02,
	JVX_IP_TYPE_GGP = 0x03,
	JVX_IP_TYPE_IP = 0x04,
	JVX_IP_TYPE_ST = 0x05,
	JVX_IP_TYPE_TCP = 0x06,
	JVX_IP_TYPE_CBT = 0x07,
	JVX_IP_TYPE_EGP = 0x08,
	JVX_IP_TYPE_IGP = 0x09,
	JVX_IP_TYPE_BBN = 0x0A,
	JVX_IP_TYPE_NVP = 0x0B,
	JVX_IP_TYPE_PUP = 0x0C,
	JVX_IP_TYPE_ARGUS = 0x0D,
	JVX_IP_TYPE_EMCON = 0x0E,
	JVX_IP_TYPE_XNET = 0x0F,
	JVX_IP_TYPE_CHAOS = 0x10,
	JVX_IP_TYPE_UDP = 0x11,
	JVX_IP_TYPE_MUX = 0x12,
	JVX_IP_TYPE_DCN = 0x13,
	JVX_IP_TYPE_HMP = 0x14,
	JVX_IP_TYPE_PRM = 0x15,
	JVX_IP_TYPE_XNS = 0x16,
	JVX_IP_TYPE_TRUNK1 = 0x17,
	JVX_IP_TYPE_TRUNK2 = 0x18,
	JVX_IP_TYPE_LEAF1 = 0x19,
	JVX_IP_TYPE_LEAF2 = 0x1A,
	JVX_IP_TYPE_RDP = 0x1B,
	JVX_IP_TYPE_IRTP = 0x1C,
	JVX_IP_TYPE_ISOTP4 = 0x1D,
	JVX_IP_TYPE_3NETBLT = 0x1E,
	JVX_IP_TYPE_MFENSP = 0x1F,
	JVX_IP_TYPE_MERITINP = 0x20,
	JVX_IP_TYPE_DCCP = 0x21,
	JVX_IP_TYPE_3PC = 0x22,
	JVX_IP_TYPE_IDPR = 0x23,
	JVX_IP_TYPE_XTP = 0x24,
	JVX_IP_TYPE_DDP = 0x25,
	JVX_IP_TYPE_IDPRCMTP = 0x26,
	JVX_IP_TYPE_TPPP = 0x27,
	JVX_IP_TYPE_IL = 0x28,
	JVX_IP_TYPE_IPv6 = 0x29,
	JVX_IP_TYPE_SDRP = 0x2A,
	JVX_IP_TYPE_IPv6Route = 0x2B,
	JVX_IP_TYPE_IPv6Frag = 0x2C,
	JVX_IP_TYPE_IDRP = 0x2D,
	JVX_IP_TYPE_RSVP = 0x2E,
	JVX_IP_TYPE_GRE = 0x2F,
	JVX_IP_TYPE_DSR = 0x30,
	JVX_IP_TYPE_BNA = 0x31,
	JVX_IP_TYPE_ESP = 0x32,
	JVX_IP_TYPE_AH = 0x33,
	JVX_IP_TYPE_INLSP = 0x34,
	JVX_IP_TYPE_SWIPE = 0x35,
	JVX_IP_TYPE_NARP = 0x36,
	JVX_IP_TYPE_MOBILE = 0x37,
	JVX_IP_TYPE_TLSP = 0x38,
	JVX_IP_TYPE_SKIP = 0x39,
	JVX_IP_TYPE_IPv6ICMP = 0x3A,
	JVX_IP_TYPE_IPv6NoNxt = 0x3B,
	JVX_IP_TYPE_IPv6Opts = 0x3C,
	JVX_IP_TYPE_ANYHOST = 0x3D,
	JVX_IP_TYPE_CFTP = 0x3E,
	JVX_IP_TYPE_ANYLOCAL = 0x3F,
	JVX_IP_TYPE_SATEXPAK = 0x40,
	JVX_IP_TYPE_KRYPTOLAN = 0x41,
	JVX_IP_TYPE_RVD = 0x42,
	JVX_IP_TYPE_IPPC = 0x43,
	JVX_IP_TYPE_ANYDISTRFILESYS = 0x44,
	JVX_IP_TYPE_SATNET = 0x45,
	JVX_IP_TYPE_VISA = 0x46,
	JVX_IP_TYPE_IPCU = 0x47,
	JVX_IP_TYPE_CPNX = 0x48,
	JVX_IP_TYPE_CPHB = 0x49,
	JVX_IP_TYPE_WSN = 0x4A,
	JVX_IP_TYPE_PVP = 0x4B,
	JVX_IP_TYPE_BRSATMON = 0x4C,
	JVX_IP_TYPE_SUNND = 0x4D,
	JVX_IP_TYPE_WBMON= 0x4E,
	JVX_IP_TYPE_WBEXPAK =  0x4F,
	JVX_IP_TYPE_ISOIP = 0x50,
	JVX_IP_TYPE_VMTP = 0x51,
	JVX_IP_TYPE_SECUREVMTP = 0x52,
	JVX_IP_TYPE_VINES = 0x53,
	JVX_IP_TYPE_TTP = 0x54,
	JVX_IP_TYPE_IPTM = 0x54,
	JVX_IP_TYPE_NSFNET = 0x55,
	JVX_IP_TYPE_DGP = 0x56,
	JVX_IP_TYPE_TCF = 0x57,
	JVX_IP_TYPE_EIGRP = 0x58,
	JVX_IP_TYPE_OSPF = 0x59, 
	JVX_IP_TYPE_SPRITERPC = 0x5A,
	JVX_IP_TYPE_LARP = 0x5B, 
	JVX_IP_TYPE_MTP = 0x5C,
	JVX_IP_TYPE_AX25 = 0x5D,
	JVX_IP_TYPE_KA9QNOS = 0x5E,
	JVX_IP_TYPE_MICP = 0x5F,
	JVX_IP_TYPE_SCCSP = 0x60,
	JVX_IP_TYPE_ETHERIP = 0x61,
	JVX_IP_TYPE_ENCAP = 0x62,
	JVX_IP_TYPE_ANYCRYPT = 0x63,
	JVX_IP_TYPE_GMTP = 0x64,
	JVX_IP_TYPE_IFMP = 0x65,
	JVX_IP_TYPE_PNNI = 0x66,
	JVX_IP_TYPE_PIM = 0x67, 
	JVX_IP_TYPE_ARIS = 0x68,
	JVX_IP_TYPE_SCPS = 0x69,
	JVX_IP_TYPE_QNX = 0x6A,
	JVX_IP_TYPE_ACTIVE = 0x6B,
	JVX_IP_TYPE_IPCOMP = 0x6C,
	JVX_IP_TYPE_SNP = 0x6D,
	JVX_IP_TYPE_COMPAQPEER = 0x6E,
	JVX_IP_TYPE_IPX = 0x6F,
	JVX_IP_TYPE_VRRP = 0x70,
	JVX_IP_TYPE_PGM = 0x71,
	JVX_IP_TYPE_ANY0HOP = 0x72,
	JVX_IP_TYPE_L2TP = 0x73,
	JVX_IP_TYPE_DDX = 0x74,
	JVX_IP_TYPE_IATP = 0x75,
	JVX_IP_TYPE_STP = 0x76,
	JVX_IP_TYPE_SRP = 0x77,
	JVX_IP_TYPE_UTI = 0x78,
	JVX_IP_TYPE_SMP = 0x79,
	JVX_IP_TYPE_SIMPLEMCAST = 0x7A,
	JVX_IP_TYPE_PTP = 0x7B,
	JVX_IP_TYPE_ISIPV4 = 0x7C, 
	JVX_IP_TYPE_FIRE = 0x7D, 
	JVX_IP_TYPE_CRTP = 0x7E,
	JVX_IP_TYPE_CRUDP = 0x7F,
	JVX_IP_TYPE_SSCOPMCE = 0x80,
	JVX_IP_TYPE_IPLT = 0x81,
	JVX_IP_TYPE_SPS = 0x82,
	JVX_IP_TYPE_PIPE = 0x83,
	JVX_IP_TYPE_SCTP = 0x84,
	JVX_IP_TYPE_FC = 0x85,
	JVX_IP_TYPE_RSVPE2E = 0x86,
	JVX_IP_TYPE_MOBILITYHEADER = 0x87,
	JVX_IP_TYPE_UDPLITE = 0x88,
	JVX_IP_TYPE_MPLS = 0x89,
	JVX_IP_TYPE_MANET = 0x8A,
	JVX_IP_TYPE_HIP = 0x8B,
	JVX_IP_TYPE_SHIM6 = 0x8C,
	JVX_IP_TYPE_WESP = 0x8D,
	JVX_IP_TYPE_ROHC = 0x8E
} jvxIpType;


#ifdef JVX_OS_WINDOWS
#pragma pack(push, 1)
#else
#pragma pack(push)
#pragma pack(1)
#endif

typedef struct
{
	// Acc to https://en.wikipedia.org/wiki/IPv4
	jvxByte version03_plus_ihl47;
	jvxByte dscp05_plus_ecn67;
	jvxUInt16 length;
	jvxUInt16 identification;
	jvxUInt16 flags02_plus_fragmentoffset315;
	jvxByte ttolive;
	jvxByte protocol;
	jvxUInt16 headerchksum;
	jvxUInt32 src_ip;
	jvxUInt32 dest_ip;
	jvxUInt32 first_option_arg;
} ipv4Header;


#ifdef JVX_OS_WINDOWS
#pragma pack(pop)
#else
#pragma pack(pop)
#endif 

#endif