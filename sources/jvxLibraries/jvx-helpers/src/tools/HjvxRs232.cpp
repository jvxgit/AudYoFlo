#include "jvx-helpers.h"

void jvx_rs232_initRs232Default(jvxRs232Config* cfg)
{
	cfg->bRate = 38400;
	cfg->bits4Byte = 8;
	cfg->stopBitsEnum = JVX_RS232_STOPBITS_ONE;
	cfg->parityEnum = JVX_RS232_PARITY_NO;
	cfg->enFlow = JVX_RS232_NO_FLOWCONTROL;
	cfg->boostPrio = false;

    cfg->secureChannel.escByte = 0;
    cfg->secureChannel.startByte = 0;
    cfg->secureChannel.stopByte = 0;
    cfg->secureChannel.receive.active = c_false;
    cfg->secureChannel.receive.maxPreReadSize = 0;
    cfg->secureChannel.transmit.active = c_false;
    cfg->secureChannel.transmit.preallocationSize = 0;
}

jvxSize 
jvx_rs232_numberBytesWrapSend(jvxByte* fld, jvxSize szFld, jvxByte byteStart, jvxByte byteStop, jvxByte byteEsc)
{
	jvxSize i;
	jvxSize numBytes = 2; // Start and stop
	for (i = 0; i < szFld; i++)
	{
		if (
			(fld[i] == byteStart) ||
			(fld[i] == byteStop) ||
			(fld[i] == byteEsc))
		{
			numBytes ++;
		}
	}
	numBytes += szFld;
	return numBytes;
}


void 
jvx_rs232_insertByteWrapReplace(jvxByte* copyForm, jvxSize szFldFrom, jvxByte* copyTo, jvxSize szFldTo, jvxByte byteStart, 
			jvxByte byteStop, jvxByte byteEsc, jvxSize& written)
{
	jvxSize i;
	jvxByte* ptrw = copyTo;
	
	written = 0;
	jvxByte in; 

	*ptrw = byteStart;
	ptrw++;
	written++;

	for (i = 0; i < szFldFrom; i++)
	{
		in = copyForm[i];
		if (
			(in == byteStart) ||
			(in == byteStop) ||
			(in == byteEsc))
		{
			*ptrw = byteEsc;
			ptrw++;
			written++;
		}
		*ptrw = in;
		ptrw++;
		written++;
	}
	*ptrw = byteStop;
	ptrw++;
	written++;
}

jvxSize jvx_rs232_numberBytesUnwrapReceive(jvxByte* fld, jvxSize offset_read, jvxSize offset_write, jvxByte byteStart, jvxByte byteStop, jvxByte byteEsc, jvxByte& theChar)
{
	jvxSize i;
	jvxSize numRet = 0;
	jvxBool precededByEsc = false;
	for (i = offset_read; i < offset_write; i++)
	{
		jvxByte in = fld[i];
		if (in == byteEsc)
		{
			if (precededByEsc)
			{
				numRet++;
				precededByEsc = false;
			}
			else
			{
				precededByEsc = true;
			}
		}
		else if (
			(in == byteStart) ||
			(in == byteStop))
		{
			if (precededByEsc)
			{
				numRet++;
				precededByEsc = false;
			}
			else
			{
				theChar = in;
				break;
			}
		}
		else
		{
			numRet++;
		}
	}
	return(numRet);
}

void 
jvx_rs232_removeByteWrapReplace(jvxByte* copyForm, jvxSize offset_from_read, jvxSize  offset_from_write,
	jvxByte* copyTo, jvxSize szFldTo,
	jvxByte byteStart, jvxByte byteStop, jvxByte byteEsc,
	jvxSize & read, jvxSize & write, jvxByte& theStopChar)
{
	jvxSize i;
	theStopChar = 0;
	read = 0;
	write = 0;
	jvxBool precededByEsc = false;
	for (i = offset_from_read; i < offset_from_write; i++)
	{
		jvxByte in = copyForm[i];
		read++;
		if (in == byteEsc)
		{
			if (precededByEsc)
			{
				copyTo[write] = in;
				write++;
				precededByEsc = false;
			}
			else
			{
				if (i == offset_from_write - 1)
				{
					// Esc at end to be rejected
					read--;
					break;
				}
				else
				{
					precededByEsc = true;
				}
			}
		}
		else if (
			(in == byteStart) ||
			(in == byteStop))
		{
			if (precededByEsc)
			{
				copyTo[write] = in;
				write++;
				precededByEsc = false;
			}
			else
			{
				theStopChar = in;
				break;
			}
		}
		else
		{
			copyTo[write] = in;
			write++;
		}
	}
}
