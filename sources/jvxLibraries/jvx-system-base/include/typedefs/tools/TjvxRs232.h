#ifndef __TJVXRS232_H__
#define __TJVXRS232_H__

typedef enum
{
	JVX_RS232_NO_FLOWCONTROL,				// No flowcontrol
	JVX_RS232_CTSRTSFLOWCONTROL,
	JVX_RS232_CTSDTRFLOWCONTROL,
	JVX_RS232_DSRRTSFLOWCONTROL,
	JVX_RS232_DSRDTRFLOWCONTROL,
	JVX_RS232_XONXOFFFLOWCONTROL
} jvxRs232FlowControlEnum;

typedef enum
{
	JVX_RS232_REPORT_CONNECTION_ESTABLISHED_SHIFT = JVX_CONNECTION_REPORT_SHFT_OFFSET + 0,
	JVX_RS232_REPORT_CONNECTION_STOPPED_SHIFT = JVX_CONNECTION_REPORT_SHFT_OFFSET + 1,
	JVX_RS232_REPORT_CTS_SHFT = JVX_CONNECTION_REPORT_SHFT_OFFSET + 2,		// Report CTS set/unset
	JVX_RS232_REPORT_RLSD_SHFT = JVX_CONNECTION_REPORT_SHFT_OFFSET  + 3,		// Report RLSD set/unset
	JVX_RS232_REPORT_RING_SHFT = JVX_CONNECTION_REPORT_SHFT_OFFSET + 4,		// Report Ring line state changed
	JVX_RS232_REPORT_DSR_SHFT = JVX_CONNECTION_REPORT_SHFT_OFFSET + 5,
	JVX_RS232_REPORT_ERROR_SHFT = JVX_CONNECTION_REPORT_SHFT_OFFSET + 6
} jvxRs232ReportEnum;

typedef enum
{
	JVX_RS232_STATUS_CTS_SHFT = 0,		// Report CTS set/unset
	JVX_RS232_STATUS_RLSD_SHFT = 1,		// Report RLSD set/unset
	JVX_RS232_STATUS_RING_SHFT = 2,		// Report Ring line state changed
	JVX_RS232_STATUS_DSR_SHFT = 3
} jvxRs232StatusEnum;

typedef enum
{
	JVX_RS232_STOPBITS_ONE,
	JVX_RS232_STOPBITS_ONE5,
	JVX_RS232_STOPBITS_TWO
} jvxRs232StopbitsEnum;

typedef enum
{
	JVX_RS232_PARITY_EVEN,
	JVX_RS232_PARITY_MARK, 
	JVX_RS232_PARITY_ODD, 
	JVX_RS232_PARITY_NO,
	JVX_RS232_PARITY_SPACE
} jvxRs232ParityEnum;

typedef enum
{
	JVX_RS232_CONTROL_NONE = 0,
    JVX_RS232_CONTROL_GET_MODEM_STATUS,
    JVX_RS232_CONTROL_SET_MODEM_STATUS
} jvxRs232ControlCommands;

typedef struct
{
	int bRate;
	int bits4Byte;
	jvxRs232StopbitsEnum stopBitsEnum; 
	jvxRs232ParityEnum parityEnum;
	jvxRs232FlowControlEnum enFlow;
	jvxCBool boostPrio;
	jvxCBitField reportEnum;

	struct
	{
		struct
		{
			jvxCBool active;
			jvxSize preallocationSize;
		} transmit;
		struct
		{
			jvxCBool active;
			jvxSize maxPreReadSize;
		} receive;

		jvxUByte startByte;
		jvxUByte stopByte;
		jvxUByte escByte;
	} secureChannel;

} jvxRs232Config;

#define JVX_CONNECTION_TAG_RS232 "JVX_RS232_CONNECTION_COMPONENT"

#define JVX_RS232_SECURE_CHANNEL_START_BYTE 0x01
#define JVX_RS232_SECURE_CHANNEL_STOP_BYTE 0xFE
#define JVX_RS232_SECURE_CHANNEL_ESC_BYTE 0x27

#endif
