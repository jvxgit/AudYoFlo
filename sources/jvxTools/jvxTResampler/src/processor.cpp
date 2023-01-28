/* 
 *****************************************************
 * Filename: processor.cpp
 *****************************************************
 * Project: RTProc-ESP (Echtzeit-Software-Plattform) *
 *****************************************************
 * Description: This file contains helping functions 
 *				for the fixed point realization of 
 *				functions, tools!
 *****************************************************
 * Developed by JAVOX SOLUTIONS GMBH, 2012           *
 *****************************************************
 * COPYRIGHT BY JAVOX SOLUTION GMBH                  *
 *****************************************************
 * Contact: rtproc@javox-solutions.com               *
 *****************************************************
*/

#include "rtprocExtensions.h"
#include "processor.h"
#include <iostream>

void 
processor::Data_2_Word16(jvxData input, Word16 Q, Word16& output)
{
	jvxData multCoeff = (Word32)1<<Q;
	input *= multCoeff;
	jvxData maxVal = (Word32)1<<15;

	// Apply saturation
	if(input > maxVal)
	{
		std::cout << "SATURATION, CASE1!" << std::endl;
		std::cout << "input:" << input << ", max Val(Q=" << Q << ")=" << maxVal << std::endl;
		input = maxVal;
	}

	if(input < -maxVal)
	{
		std::cout << "SATURATION, CASE2!" << std::endl;
		std::cout << "input:" << input << ", max Val(Q=" << Q << ")=" << maxVal << std::endl;
		input = -maxVal;
	}
	
	if(input > 0)
	{
		input += 0.5;
	}
	else
	{
		input -= 0.5;
	}

	output = (Word16)input;

	// For the maximum positive case: Avoid wrap around!!
	if((input > 0 )&&(output < 0))
	{
		//std::cout << "SATURATION, CASE3!" << std::endl;
		//std::cout << "input:" << input << ", max Val(Q=" << Q << ")=" << maxVal << std::endl;
		output = ((Word32)1<<15)-1;
	}
}

void 
processor::Data_2_Word16_FLD(jvxData* input, Word16 Q, Word16* output, size_t lField)
{
	for(size_t i = 0; i < lField; i++)
	{
		Data_2_Word16(*input++, Q, *output++);
	}
}

void 
processor::Word16_2_Data(Word16 input, Word16 Q, jvxData& output)
{
	jvxData multCoeff = (Word32)1<<Q;
	output = (jvxData)input;
	output /= multCoeff;
}

void 
processor::Word16_2_Data_FLD(Word16* input, Word16 Q, jvxData* output, size_t lField)
{
	for(size_t i = 0; i < lField; i++)
	{
		Word16_2_Data(*input++, Q, *output++);
	}
}

void 
processor::Data_2_Word32(jvxData input, Word32 Q, Word32& output)
{
	jvxData multCoeff = (jvxData)((long long)1<<Q);

	input *= multCoeff;
	jvxData maxVal = (long long)1<<31;


		// Apply saturation
	if(input > maxVal)
	{
		std::cout << "SATURATION, CASE1!" << std::endl;
		std::cout << "input:" << input << ", max Val(Q=" << Q << ")=" << maxVal << std::endl;
		input = maxVal;
	}

	if(input < -maxVal)
	{
		std::cout << "SATURATION, CASE2!" << std::endl;
		std::cout << "input:" << input << ", max Val(Q=" << Q << ")=" << maxVal << std::endl;
		input = -maxVal;
	}
	
	if(input > 0)
	{
		input += 0.5;
	}
	else
	{
		input -= 0.5;
	}

	output = (Word32)input;

	// For the maximum positive case: Avoid wrap around!!
	if((input > 0 )&&(output < 0))
	{
		//std::cout << "SATURATION, CASE3!" << std::endl;
		//std::cout << "input:" << input << ", max Val(Q=" << Q << ")=" << maxVal << std::endl;
		output = ((long long)1<<31)-1;
	}
}

void 
processor::Data_2_Word40(jvxData input, Word16 Q, Word40& output)
{
	jvxData multCoeff = (jvxData)((long long)1<<Q);

	input *= multCoeff;
	jvxData maxVal = (long long)1<<39;


		// Apply saturation
	if(input > maxVal)
	{
		std::cout << "SATURATION, CASE1!" << std::endl;
		std::cout << "input:" << input << ", max Val(Q=" << Q << ")=" << maxVal << std::endl;
		input = maxVal;
	}

	if(input < -maxVal)
	{
		std::cout << "SATURATION, CASE2!" << std::endl;
		std::cout << "input:" << input << ", max Val(Q=" << Q << ")=" << maxVal << std::endl;
		input = -maxVal;
	}
	
	if(input > 0)
	{
		input += 0.5;
	}
	else
	{
		input -= 0.5;
	}

	output = (Word40)input;

	// For the maximum positive case: Avoid wrap around!!
	if((input > 0 )&&(output < 0))
	{
		//std::cout << "SATURATION, CASE3!" << std::endl;
		//std::cout << "input:" << input << ", max Val(Q=" << Q << ")=" << maxVal << std::endl;
		output = ((long long)1<<39)-1;
	}
}

void 
processor::Data_2_Word32_FLD(jvxData* input, Word32 Q, Word32* output, size_t lField)
{
	for(size_t i = 0; i < lField; i++)
	{
		Data_2_Word32(*input++, Q, *output++);
	}
}

void 
processor::Word32_2_Data(Word32 input, Word32 Q, jvxData& output)
{
	jvxData multCoeff = (jvxData)((long long)1<<Q);
	output = (jvxData)input;
	output /= multCoeff;
}

void 
processor::Word32_2_Data_FLD(Word32* input, Word32 Q, jvxData* output, size_t lField)
{
	for(size_t i = 0; i < lField; i++)
	{
		Word32_2_Data(*input++, Q, *output++);
	}
}

void 
processor::printData(Word16 in, Word16 Q)
{
	jvxData print;
	processor::Word16_2_Data(in, Q, print);
	std::cout << "Fixed:" << in << ", in Q" << Q << "=" << print << std::endl;
}
	
void 
processor::printData(Word16* in, Word16 Q, size_t numCoeffs)
{
	jvxData print;
	for(size_t i = 0; i < numCoeffs; i++)
	{
		processor::Word16_2_Data(in[i], Q, print);
		std::cout << "Fixed[" << i << "]:" << in[i] << ", in Q" << Q << "=" << print << std::endl;
	}
}

void 
processor::printData(Word32 in, Word16 Q)
{
	jvxData print;
	processor::Word32_2_Data(in, Q, print);
	std::cout << "Fixed:" << in << ", in Q" << Q << "=" << print << std::endl;
}
	
void 
processor::printData(Word32* in, Word16 Q, size_t numCoeffs)
{
	jvxData print;
	for(size_t i = 0; i < numCoeffs; i++)
	{
		processor::Word32_2_Data(in[i], Q, print);
		std::cout << "Fixed[" << i << "]:" << in[i] << ", in Q" << Q << "=" << print << std::endl;
	}
}

void 
processor::printData(jvxData val)
{
	std::cout << "Float:" << "=" << val << std::endl;
}

void 
processor::printData(jvxData* vals, size_t numCoeffs)
{
	for(size_t i = 0; i < numCoeffs; i++)
	{
		std::cout << "Float[" << i << "]:" << "=" << vals[i] << std::endl;
	}
}
