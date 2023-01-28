#include "board.h"
#include "chip.h"
#include "jvx_audio_misc.h"

/* Get divider value */
STATIC Status getClkDiv(LPC_I2S_T *pI2S,
		uint16_t *pxDiv, uint16_t *pyDiv, uint32_t *pN,
		uint32_t SampleRate, uint8_t WordWidth)
{
	uint32_t pClk;
	uint32_t x, y;
	uint64_t divider;
	uint16_t dif;
	uint16_t xDiv = 0, yDiv = 0;
	uint32_t N;
	uint16_t err, ErrorOptimal = 0xFFFF;

	pClk = Chip_Clock_GetRate(CLK_APB1_I2S);

	/* divider is a fixed point number with 16 fractional bits */
	divider = (((uint64_t) (SampleRate) * 2 * (WordWidth) * 2) << 16) / pClk;
	/* find N that make x/y <= 1 -> divider <= 2^16 */
	for (N = 64; N > 0; N--) {
		if ((divider * N) < (1 << 16)) {
			break;
		}
	}
	if (N == 0) {
		return ERROR;
	}
	divider *= N;
	for (y = 255; y > 0; y--) {
		x = y * divider;
		if (x & (0xFF000000)) {
			continue;
		}
		dif = x & 0xFFFF;
		if (dif > 0x8000) {
			err = 0x10000 - dif;
		}
		else {
			err = dif;
		}
		if (err == 0) {
			yDiv = y;
			break;
		}
		else if (err < ErrorOptimal) {
			ErrorOptimal = err;
			yDiv = y;
		}
	}
	xDiv = ((uint64_t) yDiv * (SampleRate) * 2 * (WordWidth) * N * 2) / pClk;
	if (xDiv >= 256) {
		xDiv = 0xFF;
	}
	if (xDiv == 0) {
		xDiv = 1;
	}

	*pxDiv = xDiv;
	*pyDiv = yDiv;
	*pN = N;
	return SUCCESS;
}

STATIC Status getClkDiv2(LPC_I2S_T *pI2S,
		uint16_t *pxDiv, uint16_t *pyDiv, uint32_t *pN,
		uint32_t SampleRate, uint8_t WordWidth)
{
	*pyDiv = 255; //pClk = 204 MHz
	*pxDiv = 192; //make mClk = 76.8 MHz
	/*
	 * NOTICE for Cortex m4:
	 * mClk = pClk/2 * x/y = N * bClk
	 * 0 <= x <= y <= 255
	 */
	*pN = 96000 * 25 / SampleRate; //setup bClk according to sample rate

	assert(*pN >= 25 && *pN <= 250 && WordWidth == 16);

	return SUCCESS;
}

Status Chip_I2S_TxOnOff_jvx(LPC_I2S_T *pI2S, jvxAudioFormat* config, uint16_t on)
{
	uint32_t temp;
	uint16_t xDiv, yDiv;
	uint32_t N;
	uint16_t wwidth = 16;

	temp = pI2S->DAO & (~(I2S_DAO_WORDWIDTH_MASK | I2S_DAO_MONO | I2S_DAO_SLAVE | I2S_DAO_WS_HALFPERIOD_MASK));

	switch(config->form)
	{
	case JVX_DATAFORMAT_8BIT:
		temp |= I2S_WORDWIDTH_8;
		wwidth = 8;
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		temp |= I2S_WORDWIDTH_16;
		wwidth = 16;
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		temp |= I2S_WORDWIDTH_32;
		wwidth = 32;
		break;
	default:
		assert(0);
	}

	if (getClkDiv2(pI2S, &xDiv, &yDiv, &N, config->srate_out, wwidth) == ERROR)
	{
		return ERROR;
	}


	if(on)
	{
		temp |= (config->numchans_out) == 1 ? I2S_MONO : I2S_STEREO;
		temp |= I2S_MASTER_MODE;
		temp |= I2S_DAO_WS_HALFPERIOD(wwidth - 1);
	}
	else
	{
		temp |= (config->numchans_out) == 1 ? I2S_MONO : I2S_STEREO;
		temp |= I2S_SLAVE_MODE;
		temp |= I2S_DAO_WS_HALFPERIOD(wwidth - 1);
	}
	pI2S->DAO = temp;
	pI2S->TXMODE = I2S_TXMODE_CLKSEL(0);
	pI2S->TXBITRATE = N - 1;
	pI2S->TXRATE = yDiv | (xDiv << 8);
	return SUCCESS;
}

Status Chip_I2S_RxOnOff_jvx(LPC_I2S_T *pI2S,  jvxAudioFormat* config, uint16_t on)
{
	uint32_t temp;
	uint16_t xDiv, yDiv;
	uint32_t N;
	uint16_t wwidth = 16;

	temp = pI2S->DAI & (~(I2S_DAI_WORDWIDTH_MASK | I2S_DAI_MONO | I2S_DAI_SLAVE | I2S_DAI_WS_HALFPERIOD_MASK));
	switch(config->form)
	{
	case JVX_DATAFORMAT_8BIT:
		temp |= I2S_WORDWIDTH_8;
		wwidth = 8;
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		temp |= I2S_WORDWIDTH_16;
		wwidth = 16;
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		temp |= I2S_WORDWIDTH_32;
		wwidth = 32;
		break;
	default:
		assert(0);
	}

	if (getClkDiv2(pI2S, &xDiv, &yDiv, &N, config->srate_in, wwidth) == ERROR)
	{
		return ERROR;
	}

	if(on)
	{
		temp |= (config->numchans_in) == 1 ? I2S_MONO : I2S_STEREO;
		temp |= I2S_MASTER_MODE;
		temp |= I2S_DAI_WS_HALFPERIOD(wwidth - 1);
	}
	else
	{
		temp |= (config->numchans_in) == 1 ? I2S_MONO : I2S_STEREO;
		temp |= I2S_SLAVE_MODE;
		temp |= I2S_DAI_WS_HALFPERIOD(wwidth - 1);
	}
	pI2S->DAI = temp;
	pI2S->RXMODE = I2S_RXMODE_CLKSEL(0);
	pI2S->RXBITRATE = N - 1;
	pI2S->RXRATE = yDiv | (xDiv << 8);
	return SUCCESS;
}

Status Chip_I2S_TxRxOnOff_jvx(LPC_I2S_T *pI2S, jvxAudioFormat* config, uint16_t on)
{
	uint32_t tempi, tempo;
	uint16_t wwidth = 16;

	tempi = pI2S->DAI & (~(I2S_DAI_WORDWIDTH_MASK | I2S_DAI_MONO | I2S_DAI_SLAVE | I2S_DAI_WS_HALFPERIOD_MASK));
	tempo = pI2S->DAO & (~(I2S_DAO_WORDWIDTH_MASK | I2S_DAO_MONO | I2S_DAO_SLAVE | I2S_DAO_WS_HALFPERIOD_MASK));

	switch(config->form)
	{
	case JVX_DATAFORMAT_8BIT:
		tempi |= I2S_WORDWIDTH_8;
		tempo |= I2S_WORDWIDTH_8;
		wwidth = 8;
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		tempi |= I2S_WORDWIDTH_16;
		tempo |= I2S_WORDWIDTH_16;
		wwidth = 16;
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		tempi |= I2S_WORDWIDTH_32;
		tempo |= I2S_WORDWIDTH_32;
		wwidth = 32;
		break;
	default:
		assert(0);
	}

	if(on)
	{
		tempi |= (config->numchans_in) == 1 ? I2S_MONO : I2S_STEREO;
		tempi |= I2S_MASTER_MODE;
		tempi |= I2S_DAI_WS_HALFPERIOD(wwidth - 1);
		tempo |= (config->numchans_out) == 1 ? I2S_MONO : I2S_STEREO;
		tempo |= I2S_MASTER_MODE;
		tempo |= I2S_DAI_WS_HALFPERIOD(wwidth - 1);
	}
	else
	{
		tempi |= (config->numchans_in) == 1 ? I2S_MONO : I2S_STEREO;
		tempi |= I2S_SLAVE_MODE;
		tempi |= I2S_DAI_WS_HALFPERIOD(wwidth - 1);
		tempo |= (config->numchans_out) == 1 ? I2S_MONO : I2S_STEREO;
		tempo |= I2S_SLAVE_MODE;
		tempo |= I2S_DAI_WS_HALFPERIOD(wwidth - 1);
	}
	pI2S->DAI = tempi;
	pI2S->DAO = tempo;
	return SUCCESS;
}

void Board_Audio_i2sInit_jvx(LPC_I2S_T *pI2S, jvxAudioFormat* config)
{
	// Pin muxing for audio
	Chip_SCU_PinMuxSet(0x3, 0, (SCU_MODE_FUNC2));
	Chip_SCU_PinMuxSet(0x3, 1, (SCU_MODE_FUNC0));
	Chip_SCU_PinMuxSet(0x3, 2, (SCU_MODE_FUNC0));
	Chip_SCU_PinMuxSet(0x6, 0, (SCU_MODE_FUNC4));
	Chip_SCU_PinMuxSet(0x6, 1, (SCU_MODE_FUNC3));
	Chip_SCU_PinMuxSet(0x6, 2, (SCU_MODE_INBUFF_EN | SCU_MODE_FUNC3));

	// We need to initialize at least i2s to be allowed to talk to UDA 1380
	Chip_I2S_Init(pI2S);

	// Start transmit clocks
	Chip_I2S_TxOnOff_jvx(pI2S, config, 1);


	/* val16 = Chip_SCU_PinMuxGet(0x3, 0);
	val16 = Chip_SCU_PinMuxGet(0x3, 1);
	val16 = Chip_SCU_PinMuxGet(0x3, 2);
	val16 = Chip_SCU_PinMuxGet(0x6, 0);
	val16 = Chip_SCU_PinMuxGet(0x6, 1);
	val16 = Chip_SCU_PinMuxGet(0x6, 2);*/
}



static void delay(uint32_t i) {
	while (i--) {}
}

/* Initialize UDA1380 CODEC */
static Status Board_Audio_CodecInit_jvx(int micIn)
{
	/* Reset UDA1380 on board Hitex A4*/
	/* PE_9: UDA_RST on Hitex A4 */
	Chip_SCU_PinMuxSet(0x0E, 9, (SCU_MODE_PULLUP | SCU_MODE_FUNC4));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 7, 9);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 7, 9, true);
	/* delay 1us */
	delay(100000);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 7, 9, false);
	delay(100000);

	if (!UDA1380_Init(UDA1380_MIC_IN_LR & - (micIn != 0))) {
		return ERROR;
	}

	return SUCCESS;
}

/* Initialize I2S interface for the board and UDA1380 */
void Board_Audio_Init_jvx(LPC_I2S_T *pI2S, int micIn, jvxAudioFormat* conf_audio)
{
	// Initialize I2S on board
	Board_Audio_i2sInit_jvx(pI2S, conf_audio);

	/* Init UDA1380 CODEC */
	while (Board_Audio_CodecInit_jvx(micIn) != SUCCESS) {}

}

// ===================================================================================
// ===================================================================================


/* Optimized Peripheral Source and Destination burst size (18xx,43xx) */
static const uint8_t GPDMA_LUTPerBurst[] = {
	GPDMA_BSIZE_4,	/* MEMORY             */
	GPDMA_BSIZE_1,	/* MAT0.0             */
	GPDMA_BSIZE_1,	/* UART0 Tx           */
	GPDMA_BSIZE_1,	/* MAT0.1             */
	GPDMA_BSIZE_1,	/* UART0 Rx           */
	GPDMA_BSIZE_1,	/* MAT1.0             */
	GPDMA_BSIZE_1,	/* UART1 Tx           */
	GPDMA_BSIZE_1,	/* MAT1.1             */
	GPDMA_BSIZE_1,	/* UART1 Rx           */
	GPDMA_BSIZE_1,	/* MAT2.0             */
	GPDMA_BSIZE_1,	/* UART2 Tx           */
	GPDMA_BSIZE_1,	/* MAT2.1             */
	GPDMA_BSIZE_1,	/* UART2 Rx           */
	GPDMA_BSIZE_1,	/* MAT3.0             */
	GPDMA_BSIZE_1,	/* UART3 Tx           */
	0,				/* SCT timer channel 0*/
	GPDMA_BSIZE_1,	/* MAT3.1             */
	GPDMA_BSIZE_1,	/* UART3 Rx           */
	0,				/* SCT timer channel 1*/
	GPDMA_BSIZE_4,	/* SSP0 Rx            */
	GPDMA_BSIZE_32,	/* I2S channel 0      */
	GPDMA_BSIZE_4,	/* SSP0 Tx            */
	GPDMA_BSIZE_32,	/* I2S channel 1      */
	GPDMA_BSIZE_4,	/* SSP1 Rx            */
	GPDMA_BSIZE_4,	/* SSP1 Tx            */
	GPDMA_BSIZE_4,	/* ADC 0              */
	GPDMA_BSIZE_4,	/* ADC 1              */
	GPDMA_BSIZE_1,	/* DAC                */
	GPDMA_BSIZE_32,	/* I2S channel 0      */
	GPDMA_BSIZE_32	/* I2S channel 0      */
};

/* Optimized Peripheral Source and Destination transfer width (18xx,43xx) */
static const uint8_t GPDMA_LUTPerWid[] = {
	GPDMA_WIDTH_WORD,	/* MEMORY             */
	GPDMA_WIDTH_WORD,	/* MAT0.0             */
	GPDMA_WIDTH_BYTE,	/* UART0 Tx           */
	GPDMA_WIDTH_WORD,	/* MAT0.1             */
	GPDMA_WIDTH_BYTE,	/* UART0 Rx           */
	GPDMA_WIDTH_WORD,	/* MAT1.0             */
	GPDMA_WIDTH_BYTE,	/* UART1 Tx           */
	GPDMA_WIDTH_WORD,	/* MAT1.1             */
	GPDMA_WIDTH_BYTE,	/* UART1 Rx           */
	GPDMA_WIDTH_WORD,	/* MAT2.0             */
	GPDMA_WIDTH_BYTE,	/* UART2 Tx           */
	GPDMA_WIDTH_WORD,	/* MAT2.1             */
	GPDMA_WIDTH_BYTE,	/* UART2 Rx           */
	GPDMA_WIDTH_WORD,	/* MAT3.0             */
	GPDMA_WIDTH_BYTE,	/* UART3 Tx           */
	0,					/* SCT timer channel 0*/
	GPDMA_WIDTH_WORD,	/* MAT3.1             */
	GPDMA_WIDTH_BYTE,	/* UART3 Rx           */
	0,					/* SCT timer channel 1*/
	GPDMA_WIDTH_BYTE,	/* SSP0 Rx            */
	GPDMA_WIDTH_WORD,	/* I2S channel 0      */
	GPDMA_WIDTH_BYTE,	/* SSP0 Tx            */
	GPDMA_WIDTH_WORD,	/* I2S channel 1      */
	GPDMA_WIDTH_BYTE,	/* SSP1 Rx            */
	GPDMA_WIDTH_BYTE,	/* SSP1 Tx            */
	GPDMA_WIDTH_WORD,	/* ADC 0              */
	GPDMA_WIDTH_WORD,	/* ADC 1              */
	GPDMA_WIDTH_WORD,	/* DAC                */
	GPDMA_WIDTH_WORD,	/* I2S channel 0      */
	GPDMA_WIDTH_WORD/* I2S channel 0      */
};

/*****************************************************************************
 * Private functions
 ****************************************************************************/
/* Control which set of peripherals is connected to the DMA controller */
STATIC uint8_t configDMAMux(uint32_t gpdma_peripheral_connection_number)
{
	uint8_t function, channel;

	switch (gpdma_peripheral_connection_number) {
	case GPDMA_CONN_MAT0_0:
		function = 0;
		channel = 1;
		break;

	case GPDMA_CONN_UART0_Tx:
		function = 1;
		channel = 1;
		break;

	case GPDMA_CONN_MAT0_1:
		function = 0;
		channel = 2;
		break;

	case GPDMA_CONN_UART0_Rx:
		function = 1;
		channel = 2;
		break;

	case GPDMA_CONN_MAT1_0:
		function = 0;
		channel = 3;
		break;

	case GPDMA_CONN_UART1_Tx:
		function = 1;
		channel = 3;
		break;

	case GPDMA_CONN_I2S1_Tx_Channel_0:
		function = 2;
		channel = 3;
		break;

	case GPDMA_CONN_MAT1_1:
		function = 0;
		channel = 4;
		break;

	case GPDMA_CONN_UART1_Rx:
		function = 1;
		channel = 4;
		break;

	case GPDMA_CONN_I2S1_Rx_Channel_1:
		function = 2;
		channel =  4;
		break;

	case GPDMA_CONN_MAT2_0:
		function = 0;
		channel = 5;
		break;

	case GPDMA_CONN_UART2_Tx:
		function = 1;
		channel = 5;
		break;

	case GPDMA_CONN_MAT2_1:
		function = 0;
		channel = 6;
		break;

	case GPDMA_CONN_UART2_Rx:
		function = 1;
		channel = 6;
		break;

	case GPDMA_CONN_MAT3_0:
		function = 0;
		channel = 7;
		break;

	case GPDMA_CONN_UART3_Tx:
		function = 1;
		channel = 7;
		break;

	case GPDMA_CONN_SCT_0:
		function = 2;
		channel = 7;
		break;

	case GPDMA_CONN_MAT3_1:
		function = 0;
		channel = 8;
		break;

	case GPDMA_CONN_UART3_Rx:
		function = 1;
		channel = 8;
		break;

	case GPDMA_CONN_SCT_1:
		function = 2;
		channel = 8;
		break;

	case GPDMA_CONN_SSP0_Rx:
		function = 0;
		channel = 9;
		break;

	case GPDMA_CONN_I2S_Tx_Channel_0:
		function = 1;
		channel = 9;
		break;

	case GPDMA_CONN_SSP0_Tx:
		function = 0;
		channel = 10;
		break;

	case GPDMA_CONN_I2S_Rx_Channel_1:
		function = 1;
		channel = 10;
		break;

	case GPDMA_CONN_SSP1_Rx:
		function = 0;
		channel = 11;
		break;

	case GPDMA_CONN_SSP1_Tx:
		function = 0;
		channel = 12;
		break;

	case GPDMA_CONN_ADC_0:
		function = 0;
		channel = 13;
		break;

	case GPDMA_CONN_ADC_1:
		function = 0;
		channel = 14;
		break;

	case GPDMA_CONN_DAC:
		function = 0;
		channel = 15;
		break;

	default:
		function = 3;
		channel = 15;
		break;
	}
	/* Set select function to dmamux register */
	if (0 != gpdma_peripheral_connection_number) {
		uint32_t temp;
		temp = LPC_CREG->DMAMUX & (~(0x03 << (2 * channel)));
		LPC_CREG->DMAMUX = temp | (function << (2 * channel));
	}
	return channel;
}

uint32_t makeCtrlWord(const GPDMA_CH_CFG_T *GPDMAChannelConfig,
					  uint32_t GPDMA_LUTPerBurstSrcConn,
					  uint32_t GPDMA_LUTPerBurstDstConn,
					  uint32_t GPDMA_LUTPerWidSrcConn,
					  uint32_t GPDMA_LUTPerWidDstConn)
{
	uint32_t ctrl_word = 0;

	switch (GPDMAChannelConfig->TransferType) {
	/* Memory to memory */
	case GPDMA_TRANSFERTYPE_M2M_CONTROLLER_DMA:
		ctrl_word = GPDMA_DMACCxControl_TransferSize(GPDMAChannelConfig->TransferSize)
					| GPDMA_DMACCxControl_SBSize((4UL))				/**< Burst size = 32 */
					| GPDMA_DMACCxControl_DBSize((4UL))				/**< Burst size = 32 */
					| GPDMA_DMACCxControl_SWidth(GPDMAChannelConfig->TransferWidth)
					| GPDMA_DMACCxControl_DWidth(GPDMAChannelConfig->TransferWidth)
					| GPDMA_DMACCxControl_SI
					| GPDMA_DMACCxControl_DI
					| GPDMA_DMACCxControl_I;
		break;

	case GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA:
	case GPDMA_TRANSFERTYPE_M2P_CONTROLLER_PERIPHERAL:
		ctrl_word = GPDMA_DMACCxControl_TransferSize((uint32_t) GPDMAChannelConfig->TransferSize)
					| GPDMA_DMACCxControl_SBSize(GPDMA_LUTPerBurstDstConn)
					| GPDMA_DMACCxControl_DBSize(GPDMA_LUTPerBurstDstConn)
					| GPDMA_DMACCxControl_SWidth(GPDMA_LUTPerWidDstConn)
					| GPDMA_DMACCxControl_DWidth(GPDMA_LUTPerWidDstConn)
					| GPDMA_DMACCxControl_DestTransUseAHBMaster1
					| GPDMA_DMACCxControl_SI
					| GPDMA_DMACCxControl_I;
		break;

	case GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA:
	case GPDMA_TRANSFERTYPE_P2M_CONTROLLER_PERIPHERAL:
		ctrl_word = GPDMA_DMACCxControl_TransferSize((uint32_t) GPDMAChannelConfig->TransferSize)
					| GPDMA_DMACCxControl_SBSize(GPDMA_LUTPerBurstSrcConn)
					| GPDMA_DMACCxControl_DBSize(GPDMA_LUTPerBurstSrcConn)
					| GPDMA_DMACCxControl_SWidth(GPDMA_LUTPerWidSrcConn)
					| GPDMA_DMACCxControl_DWidth(GPDMA_LUTPerWidSrcConn)
					| GPDMA_DMACCxControl_SrcTransUseAHBMaster1
					| GPDMA_DMACCxControl_DI
					| GPDMA_DMACCxControl_I;
		break;

	case GPDMA_TRANSFERTYPE_P2P_CONTROLLER_DMA:
	case GPDMA_TRANSFERTYPE_P2P_CONTROLLER_DestPERIPHERAL:
	case GPDMA_TRANSFERTYPE_P2P_CONTROLLER_SrcPERIPHERAL:
		ctrl_word = GPDMA_DMACCxControl_TransferSize((uint32_t) GPDMAChannelConfig->TransferSize)
					| GPDMA_DMACCxControl_SBSize(GPDMA_LUTPerBurstSrcConn)
					| GPDMA_DMACCxControl_DBSize(GPDMA_LUTPerBurstDstConn)
					| GPDMA_DMACCxControl_SWidth(GPDMA_LUTPerWidSrcConn)
					| GPDMA_DMACCxControl_DWidth(GPDMA_LUTPerWidDstConn)
					| GPDMA_DMACCxControl_SrcTransUseAHBMaster1
					| GPDMA_DMACCxControl_DestTransUseAHBMaster1
					| GPDMA_DMACCxControl_I;

		break;

	/* Do not support any more transfer type, return ERROR */
	default:
		return ERROR;
	}
	return ctrl_word;
}

/* Set up the DPDMA according to the specification configuration details */
Status setupChannel(LPC_GPDMA_T *pGPDMA,
					GPDMA_CH_CFG_T *GPDMAChannelConfig,
					uint8_t SrcPeripheral,
					uint8_t DstPeripheral,
					DMA_TransferDescriptor* dma_start)
{
	GPDMA_CH_T *pDMAch;

	if (pGPDMA->ENBLDCHNS & ((((1UL << (GPDMAChannelConfig->ChannelNum)) & 0xFF)))) {
		/* This channel is enabled, return ERROR, need to release this channel first */
		return ERROR;
	}

	/* Get Channel pointer */
	pDMAch = (GPDMA_CH_T *) &(pGPDMA->CH[GPDMAChannelConfig->ChannelNum]);

	/* Reset the Interrupt status */
	pGPDMA->INTTCCLEAR = (((1UL << (GPDMAChannelConfig->ChannelNum)) & 0xFF));
	pGPDMA->INTERRCLR = (((1UL << (GPDMAChannelConfig->ChannelNum)) & 0xFF));


	/* Enable DMA channels, little endian */
	pGPDMA->CONFIG = GPDMA_DMACConfig_E;
	while (!(pGPDMA->CONFIG & GPDMA_DMACConfig_E)) {}

	pDMAch->LLI = dma_start->lli;
	pDMAch->SRCADDR = dma_start->src;
	pDMAch->DESTADDR = dma_start->dst;
	pDMAch->CONTROL = dma_start->ctrl;

	/* Configure DMA Channel, enable Error Counter and Terminate counter */
	pDMAch->CONFIG = GPDMA_DMACCxConfig_IE
					 | GPDMA_DMACCxConfig_ITC		/*| GPDMA_DMACCxConfig_E*/
					 | GPDMA_DMACCxConfig_TransferType((uint32_t) GPDMAChannelConfig->TransferType)
					 | GPDMA_DMACCxConfig_SrcPeripheral(SrcPeripheral)
					 | GPDMA_DMACCxConfig_DestPeripheral(DstPeripheral);



	return SUCCESS;
}




/* Do a DMA transfer M2M, M2P,P2M or P2P */
Status Chip_GPDMA_Transfer_jvx(LPC_GPDMA_T *pGPDMA,
						   uint8_t ChannelNum,
						   uint32_t src,
						   uint32_t dst,
						   GPDMA_FLOW_CONTROL_T TransferType,
						   uint32_t Size,
						   DMA_TransferDescriptor* dma_0,
						   DMA_TransferDescriptor* dma_1)
{
	GPDMA_CH_CFG_T GPDMACfg;
	uint8_t SrcPeripheral = 0, DstPeripheral = 0;
	uint32_t cwrd;
	int ret;

	ret = Chip_GPDMA_InitChannelCfg(pGPDMA, &GPDMACfg, ChannelNum, src, dst, Size, TransferType);
	if (ret < 0) {
		return ERROR;
	}

	/* Adjust src/dst index if they are memory */
	if (ret & 1) {
		src = 0;
	}
	else {
		SrcPeripheral = configDMAMux(src);
		dma_0->src = GPDMACfg.SrcAddr;
		dma_1->src = GPDMACfg.SrcAddr;
	}

	if (ret & 2) {
		dst = 0;
	}
	else {
		DstPeripheral = configDMAMux(dst);
		dma_0->dst = GPDMACfg.DstAddr;
		dma_1->dst = GPDMACfg.DstAddr;;
	}


	cwrd = makeCtrlWord(&GPDMACfg,
						(uint32_t) GPDMA_LUTPerBurst[src],
						(uint32_t) GPDMA_LUTPerBurst[dst],
						(uint32_t) GPDMA_LUTPerWid[src],
						(uint32_t) GPDMA_LUTPerWid[dst]);
	dma_0->ctrl = cwrd;
	dma_1->ctrl = cwrd;

	if (setupChannel(pGPDMA, &GPDMACfg, SrcPeripheral, DstPeripheral, dma_0) == ERROR) {
		return ERROR;
	}

	return SUCCESS;
}

Status Chip_GPDMA_SStart_jvx(LPC_GPDMA_T *pGPDMA, uint8_t chanId_in, uint8_t chanId_out)
{
	/* Start the Channel */
	Chip_GPDMA_ChannelCmd(pGPDMA, chanId_in, ENABLE);
	Chip_GPDMA_ChannelCmd(pGPDMA, chanId_out, ENABLE);
	return SUCCESS;
}

void verifyDma1()
{
	printf("Hallo\r\n");

	/*volatile uint32_t src_add = 0;
	volatile uint32_t dst_add = 0;
	volatile uint32_t cntrl = 0;
*/
	//LPC_GPDMA->CH[0].SRCADDR;
	//LPC_GPDMA->CH[0].DESTADDR;
	//LPC_GPDMA->CH[0].CONTROL;
}

