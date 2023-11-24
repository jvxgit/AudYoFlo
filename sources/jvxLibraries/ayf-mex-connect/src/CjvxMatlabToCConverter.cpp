#include "CjvxMatlabToCConverter.h"
#include "localMexIncludes.h"

jvxErrorType 
CjvxMatlabToCConverter::mexArgument2Bool(bool& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (idx < numEntries)
	{
		if (thePointer[idx])
		{
			if (mxIsLogical(thePointer[idx]))
			{
				value = *((bool*)mxGetData(thePointer[idx]));
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return(res);
}

jvxErrorType 
CjvxMatlabToCConverter::mexArgument2String(std::string& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxErrorType res = JVX_NO_ERROR;

	value = "";

	if (idx < numEntries)
	{
		if (thePointer[idx])
		{
			if (mxIsChar(thePointer[idx]))
			{
				value = CjvxMatlabToCConverter::jvx_mex_2_cstring(thePointer[idx]);
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return(res);
}

jvxErrorType 
CjvxMatlabToCConverter::mexArgument2ComponentIdentification(jvxComponentIdentification& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string cpName = "unknown";
	jvxInt32 valI = -1;
	std::string nm;

	value.reset();

	res = mexArgument2String(nm, thePointer, idx, numEntries);
	if (res == JVX_NO_ERROR)
	{
		res = jvxComponentIdentification_decode(value, nm);
	}
	else
	{
		res = mexArgument2Type<jvxComponentType>(value.tp, thePointer, idx, numEntries, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
	}
	return(res);
}

jvxErrorType 
CjvxMatlabToCConverter::mexArgument2String(std::vector<std::string>& valueList, jvxSize expectedNumberEntries, const mxArray** thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	const mxArray* arr = NULL;
	valueList.clear();
	if (idx < numEntries)
	{
		if (thePointer[idx])
		{
			if (mxIsCell(thePointer[idx]))
			{
				jvxSize numEntries = mxGetNumberOfElements(thePointer[idx]);
				if (expectedNumberEntries == numEntries)
				{
					for (i = 0; i < numEntries; i++)
					{
						arr = mxGetCell(thePointer[idx], JVX_SIZE_2_MAT_IDX(i));
						if (arr)
						{
							if (mxIsChar(arr))
							{
								valueList.push_back(CjvxMatlabToCConverter::jvx_mex_2_cstring(arr));
							}
							else
							{
								res = JVX_ERROR_INVALID_ARGUMENT;
							}
						}
						else
						{
							res = JVX_ERROR_INVALID_ARGUMENT;
						}
					}
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return(res);
}

jvxErrorType 
CjvxMatlabToCConverter::mexArgument2StringList(std::vector<std::string>& valueList, const mxArray** thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxSize i;
	std::string oneToken;
	jvxErrorType res = JVX_NO_ERROR;
	const mxArray* arr = NULL;
	valueList.clear();
	if (idx < numEntries)
	{
		if (thePointer[idx])
		{
			if (mxIsCell(thePointer[idx]))
			{
				jvxSize numEntries = mxGetNumberOfElements(thePointer[idx]);
				for (i = 0; i < numEntries; i++)
				{
					arr = mxGetCell(thePointer[idx], JVX_SIZE_2_MAT_IDX(i));
					if (arr)
					{
						if (mxIsChar(arr))
						{
							valueList.push_back(CjvxMatlabToCConverter::jvx_mex_2_cstring(arr));
						}
						else
						{
							res = JVX_ERROR_INVALID_ARGUMENT;
						}
					}
					else
					{
						res = JVX_ERROR_INVALID_ARGUMENT;
					}
				}
			}
			else if (mxIsChar(thePointer[idx]))
			{
				// There may also be just one string
				oneToken = CjvxMatlabToCConverter::jvx_mex_2_cstring(thePointer[idx]);
				valueList.push_back(oneToken);
			}
			else if (mxIsEmpty(thePointer[idx]))
			{
				// No entry may also happen
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return(res);
}

jvxErrorType 
CjvxMatlabToCConverter::mexArgument2Data(jvxData& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries)
{
	jvxErrorType res = JVX_NO_ERROR;
	value = 0.0;

	if (idx < numEntries)
	{
		if (thePointer[idx])
		{
			if (mxIsDouble(thePointer[idx]))
			{
				value = (jvxData)(*((double*)mxGetData(thePointer[idx])));
			}
			else if (mxIsSingle(thePointer[idx]))
			{
				value = (jvxData)(*((float*)mxGetData(thePointer[idx])));
			}
			else if (mxIsInt64(thePointer[idx]))
			{
				value = (jvxData)(*((jvxInt64*)mxGetData(thePointer[idx])));
			}
			else if (mxIsInt32(thePointer[idx]))
			{
				value = (jvxData)(*((jvxInt32*)mxGetData(thePointer[idx])));
			}
			else if (mxIsInt16(thePointer[idx]))
			{
				value = (jvxData)(*((jvxInt16*)mxGetData(thePointer[idx])));
			}
			else if (mxIsInt8(thePointer[idx]))
			{
				value = (jvxData)(*((jvxInt8*)mxGetData(thePointer[idx])));
			}
			else if (mxIsUint64(thePointer[idx]))
			{
				value = (jvxData)(*((jvxUInt64*)mxGetData(thePointer[idx])));
			}
			else if (mxIsUint32(thePointer[idx]))
			{
				value = (jvxData)(*((jvxUInt32*)mxGetData(thePointer[idx])));
			}
			else if (mxIsUint16(thePointer[idx]))
			{
				value = (jvxData)(*((jvxUInt16*)mxGetData(thePointer[idx])));
			}
			else if (mxIsUint8(thePointer[idx]))
			{
				value = (jvxData)(*((jvxUInt8*)mxGetData(thePointer[idx])));
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		res = JVX_ERROR_ID_OUT_OF_BOUNDS;
	}
	return(res);
}

std::string 
CjvxMatlabToCConverter::jvx_mex_2_cstring(const mxArray* phs)
{
	jvxSize bufLen = mxGetM(phs)*mxGetN(phs)*sizeof(char)+1;
	char* buf = new char[bufLen];
	mxGetString(phs, buf, JVX_SIZE_INT(bufLen));
	std::string str = buf;
	delete[](buf);
	return(str);
}

std::vector<jvxValue> 
CjvxMatlabToCConverter::jvx_mex_2_numeric(const mxArray* phs, jvxSize lineNo)
{
	std::vector<jvxValue> lstOnReturn;
	if (mxIsNumeric(phs))
	{
		jvxSize M = mxGetM(phs);
		jvxSize N = mxGetN(phs);
		jvxSize i = 0;
		lstOnReturn.resize(N);

		if (mxIsDouble(phs))
		{
			double* ptr = (double*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsSingle(phs))
		{
			float* ptr = (float*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsInt64(phs))
		{
			jvxInt64* ptr = (jvxInt64*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsInt32(phs))
		{
			jvxInt32* ptr = (jvxInt32*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsInt16(phs))
		{
			jvxInt16* ptr = (jvxInt16*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsUint64(phs))
		{
			jvxUInt64* ptr = (jvxUInt64*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsUint32(phs))
		{
			jvxUInt32* ptr = (jvxUInt32*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsUint16(phs))
		{
			jvxUInt16* ptr = (jvxUInt16*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsInt8(phs))
		{
			jvxInt8* ptr = (jvxInt8*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else if (mxIsUint8(phs))
		{
			jvxUInt8* ptr = (jvxUInt8*)mxGetData(phs);
			for (i = 0; i < N; i++)
			{
				//lstOnReturn[i].assign(ptr[lineNo * M +i])
				lstOnReturn[i].assign(ptr[i * N + lineNo]);
			}
		}
		else
		{
		}
	}
	return lstOnReturn;
}

const mxArray* 
CjvxMatlabToCConverter::jvx_mex_lookup_strfield_core(const mxArray* strEntry, const std::string& key, jvxSize curLevel, int levelMax, jvxBool& moreLevels)
{
	jvxSize i;
	const mxArray* retVal = nullptr;
	jvxBool foundThisLevel = false;
	int num = mxGetNumberOfFields(strEntry);
	for (i = 0; i < num; i++)
	{
		const char* nm = mxGetFieldNameByNumber(strEntry, i);
		if ((std::string)nm == key)
		{
			retVal = mxGetField(strEntry, 0, key.c_str());
			foundThisLevel = true;
			break;
		}
	}

	if (!foundThisLevel)
	{
		moreLevels = false;
		for (i = 0; i < num; i++)
		{
			const mxArray* localFld = mxGetFieldByNumber(strEntry, 0, i);
			if (localFld)
			{
				if (mxIsStruct(localFld))
				{
					jvxBool moreLevelsLoc = true;
					if (curLevel < levelMax)
					{
						retVal = jvx_mex_lookup_strfield_core(localFld, key, curLevel + 1, levelMax, moreLevelsLoc);
						if (retVal != nullptr)
						{
							break;
						}
					}
					if (moreLevelsLoc)
					{
						moreLevels = true;
					}
				}
			}
		}
	}
	return retVal;
}

const mxArray* 
CjvxMatlabToCConverter::jvx_mex_lookup_strfield(const mxArray* strEntry, const std::string& key, jvxSize levelMax)
{
	const mxArray* retVal = nullptr;
	jvxBool moreLevels = false;
	jvxSize levelMaxCur = 0;
	do
	{
		retVal = jvx_mex_lookup_strfield_core(strEntry, key, 0, levelMax, moreLevels);
		if (retVal != nullptr)
		{
			break;
		}
		if (!moreLevels)
		{
			break;
		}
		levelMaxCur++;

	} while (levelMaxCur < levelMax);
	return retVal;
}

const mxArray* 
CjvxMatlabToCConverter::jvx_mex_read_single_reference(const mxArray* fld, const std::string& expr)
{
	jvxSize i;
	std::vector<std::string> tokens;
	if (jvx::helper::parseStringListIntoTokens(expr, tokens, '/') == JVX_NO_ERROR)
	{
		const mxArray* curPtr = fld;
		for (i = 0; i < tokens.size(); i++)
		{
			jvxBool moreLevel = false;
			curPtr = jvx_mex_lookup_strfield_core(curPtr, tokens[i], 0, 0, moreLevel);
			if (curPtr == nullptr)
			{
				return nullptr;
			}
			if (i < tokens.size() - 1)
			{
				if (!mxIsStruct(curPtr))
				{
					return nullptr;
				}
			}
			else
			{
				return curPtr;
			}
		}
	}
	return nullptr;
}
