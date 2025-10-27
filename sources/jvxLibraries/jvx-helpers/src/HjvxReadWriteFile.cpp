#include "jvx-helpers.h"

jvxErrorType
jvx_writeContentToFile(std::string fName,
		       const std::string& writeContentStr,
		       IjvxCrypt* theCryptor)
{
	FILE* fPtr = NULL;
	size_t val = 0;
	jvxByte* ptrContent = (jvxByte*)writeContentStr.c_str();
	jvxSize numWrite = writeContentStr.size();
	jvxErrorType res = JVX_NO_ERROR;

	JVX_FOPEN(fPtr, fName.c_str(),"wb");
	if(!fPtr)
	{
		return(JVX_ERROR_OPEN_FILE_FAILED);
	}

	if(theCryptor)
	{
		// Replace the byte field involvng encryption
		res = theCryptor->encrypt(ptrContent, numWrite, &ptrContent, &numWrite);
		if(res != JVX_NO_ERROR)
		{
			return(JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
		}
		assert(ptrContent);
	}

	//writeContent = writeContent + "\n";
	val =	fwrite(ptrContent, sizeof(char),numWrite , fPtr);

	if(theCryptor)
	{
		theCryptor->release(ptrContent);
		ptrContent = NULL;
	}

	JVX_FCLOSE(fPtr, fName.c_str());

	if(val != numWrite)
	{
		return(JVX_ERROR_UNEXPECTED);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
jvx_readContentFromFile(std::string fName, std::string& storeContent, IjvxCrypt* theCryptor)
{
	char c;
	FILE* fPtr = NULL;
	jvxByte* fldOnReturn = NULL;
	jvxSize numElms = 0;
	jvxErrorType res = JVX_NO_ERROR;
	int cnt = 0;

	JVX_FOPEN(fPtr, fName.c_str(),"rb");

	if(!fPtr)
	{
		return(JVX_ERROR_OPEN_FILE_FAILED);
	}
	while(1)
	{
		size_t val = 0;
		val =	fread(&c, sizeof(char), 1, fPtr);
		if(val == 1)
		{
			storeContent += c;
		}
		else
		{
			break;
		}
		cnt ++;
	}
	JVX_FCLOSE(fPtr, fName.c_str());

	if(theCryptor)
	{
		res = theCryptor->decrypt((jvxByte*)storeContent.c_str(), storeContent.size(), &fldOnReturn, &numElms);
		if(res != JVX_NO_ERROR)
		{
			return(JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
		}
		storeContent = std::string(fldOnReturn, numElms);
		theCryptor->release(fldOnReturn);
	}

	return(JVX_NO_ERROR);
}
