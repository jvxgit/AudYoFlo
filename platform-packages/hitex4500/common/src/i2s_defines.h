/*
 * i2defines.h
 *
 *  Created on: 08.06.2017
 *      Author: bernd
 */

#ifndef I2S_DEFINES_H_
#define I2S_DEFINES_H_


#ifndef BOARD_NXP_LPCXPRESSO_4337
#define CODEC_I2S_BUS   LPC_I2S0
#define I2S_IRQ         I2S0_IRQn
#define I2S_IRQHandler  I2S0_IRQHandler
#define CODEC_INPUT_DEVICE UDA1380_LINE_IN
#define I2S_DMA_TX_CHAN GPDMA_CONN_I2S_Tx_Channel_0
#define I2S_DMA_RX_CHAN GPDMA_CONN_I2S_Rx_Channel_1
#else
#define CODEC_I2S_BUS   LPC_I2S1
#define I2S_IRQ         I2S1_IRQn
#define I2S_IRQHandler  I2S1_IRQHandler
#define CODEC_INPUT_DEVICE CODEC_LINE_IN
#define I2S_DMA_TX_CHAN GPDMA_CONN_I2S1_Tx_Channel_0
#define I2S_DMA_RX_CHAN GPDMA_CONN_I2S1_Rx_Channel_1
#define I2S_SLAVE_RX
#define I2S_SLAVE_TX
#undef DEBUG_ENABLE
#endif



#endif /* I2S_DEFINES_H_ */
