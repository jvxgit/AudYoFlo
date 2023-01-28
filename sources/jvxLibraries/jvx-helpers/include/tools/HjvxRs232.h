#ifndef __HJVXRS232_H__
#define __HJVXRS232_H__

void jvx_rs232_initRs232Default(jvxRs232Config* cfg);

jvxSize jvx_rs232_numberBytesWrapSend(jvxByte* fld, jvxSize szFld, jvxByte byteStart, jvxByte byteStop, jvxByte byteEsc);

void jvx_rs232_insertByteWrapReplace(jvxByte* copyForm, jvxSize szFldFrom, jvxByte* copyTo, jvxSize szFldTo, jvxByte byteStart, 
			jvxByte byteStop, jvxByte byteEsc, jvxSize& written);

			jvxSize jvx_rs232_numberBytesUnwrapReceive(jvxByte* fld, jvxSize offset_read, jvxSize offset_write, jvxByte byteStart, jvxByte byteStop, jvxByte byteEsc, jvxByte& theChar);

void jvx_rs232_removeByteWrapReplace(jvxByte* copyForm, jvxSize offset_from_read, jvxSize  offset_from_write,
	jvxByte* copyTo, jvxSize szFldTo,
	jvxByte byteStart, jvxByte byteStop, jvxByte byteEsc,
	jvxSize & read, jvxSize & write, jvxByte& theStopChar);
#endif