/*
 *****************************************************
 * Filename: fileWriter.cpp
 *****************************************************
 * Project: RTProc-ESP (Echtzeit-Software-Plattform) *
 *****************************************************
 * Description: 
 *
 *****************************************************
 * Developed by JAVOX SOLUTIONS GMBH, 2012           *
 *****************************************************
 * COPYRIGHT BY JAVOX SOLUTION GMBH                  *
 *****************************************************
 * Contact: rtproc@javox-solutions.com               *
 *****************************************************
*/

#include "jvx.h"
#include "CjvxSimpleDotfuscation.h"

/**
 * Constructor: Set all into uninitialized.
 *///===================================================
CjvxSimpleDotfuscation::CjvxSimpleDotfuscation(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): 
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_CRYPT);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxCrypt*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

}

/**
 * Initialize the file writer module, that is set the size for the buffer.
 *///=======================================================================
jvxErrorType
CjvxSimpleDotfuscation::initialize(IjvxHiddenInterface* theHost)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxObject::_initialize(theHost);
	if(res == JVX_NO_ERROR)
	{
		
	}
	return(res);
}

/**
 * Opposite of the initialize function. From wherever you come, always return
 * into uninitialized state.
 *///========================================================================
jvxErrorType
CjvxSimpleDotfuscation::terminate()
{
	return(JVX_NO_ERROR);
}

jvxErrorType
CjvxSimpleDotfuscation::encrypt(jvxByte* fldIn, jvxSize numElmsIn, jvxByte** byteFldOnReturn, jvxSize* numBytesOnReturn)
{
	jvxSize i;
	jvxUInt32 modVal = (1<<8)-1;

	if(memory.output.size() == 0)
	{
		memory.output = std::string(fldIn, numElmsIn);
		// Very simple modification
		for(i = 0; i < memory.output.size(); i++)
		{
			// out = mod(in+mod(inc,modV), modV)
			jvxUInt32 tt = memory.output[i];
			assert(tt > 0);
			tt = (tt + (i % modVal)) % modVal;
			tt += 1; // <- avoid 
			memory.output[i] = (char)tt;
		}

		memory.output += JVX_SIMPLE_CRYPT_TOKEN;
		*byteFldOnReturn = (jvxByte*)memory.output.c_str();
		*numBytesOnReturn = memory.output.size();
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxSimpleDotfuscation::decrypt(jvxByte* fldIn, jvxSize numElmsIn, jvxByte** byteFldOnReturn, jvxSize* numBytesOnReturn)
{
	jvxSize i;
	std::string endToken;
	std::string lookfor = JVX_SIMPLE_CRYPT_TOKEN;
	jvxUInt32 modVal = (1<<8)-1;

	assert(byteFldOnReturn);
	assert(numBytesOnReturn);
	if(memory.output.size() == 0)
	{
		memory.output = std::string((char*)fldIn, numElmsIn);
		endToken = memory.output.substr(memory.output.size()-lookfor.size(),lookfor.size());
		if(endToken == lookfor)
		{
			// Here, we need to modify the bytes
			for(i = 0; i < memory.output.size(); i++)
			{
				jvxUInt32 tt = (memory.output[i] & 0xFF);
				tt -= 1;
				// in = mod(out + (modV - mod(inc,mod)), modV)
				tt = (tt + (modVal - (i % modVal))) % modVal;
				memory.output[i] = (char) tt;
			}
			memory.output = memory.output.substr(0, memory.output.size() - lookfor.size());
		}

		// If we do not find the token, just return
		*byteFldOnReturn = (jvxByte*)memory.output.c_str();
		*numBytesOnReturn = memory.output.size();
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_WRONG_STATE);
}

jvxErrorType
CjvxSimpleDotfuscation::release(jvxByte* releaseMe)
{
	if(releaseMe == memory.output.c_str())
	{
		memory.output.clear();
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}
