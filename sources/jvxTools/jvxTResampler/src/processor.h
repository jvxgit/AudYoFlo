/* 
 *****************************************************
 * Filename: processor.h
 *****************************************************
 * Project: RTProc-ESP (Echtzeit-Software-Plattform) *
 *****************************************************
 * Description:                                      *
 *****************************************************
 * Developed by JAVOX SOLUTIONS GMBH, 2012           *
 *****************************************************
 * COPYRIGHT BY JAVOX SOLUTION GMBH                  *
 *****************************************************
 * Contact: rtproc@javox-solutions.com               *
 *****************************************************
*/

#ifndef _PROCESSOR_H__
#define _PROCESSOR_H__

#include "typedef.h"

class processor
{
public:

	static void Data_2_Word16_FLD(jvxData* input, Word16 Q, Word16* output, size_t lField);
	static void Word16_2_Data_FLD(Word16* input, Word16 Q, jvxData* output, size_t lField);

	static void Data_2_Word16(jvxData input, Word16 Q, Word16& output);
	static void Word16_2_Data(Word16 input, Word16 Q, jvxData& output);

	static void Data_2_Word32_FLD(jvxData* input, Word32 Q, Word32* output, size_t lField);
	static void Word32_2_Data_FLD(Word32* input, Word32 Q, jvxData* output, size_t lField);

	static void Data_2_Word32(jvxData input, Word32 Q, Word32& output);
	static void Word32_2_Data(Word32 input, Word32 Q, jvxData& output);

	static void Data_2_Word40(jvxData input, Word16 Q, Word40& output);

	static void printData(Word16 in, Word16 Q);
	static void printData(Word16* in, Word16 Q, size_t numCoeffs);

	static void printData(Word32 in, Word16 Q);
	static void printData(Word32* in, Word16 Q, size_t numCoeffs);

	static void printData(jvxData val);
	static void printData(jvxData* vals, size_t numCoeffs);
};

#endif