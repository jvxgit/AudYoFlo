/*
 * ==============================================
 * Filename: uart.c
 * ==============================================
 * Purpose: Low level UART processing. We collect bytes in the interrupt service routine 
 *             on the input side and use DMA transfers to output data to RS232 link.
 *             The output is pretty simple: Just flush out the bytes in the specified buffer.
 *            The input is more complex: Scan for the message header first, and the second part is variable.
 *             I implemented DMA based input earlier (v8) but the link sometimes stopped working
 *             due to something that I can not really understand. I guess that there is a delay
 *             when starting the read process for the varoable part.
 *            Interrupt based does a good job as well and is more flexible, however.
 *             COPYRIGHT HK, Javox, 2011
 */


#include <string.h>

#include "dskda830.h"
#include "dskda830_led.h"
#include "dskda830_edma3.h"
#include "systemConfig.h"

#include "jvx-crc.h"

#ifdef COMPILE_FOR_OMAP_137
#define JVX_ASSERT_STRING_TOKEN "jvx_remote_call.c"
#define JVX_ASSERT_INT_ID __LINE__
#else
#define JVX_ASSERT_STRING_TOKEN __FUNCTION__
#define JVX_ASSERT_INT_ID __LINE__
#endif

extern void postSwi_uartrecv();

// Interface to higher level API, defined in manageExternalCalls.c

//=====================================================================

// Include the higher level UART API
#include "jvx_remote_call_cb.h"
#include "typedefs/tools/TjvxConnection.h"
#include "typedefs/tools/TjvxRs232.h"
/*
#define JVX_RS232_SECURE_CHANNEL_START_BYTE 0x01
#define JVX_RS232_SECURE_CHANNEL_STOP_BYTE 0xFE
#define JVX_RS232_SECURE_CHANNEL_ESC_BYTE 0x27
*/

#include "jvx_project_local_constraints.h"

#define RX_BUF_LEN MAX_LENGTH_SINGLE_PACKET
#define TX_BUF_LEN ((2*RX_BUF_LEN)+2) // worst case: every character is escaped except start and end

#ifdef NRMOTOR_TARGET_OMAPL137
//#pragma DATA_SECTION(txBuf, ".userDefinedData")
//#pragma DATA_SECTION(rxBuf, ".userDefinedData")
#endif
Uint8 txBuf[TX_BUF_LEN]; // TODO maybe move these into oneReceiveEvent
Uint8 rxBuf[RX_BUF_LEN];

//=====================================================================

// State machine for incoming messages
typedef enum 
{
    UART_RECEIVE_STATE_NONE,
    UART_RECEIVE_STATE_RX,
    UART_RECEIVE_STATE_ESCAPE,
    UART_RECEIVE_STATE_LOCKED
} stateUartReceive;

// Struct to manage incoming messages in interrupt routines
struct
{
    stateUartReceive state;
    size_t pos;
} oneReceiveEvent;

//============================================================== 
// paRAM stuct pointers for DMA transfer UART2 (only transmit)
//============================================================== 
struct
{
    edma_paRAM* paramX;

} uartControl = {0};

// ###############################################################################

/*
 * Initialize data buffers, DMA trasnfers and interrupt handling
 *///=====================================================================
void uart_initializeMemory()
{
    jvxDspBaseErrorType resL = JVX_DSP_NO_ERROR;
    //int i,j,k;
    //int idx = 0;

    Uint32 readEdma_drae1 = EDMA3CC_DRAE1;
    Uint32 readEdma_iesr = EDMA3CC_DRAE1;

    // INIT THE RECEIVER SIDE

    // Initialize the remote call engine
    resL = jvx_remote_call_initCfg(&hdlRemoteCall);
    assert(resL == JVX_DSP_NO_ERROR);

    jvx_remote_call_cb_init();
    resL = jvx_remote_call_init(&hdlRemoteCall);
    assert(resL == JVX_DSP_NO_ERROR);

    // Get pointer to receive buffer
    //manageRemoteCalls_pointerCommDataReceiver_input(&oneReceiveEvent.ptrWriteStart, &oneReceiveEvent.maxNumBytes);
    oneReceiveEvent.pos = 0;
    oneReceiveEvent.state = UART_RECEIVE_STATE_NONE;

    // INIT THE TRANMIT SIDE

    // Prepare DMA transfers
    EDMA3CC_DRAE1 = readEdma_drae1 | (Uint32)(1 << X_DMA_UART); // Enable event ER-X_DMA_NUM_S0

    // Enable the interrupt reporting for McAsp trasnmit and receive
    EDMA3CC_REGION1_IESR =  (Uint32)(1 << X_DMA_UART); // Enable interrupt X_DMA_UART

    // Setup the paRAM structs for transmit
    uartControl.paramX = EDMA3_PARAMS_RAM;

    // McAsp1: Transmit event is routed to channel 3
    uartControl.paramX += X_DMA_UART;

    // Set paRAM struct to inactive
    memset(uartControl.paramX, 0, sizeof(edma_paRAM));
    uartControl.paramX->link_bcntrld = 0xFFFF;

    // ====================================================
    // Enable the events: receive and transmit
    // ====================================================
    EDMA3CC_REGION1_EESR = (Uint32)(1 << X_DMA_UART);
    EDMA3CC_REGION1_SECR = (Uint32)(1 << X_DMA_UART);
}

/*
 * SWI callback to indicate a complete UART transmit sequence
 * NOTE this reacts on swiUartTransmitComplete posted in audio.c on DMA complete event
 *///=======================================================
void uart_sendComplete()
{
    EDMA3CC_REGION1_SECR = (Uint32)(1 << X_DMA_UART);
    // If we are here, the UART transmit sequence is complete
    DSKDA830_LED_off(1);

    // Report complete send to higher software layer
    //manageRemoteCalls_reportSendComplete();
}

/*
 * SWI callback to indicate a complete UART receive sequence
 *///=======================================================
void uart_receiveComplete()
{
    jvxDspBaseErrorType res = JVX_DSP_NO_ERROR;

    jvxRcProtocol_simple_message* mess = NULL;

    jvxUInt16 crcS = 0;
    jvxUInt16 crcC = 0;

#ifdef JVX_INTEGRATE_CRC_REMOTE_CALL
    mess = (jvxRcProtocol_simple_message*)rxBuf;
    crcS = mess->crc;
    mess->crc = 0;
    crcC = jvx_crc16_ccitt((jvxByte*)mess, mess->loc_header.paketsize);

    if(crcC != crcS)
    {
    	report_event(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID);
		DSKDA830_LED_on( 1 );
    	DSKDA830_LED_on( 2 );
    	DSKDA830_LED_on( 3 );
    	while(1)
    	{
    		;// Hang forever due to a crc check fail
    	}
    }
#endif

    // TODO implement protocol header validation
    res = jvx_remote_call_incoming(&hdlRemoteCall, (jvxProtocolHeader*)rxBuf, 0, NULL, NULL);
    assert(res == JVX_DSP_NO_ERROR);

    DSKDA830_LED_off(2);
}

/*
 * Initialize the UART part of the C6747 chip
 *///=========================================================
void uart_initialize()
{
    Uint32 divisor;
    volatile Uint16 dummy;
    Uint32 tmp = 0;

    /*
    *  UART clk / baudrate
    *  = 150,000,000 / (115200 * 16)
    * Taken from here: http://e2e.ti.com/support/dsp/omap_applications_processors/f/42/t/116098.aspx
    */
    divisor = 150000000 / (BAUDRATE * 16);

   
    UART2_PWREMU_MGMT = 0; // Reset UART TX & RX components

    DSKDA830_wait(100);

    UART2_DLL = (divisor & 0xff); // Set baud rate
    UART2_DLH = (divisor >> 8);
        
    UART2_FCR = 0x0007; // Clear UART TX & RX FIFOs
    UART2_FCR = 0x0000; // Non-FIFO mode

    UART2_FCR = 0x0001; // Enable FIFO
    tmp = (0x000000 | (0 << 6) | (1 << 3) | (1 << 2) | (1 << 1) | 1); // 4 bytes in FIFO, DMA mode on, TXCLR, RXCLR. FIFO enable
    UART2_FCR = tmp;

    //UART2_IER = 0x0007;          // Enable interrupts
    UART2_IER = 0x0005;            // Enable interrupts, do not enable TX interrupt, we handle this by DMA
    UART2_LCR = 0x0003;            // 8-bit words,
                                   // 1 STOP bit generated,
                                   // No Parity, No Stick paritiy,
                                   // No Break control
    UART2_MCR = 0x0000;            // RTS & CTS disabled, (note: RTS/CTS does not work on UART2)
                                   // Loopback mode disabled,
                                   // Autoflow disabled
                                       
    UART2_PWREMU_MGMT = 0xE001;    // Enable TX & RX componenets

    // Clear any pre-existing characters
    dummy = UART2_THR;
}

/*
 * Function to initiate a uart transfer. The buffer will be used in DMA transfer as it is.
 *///================================================================
void  uart_initializeTransfer(Uint8* buf, Uint32 numBytes)
{
    size_t i, pos;

    DSKDA830_LED_on( 1 );

    JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, numBytes <= TX_BUF_LEN);

    jvxRcProtocol_simple_message* mess = (jvxRcProtocol_simple_message*)buf;
    JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, numBytes >= sizeof(jvxRcProtocol_simple_message));

#ifdef JVX_INTEGRATE_CRC_REMOTE_CALL
    mess = (jvxRcProtocol_simple_message*)buf;
    mess->crc = jvx_crc16_ccitt((jvxByte*)mess, mess->loc_header.paketsize);
#endif

    // Add framing to data buffer
    pos = 0;
    txBuf[pos++] = JVX_RS232_SECURE_CHANNEL_START_BYTE;
    for (i = 0; i < numBytes; i++)
    {
        // Bit stuffing for flags and escape character
        if (buf[i] == JVX_RS232_SECURE_CHANNEL_START_BYTE || buf[i] == JVX_RS232_SECURE_CHANNEL_STOP_BYTE || buf[i] == JVX_RS232_SECURE_CHANNEL_ESC_BYTE)
        {
            txBuf[pos++] = JVX_RS232_SECURE_CHANNEL_ESC_BYTE;
            txBuf[pos++] = buf[i];
        }
        else
        {
            txBuf[pos++] = buf[i];
        }
    }
    txBuf[pos++] = JVX_RS232_SECURE_CHANNEL_STOP_BYTE;
    // New number of tx bytes including escape characters
    numBytes = pos;

    // Prepare the paRAM struct for transmit interrupt
    // IRQ triggered for DMA transfer complete event
    uartControl.paramX->opt = 0x00100000 | ( X_DMA_UART << 12) | (1 << 2); // ITCCHEN, TCCHEN, ITCINTEN = 0, TCINTEN = 1, TCC = 3 (as DMA channel), TCCMODE = 0, FWID = 0, STATIC = 0, SYNCDIM = 1 (AB), DAM, SAM = 0

    // Set the memory to stream data to DMA output
    uartControl.paramX->src = (Uint32)txBuf;//fldDMA; // Set the DMA field address pointer

    // One event means to feed data for 4 serializers, for each 4 byte
    uartControl.paramX->a_b_cnt = (1 << 16) | (1);

    // Destination address: Transfer buffer register
    uartControl.paramX->dst = UART2_THR_PTR;

    // On memory side, move pointer forward, on McAsp, do not move it
    uartControl.paramX->src_dst_bidx = (0 << 16) | 1;

    // New link address: Link to self, always reload B to 4
    uartControl.paramX->link_bcntrld = (1 << 16) | (0xFFFF);

    // Shift forward the source address by 4 a-frames (for each serializer), do not shift forward the destnation address
    uartControl.paramX->src_dst_cidx = (0 << 16) | 1;

    // Stop data transfer after each complete frame
    uartControl.paramX->ccnt = numBytes;

    EDMA3CC_REGION1_SECR = (Uint32)(1 << X_DMA_UART);

    // Now start the DMA transfer manually
    EDMA3CC_REGION1_ESR = (Uint32)(1 <<  X_DMA_UART);
}

/*
 * UART interrupt service routine. Only the receive side has been realized since the receive side 
 * is interrupt based whereas the transmit side is DMA based.
 *///=============================================================
void uart_serveInterrupt()
{
    Uint8 rxChr;
    Uint32 it_type, iir, lstat;
    while (1)
    {
        iir = UART2_IIR;
        if (iir & 0x01)
            break;
        it_type = (iir >> 1) & 0x1F;

        switch(it_type)
        {
            case 1:
                // Here: Transmitter empty, ignore, but should not occur anyway
                break;

            case 2:
                rxChr = UART2_RBR;
                if (oneReceiveEvent.state == UART_RECEIVE_STATE_LOCKED)
                    break; // Leave if receive buffer is locked NOTE this will result in data loss!

                if (oneReceiveEvent.state != UART_RECEIVE_STATE_ESCAPE && rxChr == JVX_RS232_SECURE_CHANNEL_START_BYTE)
                {
                    // Wait for frame start flag
                        oneReceiveEvent.state = UART_RECEIVE_STATE_RX;
                        oneReceiveEvent.pos = 0;
                        DSKDA830_LED_on(2);
                }
                else if (oneReceiveEvent.state == UART_RECEIVE_STATE_RX)
                {
                    if (rxChr == JVX_RS232_SECURE_CHANNEL_ESC_BYTE)
                    {
                        oneReceiveEvent.state = UART_RECEIVE_STATE_ESCAPE;
                    }
                    else if (rxChr == JVX_RS232_SECURE_CHANNEL_STOP_BYTE)
                    {
                        // frame rx complete
                        oneReceiveEvent.state = UART_RECEIVE_STATE_LOCKED; // lock receive buffer
                        postSwi_uartrecv();
                        //SWI_post(&swiUartReceiveComplete); // unlock as soon as possible
                        oneReceiveEvent.state = UART_RECEIVE_STATE_NONE;
                        oneReceiveEvent.pos = 0;
                    }
                    else
                    {
                    	// Avoid segmentation fault
                        JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, oneReceiveEvent.pos < RX_BUF_LEN);
                        rxBuf[oneReceiveEvent.pos++] = rxChr;
                    }
                }
                else if (oneReceiveEvent.state == UART_RECEIVE_STATE_ESCAPE)
                {
                	// Avoid segmentation fault
                    JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, oneReceiveEvent.pos < RX_BUF_LEN);
                    rxBuf[oneReceiveEvent.pos++] = rxChr;
                    oneReceiveEvent.state = UART_RECEIVE_STATE_RX;
                }

                /*
                 if ( >= RX_BUF_LEN)
                {
                    // discard current frame due to buffer overflow
                    oneReceiveEvent.state = UART_RECEIVE_STATE_NONE;
                    oneReceiveEvent.pos = 0;
                }*/

                break;

            case 3:
                lstat = UART2_LSR; // read this to clear the interrupt; see UART2_LSR
                JVX_ASSERT_X(JVX_ASSERT_STRING_TOKEN, JVX_ASSERT_INT_ID, 0);
                break;
        }
    }
}

// Function to let the low level module know that the input buffer is no longer used..
void uart_unlockReceiverBuffer()
{
    // Set the state back to initial
    oneReceiveEvent.state =  UART_RECEIVE_STATE_NONE; // TODO maybe double buffering; jvx_remote_call copies data -> unlock early
}

// ===================================================================
