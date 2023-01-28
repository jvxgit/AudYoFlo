/*
 * uart_printf.cpp
 *
 *  Created on: 09.06.2017
 *      Author: bernd
 */


#include "uart_printf.h"
#include "board.h"

/* =============================================================
 * Redirection of printf to whatever is desired in the newLib
 * In RedLib, the function to be redefined is __sys_write
 * Must be a pure C function as retargetting happens on link level.
 * =============================================================
 */

int _write(int iFileHandle, char *pcBuffer, int iLength)
{
#if defined(DEBUG_ENABLE)
	int i;
	for (i = 0; i < iLength; i++) {
		Board_UARTPutChar(pcBuffer[i]);
	}
#endif

	return iLength;
}

int _read(void)
{
#if defined(DEBUG_ENABLE)
	int c = Board_UARTGetChar();
	return c;

#else
	return (int) -1;
#endif
}
