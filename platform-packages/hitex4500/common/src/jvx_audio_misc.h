#ifndef __JVX_AUDIO_MISC_H__
#define __JVX_AUDIO_MISC_H__

#include "jvx_small.h"

typedef struct
{
	jvxSize srate_in;
	jvxSize srate_out;
	jvxSize bsize_out;
	jvxSize bsize_in;
	jvxSize numchans_in;
	jvxSize numchans_out;
	jvxDataFormat form;
} jvxAudioFormat;


void Board_Audio_Init_jvx(LPC_I2S_T *pI2S, int micIn, jvxAudioFormat* config);

Status Chip_GPDMA_Transfer_jvx(LPC_GPDMA_T *pGPDMA,
						   uint8_t ChannelNum,
						   uint32_t src,
						   uint32_t dst,
						   GPDMA_FLOW_CONTROL_T TransferType,
						   uint32_t Size, DMA_TransferDescriptor* dma_0,
						   DMA_TransferDescriptor* dma_1);

Status Chip_GPDMA_SStart_jvx(LPC_GPDMA_T *pGPDMA, uint8_t chanId_in, uint8_t chanId_out);

Status Chip_I2S_TxOnOff_jvx(LPC_I2S_T *pI2S, jvxAudioFormat* config, uint16_t on);
Status Chip_I2S_RxOnOff_jvx(LPC_I2S_T *pI2S, jvxAudioFormat* config, uint16_t on);
Status Chip_I2S_TxRxOnOff_jvx(LPC_I2S_T *pI2S, jvxAudioFormat* config, uint16_t on);

void verifyDma1();

#endif
