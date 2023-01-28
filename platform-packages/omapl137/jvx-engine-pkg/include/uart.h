/*
 * ==============================================
 * Filename: uart.h
 * ==============================================
 * Purpose: Uart API specification of function level.
 *			COPYRIGHT HK, Javox, 2011
 */
 
#ifndef _UART_H__
#define _UART_H__

// Initialize the send process based on the uart low level functions


// Initialize UART module
void uart_initialize();

// Initialize the UART memory
void uart_initializeMemory();

// Receiver buffer is locked until explicitely freed by high level module
void uart_unlockReceiverBuffer();

#endif
void uart_initializeTransfer(jvxByte* buf, jvxSize numBytes);
