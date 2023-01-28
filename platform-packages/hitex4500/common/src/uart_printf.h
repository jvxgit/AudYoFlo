/*
 * uart_printf.h
 *
 *  Created on: 09.06.2017
 *      Author: bernd
 */

#ifndef UART_PRINTF_H_
#define UART_PRINTF_H_

#define DEBUG_ENABLE

#ifdef __cplusplus
extern "C"
{
#endif

	int _write(int iFileHandle, char *pcBuffer, int iLength);
	int _read(void);

#ifdef __cplusplus
}
#endif

#endif /* UART_PRINTF_H_ */
