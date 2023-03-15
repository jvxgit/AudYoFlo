#include "jvx-helpers.h"

#include "common/CjvxObject.h"

#define JVX_ONE_BRACE_LIST_NUM 4
// #define #ifdef JVX_ADD_DEFAULT_SEQUENCE_OLD

typedef struct
{
	char start;
	char stop;
}jvxOneBrace;

static jvxOneBrace braces[JVX_ONE_BRACE_LIST_NUM] =
{
	{'(', ')' },
	{'{', '}' },
	{'<', '>' },
	{'[', ']' }
};

static char findBraceInList(char inBrace)
{
	char retc = 0;
	jvxSize i;
	for (i = 0; i < JVX_ONE_BRACE_LIST_NUM; i++)
	{
		if (inBrace == braces[i].start)
		{
			retc = braces[i].stop;
			break;
		}
	}
	return retc;
}

// ============================================================================
// ============================================================================

jvxDataFormat JVX_DATAFORMAT_FROM_POINTER(jvxInt32* ptr)
{
	return JVX_DATAFORMAT_32BIT_LE;
}

jvxDataFormat JVX_DATAFORMAT_FROM_POINTER(jvxInt16* ptr)
{
	return JVX_DATAFORMAT_16BIT_LE;
}

jvxDataFormat JVX_DATAFORMAT_FROM_POINTER(jvxInt8* ptr)
{
	return JVX_DATAFORMAT_8BIT;
}

jvxDataFormat JVX_DATAFORMAT_FROM_POINTER(jvxInt64* ptr)
{
	return JVX_DATAFORMAT_64BIT_LE;
}

jvxDataFormat JVX_DATAFORMAT_FROM_POINTER(jvxData* ptr)
{
	return JVX_DATAFORMAT_DATA;
}

jvxDataFormat JVX_DATAFORMAT_FROM_POINTER(jvxSize* ptr)
{
	return JVX_DATAFORMAT_SIZE;
}

jvxDataFormat JVX_DATAFORMAT_FROM_POINTER(jvxCBool* ptr)
{
	return JVX_DATAFORMAT_U16BIT_LE;
}

// ===========================================================

#ifdef JVX_PROPERTY_ACCESS_CHECK_VERBOSE 

bool jvx_check_property_access_ok(jvxErrorType res, jvxAccessProtocol accProt, const std::string& context, IjvxProperties* props, const std::string& loc, jvxBool acceptFail)
{
	bool result = (res == JVX_NO_ERROR);
	// Invalid is also a valid return value!
	result = result &&
		((accProt == JVX_ACCESS_PROTOCOL_OK) ||
			(accProt == JVX_ACCESS_PROTOCOL_INVALID));


	if(result == false)
	{
		if (!acceptFail)
		{
			std::string obj_descr;
			if (props)
			{
				IjvxObject* obj = NULL;
				props->request_reference_object(&obj);

				if (obj)
				{
					jvxApiString astr;
					obj->description(&astr);
					obj_descr = astr.std_str();
				}
				props->return_reference_object(obj);
			}

#ifdef JVX_PROPERTY_ACCESS_CHECK_VERBOSE_LOCATION 
			std::cout << "::" << loc << "-> " << std::flush;
#endif
			std::cout << "At location " << loc << ": access property error, object: <" << obj_descr << ">, property : <" << context << ">, reason : < " << jvxErrorType_descr(res)
				<< ">" << std::flush;
			
			std::cout << ", protocol: <" << jvxAccessProtocol_txt(accProt) << ">" << std::flush;
			std::cout << "." << std::endl;
		}
	}
	return(result);
}

bool jvx_check_property_access_ok(jvxErrorType res, jvxAccessProtocol accProt, const std::string& context, IjvxAccessProperties* ac_prop, const std::string& loc, jvxBool acceptFail)
{
	bool result = (res == JVX_NO_ERROR);
	// Invalid is also a valid return value!
	result = result &&
		((accProt == JVX_ACCESS_PROTOCOL_OK) ||
			(accProt == JVX_ACCESS_PROTOCOL_INVALID));

	if (result == false)
	{
		if (!acceptFail)
		{
			IjvxObject* obj = NULL;
			std::string obj_descr;
			if (ac_prop)
			{
				jvxApiString astr;
				ac_prop->description_object(&astr);
				obj_descr = astr.std_str();
			}
#ifdef JVX_PROPERTY_ACCESS_CHECK_VERBOSE_LOCATION 
			std::cout << "::" << loc << "-> " << std::flush;
#endif
			std::cout << "At location " << loc << ": access property error, object: <" << obj_descr << ">, property: <" << context << ">, reason: <" <<
				jvxErrorType_descr(res) << ">, protocol: <" << std::flush;

			std::cout << ", protocol: <" << jvxAccessProtocol_txt(accProt) << ">" << std::flush;
			std::cout << "." << std::endl;
		}
	}
	return(result);
}

bool jvx_check_property_access_ok(jvxErrorType res, jvxAccessProtocol accProt, const std::string& context, const std::string& compId, const std::string& loc, jvxBool acceptFail)
{
	bool result = (res == JVX_NO_ERROR);
	
		// Invalid is also a valid return value!
		result = result &&
			((accProt == JVX_ACCESS_PROTOCOL_OK) ||
			(accProt == JVX_ACCESS_PROTOCOL_INVALID));
	

	if (result == false)
	{
		if (!acceptFail)
		{
#ifdef JVX_PROPERTY_ACCESS_CHECK_VERBOSE_LOCATION
			std::cout << "::" << loc << "-> " << std::flush;
#endif
			std::cout << "At location " << loc << ": access property error, object: <" << compId << ">, property: <" << context << ">, reason: <" <<
				jvxErrorType_descr(res) << ">" << std::flush;
			if (accProt)
				std::cout << ", protocol: <" << jvxAccessProtocol_txt(accProt) << ">" << std::flush;
			std::cout << "." << std::endl;
		}
	}
	return(result);
}

#endif

bool jvx_check_property_access_ok(jvxErrorType res, jvxAccessProtocol accProt)
{
	bool result = (res == JVX_NO_ERROR);
		result = result && (accProt == JVX_ACCESS_PROTOCOL_OK);
	return(result);
}


/*
bool jvx_check_property_access_of(jvxErrorType res, jvxAccessProtocol accProt, const std::string& context)
{
	bool result = (res == JVX_NO_ERROR) && (accProt == JVX_ACCESS_PROTOCOL_OK);
	if (result == false)
	{
		std::cout << "Access Property Error, context: <" << context << ">, reason: <" << jvxErrorType_descr(res) << ">, protocol: <" << jvxAccessProtocol_txt(accProt) << ">." << std::endl;
	}
	return(result);
}


bool JVX_CHECK_PROPERTY_ACCESS_OK(jvxErrorType res, jvxAccessProtocol* accProt)
{
	if (accProt)
	{
		return((res == JVX_NO_ERROR) && (*accProt == JVX_ACCESS_PROTOCOL_OK));
	}
	return(res == JVX_NO_ERROR);
}
*/
// ===========================================================

std::string jvx_int2String(int val)
{
	char str[JVX_MAXSTRING] = { 0 };
#if _MSC_VER >= 1600
	sprintf_s(str, JVX_MAXSTRING, "%d", val);
#else
	sprintf(str, "%d", val);
#endif
	return(str);
}

std::string jvx_uint2String(unsigned int val)
{
	char str[JVX_MAXSTRING] = { 0 };
#if _MSC_VER >= 1600
	sprintf_s(str, JVX_MAXSTRING, "%u", val);
#else
	sprintf(str, "%u", val);
#endif
	return(str);
}

std::string jvx_int322String(jvxInt32 val)
{
	char str[JVX_MAXSTRING] = { 0 };
#if _MSC_VER >= 1600
	sprintf_s(str, JVX_MAXSTRING, JVX_PRINTF_CAST_INT32, val);
#else
	sprintf(str, JVX_PRINTF_CAST_INT32, val);
#endif
	return(str);
}

std::string jvx_int162String(jvxInt16 val)
{
	char str[JVX_MAXSTRING] = { 0 };
#if _MSC_VER >= 1600
	sprintf_s(str, JVX_MAXSTRING, JVX_PRINTF_CAST_INT16, val);
#else
	sprintf(str, JVX_PRINTF_CAST_INT16, val);
#endif
	return(str);
}
std::string jvx_uint322String(jvxUInt32 val)
{
	char str[JVX_MAXSTRING] = { 0 };
#if _MSC_VER >= 1600
	sprintf_s(str, JVX_MAXSTRING, JVX_PRINTF_CAST_UINT32, val);
#else
	sprintf(str, JVX_PRINTF_CAST_UINT32, val);
#endif
	return(str);
}

std::string jvx_uint162String(jvxUInt16 val)
{
	char str[JVX_MAXSTRING] = { 0 };
#if _MSC_VER >= 1600
	sprintf_s(str, JVX_MAXSTRING, JVX_PRINTF_CAST_UINT16, val);
#else
	sprintf(str, JVX_PRINTF_CAST_UINT16, val);
#endif
	return(str);
}

std::string
jvx_intx2String(int val)
{
	char str[JVX_MAXSTRING] = { 0 };
#if _MSC_VER >= 1600
	sprintf_s(str, JVX_MAXSTRING, "%x", val);
#else
	sprintf(str, "%x", val);
#endif
	return(str);
}
 
std::string
jvx_size2String(jvxSize val, jvxSize modeGen)
{
	char str[JVX_MAXSTRING] = { 0 };

	if (val >= JVX_SIZE_LIMIT)
	{
		int idx = (int)-val;
		assert(idx < JVX_SIZE_NUM_EXTRA_CASES_WITH_0);
		switch (modeGen)
		{
		case 1:
			return jvxSizeSpecialValues[idx].full;
		case 2:
			return jvxSizeSpecialValues[idx].friendly;
		default:
			return(jvx_int2String(-idx));
		}
	}

	/*
	if (val == JVX_SIZE_DONTCARE)
	{
		switch (modeGen)
		{
		case 1:
			return("JVX_SIZE_DONTCARE");
		case 2:
			return "dontcare";
		default:
			return("-2");
		}
	}

	if (val == JVX_SIZE_SLOT_RETAIN)
	{
		switch (modeGen)
		{
		case 1:
			return("JVX_SIZE_SLOT_RETAIN");
		case 2:
			return "retain";
		default:
			return("-3");
		}
	}

	if (val == JVX_SIZE_SLOT_OFF_SYSTEM)
	{
		switch (modeGen)
		{
		case 1:
			return("JVX_SIZE_SLOT_OFF_SYSTEM");
		case 2:
			return "off";
		default:
			return("-4");
		}
	}

	if (val == JVX_SIZE_STATE_INCREMENT)
	{
		switch (modeGen)
		{
		case 1:
			return("JVX_SIZE_STATE_INCREMENT");
		case 2:
			return "incs";
		default:
			return("-5");
		}
	}

	if (val == JVX_SIZE_STATE_DECREMENT)
	{
		switch (modeGen)
		{
		case 1:
			return("JVX_SIZE_STATE_DECREMENT");
		case 2:
			return "decs";
		default:
			return("-6");
		}
	}
	*/

#if _MSC_VER >= 1900 
	sprintf_s(str, JVX_MAXSTRING, "%zu", val);
#else

	if (sizeof(jvxSize) == 8)
	{
#if _MSC_VER >= 1600
		sprintf_s(str, JVX_MAXSTRING, JVX_PRINTF_CAST_SIZE, val);
#else
		sprintf(str, JVX_PRINTF_CAST_INT64, (jvxInt64)val);
#endif
	}
	else
	{
#if _MSC_VER >= 1600
		sprintf_s(str, JVX_MAXSTRING, "%lu", val);
#else
		sprintf(str, "%lu", val);
#endif
	}
#endif
	return(str);
}

std::string
jvx_int642String(jvxInt64 val)
{
	char str[JVX_MAXSTRING] = { 0 };
#if _MSC_VER >= 1600
	sprintf_s(str, JVX_MAXSTRING, JVX_PRINTF_CAST_INT64, val);
#else
	sprintf(str, JVX_PRINTF_CAST_INT64, val);
#endif
	return(str);
}
//#endif

std::string
jvx_uint642String(jvxUInt64 val)
{
	char str[JVX_MAXSTRING] = { 0 };
#if _MSC_VER >= 1600
	sprintf_s(str, JVX_MAXSTRING, JVX_PRINTF_CAST_UINT64, val);
#else
	sprintf(str, JVX_PRINTF_CAST_UINT64, val);
#endif
	return(str);
}

std::string
jvx_uint642Hexstring(jvxUInt64 val)
{
	jvxSize i;
	std::string collect;
	jvxUInt16 valUI16 = 0;
    char str[5] = { 0 };

	for (i = 0; i < 4; i++)
	{
		valUI16 = val & 0xFFFF;
		val = val >> 16;
#if _MSC_VER >= 1600
        sprintf_s(str, 5, "%04x", valUI16);
#else
		sprintf(str, "%04x", valUI16);
#endif
		collect = str +collect;
	}
	collect = "0x" + collect;
	return(collect);
}

std::string
jvx_uint322Hexstring(jvxUInt32 val)
{
	jvxSize i;
	std::string collect;
	jvxUInt16 valUI16 = 0;
	char str[5] = { 0 }; // 4 + 1 : do not forget 0 at end

	for (i = 0; i < 2; i++)
	{
		valUI16 = val & 0xFFFF;
		val = val >> 16;
#if _MSC_VER >= 1600
		sprintf_s(str, 5, "%04x", valUI16);
#else
		sprintf(str, "%04x", valUI16);
#endif
		collect = str + collect;
	}

	collect = "0x" + collect;
	return(collect);
}

std::string
jvx_uint162Hexstring(jvxUInt16 valUI16)
{
	std::string collect;
	char str[5] = { 0 }; // 4 + 1 : do not forget 0 at end

#if _MSC_VER >= 1600
	sprintf_s(str, 5, "%04x", valUI16);
#else
	sprintf(str, "%04x", valUI16);
#endif

	collect = "0x";
	collect += str;
	return(collect);
}
std::string
jvx_data2String(jvxData convert)
{
	char str[JVX_MAXSTRING] = { 0 };

	if (JVX_ISINF(convert))
	{
		jvx_fillCharStr(str, JVX_MAXSTRING, "inf");
	}
	else if (JVX_ISNEGINF(convert))
	{
		jvx_fillCharStr(str, JVX_MAXSTRING, "-inf");
	}
	else
	{
#if _MSC_VER >= 1600
		sprintf_s(str, JVX_MAXSTRING, "%g", convert);
#else
		sprintf(str, "%g", convert);
#endif
	}
	return(str);
}

/**
 * Function to convert a jvxData value into a string.
 *///=======================================================================
#define JVX_DATA_2_STRING_MAX_VALUE 1e10
#define JVX_DATA_2_STRING_MIN_VALUE -1e10

std::string
jvx_data2String(jvxData convert, int digits)
{
	char str[JVX_MAXSTRING] = { 0 };

	if (JVX_ISINF(convert))
	{
		jvx_fillCharStr(str, JVX_MAXSTRING, "inf");
	}
	if (JVX_ISNEGINF(convert))
	{
		jvx_fillCharStr(str, JVX_MAXSTRING, "-inf");
	}
	else
	{
		if (convert > JVX_DATA_2_STRING_MAX_VALUE)
		{
			jvx_fillCharStr(str, JVX_MAXSTRING, "+lim");
		}
		else
		{
			if (convert < JVX_DATA_2_STRING_MIN_VALUE)
			{
				jvx_fillCharStr(str, JVX_MAXSTRING, "-lim");
			}
			else
			{
				std::string formStr;
				if (digits >= 0)
				{
					formStr = "%." + jvx_int2String(digits) + "f";
				}
				else if (digits == JVX_DATA_2STRING_CONST_FORMAT_E)
				{
					// special rule
					formStr = "%e";
				}
				else if (digits == JVX_DATA_2STRING_CONST_FORMAT_G)
				{
					// special rule
					formStr = "%g";
				}
				else
				{
					int numDigits = JVX_DATA2INT32(JVX_DBL_2_DATA(floor(log10(JVX_ABS(convert) + 1e-10))));
					numDigits = -numDigits + NUM_DIGITS_RELATION;
					numDigits = JVX_MAX(numDigits, 0);
					numDigits = JVX_MIN(numDigits, -digits);

					formStr = "%." + jvx_int2String(numDigits) + "f";
				}

#if _MSC_VER >= 1600
				sprintf_s(str, JVX_MAXSTRING, formStr.c_str(), convert);
#else
				sprintf(str, formStr.c_str(), convert);
#endif
			}
		}
	}
	return(str);
}

std::string
jvx_data2String_highPrecision(jvxData convert, jvxSize digits)
{
	char str[JVX_MAXSTRING] = { 0 };
	int cf = std::fpclassify(convert);
	switch(cf)
	{
	case FP_INFINITE:
		if (convert > 0)
		{
			jvx_fillCharStr(str, JVX_MAXSTRING, "inf");
		}
		else
		{
			jvx_fillCharStr(str, JVX_MAXSTRING, "-inf");
		}
		break;
	case FP_NAN:
		if (convert > 0)
		{
			jvx_fillCharStr(str, JVX_MAXSTRING, "nan");
		}
		else
		{
			jvx_fillCharStr(str, JVX_MAXSTRING, "-nan");
		}
		break;
	default:

/*
			if (JVX_ISINF(convert))
	{
		jvx_fillCharStr(str, JVX_MAXSTRING, "inf");
	}
	else if (JVX_ISNEGINF(convert))
	{
		jvx_fillCharStr(str, JVX_MAXSTRING, "-inf");
	}
	else if (JVX_ISNAN(convert))
	{
		jvx_fillCharStr(str, JVX_MAXSTRING, "nan");
	}
	else if (JVX_ISNEGNAN(convert))
	{
		jvx_fillCharStr(str, JVX_MAXSTRING, "-nan");
	}
	else
	{
*/		
		std::string formStr = "%e";

		if (JVX_CHECK_SIZE_SELECTED(digits))
		{
			formStr = "%." + jvx_size2String(digits) + "e";
		}

#if _MSC_VER >= 1600
		sprintf_s(str, JVX_MAXSTRING, formStr.c_str(), convert);
#else
		sprintf(str, formStr.c_str(), convert);
#endif
		break;
	}
	return(str);
}

std::string
jvx_produceFilenameDateClock(const std::string& prefix)
{
	std::string fNameOut = prefix;
	fNameOut += JVX_DATE();
	fNameOut += "__";
	fNameOut += JVX_TIME();
	fNameOut = jvx_replaceCharacter(fNameOut, '/', '_');
	fNameOut = jvx_replaceCharacter(fNameOut, ':', '_');
	return fNameOut;
}
// =========================================================================0

void jvx_static_lock(jvxHandle* priv)
{
	JVX_MUTEX_HANDLE* theHdl = reinterpret_cast<JVX_MUTEX_HANDLE*>(priv);
	assert(theHdl);
	JVX_LOCK_MUTEX(*theHdl);
}

void jvx_static_unlock(jvxHandle* priv)
{
	JVX_MUTEX_HANDLE* theHdl = reinterpret_cast<JVX_MUTEX_HANDLE*>(priv);
	assert(theHdl);
	JVX_UNLOCK_MUTEX(*theHdl);
}

jvxErrorType jvx_static_try_lock(jvxHandle* priv)
{
	JVX_MUTEX_HANDLE* theHdl = reinterpret_cast<JVX_MUTEX_HANDLE*>(priv);
	assert(theHdl);
	JVX_TRY_LOCK_MUTEX_RESULT_TYPE res = JVX_TRY_LOCK_MUTEX_SUCCESS;
	JVX_TRY_LOCK_MUTEX(res, *theHdl);
	if (res == JVX_TRY_LOCK_MUTEX_SUCCESS)
	{
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_COMPONENT_BUSY);
}


std::string jvx_valueList2String(jvxHandle* ptrVal, jvxDataFormat format, jvxSize num, jvxSize numDigits, const std::string& sep)
{
	std::string ret;
	jvxSize i;
	jvxData* ptrData = NULL;
	jvxInt16* ptrI16 = NULL;
	jvxInt32* ptrI32 = NULL;
	jvxInt64* ptrI64 = NULL;
	jvxInt8* ptrI8 = NULL;
	jvxUInt16* ptrUI16 = NULL;
	jvxUInt32* ptrUI32 = NULL;
	jvxUInt64* ptrUI64 = NULL;
	jvxUInt8* ptrUI8 = NULL;
	jvxSize* ptrS = NULL;

	switch (format)
	{
	case JVX_DATAFORMAT_DATA:
		ptrData = (jvxData*)ptrVal;
		ret = "[";
		for (i = 0; i < num; i++)
		{
			if (i != 0)
			{
				ret += sep;
			}

			// Convert to int by simple typecast, no check. SIZE Values of 
			// JVX_DATA_2STRING_CONST_FORMAT_G, JVX_DATA_2STRING_CONST_FORMAT_E
			// and another format which I do not understand anymore :-)
			ret += jvx_data2String(ptrData[i], (int)numDigits);
		}
		ret += "]";
		break;

	case JVX_DATAFORMAT_16BIT_LE:
		ptrI16 = (jvxInt16*)ptrVal;
		ret = "[";
		for (i = 0; i < num; i++)
		{
			if (i != 0)
			{
				ret += sep;
			}
			ret += jvx_int2String((int)ptrI16[i]);
		}
		ret += "]";
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		ptrI32 = (jvxInt32*)ptrVal;
		ret = "[";
		for (i = 0; i < num; i++)
		{
			if (i != 0)
			{
				ret += sep;
			}
			ret += jvx_int322String((int)ptrI32[i]);
		}
		ret += "]";
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		ptrI64 = (jvxInt64*)ptrVal;
		ret = "[";
		for (i = 0; i < num; i++)
		{
			if (i != 0)
			{
				ret += sep;
			}
			ret += jvx_int642String(ptrI64[i]);
		}
		ret += "]";
		break;
	case JVX_DATAFORMAT_8BIT:
		ptrI8 = (jvxInt8*)ptrVal;
		ret = "[";
		for (i = 0; i < num; i++)
		{
			if (i != 0)
			{
				ret += sep;
			}
			ret += jvx_int2String(ptrI8[i]);
		}
		ret += "]";
		break;

	case JVX_DATAFORMAT_U16BIT_LE:
		ptrUI16 = (jvxUInt16*)ptrVal;
		ret = "[";
		for (i = 0; i < num; i++)
		{
			if (i != 0)
			{
				ret += sep;
			}
			ret += jvx_uint322Hexstring((jvxUInt32)ptrUI16[i]);
		}
		ret += "]";
		break;
	case JVX_DATAFORMAT_U32BIT_LE:
		ptrUI32 = (jvxUInt32*)ptrVal;
		ret = "[";
		for (i = 0; i < num; i++)
		{
			if (i != 0)
			{
				ret += sep;
			}
			ret += jvx_uint322Hexstring(ptrUI32[i]);
		}
		ret += "]";
		break;
	case JVX_DATAFORMAT_U64BIT_LE:
		ptrUI64 = (jvxUInt64*)ptrVal;
		ret = "[";
		for (i = 0; i < num; i++)
		{
			if (i != 0)
			{
				ret += sep;
			}
			ret += jvx_uint642Hexstring(ptrUI64[i]);
		}
		ret += "]";
		break;
	case JVX_DATAFORMAT_U8BIT:
		ptrUI8 = (jvxUInt8*)ptrVal;
		ret = "[";
		for (i = 0; i < num; i++)
		{
			if (i != 0)
			{
				ret += sep;
			}
			ret += jvx_uint322Hexstring((jvxUInt32)ptrUI8[i]);
		}
		ret += "]";
		break;

	case JVX_DATAFORMAT_SIZE:
		ptrS = (jvxSize*)ptrVal;
		ret = "[";
		for (i = 0; i < num; i++)
		{
			if (i != 0)
			{
				ret += sep;
			}
			ret += jvx_size2String(ptrS[i]);
		}
		ret += "]";
		break;
	}
	return ret;
}

std::string jvx_valueList2BinString(jvxHandle* ptrVal, jvxDataFormat format, jvxSize num, std::string& retValPP)
{
	std::string ret;
	char* ptrChar = (char*)ptrVal;
	jvxSize numBytes = jvxDataFormat_getsize(format) * num;
	ret = "<";
	ret += jvxDataFormat_txt(format);
	ret += ":";
	ret += jvx_size2String(jvxDataFormat_getsize(format)); 
	ret += ":";
	ret += jvx_size2String(num);
	ret +=">";
	retValPP = base64_encode((const unsigned char *)ptrChar, numBytes);
	return ret;
}

bool jvx_extractBase64FromExpression(const std::string& in, std::string& procStr, jvxDataFormat& formout, jvxSize& bytesout, jvxSize& numout)
{
	bool err = false;
	size_t posi = std::string::npos;
	std::string token;
	posi = in.find('<');
	if (posi != std::string::npos)
	{
		procStr = in.substr(posi + 1, std::string::npos);
	}
	else
	{
		err = true;
	}
	if (!err)
	{
		posi = procStr.find(':');
		if (posi != std::string::npos)
		{
			token = procStr.substr(0, posi);
			procStr = procStr.substr(posi + 1, std::string::npos);
			formout = jvxDataFormat_decode(token.c_str());
		}
		else
		{
			err = true;
		}
	}
	if (!err)
	{
		posi = procStr.find(':');
		if (posi != std::string::npos)
		{
			token = procStr.substr(0, posi);
			procStr = procStr.substr(posi + 1, std::string::npos);
			bytesout = atoi(token.c_str());
		}
		else
		{
			err = true;
		}
	}
	if (!err)
	{
		posi = procStr.find('>');
		if (posi != std::string::npos)
		{
			token = procStr.substr(0, posi);
			procStr = procStr.substr(posi + 1, std::string::npos);
			numout = atoi(token.c_str());
		}
		else
		{
			err = true;
		}
	}
	
	if (!err)
	{
		posi = procStr.rfind('>');
		if (posi != std::string::npos)
		{
			procStr = procStr.substr(0, posi);
		}
		else
		{
			err = true;
		}
	}
	if (!err)
	{
		posi = procStr.rfind('<');
		if (posi != std::string::npos)
		{
			procStr = procStr.substr(0, posi);
		}
		else
		{
			err = true;
		}
	}

	return err;
}

jvxErrorType jvx_binString2ValueListAlloc(const std::string in, jvxHandle** fld_alloc, jvxSize* num_entries, jvxDataFormat* format)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxDataFormat formout = JVX_DATAFORMAT_NONE;
	jvxSize numout = 0;
	jvxSize bytesout = 0;
	std::string procStr;
	bool err = false;

	err = jvx_extractBase64FromExpression(in,
		procStr, formout, bytesout, numout);

	if (!err)
	{
		jvxSize numBytesToApp = numout * bytesout; // Decoded bytes, not characters in base64 string
		jvxByte* fld = nullptr;
		if (fld_alloc)
		{
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(fld, jvxByte, numBytesToApp);
			size_t numBytesOut = numBytesToApp;
			b64decode(procStr, (unsigned char*)fld, &numBytesOut);
			*fld_alloc = fld;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_ARGUMENT;
	}
	return JVX_ERROR_PARSE_ERROR;
}

jvxErrorType jvx_binString2ValueListDeallocate(jvxHandle* fldAllocated)
{
	JVX_DSP_SAFE_DELETE_FIELD_TYPE(fldAllocated, jvxByte);
	return JVX_NO_ERROR;
}

// ====================================================================================================

jvxErrorType jvx_binString2ValueList(const std::string& in, const std::string& inPP, jvxHandle* ptrOut, jvxDataFormat format, jvxSize num, jvxSize* requiredBytes, jvxSize* numValuesCopied)
{
	std::string ret;
	
	jvxSize numBytesFld = jvxDataFormat_getsize(format) * num;
	std::string procStr;
	bool err = false;
	
	jvxDataFormat formout = JVX_DATAFORMAT_NONE;
	jvxSize numout = 0;
	jvxSize bytesout = 0;
	
	err = jvx_extractBase64FromExpression(in,
		procStr, formout, bytesout, numout);

	if (!err)
	{
		jvxSize numBytesToApp = numout * bytesout; // Decoded bytes, not characters in base64 string
		if (requiredBytes)
		{
			*requiredBytes = numBytesToApp;
		}

		if (ptrOut)
		{
			if (numBytesToApp <= numBytesFld)
			{
				size_t numBytesOut = numBytesToApp;
				if (inPP.empty())
				{
					b64decode(procStr, (unsigned char*)ptrOut, &numBytesOut);
				}
				else
				{
					b64decode(inPP, (unsigned char*)ptrOut, &numBytesOut);
				}
				if (numValuesCopied)
					*numValuesCopied = numout;
				return JVX_NO_ERROR;
			}
			return JVX_ERROR_SIZE_MISMATCH;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_PARSE_ERROR;
}

void jvx_check_value(jvxInt64 val, jvxDataFormat form)
{
	jvxInt64 maxVal = ((jvxInt64)1 << (jvxDataFormat_size[form]*8 - 1));
	jvxInt64 minVal = -((jvxInt64)1 << (jvxDataFormat_size[form]*8 - 1));

	if (val > maxVal)
	{
		std::cout << "Maximum value exceded, <" << val << "> is larger than maximum value of <" << maxVal << "> for datatype " << jvxDataFormat_txt(form) << std::endl;
	}
	if (val < minVal)
	{
		std::cout << "Minimum value exceded, <" << val << "> is lower than minimum value of <" << minVal << "> for datatype " << jvxDataFormat_txt(form) << std::endl;
	}
}

void jvx_check_value(jvxUInt64 val, jvxDataFormat form)
{
	jvxUInt64 maxVal = ((jvxUInt64)1 << (jvxDataFormat_size[form]*8));

	if (val > maxVal)
	{
		std::cout << "Maximum value exceded, <" << val << "> is larger than maximum value of <" << maxVal << "> for datatype " << jvxDataFormat_txt(form) << std::endl;
	}
}

jvxErrorType
jvx_add_default_sequence_sequencer(IjvxSequencer* theSequencer,
	bool onlyIfNoSequence,
	jvxOneSequencerStepDefinition* defaultSteps_run,
	jvxSize numSteps_run,
	jvxOneSequencerStepDefinition* defaultSteps_leave,
	jvxSize numSteps_leave,
	const std::string& nmDefaultSequence,
	const std::string& labelDefaultSequence)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize numSeq = 0;
	jvxSize numStepsRun = 0;
	jvxSize numStepsLeave = 0;
	jvxSize i, k;
	jvxSize idSeq = 0;
	jvxSequencerQueueType seqType = JVX_SEQUENCER_QUEUE_TYPE_RUN;
	jvxSequencerElementType tpElm = JVX_SEQUENCER_TYPE_COMMAND_COMPONENT_PREPARE;
	jvxComponentIdentification tpComp(JVX_COMPONENT_AUDIO_NODE, 0, 0);
	std::string descr = "description";
	jvxSize funcId = JVX_SIZE_UNSELECTED;
	jvxInt64 timeout = 10000;
	std::string label_step;
	std::string label_cond1;
	std::string label_cond2;
	jvxSize idSeqActivate = JVX_SIZE_UNSELECTED;
	jvxBool foundDefaultWithinDefinedSequences = false;
	jvxBool isDefault = false;

	theSequencer->number_sequences(&numSeq);

	// jump out if there is already one sequence which may e.g. be taken from config file
	if (!
		((numSeq > 0) && onlyIfNoSequence))
	{
#if JVX_ADD_DEFAULT_SEQUENCE_OLD
		// Deactivate all sequences
		for (i = 0; i < numSeq; i++)
		{
			theSequencer->mark_sequence_process(i, false);
		}
#endif
		jvxBool wantToAddSequence = true;

		// Check if there is a sequence to realize default sequencing
		for (i = 0; i < numSeq; i++)
		{
			jvxApiString nmSequenceList;
			theSequencer->description_sequence(i, &nmSequenceList, NULL, &numStepsRun, &numStepsLeave, NULL, &isDefault);
			if (nmSequenceList.std_str() == nmDefaultSequence)
			{

				std::cout << "A default sequence with name <" << nmSequenceList.std_str() << "> already exists." << std::flush;

#ifdef JVX_ADD_DEFAULT_SEQUENCE_OLD
				std::cout << "Testing content in the next step." << std::endl;
				if (
					(numStepsRun == numSteps_run) &&
					(numStepsLeave == numSteps_leave))
				{
					jvxBool runQueueMatches = true;
					jvxBool leaveQueueMatches = true;

					for (k = 0; k < numSteps_run; k++)
					{
						theSequencer->description_step_sequence(i, k, JVX_SEQUENCER_QUEUE_TYPE_RUN, NULL,
							&tpElm, &tpComp, NULL, NULL, NULL, NULL, NULL);
						if (!
							(defaultSteps_run[k].tpElm == tpElm) &&
							(defaultSteps_run[k].tpComp == tpComp.tp) &&
							(defaultSteps_run[k].slotid == tpComp.slotid) &&
							(defaultSteps_run[k].slotsubid == tpComp.slotsubid))
						{
							runQueueMatches = false;
							break;
						}
					}
					for (k = 0; k < numSteps_leave; k++)
					{
						theSequencer->description_step_sequence(i, k, JVX_SEQUENCER_QUEUE_TYPE_LEAVE,
							NULL, &tpElm, &tpComp, NULL, NULL, NULL, NULL, NULL);
						if (!
							(defaultSteps_leave[k].tpElm == tpElm) &&
							(defaultSteps_leave[k].tpComp == tpComp.tp) &&
							(defaultSteps_leave[k].slotid == tpComp.slotid) &&
							(defaultSteps_leave[k].slotsubid == tpComp.slotsubid))
						{
							leaveQueueMatches = false;
							break;
						}
					}

					if (runQueueMatches && leaveQueueMatches)
					{
						std::cout << "On adding default sequence: a match was found in sequencer list taken from config file, therefore, no default sequence will be added." << std::endl;
						wantToAddSequence = false;
						idSeqActivate = i;
						break;
					}
					else
					{
						std::cout << "Default sequence deviates from sequence read from config file." << std::endl;
					}
				}
#else
				std::cout << " Default sequence will not be added." << std::endl;
				wantToAddSequence = false;
#endif
				
			}
		}

		if (!wantToAddSequence)
		{
#ifdef JVX_ADD_DEFAULT_SEQUENCE_OLD
			assert(JVX_CHECK_SIZE_SELECTED(idSeqActivate));
			std::cout << "Setting the existing default sequence <" << nmDefaultSequence << "> to active." << std::endl;
			theSequencer->mark_sequence_process(idSeqActivate, true);
#endif
		}
		else
		{
			// If no instance of the default sequence is present, add the appropriate default steps
			std::cout << "Adding the default sequence <" << nmDefaultSequence << "> to list of sequences." << std::endl;
			resL = theSequencer->add_sequence(nmDefaultSequence.c_str(), labelDefaultSequence.c_str());
			if (res == JVX_NO_ERROR)
			{
				for (k = 0; k < numSteps_run; k++)
				{
					jvxComponentIdentification tpC = defaultSteps_run[k].tpComp;
					tpC.slotid = defaultSteps_run[k].slotid;
					tpC.slotsubid = defaultSteps_run[k].slotsubid;

					res = theSequencer->insert_step_sequence_at(numSeq, JVX_SEQUENCER_QUEUE_TYPE_RUN, defaultSteps_run[k].tpElm,
						tpC, defaultSteps_run[k].descr.c_str(), defaultSteps_run[k].funcId, defaultSteps_run[k].timeout, defaultSteps_run[k].label_step.c_str(),
						defaultSteps_run[k].label_cond1.c_str(), defaultSteps_run[k].label_cond2.c_str(), defaultSteps_run[k].break_action, 
						defaultSteps_run[k].mode_association);
					if (resL != JVX_NO_ERROR)
					{
						std::cout << "Failed to insert sequencer run queue step <" << defaultSteps_run[k].descr << ">, reason: " <<
							jvxErrorType_txt(resL) << std::endl;
					}
				}
				for (k = 0; k < numSteps_leave; k++)
				{
					jvxComponentIdentification tpC = defaultSteps_leave[k].tpComp;
					tpC.slotid = defaultSteps_leave[k].slotid;
					tpC.slotsubid = defaultSteps_leave[k].slotsubid;
					resL = theSequencer->insert_step_sequence_at(numSeq, JVX_SEQUENCER_QUEUE_TYPE_LEAVE, defaultSteps_leave[k].tpElm,
						tpC, defaultSteps_leave[k].descr.c_str(), defaultSteps_leave[k].funcId, defaultSteps_leave[k].timeout, defaultSteps_leave[k].label_step.c_str(),
						defaultSteps_leave[k].label_cond1.c_str(), defaultSteps_leave[k].label_cond2.c_str(), false, JVX_SIZE_UNSELECTED);
					if (resL != JVX_NO_ERROR)
					{
						std::cout << "Failed to insert sequencer leave queue step <" << defaultSteps_leave[k].descr << ">, reason: " <<
							jvxErrorType_txt(resL) << std::endl;
					}
				}

				// std::cout << "Setting default sequence <" << nmDefaultSequence << "> to active and marking it as default (will not be saved to config file)." << std::endl;

				theSequencer->mark_sequence_process(numSeq, true);
				theSequencer->set_sequence_default(numSeq); // Highlight as the default chain
			}
			else
			{
				std::cout << JVX_OUTPUT_REPORT_DEBUG_LOCATION << "Failed to add default sequencer definition <" << nmDefaultSequence << ">, reason: " << jvxErrorType_txt(resL) << std::endl;
			}
		}
	}
#ifdef JVX_ADD_DEFAULT_SEQUENCE_OLD
	else
	{
		std::cout << "No default sequence is added since there are sequences already." << std::endl;
		res = JVX_ERROR_ALREADY_IN_USE;
	}
#endif
	return res;
}

void jvx_select_next_avail_slot(IjvxHost* host, jvxComponentType tp, jvxComponentIdentification* tps)
{
	jvxComponentIdentification cpId(tp);
	jvxSize numSlots = 0;
	jvxSize numSubSlots = 0;
	jvxSize numSlotsMax = 0;
	jvxSize numSubSlotsMax = 0;
	jvxComponentType parentTp = JVX_COMPONENT_UNKNOWN;
	jvxComponentType childTp = JVX_COMPONENT_UNKNOWN;
	jvxSize i;
	jvxState stat;
	jvxErrorType res = JVX_NO_ERROR;

	host->role_component_system(tp, &parentTp, &childTp, nullptr);
	host->number_slots_component_system(tp, &numSlots, &numSubSlots, &numSlotsMax, &numSubSlotsMax);
	if (parentTp == JVX_COMPONENT_UNKNOWN)
	{
		jvxComponentIdentification tpCheck(tp);
		tps[tp].slotid = JVX_SIZE_UNSELECTED;
		for (i = 0; i < numSlotsMax; i++)
		{
			stat = JVX_STATE_NONE;
			tpCheck.slotid = i;
			res = host->state_selected_component(tpCheck, &stat);
			if (stat == JVX_STATE_NONE)
			{
				tps[tp].slotid = i;
				break;
			}
		}
		if (childTp != JVX_COMPONENT_UNKNOWN)
		{
			tps[childTp].slotid = tps[tp].slotid;
		}
	}
	else
	{
		jvxComponentIdentification tpCheck(tp);
		tps[tp].slotsubid = JVX_SIZE_UNSELECTED;
		for (i = 0; i < numSubSlotsMax; i++)
		{
			stat = JVX_STATE_NONE;
			tpCheck.slotsubid = i;
			res = host->state_selected_component(tpCheck, &stat);
			if (stat == JVX_STATE_NONE)
			{
				tps[tp].slotsubid = i;
				break;
			}
		}
	}
}

jvxErrorType
jvx_activateComponentAsOwnerDevice(
	IjvxHost* theHostHdl,
	jvxComponentType tpTech,
	const std::string& cpTag,
	IjvxObject*& theObjHdl,
	jvxComponentIdentification& theObjId, 
	IjvxObject* thisisme)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxSize num = 0;
	jvxSize szSlots = 0;
	jvxComponentIdentification cpTp = tpTech;
	theHostHdl->number_slots_component_system(cpTp, &szSlots, nullptr, nullptr, nullptr);
	jvxSize i;
	for (i = 0; i < szSlots; i++)
	{
		jvxApiString astrsub;
		cpTp.slotid = i;
		theHostHdl->descriptor_selected_component(cpTp, NULL, &astrsub);
		if (astrsub.std_str() == cpTag)
		{
			// Found the slotid, next activate the next available device
			cpTp.tp = (jvxComponentType)(cpTp.tp + 1);
			cpTp.slotsubid = JVX_SIZE_DONTCARE; // <- indicates: reserve a new slot!
			res = theHostHdl->select_component(cpTp, 0, thisisme);
			if (res == JVX_NO_ERROR)
			{
				res = theHostHdl->activate_selected_component(cpTp);
				assert(res == JVX_NO_ERROR);

				res = theHostHdl->request_object_selected_component(cpTp, &theObjHdl);
				assert(res == JVX_NO_ERROR);

				theObjId = cpTp;
			}
		}
	}
	return res;
}

jvxErrorType
jvx_deactivateComponentAsOwnerDevice(
	IjvxHost* theHostHdl,
	jvxComponentIdentification& theObjId)
{
	jvxErrorType res = theHostHdl->unselect_selected_component(theObjId);
	return res;
}
jvxErrorType
jvx_string2ValueList(const std::string& input, jvxHandle* ptrVal, jvxDataFormat format, jvxSize num, jvxSize* numUsed)
{
	jvxBool err = false; 
	jvxSize i;
	jvxData* ptrD = NULL;
	jvxInt64* ptrI64 = NULL;
	jvxInt32* ptrI32 = NULL;
	jvxInt16* ptrI16 = NULL;
	jvxInt8* ptrI8 = NULL;
	jvxUInt64* ptrUI64 = NULL;
	jvxUInt32* ptrUI32 = NULL;
	jvxUInt16* ptrUI16 = NULL;
	jvxUInt8* ptrUI8 = NULL;

	jvxSize numCopy = num;

	std::vector<std::vector<jvxData> > lstData;
	std::vector<std::vector<jvxInt64> > lstI64;
	std::vector<std::vector<jvxUInt64> > lstUI64;

	switch (format)
	{
	case JVX_DATAFORMAT_DATA:
		jvx_parseNumericExpression(input, err, lstData);
		if (err)
		{
			return JVX_ERROR_PARSE_ERROR;
		}
		if (lstData.size() != 1)
		{
			return JVX_ERROR_INVALID_SETTING;
		}
		if (lstData[0].size() > num)
		{
			return JVX_ERROR_SIZE_MISMATCH;
		}
		else
		{
			numCopy = JVX_MIN(lstData[0].size(), num);
		}
		break;

	case JVX_DATAFORMAT_64BIT_LE:
	case JVX_DATAFORMAT_32BIT_LE:
	case JVX_DATAFORMAT_16BIT_LE:
	case JVX_DATAFORMAT_8BIT:
		jvx_parseNumericExpression(input, err, lstI64);
		if (err)
		{
			return JVX_ERROR_PARSE_ERROR;
		}
		if (lstI64.size() != 1)
		{
			return JVX_ERROR_INVALID_SETTING;
		}
		if (lstI64[0].size() > num)
		{
			return JVX_ERROR_SIZE_MISMATCH;
		}
		else
		{
			numCopy = JVX_MIN(lstI64[0].size(), num);
		}
		break;

	case JVX_DATAFORMAT_U64BIT_LE:
	case JVX_DATAFORMAT_U32BIT_LE:
	case JVX_DATAFORMAT_U16BIT_LE:
	case JVX_DATAFORMAT_U8BIT:
		jvx_parseNumericExpression(input, err, lstUI64);
		if (err)
		{
			return JVX_ERROR_PARSE_ERROR;
		}
		if (lstUI64.size() != 1)
		{
			return JVX_ERROR_INVALID_SETTING;
		}
		if (lstUI64[0].size() > num)
		{
			return JVX_ERROR_SIZE_MISMATCH;
		}
		else
		{
			numCopy = JVX_MIN(lstUI64[0].size(), num);
		}
		break;
	}

	
	switch (format)
	{
	case JVX_DATAFORMAT_DATA:
		ptrD = (jvxData*)ptrVal;
		for (i = 0; i < numCopy; i++)
		{
			ptrD[i] = lstData[0][i];
		}
		break;

	case JVX_DATAFORMAT_64BIT_LE:
		ptrI64 = (jvxInt64*)ptrVal;
		for (i = 0; i < numCopy; i++)
		{
			ptrI64[i] = lstI64[0][i];
		}
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		ptrI32 = (jvxInt32*)ptrVal;
		for (i = 0; i < numCopy; i++)
		{
			jvx_check_value(lstI64[0][i], format);
			ptrI32[i] = (jvxInt32)lstI64[0][i];
		}
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		ptrI16 = (jvxInt16*)ptrVal;
		for (i = 0; i < numCopy; i++)
		{
			jvx_check_value(lstI64[0][i], format);
			ptrI16[i] = (jvxInt16)lstI64[0][i];
		}
		break;
	case JVX_DATAFORMAT_8BIT:
		ptrI8 = (jvxInt8*)ptrVal;
		for (i = 0; i < numCopy; i++)
		{
			jvx_check_value(lstI64[0][i], format);
			ptrI8[i] = (jvxInt8)lstI64[0][i];
		}
		break;

	case JVX_DATAFORMAT_U64BIT_LE:
		ptrUI64 = (jvxUInt64*)ptrVal;
		for (i = 0; i < numCopy; i++)
		{
			jvx_check_value(lstUI64[0][i], format);
			ptrUI64[i] = lstUI64[0][i];
		}
		break;
	case JVX_DATAFORMAT_U32BIT_LE:
		ptrUI32 = (jvxUInt32*)ptrVal;
		for (i = 0; i < numCopy; i++)
		{
			jvx_check_value(lstUI64[0][i], format);
			ptrUI32[i] = (jvxUInt32)lstUI64[0][i];
		}
		break;
	case JVX_DATAFORMAT_U16BIT_LE:
		ptrUI16 = (jvxUInt16*)ptrVal;
		for (i = 0; i < numCopy; i++)
		{
			jvx_check_value(lstUI64[0][i], format);
			ptrUI16[i] = (jvxInt16)lstUI64[0][i];
		}
		break;
	case JVX_DATAFORMAT_U8BIT:
		ptrUI8 = (jvxUInt8*)ptrVal;
		for (i = 0; i < numCopy; i++)
		{
			jvx_check_value(lstUI64[0][i], format);
			ptrUI8[i] = (jvxInt8)lstUI64[0][i];
		}
		break;
	default:
		return JVX_ERROR_UNSUPPORTED;
	}

	if (numUsed)
		*numUsed = numCopy;

	return JVX_NO_ERROR;
}

jvxComponentType jvx_componentNameToComponentType(std::string cpString)
{
	jvxSize i;
	jvxBool foundit = false;
	jvxComponentType tp = JVX_COMPONENT_UNKNOWN;
	for(i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++ )
	{
		if(jvxComponentType_txt(i) == cpString)
		{
			tp = (jvxComponentType)i;
			foundit = true;
			break;
		}
	}

	if (!foundit)
	{
		std::cout << "Warning: Tried to match compmponent type \"" << cpString << "\" which is unknown." << std::endl;
	}
	return(tp);
}

// ==================================================================
// ==================================================================

/**
 * Function to copy a string type variable in a fixed length character buffer.
 * The field will definitely be terminated by a NULL character.
 *///=======================================================================
bool
jvx_fillCharStr(char* ptrField, size_t lField, const std::string& str)
{
	size_t lStr = str.size();
	if(ptrField)
	{
		memset(ptrField, 0, lField);
		lStr = (lStr <= (lField-1)? lStr:(lField-1));
		memcpy(ptrField, str.c_str(), lStr);
	}
	return(true);
}

/**
 * Function to convert an integer value into a string.
 *///=======================================================================

jvxSize
jvx_bitfieldSelection2Id(jvxBitField sel, jvxSize numSel)
{
	jvxSize i;
	jvxSize retVal = JVX_SIZE_UNSELECTED;
	for(i = 0; i < numSel; i++)
	{
		if(jvx_bitTest(sel, i))
		{
			retVal = i;
			break;
		}
	}
	return(retVal);
}

jvxSize
jvx_cbitfieldSelection2Id(jvxCBitField sel)
{
	jvxSize i;
	jvxSize retVal = JVX_SIZE_UNSELECTED;
	for (i = 0; i < sizeof(jvxCBitField)*8; i++)
	{
		if (jvx_bitTest(sel, i))
		{
			retVal = i;
			break;
		}
	}
	return(retVal);
}

jvxInt32
jvx_uint642Id(jvxUInt64 sel)
{
	jvxSize i;
	jvxInt32 retVal = -1;
	for(i = 0; i < sizeof(jvxUInt64); i++)
	{
		if((sel & (jvxUInt64) 1 << i))
		{
			retVal = (jvxInt32)i;
			break;
		}
	}
	return(retVal);
}

jvxInt32
jvx_bitfieldSelectionMax(jvxBitField sel)
{
    jvxSize i;
    jvxInt32 retVal = -1;
    for(i = 0; i < sizeof(jvxBitField)*8; i++)
    {
        if(jvx_bitTest(sel, i))
        {
            retVal = (jvxInt32)i;
        }
    }
    return(retVal+1); // One more than the highest position
}

/**
 *
 *///=======================================================================
std::string
jvx_replaceDirectorySeparators_toWindows(std::string path, char replaceChar_from, char replaceChar_to)
{
	unsigned int i;
	std::string tokenReturn;
	for(i = 0; i < path.size(); i++)
	{
		char c = path[i];
		if(c == replaceChar_from)
		{
			tokenReturn += replaceChar_to;
		}
		else
		{
			tokenReturn += c;
		}
	}
	return(tokenReturn);
}

std::string
jvx_replaceDirectorySeparators_toWindows(std::string path, char replaceChar_from, const char* replaceChar_to)
{
	unsigned int i;
	std::string tokenReturn;
	for(i = 0; i < path.size(); i++)
	{
		char c = path[i];
		if(c == replaceChar_from)
		{
			tokenReturn += replaceChar_to;
		}
		else
		{
			tokenReturn += c;
		}
	}
	return(tokenReturn);
}

std::string
jvx_replaceSpecialCharactersBeforeWrite(std::string text, std::string characterReplace)
{
/* 	int state = 0; */
	bool cont = true;
	std::string tokenReturn;
	int posEnd = (int)text.size();
/* 	int posStart = 0; */
	size_t posQ = std::string::npos;
	while(cont)
	{
		// Find the last "
		posQ = text.rfind(characterReplace);
		if(posQ != std::string::npos)
		{
			// Take the end of the token up to last " and copy to output
			tokenReturn = text.substr(posQ, posEnd-posQ) + tokenReturn;
			tokenReturn = "\\" + tokenReturn;
			text = text.substr(0, posQ);
		}// if(posQ != std::npos)
		else
		{
			tokenReturn = text + tokenReturn;
			cont = false;
		}
	}
	return(tokenReturn);
}

/*
std::string
jvx_removePathFromFile(const std::string& pathToFile)
{
	size_t posi = std::string::npos;
	posi = pathToFile.rfind(JVX_SEPARATOR_DIR);
	if (posi != std::string::npos)
		return pathToFile.substr(posi + 1, std::string::npos);
	return pathToFile;
}
*/

std::string
jvx_replaceCharacter(std::string path, char replaceChar_from, char replaceChar_to)
{
	unsigned int i;
	std::string tokenReturn;
	for(i = 0; i < path.size(); i++)
	{
		char c = path[i];
		if(c == replaceChar_from)
		{
			tokenReturn += replaceChar_to;
		}
		else
		{
			tokenReturn += c;
		}
	}
	return(tokenReturn);
}
/**
 * Function to convert a jvxData value into a string.
 *///=======================================================================
std::string
jvx_absoluteToRelativePath(std::string path, bool isFile, const char* curPath)
{
	std::list<std::string>::iterator elm;
	std::list<std::string>::iterator elmc;
	std::list<std::string>::reverse_iterator relm;

	// Scan for empty path
	if(path.size() == 0)
	{
		return(path);
	}

	// Path is already relative
	if(path.size() > 1)
	{
		if(path[0] == '.')
		{
			// path is already relativ
			return(path);
		}
	}

	std::string wd;
	if (curPath == NULL)
	{
		char str[JVX_MAXSTRING + 1] = { 0 };
		JVX_GETCURRENTDIRECTORY(str, JVX_MAXSTRING);
		wd = str;
	}
	else
	{
		wd = curPath;
	}

	std::string p = path;
	std::string tmp;
#ifdef JVX_OS_WINDOWS
	std::string driveletter;
	std::string driveletterwd;
#endif
	std::list<std::string> directories;
	std::list<std::string> directorieswd;
	std::string fileName;

	size_t pos = std::string::npos;

	if(path.size() == 0)
	{
		return("." JVX_SEPARATOR_DIR);
	}

#if defined(JVX_OS_LINUX) || defined(JVX_OS_MACOSX)

	// Check for linux absolute path (leading /)
	if(path.substr(0,1) != JVX_SEPARATOR_DIR)
	{
		// path is already a relative path..
		return(path);
	}

#endif
	while(1)
	{
		pos = p.find(JVX_SEPARATOR_DIR);
		if(pos != std::string::npos)
		{
			tmp = p.substr(0, pos);
			if(!tmp.empty())
			{
				directories.push_back(tmp);
			}
			if(pos+1 < (size_t)p.size())
			{
				p = p.substr(pos+1, p.size());
			}
			else
			{
				break;
			}
		}
		else
		{
			if(!p.empty())
			{
				directories.push_back(p);
			}
			break;
		}
	}

	pos = std::string::npos;
	while(1)
	{
		pos = wd.find(JVX_SEPARATOR_DIR);
		if(pos != std::string::npos)
		{
			tmp = wd.substr(0, pos);
			if(!tmp.empty())
			{
				directorieswd.push_back(tmp);
			}
			if(pos+1 < (size_t)wd.size())
			{
				wd = wd.substr(pos+1, wd.size());
			}
			else
			{
				break;
			}
		}
		else
		{
			if(!wd.empty())
			{
				directorieswd.push_back(wd);
			}
			break;
		}
	}


#ifdef JVX_OS_WINDOWS
	elm = directories.begin();
	if(elm != directories.end())
	{
		driveletter = *elm;

		// Small correction: There might be upper and lower case mixed somehow.
		// Solution: Convert letters to lowercase in all cases for the driveletter
		std::transform(driveletter.begin(), driveletter.end(), driveletter.begin(), ::tolower);
		directories.pop_front();
	}
	else
	{
		return(path);
	}
	elm = directorieswd.begin();
	if(elm != directorieswd.end())
	{
		driveletterwd = *elm;

		// Small correction: There might be upper and lower case mixed somehow
		// Solution: Convert letters to lowercase in all cases for the driveletter
		std::transform(driveletterwd.begin(), driveletterwd.end(), driveletterwd.begin(), ::tolower);
		directorieswd.pop_front();
	}
	else
	{
		return(path);
	}
	if(driveletter != driveletterwd)
	{
		// No relative path possible
		return(path);
	}
#endif

	if(isFile)
	{
		relm = directories.rbegin();
		if(relm != directories.rend())
		{
			fileName = *relm;
		}
		directories.pop_back();
	}

	int cnt = 0;
	elm = directories.begin();
	elmc = directorieswd.begin();
	while((elm != directories.end()) && (elmc != directorieswd.end()))
	{
		if(*elm == *elmc)
		{
			cnt++;
		}
		else
		{
			break;
		}
		elm++;
		elmc++;
	}
	std::string relativePath = ".";
	
	while(elmc != directorieswd.end())
	{
		if(!relativePath.empty())
			relativePath += JVX_SEPARATOR_DIR;
		relativePath += "..";
		elmc++;
	}
	while(elm != directories.end())
	{
		if(!relativePath.empty())
			relativePath += JVX_SEPARATOR_DIR;
		relativePath += *elm;
		elm++;
	}
	if(isFile)
	{
		if(!relativePath.empty())
			relativePath += JVX_SEPARATOR_DIR;
		relativePath += fileName;
	}
	return(relativePath);
}



std::string
jvx_extractFileFromFilePath(const std::string fName)
{
	std::string bName = "";
	size_t pos = std::string::npos;
	pos = fName.rfind(JVX_SEPARATOR_DIR_CHAR);
	if (pos != std::string::npos)
	{
		bName = fName.substr(pos + 1, std::string::npos);
	}
	else
	{
		pos = fName.rfind(JVX_SEPARATOR_DIR_CHAR_THE_OTHER);
		if (pos != std::string::npos)
		{
			bName = fName.substr(pos + 1, std::string::npos);
		}
	}
	return bName;
}

std::string
jvx_extractDirectoryFromFilePath(const std::string fName)
{
	std::string dName = "";
	size_t pos = std::string::npos;
	pos = fName.rfind(JVX_SEPARATOR_DIR_CHAR);
	if (pos != std::string::npos)
	{
		dName = fName.substr(0, pos);
	}
	else
	{
		pos = fName.rfind(JVX_SEPARATOR_DIR_CHAR_THE_OTHER);
		if (pos != std::string::npos)
		{
			dName = fName.substr(0, pos);
		}
	}

	if (!JVX_DIRECTORY_EXISTS(dName.c_str()))
	{
		dName = ".";
	}

	return dName;
}

std::string
jvx_fileBaseName(const std::string& fName)
{

	std::string retStr = fName;
	size_t pos = std::string::npos;
	pos = fName.rfind('.');
	if (pos != std::string::npos)
	{
		retStr = fName.substr(0, pos);
	}
	return retStr;
}

std::string jvx_changeDirectoryUp(const std::string& path)
{
	std::string dName = path;
	size_t pos = path.rfind(JVX_SEPARATOR_DIR_CHAR);
	if (pos != std::string::npos)
	{
		dName = dName.substr(0, pos);
	}
	else
	{
		pos = dName.rfind(JVX_SEPARATOR_DIR_CHAR_THE_OTHER);
		if (pos != std::string::npos)
		{
			dName = dName.substr(0, pos);
		}
	}
	return dName;
}

std::string jvx_changeDirectoryPath(const std::string& path, const std::string& path_ext)
{
	std::string dName = path + JVX_SEPARATOR_DIR + path_ext;
	if (!JVX_DIRECTORY_EXISTS(dName.c_str()))
	{
		dName = path;
	}
	return dName;
}


std::string
jvx_replaceStrInStr(std::string strText, const std::string& strLookFor, const std::string& strReplace, jvxSize* numReplaced)
{
	jvxSize cnt = 0;
	for (std::string::size_type i = 0; (i = strText.find(strLookFor, i)) != std::string::npos;)
	{
		strText.replace(i, strLookFor.length(), strReplace);
		i += strReplace.length() - strLookFor.length() + 1;
		cnt++;
	}
	if (numReplaced)
	{
		*numReplaced = cnt;
	}
	return(strText);
}

std::string 
jvx_replaceSpecialCharactersBeforeWriteSystem(std::string inputStr)
{
	inputStr = jvx_replaceSpecialCharactersBeforeWrite(inputStr, "\\");
	inputStr = jvx_replaceSpecialCharactersBeforeWrite(inputStr, "\"");
	return(inputStr);
}
/**
 * Given the buffersize, the processing format and the number of channels, this function computes the size
 * in bytes
 *///==========================================================
size_t
jvx_numberBytesField(int buffersize, jvxDataFormat format, int chans)
{
	size_t szElm = 0;
	switch(format)
	{
	case JVX_DATAFORMAT_DATA:
		szElm = sizeof(jvxData);
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		szElm = sizeof(jvxInt32);
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		szElm = sizeof(jvxInt16);
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		szElm = sizeof(jvxInt64);
		break;
	case JVX_DATAFORMAT_8BIT:
		szElm = sizeof(jvxInt8);
		break;
	case JVX_DATAFORMAT_SIZE:
		szElm = sizeof(jvxSize);
		break;
	case JVX_DATAFORMAT_BYTE:
		szElm = sizeof(jvxByte);
		break;
	case JVX_DATAFORMAT_POINTER:
		szElm = sizeof(jvxHandle*);
		break;
	case JVX_DATAFORMAT_NONE:
	case JVX_DATAFORMAT_STRING:
	case JVX_DATAFORMAT_HANDLE:
          /* BG: unhandled, is this correct? */
		  /* HK: yes, this function is not used for those datatypes */
          break;
    default:
          break;
	}
	szElm *= buffersize * chans;
	return(szElm);
}

#ifdef JVX_OS_WINDOWS

#define L_BUF_CHAR 32767

#ifdef UNICODE
#error UNICODE DEFINED
#else
void
jvx_addPathEnvironment_onlywindows(std::vector<std::string>& additionalPaths)
{
	int i;

	// Only possible in Windows: Set the path for the location of the dynamic link libraries
	// dynamically. Add the rtpComponents subdirectory
	char* tmpC = new char[L_BUF_CHAR]; // <- given according to Win32 API
	std::string envStr;
	std::string addPath = "";

	for(i = 0; i < (int)additionalPaths.size(); i++)
	{
		if(i != 0)
			addPath += ";";
		addPath += additionalPaths[i];
	}

	if(JVX_GETENVIRONMENTVARIABLE(JVX_DYN_LIB_PATH_VARIABLE, tmpC, L_BUF_CHAR) != 0) //GetEnvironmentVariable("path", tmpC, L_BUF_CHAR)
	{
		envStr = tmpC;
		if(JVX_GETCURRENTDIRECTORY(tmpC, L_BUF_CHAR) != 0)
		{
			envStr = JVX_PASTE_ENV_VARIABLE_ATT(tmpC, envStr, JVX_COMPONENT_DIR);//(std::string)tmpC + "\\" + COMPONENT_DIR + ";" + envStr;
			if(!addPath.empty())
			{
				envStr = JVX_PASTE_ENV_VARIABLE_ADD_PATH_FRONT(envStr, addPath);//(std::string)tmpC + "\\" + COMPONENT_DIR + ";" + envStr;
			}
			JVX_SETENVIRONMENTVARIABLE(JVX_DYN_LIB_PATH_VARIABLE, envStr.c_str(), 1);//SetEnvironmentVariable("path", envStr.c_str());
		}
	}
	else
	{
		if(JVX_GETCURRENTDIRECTORY(tmpC, L_BUF_CHAR) != 0)
		{
			envStr = JVX_PASTE_ENV_VARIABLE_NEW(tmpC, JVX_COMPONENT_DIR);//(std::string)tmpC + "\\" + COMPONENT_DIR + ";" + envStr;
			if(!addPath.empty())
			{
				envStr = JVX_PASTE_ENV_VARIABLE_ADD_PATH_FRONT(envStr, addPath);//(std::string)tmpC + "\\" + COMPONENT_DIR + ";" + envStr;
			}
			JVX_SETENVIRONMENTVARIABLE(JVX_DYN_LIB_PATH_VARIABLE, envStr.c_str(), 1);//SetEnvironmentVariable("path", envStr.c_str());
		}
	}
	if(JVX_GETENVIRONMENTVARIABLE(JVX_DYN_LIB_PATH_VARIABLE, tmpC, L_BUF_CHAR) != 0) //GetEnvironmentVariable("path", tmpC, L_BUF_CHAR)
	{
		envStr = tmpC;
		//std::cout << "ENV: " << JVX_DYN_LIB_PATH_VARIABLE << "=" << envStr << std::endl;
	}
	delete[](tmpC);
}
#endif
#endif

// ============================================================================

#ifdef JVX_OS_WINDOWS

/**
 * Function to convert a standard string into a LPWSTR. This is required for the IPC since MS
 * tends to use their own specific non-standard conform strings.
 *///==========================================================================
wchar_t*
jvx_stdString2LPWSTR_alloc(std::string txt)
{
	int i;
	wchar_t* newField = new wchar_t[txt.size()+1];
	memset(newField, 0, sizeof(wchar_t) * (txt.size()+1));
	for(i = 0; i < (int)txt.size(); i++)
	{
		newField[i] = (wchar_t)txt[i];
	}
	return(newField);
}

/**
 * Function to convert a standard string into a LPWSTR. This is required for the IPC since MS
 * tends to use their own specific non-standard conform strings.
 *///==========================================================================
void
jvx_stdString2LPWSTR_delete(wchar_t* ptr)
{
	delete[](ptr);
}
#endif


/**
* Convert all letter to uppercases letters
*///=======================================================================
std::string jvx_toUpper(const std::string& txt)
{
	char buf[2] = { 0 };
	std::string txtOut;
	memset(buf, 0, 2);
	for(int i = 0; i < (int)txt.size(); i++)
	{
		buf[0] = (char)toupper(txt.c_str()[i]);
		txtOut += buf;
	}
	return(txtOut);
}

jvxBool jvx_compare_match_component_identification(const jvxComponentIdentification& comp_this, const jvxComponentIdentification& to_match)
{
	if ((comp_this.tp == to_match.tp) || (to_match.tp == JVX_COMPONENT_UNKNOWN))
	{
		if ((comp_this.slotid == to_match.slotid) || (to_match.slotid == JVX_SIZE_DONTCARE))
		{
			if ((comp_this.slotsubid == to_match.slotsubid) || (to_match.slotsubid == JVX_SIZE_DONTCARE))
			{
				return true;
			}
		}
	}
	return false;
}

/**
 * Check a 64 bit bitfield for allowed states
 *///=====================================================================
std::string jvx_validStates2String(jvxBitField states)
{
	int i;
	std::string out;
	for(i = 1; i < JVXSTATE_NUM_USE; i++)
	{
		jvxState stat = jvx_idxToState(i);
		if(JVX_EVALUATE_BITFIELD(states & stat))
		{
			if(out.empty())
			{
				out += jvxState_txt(stat);
			}
			else
			{
				out += ", ";
				out += jvxState_txt(stat);
			}
		}
	}
	return(out);
}

bool jvxFileExists(const char *filePath, const char* fileName)
{
	JVX_DIR_FIND_DATA findFileData;
	JVX_HANDLE_DIR hFind;

	//Check if file can be found.
	hFind = JVX_FINDFIRSTFILE_S(filePath, fileName, findFileData);
	if(hFind == JVX_INVALID_HANDLE_VALUE)
	{
		return false;
	}
	JVX_FINDCLOSE(hFind);
	return true;
}

// The following two functions allow to exchange properties via a floating pointer reference.
// This feature was introduced to allow to e.g. configure the autostart feature of the
// application in which the host is controlled:
// The user sets a property and the impact is forwarded to the surrounding application.
// I needed this for the autostart feature of the web controlled host: there is no way to configure the
// autostart from the web control application than via the set_property function.
// 
// It is indeed an abuse of the floating pointer
// We receive the data in this property set forward in content and convert
// it to a command (cmd) and a bool to indicate wether set or get (do_set)
// In adm we get the other property set/get constraints.
// The floating pointer is fully abused, therefore we use the two functions 
// <jvx_exchange_property_callback_local_pack> and <jvx_exchange_property_callback_local_unpack>
// to pack and unpack.
// The constr pointer is placed in the priv part of the floatig pointer,
// The set/get bool is passed in the <flags> and the text command description stored in the
// data part of the pointer - right behind the header.
jvxErrorType
jvx_exchange_property_callback_local_pack(jvxPropertyCallback cb, jvxHandle* cb_priv, jvxFloatingPointer_propadmin* constr, const std::string& propDescription, jvxBool do_set)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (cb)
	{
		jvxByte fld[JVX_FLOATING_POINTER_PROP_ABUSE_SIZE];
		memset(fld, 0, sizeof(fld));

		jvxSize sz = sizeof(jvxFloatingPointer) + propDescription.size() + 1; // One additional for 0-term
		sz = JVX_MIN(sz, JVX_FLOATING_POINTER_PROP_ABUSE_SIZE);

		jvxFloatingPointer* fldFlt = (jvxFloatingPointer*)fld;
		fldFlt->copy = NULL;
		fldFlt->deallocate = NULL;
		if (do_set)
		{
			jvx_bitZSet(fldFlt->flags, JVX_FLOATPOINTER_REQUIRES_XCHG_DATA_LOCAL_SET);

		}
		else
		{
			jvx_bitZSet(fldFlt->flags, JVX_FLOATPOINTER_REQUIRES_XCHG_DATA_LOCAL_GET);
		}
		fldFlt->tagid = JVX_SIZE_UNSELECTED;
		fldFlt->priv = constr;
		fldFlt->sz = sz;		
		char* txt = (char*)fldFlt + sizeof(jvxFloatingPointer);
		jvx_fillCharStr(txt, (sz - (sizeof(jvxFloatingPointer))), propDescription); // 
		res = cb(cb_priv, (jvxFloatingPointer*)fld);
	}
	else
	{
		res = JVX_ERROR_UNSUPPORTED;
	}
	return res;
}

/* Comments a few lines up for jvx_exchange_property_callback_local_pack */
jvxErrorType
jvx_exchange_property_callback_local_unpack(jvxFloatingPointer* in, jvxFloatingPointer_propadmin*& constr, std::string& propDescription, jvxBool& do_set)
{
	jvxErrorType res = JVX_NO_ERROR;
	int strsz = 0;

	propDescription.clear();
	if (jvx_bitTest(in->flags, JVX_FLOATPOINTER_REQUIRES_XCHG_DATA_LOCAL_GET))
	{
		do_set = false;
	}
	else
	{
		if (jvx_bitTest(in->flags, JVX_FLOATPOINTER_REQUIRES_XCHG_DATA_LOCAL_SET))
		{
			do_set = true;
		}
		else
		{
			res = JVX_ERROR_INVALID_SETTING;
		}
	}

	if (res == JVX_NO_ERROR)
	{
		strsz = (int)in->sz - (int)(sizeof(jvxFloatingPointer));
		if (strsz < 0)
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}

	if (res == JVX_NO_ERROR)
	{
		jvxByte* ptr = (jvxByte*)in;
		constr = (jvxFloatingPointer_propadmin*)in->priv;

		if (strsz >= 1)
		{
			char* txt = ptr + sizeof(jvxFloatingPointer);
			propDescription = txt;
		}
	}
	return res;
}

std::string jvx_removeSpacesLR(std::string in)
{
	int i;
	std::string out;
	jvxSize istart = 0;
	jvxSize istop = in.size()-1;

	for (i = 0; i < in.size(); i++)
	{
		if (in[i] == ' ')
		{
			istart++;
		}
		else
		{
			break;
		}
	}

	for (i = (int)(in.size())-1; i >= 0; i--)
	{
		if (in[i] == ' ')
		{
			istop--;
		}
		else
		{
			break;
		}
	}
	out = in.substr(istart, istop - istart + 1);
	return out;
}

jvxErrorType
jvx_parseCommandIntoToken(std::string command, std::string& f_expr, std::vector<std::string>& args, std::string* addArg)
{
	char bc = 0;
	size_t i;
	jvxErrorType res = JVX_ERROR_PARSE_ERROR;
	std::string remainItems;
	std::string tok_taken;
	std::list<char> waitforcharclose;
	std::list<char>::iterator elm;
	args.clear();
	f_expr.clear();
	size_t pos_openBraces = command.find("(");
	size_t pos_closeBraces = command.rfind(")");
	if(pos_openBraces == std::string::npos)
	{
		return(res);
	}
	if(pos_closeBraces == std::string::npos)
	{
		return(res);
	}
	f_expr = command.substr(0,pos_openBraces);
	if (addArg)
	{
		size_t posOpenSqB = f_expr.find('[');
		size_t posCloseSqB = f_expr.find(']');
		if (
			(posOpenSqB != std::string::npos) &&
			(posCloseSqB != std::string::npos) &&
			(posCloseSqB > posOpenSqB))
		{
			*addArg = f_expr.substr(posOpenSqB + 1, posCloseSqB - posOpenSqB - 1);
			f_expr = f_expr.substr(0, posOpenSqB);
		}
	}
	remainItems = command.substr(pos_openBraces+1, pos_closeBraces - pos_openBraces - 1);
	res = JVX_NO_ERROR;

	tok_taken.clear();
	int state = 0;
	std::list<int> stacks;
	std::list<int>::reverse_iterator elmr;
	char oneChar;
	for(i = 0; i < remainItems.size(); i++)
	{
		oneChar = remainItems[i];
		switch(state)
		{
		case 0:
			switch(oneChar)
			{
			case ',':
				//tok_taken += oneChar;
				tok_taken = jvx_removeSpacesLR(tok_taken);
				args.push_back(tok_taken);
				tok_taken.clear();
				break;
			case '"':
				stacks.push_back(0);
				state = 1;
				break;
			case '\\':
				stacks.push_back(0);
				state = 5;
				break;

			default:
				bc = findBraceInList(oneChar);
				if (bc != 0)
				{
					waitforcharclose.push_back(bc);
					elm = waitforcharclose.begin();
					state = 4;
				}
				tok_taken += oneChar;
				break;
			}
			break;
		case 1:
			switch(oneChar)
			{
			case '"':
				elmr = stacks.rbegin();
				assert(elmr != stacks.rend());
				state = *elmr;
				stacks.pop_back(); //old_state;
				break;
			case '\\':
				stacks.push_back(1);
				state = 5;
				break;
			/*case '\\':
				state = 2;
				break;*/
			default:
				tok_taken += oneChar;
				break;
			}
			break;
		case 2:
			assert(0);
			break;
		case 3:
			assert(0);
			/*
			switch(oneChar)
			{
			case ' ':
			case '\t':
				break;
			case ',':
				tok_taken = jvx_removeSpacesLR(tok_taken);
				args.push_back(tok_taken);
				tok_taken.clear();
				state = 0;
				break;
			default:
				res = JVX_ERROR_PARSE_ERROR;
			}*/
			break;
		case 4:

			if (oneChar == *elm)
			{
				waitforcharclose.erase(elm);
				if (waitforcharclose.empty())
				{
					state = 0;
				}
				else
				{
					elm = waitforcharclose.end();
					elm--;
				}
			}
			else
			{
				bc = findBraceInList(oneChar);
				if (bc != 0)
				{
					waitforcharclose.push_back(bc);
					elm = waitforcharclose.end();
					elm--;
				}
			}
			tok_taken += oneChar;
			break;
		case 5:
			elmr = stacks.rbegin();
			assert(elmr != stacks.rend());
			state = *elmr;
			stacks.pop_back();

			if (state == 1)
			{
				// We come from quote mode..
				if (oneChar != '\"')
				{
					// Forward the "\\", only in case of the " 
					// We come from the quotes case, the \ remains as it is
					tok_taken += '\\';
				}				
				tok_taken += oneChar;
			}
			else
			{
				switch (oneChar)
				{
				case 't':
					tok_taken += '\t';
					break;
				case 'r':
					tok_taken += '\r';
					break;
				case 'n':
					tok_taken += '\n';
					break;
				default:

					// All other cases: use the character as it came in
					tok_taken += '\\';
					tok_taken += oneChar;
				}
			}
			 //old_state;
			break;

		}
		if(res != JVX_NO_ERROR)
		{
			break;
		}
	}
	//if(!tok_taken.empty())
	//{
	switch (state)
	{
	case 5:
		// Expression ended with "\\"
		tok_taken += "\\"; 
		// no break here, fall through and add "\\"

	case 0:

		tok_taken = jvx_removeSpacesLR(tok_taken);
		args.push_back(tok_taken);
		tok_taken.clear();
		break;

	default:
		// Parsing ended in braces
		res = JVX_ERROR_PARSE_ERROR;
	}
	//}
	return(res);
}

/*
jvxErrorType jvx_parseStrArrayIntoTokens(std::string expr, std::list<std::string>& args, char sepTok)
{
	args.clear();
	if (expr.size() > 1)
	{
		if ((expr[0] == '[') && (expr[expr.size() - 1] == ']'))
		{
			std::list<std::string> stack;
			bool quotesOn = false;
			std::string token = expr.substr(1, expr.size() - 1);
			std::string text = "";
			for (int i = 0; i < token.size(); i++)
			{
				if ((stack.size() == 0) && (!quotesOn))
				{
					switch (token[i])
					{
					case '(':
						stack.push_back("()");
						text += token[i];
						break;
					case '{':
						stack.push_back("{}");
						text += token[i];
						break;
					case '[':
						stack.push_back("[]");
						text += token[i];
						break;
					case '\"':
						quotesOn = true;
						text += token[i];
						break;
					default:
						if (token[i] == sepTok)
						{
							text = jvx_removeSpacesLR(text);
							args.push_back(text);
						}
						else
						{
							text += token[i];
						}
						break;
					}
				}
				else
				{
					if (quotesOn)
					{
						text += token[i];
						if (token[i] == '\"')
						{
							quotesOn = false;
						}
					}
					else
					{
						text += token[i];
						std::string setStr = *(stack.rbegin());
						if (token[i] == setStr[1])
						{
							stack.pop_back();
						}
					}
				}
			}
			if((stack.size() == 0) &&
				!quotesOn)
			{
				text = jvx_removeSpacesLR(token);
				if (text.size())
				{
					args.push_back(text);
				}
				return JVX_NO_ERROR;
			}
			args.clear();		
		}
	}
	return JVX_ERROR_INVALID_ARGUMENT;
}
*/


// template <class T>
jvxErrorType jvx_parseStringListIntoTokens(std::string expr, std::vector<std::string>& args, char sep_token)
{
	char bc = 0;
	jvxSize i;
	int state = 0;
	std::list<int> stacks;
	std::list<int>::reverse_iterator elmr;
	char oneChar;
	jvxErrorType res = JVX_NO_ERROR;

	std::string tok_taken;
	std::list<char> waitforcharclose;
	std::list<char>::iterator elm;
	args.clear();
	for (i = 0; i < expr.size(); i++)
	{
		oneChar = expr[i];
		switch (state)
		{
		case 0:
			if (oneChar == sep_token)
			{
				//tok_taken += oneChar;
				tok_taken = jvx_removeSpacesLR(tok_taken);
				args.push_back(tok_taken);
				tok_taken.clear();
			}
			else
			{
				switch (oneChar)
				{
				case '"':
					stacks.push_back(0);
					state = 1;
					break;
				case '\\':
					stacks.push_back(0);
					state = 5;
					break;

				default:
					bc = findBraceInList(oneChar);
					if (bc != 0)
					{
						waitforcharclose.push_back(bc);
						elm = waitforcharclose.begin();
						state = 4;
					}
					tok_taken += oneChar;
					break;
				}
			}
			break;
		case 1:
			switch (oneChar)
			{
			case '"':
				elmr = stacks.rbegin();
				assert(elmr != stacks.rend());
				state = *elmr;
				stacks.pop_back(); //old_state;
				break;
			case '\\':
				stacks.push_back(1);
				state = 5;
				break;
				/*case '\\':
					state = 2;
					break;*/
			default:
				tok_taken += oneChar;
				break;
			}
			break;
		case 2:
			assert(0);
			break;
		case 3:
			assert(0);
			/*
			switch(oneChar)
			{
			case ' ':
			case '\t':
				break;
			case ',':
				tok_taken = jvx_removeSpacesLR(tok_taken);
				args.push_back(tok_taken);
				tok_taken.clear();
				state = 0;
				break;
			default:
				res = JVX_ERROR_PARSE_ERROR;
			}*/
			break;
		case 4:

			if (oneChar == *elm)
			{
				waitforcharclose.erase(elm);
				if (waitforcharclose.empty())
				{
					state = 0;
				}
				else
				{
					elm = waitforcharclose.end();
					elm--;
				}
			}
			else
			{
				bc = findBraceInList(oneChar);
				if (bc != 0)
				{
					waitforcharclose.push_back(bc);
					elm = waitforcharclose.end();
					elm--;
				}
			}
			tok_taken += oneChar;
			break;
		case 5:
			switch (oneChar)
			{
			case 't':
				tok_taken += '\t';
				break;
			case 'r':
				tok_taken += '\r';
				break;
			case 'n':
				tok_taken += '\n';
				break;
			default:

				// All other cases: use the character as it came in
				tok_taken += oneChar;
			}
			elmr = stacks.rbegin();
			assert(elmr != stacks.rend());
			state = *elmr;
			stacks.pop_back(); //old_state;
			break;

		}
		if (res != JVX_NO_ERROR)
		{
			break;
		}
	}
	//if(!tok_taken.empty())
	//{
	switch (state)
	{
	case 5:
		// Expression ended with "\\"
		tok_taken += "\\";
		// no break here, fall through and add "\\"

	case 0:

		tok_taken = jvx_removeSpacesLR(tok_taken);
		args.push_back(tok_taken);
		tok_taken.clear();
		break;

	default:
		// Parsing ended in braces
		res = JVX_ERROR_PARSE_ERROR;
	}
	return res;
}


void jvx_TokenRemoveWSpaceLeftright(std::string& oneToken)
{
	int cnt = 0;
	jvxSize posi2 = 0;

	// Remove from left side
	for(; cnt < oneToken.size(); cnt++)
	{
		if (oneToken[cnt] == ' ')
		{
			posi2++;
		}
		else
		{
			break;
		}
	}
	oneToken = oneToken.substr(posi2, std::string::npos);

	// Remove from right side
	jvxSize sz2 = oneToken.size();
	for (cnt = oneToken.size()-1; cnt >= 0; cnt--)
	{
		if (oneToken[cnt] == ' ')
		{
			sz2--;
		}
		else
		{
			break;
		}
	}	
	oneToken = oneToken.substr(0, sz2);
}

std::string jvx_constructPropertyLinkDescriptor(std::string tag, std::string propname, std::vector<std::string> paramlst)
{
	jvxSize i;
	std::string retVal;
	retVal += tag + ":";
	retVal += propname + ":";
	if (paramlst.size())
	{
		for (i = 0; i < paramlst.size(); i++)
		{
			if (i != 0)
			{
				retVal += ",";
			}
			retVal += paramlst[i];
		}
	}
	return(retVal);
}

std::string jvx_createPropertyLinkDescriptor(std::string tag, std::string propname, std::vector<std::string> paramlst)
{
	jvxSize i;
	std::string retVal;
	if(!tag.empty())
	{
		retVal = tag;
	}
	if(!propname.empty())
	{
		if(!retVal.empty())
			retVal += ":";
		retVal += propname;
	}
	if(!paramlst.empty())
	{
		if(!retVal.empty())
			retVal += ":";
		for(i = 0; i < paramlst.size(); i++)
		{
			if(i > 0)
			{
				retVal += ", ";
			}
			retVal += paramlst[i];
		}
	}
	return(retVal);
}


jvxErrorType	jvx_parsePropertyKeyList(std::vector<std::string>& paramlst, std::string propname, std::string& propval, jvxSize* matchedId)
{
	jvxSize i;
	propval = "";
	for (i = 0; i < paramlst.size(); i++)
	{
		std::string assignment;
		std::string value;
		std::string oneToken = paramlst[i];
		size_t posi = oneToken.find("=");
		if (posi != std::string::npos)
		{
			assignment = oneToken.substr(0, posi);
			value = oneToken.substr(posi + 1, std::string::npos);

			if (assignment == propname)
			{
				if (matchedId)
					*matchedId = i;
				propval = value;
				return(JVX_NO_ERROR);
			}
		}
	}
	return(JVX_ERROR_ELEMENT_NOT_FOUND);
}

jvxErrorType	jvx_parsePropertyStringToKeylist(std::string propTargetNameStr, std::vector<std::string>& paramlst)
{
	size_t posi;
	paramlst.clear();
	std::string oneToken;
	while (propTargetNameStr.size())
	{
		posi = propTargetNameStr.find(",");
		if (posi == std::string::npos)
		{
			oneToken = propTargetNameStr;
			propTargetNameStr = "";
		}
		else
		{
			oneToken = propTargetNameStr.substr(0, posi);
			if (posi < propTargetNameStr.size())
			{
				propTargetNameStr = propTargetNameStr.substr(posi + 1, std::string::npos);
			}
			else
			{
				propTargetNameStr = "";
			}
		}
		jvx_TokenRemoveWSpaceLeftright(oneToken);
		if (oneToken.size())
		{
			paramlst.push_back(oneToken);
		}
	}
	return JVX_NO_ERROR;
}


jvxErrorType	jvx_parsePropertyLinkDescriptor(std::string propTargetNameStr, std::string& tag, std::string& propname, std::vector<std::string>& paramlst)
{
	size_t posi;
	std::string oneToken;
	std::string oneTokenC;
	if(propTargetNameStr[0] == '/')
	{
		tag = "";
	}
	else
	{
		posi = propTargetNameStr.find(":");
		if(posi != std::string::npos)
		{
			tag = propTargetNameStr.substr(0, posi);
			propTargetNameStr = propTargetNameStr.substr(posi+1, std::string::npos);
		}
		else
		{
			// Only a tag is also valid
			tag = propTargetNameStr;
			return JVX_NO_ERROR;
		}
	}
	posi = propTargetNameStr.rfind(":");
	if(posi == std::string::npos)
	{
		propname = propTargetNameStr;
	}
	else
	{
		propname = propTargetNameStr.substr(0, posi);
		propTargetNameStr = propTargetNameStr.substr(posi+1, std::string::npos);
		jvx_parsePropertyStringToKeylist(propTargetNameStr, paramlst);
	}
	return JVX_NO_ERROR;
}

std::string 
jvx_parseStringFromBrackets(const std::string& in, char lB, char rB)
{
	std::string ret = "";
	jvxSize idxL = in.find(lB);
	jvxSize idxR = in.rfind(rB);
	if ((idxL != std::string::npos) &&
		(idxR != std::string::npos))
	{
		if (idxL < idxR)
		{
			ret = in.substr(idxL + 1, idxR - idxL - 1);
		}
	}
	return ret;
}

jvxErrorType 
jvx_stringToMacAddress(std::string str, jvxUInt8* mac)
{
	int i;
	std::string token;
	std::string remain = str;
	jvxErrorType res = JVX_NO_ERROR;
	size_t pos;
	for(i = 0; i < 5; i++)
	{
		pos = remain.find(':');
		if(pos == 2)
		{
			token = remain.substr(0,2);
			remain = remain.substr(3, std::string::npos); // get rid of ":"
			unsigned long number = strtoul(token.c_str(), NULL, 16);
			assert(number <= 255);
			mac[i] = (jvxUInt8) number;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
			break;
		}
	}
	if(res == JVX_NO_ERROR)
	{
		unsigned long number = strtoul(remain.c_str(), NULL, 16);
		assert(number <= 255);
		mac[i] = (jvxUInt8) number;
	}
	return(res);
}

std::string 
jvx_macAddressToString(jvxUInt8* mac)
{
	int i;

	std::string out;
	for(i = 0; i < 5; i++)
	{
		out += jvx_intx2String(mac[i]);
		out += ":";
	}
	out += jvx_intx2String(mac[i]);
	return(out);
}

std::string jvx_shortenStringName(jvxSize numLetters, std::string input, jvxSize relation, jvxSize fragmnts)
{
	std::string ret = input;
	if(input.size() > numLetters)
	{
		relation = JVX_MIN(relation,fragmnts-1);
		relation = JVX_MAX(relation,1);
		jvxSize numStart = (numLetters - 3) * relation/fragmnts;
		jvxSize numStop = numLetters - 3 - numStart;

		ret = input.substr(0, numStart) + "..." + input.substr(input.size()-numStop, std::string::npos);
		//ret = input.substr(0, relation * numLetters/fragmnts) + "..." + input.substr(input.size()-(fragmnts-relation)*numLetters/fragmnts, (4-relation)*numLetters/fragmnts);
	}
	return ret;
}

std::string jvx_makeFilePath(const std::string& folder, const std::string& fname)
{
	std::string fName = folder;
	if (!fName.empty())
	{
		if (fName[fName.size() - 1] != JVX_SEPARATOR_DIR_CHAR)
		{
			fName += JVX_SEPARATOR_DIR;
		}
		fName += fname;
	}
	else
	{
		fName = fname;
	}
	return fName;
}

std::string jvx_makePathExpr(const std::string& prefix, const std::string& suffix, jvxBool outputIsAPrefix , jvxBool forceLeadingSlash)
{
	std::string out = "";
	if(suffix.size())
	{
		if(prefix.size() > 0)
		{
			if(prefix[prefix.size()-1] == '/')
			{
				if(suffix[0] == '/')
				{
					// Skip first /
					out = prefix + suffix.substr(1, suffix.size()-1);
				}
				else
				{
					out = prefix + suffix;
				}
			}
			else
			{
				if(suffix[0] == '/')
				{
					out = prefix + suffix;
				}
				else
				{
					// Add glue-/
					out = prefix + "/" + suffix;
				}
			}
		}
		else
		{
			out = suffix;
		}

		if(forceLeadingSlash)
		{
			if(out[0] != '/')
			{
				out = "/" + out;
			}
		}
	}
	else
	{
		if(outputIsAPrefix)
		{
			out = prefix;
		}
		else
		{
			out = suffix;
		}
		if (forceLeadingSlash)
		{
			if (out[0] != '/')
			{
				out = "/" + out;
			}
		}
	}
	return(out);
}

std::string
jvx_pathExprFromFilePath(std::string path, char sep)
{
	jvxSize sz = path.size();
	jvxSize posilast = path.rfind(sep);
	if (posilast == std::string::npos)
	{
		return(path);
	}
	else if (posilast == sz)
	{
		return(path);
	}
	return(path.substr(0, posilast));
}

std::string
jvx_fileNameFromFilePath(std::string path)
{
	jvxSize sz = path.size();
	jvxSize posilast = path.rfind('/');
	if (posilast == std::string::npos)
	{
		return(path);
	}
	else if (posilast == sz)
	{
		return("");
	}
	return(path.substr(posilast+1, std::string::npos));
}

void
jvx_composePathExpr(std::vector<std::string>& lst, std::string& path, jvxSize idx)
{
	jvxSize i;
	path = "";
	for (i = idx; i < lst.size(); i++)
	{
		path = jvx_makePathExpr(path, lst[i]);
	}
	path = jvx_makePathExpr(path, "", true); // force leading /
}

void
jvx_decomposePathExpr(std::string path, std::vector<std::string>& lst)
{
	lst.clear();
	std::string token;
	jvxSize posi = std::string::npos;
	while(1)
	{ 
		posi = path.find('/');
		if (posi < path.size())
		{
			token = path.substr(0, posi);
			if (token.size())
			{
				lst.push_back(token);
			}
			path = path.substr(posi + 1, std::string::npos);
		}
		else
		{ 
			break;
		}
	}
	if (path.size())
	{
		lst.push_back(path);
	}
}

bool
jvx_findPathExprEntry_idx(std::string path, std::string& theEntry, jvxSize idx)
{
	theEntry = "";
	std::string token;
	jvxSize cnt = 0;
	jvxSize posi = std::string::npos;
	while (1)
	{
		posi = path.find('/');
		if (posi < path.size())
		{
			token = path.substr(0, posi);
			if (!token.empty())
			{
				if (cnt == idx)
				{
					theEntry = token;
					return true;
				}
				else
				{
					cnt++;
				}
			}
			path = path.substr(posi + 1, std::string::npos);
		}
		else
		{
			break;
		}
	}
	if (!path.empty())
	{
		if (cnt == idx)
		{
			theEntry = token;
			return true;
		}
	}
	return false;
}

jvxErrorType jvx_parseCommandLineOneToken(std::string in, std::vector<std::string>& out, char sep )
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;

	int state = 0; // 0: SEP, 1: normal, 2: "
	std::string oneToken = "";
	jvxSize pos = std::string::npos;
	for (i = 0; i < in.size(); i++)
	{
		switch (state)
		{
		case 0:
			if (in[i] == sep)
			{
				// SEP after SEP, skip it
				break;
			}
			if (in[i] == '\"')
			{
				state = 2;
			}
			else
			{
				state = 1;
			}
			// no break here
		case 1:
			if (in[i] == sep)
			{
				if (oneToken.size())
				{
					out.push_back(oneToken);
				}
				oneToken.clear();
				state = 0;
				break;
			}
			if (in[i] == '\"')
			{
				state = 2;
				//oneToken += in[i];
				break;
			}
			else
			{
				oneToken += in[i];
				break;
			}
			// no break here
		case 2:
			if (in[i] == '\"')
			{
				state = 1;
			}
			else
			{
				oneToken += in[i];
			}
			break;
		}
	}
	if (oneToken.size())
	{
		out.push_back(oneToken);
	}
	if (state != 1)
	{
		res = JVX_ERROR_PARSE_ERROR;
	}
	return(res);
}

std::string
jvx_charReplace(std::string input, char* replaceChar_from,  const char** replaceChar_to, jvxSize numReplace)
{
	unsigned int i,j;
	std::string tokenReturn;
	jvxBool found = false;
	for(i = 0; i < input.size(); i++)
	{
		char c = input[i];
		found = false;
		for(j = 0; j < numReplace; j++)
		{
			if(c == replaceChar_from[j])
			{
				found = true;
				break;
			}
		}

		if(found)
		{
			tokenReturn += replaceChar_to[j];
		}
		else
		{
			tokenReturn += c;
		}
	}
	return(tokenReturn);
}


jvxErrorType jvx_parseValueString_size(std::string input, jvxSize* numberValues)
{
	std::string token;
	if(numberValues)
	{
		*numberValues = 0;
		if(input.size() > 0)
		{
			// Remove leading and ending []
			if((input[0] == '[') && (input[input.size()-1] == ']'))
			{
				input = input.substr(1,input.size()-2);
			}

			while(1)
			{
				size_t pp = std::string::npos;
				pp = input.find(input, ',');
				if(pp == std::string::npos)
				{
					break;
				}
				else
				{
					token = input.substr(0, pp-1);
					input = input.substr(pp, std::string::npos);
					(*numberValues)++;
				}
			}
			if(input.size())
			{
				(*numberValues)++;
			}
		}
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}


jvxErrorType jvx_parseValueString_content(std::string input, jvxHandle* fld, jvxSize* numberValues, jvxDataFormat form)
{
	std::string token;
	jvxSize cnt = 0;
	jvxData* fldData = (jvxData*)fld;
	jvxInt8* fldInt8 = (jvxInt8*)fld;
	jvxInt16* fldInt16 = (jvxInt16* )fld;
	jvxInt32* fldInt32 = (jvxInt32* )fld;
	//jvxInt64* fldInt64 = (jvxInt64* )fld;

	jvxData valD; 
	if(numberValues)
	{
		*numberValues = 0;
		if(input.size() > 0)
		{
			// Remove leading and ending []
			if((input[0] == '[') && (input[input.size()-1] == ']'))
			{
				input = input.substr(1,input.size()-2);
			}

			while(1)
			{
				size_t pp = std::string::npos;
				pp = input.find(input, ',');
				if(pp == std::string::npos)
				{
					break;
				}
				else
				{
					token = input.substr(0, pp-1);
					input = input.substr(pp, std::string::npos);
					if(cnt < *numberValues)
					{
						valD = (jvxData)atof(token.c_str());
						switch(form)
						{
						case JVX_DATAFORMAT_DATA:
							fldData[cnt] = (jvxData)valD;
							break;
						case JVX_DATAFORMAT_8BIT:
							fldInt8[cnt] = (jvxInt8)valD;
							break;
						case JVX_DATAFORMAT_16BIT_LE:
							fldInt16[cnt] = (jvxInt16)valD;
							break;
						case JVX_DATAFORMAT_32BIT_LE:
							fldInt32[cnt] = (jvxInt32)valD;
							break;
						case JVX_DATAFORMAT_64BIT_LE:
							fldInt16[cnt] = (jvxInt16)valD;
							break;
						default:
							assert(0);
						}
						cnt++;
					}
					else
					{
						break;
					}
				}
			}
			if(input.size())
			{
				if(cnt < *numberValues)
				{
					valD = (jvxData)atof(input.c_str());
					switch(form)
					{
					case JVX_DATAFORMAT_DATA:
						fldData[cnt] = (jvxData)valD;
						break;
					case JVX_DATAFORMAT_8BIT:
						fldInt8[cnt] = (jvxInt8)valD;
						break;
					case JVX_DATAFORMAT_16BIT_LE:
						fldInt16[cnt] = (jvxInt16)valD;
						break;
					case JVX_DATAFORMAT_32BIT_LE:
						fldInt32[cnt] = (jvxInt32)valD;
						break;
					case JVX_DATAFORMAT_64BIT_LE:
						fldInt16[cnt] = (jvxInt16)valD;
						break;
					default:
						assert(0);
					}
					cnt++;
				}
			}
		}
		*numberValues = cnt;
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
}

jvxBool jvx_compareComponentIdentWildcard(jvxComponentIdentification cp_this_wc, jvxComponentIdentification to_this)
{
	if (
		(cp_this_wc.tp == JVX_COMPONENT_UNKNOWN) ||
		(cp_this_wc.tp == to_this.tp))
	{
		if (
			JVX_CHECK_SIZE_UNSELECTED(cp_this_wc.slotid) ||
			(cp_this_wc.slotid == to_this.slotid))
		{
			if (
				JVX_CHECK_SIZE_UNSELECTED(cp_this_wc.slotsubid) ||
				(cp_this_wc.slotsubid == to_this.slotsubid))
			{
				return true;
			}
		}
	}
	return false;
}

jvxBool jvx_compareStringsWildcard(std::string compareme_wc, std::string tome)
{
	jvx_oneWildcardEntry theEntry;

	std::vector<jvx_oneWildcardEntry> lstWildcards;
	std::string wildcardToken;
	size_t idx = 0;
	//size_t inc = 0;
	jvxBool matches = true;
	jvxSize cnt = 0;
	jvxSize i;

	jvxBool bslashb4 = false;

	if(compareme_wc.empty())
	{
		return(compareme_wc == tome);
	}

	cnt = 0;
	while(1)
	{
		if(cnt < compareme_wc.size())
		{
			if(bslashb4 == false)
			{
				if(compareme_wc[cnt] == '\\')
				{
					bslashb4 = true;
				}	
				else if(
					(compareme_wc[cnt] == '?') ||
					(compareme_wc[cnt] == '*') ||
					(compareme_wc[cnt] == '+'))
				{
					if (!wildcardToken.empty())
					{
						theEntry.token = wildcardToken;
						theEntry.isWildcard = false;
						lstWildcards.push_back(theEntry);
						wildcardToken.clear();
					}

					theEntry.token = compareme_wc[cnt];
					theEntry.isWildcard = true;
					lstWildcards.push_back(theEntry);
				}
				else
				{
					wildcardToken += compareme_wc[cnt];
				}
			}
			else
			{
				wildcardToken += compareme_wc[cnt];
				bslashb4 = false;
			}
			cnt++;
		}
		else
		{
			break;
		}
	}
	
	if(!wildcardToken.empty())
	{
		theEntry.token = wildcardToken;
		theEntry.isWildcard = false;
		lstWildcards.push_back(theEntry);
	}

	cnt = 0;
	jvxSize minVal = 0;
	jvxSize maxVal = 0;

	for(i = 0; i < lstWildcards.size();i++)
	{
		std::string ctoken = lstWildcards[i].token;

		if(lstWildcards[i].isWildcard)
		{
			if(ctoken == "*")
			{
				minVal += 0;
				maxVal += tome.size();
			}
			if(ctoken == "?")
			{
				minVal += 0;
				maxVal += 1;
			}
			if(ctoken == "+")
			{
				minVal += 1;
				maxVal += tome.size();
			}
		}
		else
		{
			idx = tome.find(ctoken);
			if(idx == std::string::npos)
			{
				matches = false;
				break;
			}
			else
			{
				if(
					!((idx >= minVal ) && (idx <= maxVal))
					)
				{
					matches = false;
					break;
				}
				else
				{
					// It is all good, pattern checked
					tome = tome.substr(idx + ctoken.size(), std::string::npos);
					minVal = 0;
					maxVal = 0;
				}
			}
		}
	}
	if(!
		((tome.size()  >= minVal ) && (tome.size() <= maxVal)))
	{
		matches = false;
	}
	return(matches);
}

namespace jvx {
	namespace helper {
		std::string asciToUtf8(const std::string& in)
		{
			/* RFC 2044 in short:
			0000 0000 - 0000 007F   0xxxxxxx
			0000 0080 - 0000 07FF   110xxxxx 10xxxxxx
			0000 0800 - 0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

			0001 0000 - 001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
			0020 0000 - 03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
			0400 0000 - 7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx

			https://datatracker.ietf.org/doc/html/rfc2044

			*/

			std::string ret;
			jvxSize i;
			jvxSize cnt = 0;
			int stateUtf8 = 0;
			int numFollow = 0;

			// First: determine the size
			for (i = 0; i < in.size(); i++)
			{
				unsigned char c = in[i];
				if (c & 0x80)
				{
					jvxBool isUtf8Already = false;

					// This could be a UTF8 character
					// However, I accept only asci char origins for UTF8
					if ((c & 0xE0) == 0xC0)
					{
						if (i < in.size() - 1)
						{
							unsigned char cc = in[i + 1];
							if ((cc & 0xC0) == 0x80)
							{
								// Yes, it is a UTF8 character
								isUtf8Already = true;
								i++;
								cnt++;
							}
						}
					}
					if (!isUtf8Already)
					{
						cnt++;
					}
				}
				cnt++;
			}

			ret.resize(cnt);
			cnt = 0;

			for (i = 0; i < in.size(); i++)
			{
				unsigned char c = in[i];
				if (c & 0x80)
				{
					jvxBool isUtf8Already = false;

					// This could be a UTF8 character
					// However, I accept only asci char origins for UTF8
					if ((c & 0xE0) == 0xC0)
					{
						if (i < in.size() - 1)
						{
							unsigned char cc = in[i + 1];
							if ((cc & 0xC0) == 0x80)
							{
								// Yes, it is a UTF8 character
								isUtf8Already = true;

								// Add this and the next char
								ret[cnt++] = c;
								ret[cnt++] = cc;
								i++;
							}
						}
					}
					if (!isUtf8Already)
					{
						jvxUInt32 c32 = (c & 0xFF);

						// Copy all bits higher than the 6 lsb and constrain to 5 bits 
						c = (unsigned char)0x6;
						c <<= 5;
						c |= ((c32 >> 6) & 0x1F);
						ret[cnt++] = c;

						c = (unsigned char)0x2;
						c <<= 6;
						c |= (c32 & 0x3F);
						ret[cnt++] = c;
					}
				}
				else
				{
					ret[cnt++] = c;
				}
			}
			return ret;
		}

		std::string utf82Ascii(const std::string& in)
		{
			std::string ret;
			jvxSize i;
			jvxSize cnt = 0;
			int numFollow = 0;

			// First: determine the size
			for (i = 0; i < in.size(); i++)
			{
				unsigned char c = in[i];
				if (c & 0x80)
				{
					// This could be a UTF8 character
					// However, I accept only asci char origins for UTF8
					if ((c & 0xE0) == 0xC0)
					{
						if (i < in.size() - 1)
						{
							unsigned char cc = in[i + 1];
							if ((cc & 0xC0) == 0x80)
							{
								// Yes, it is a UTF8 character
								i++;
							}
						}
					}

				}
				cnt++;
			}

			ret.resize(cnt);
			cnt = 0;

			for (i = 0; i < in.size(); i++)
			{
				jvxBool isAUtf8Char = false;
				unsigned char c = in[i];
				if (c & 0x80)
				{
					// This could be a UTF8 character
					// However, I accept only asci char origins for UTF8
					if ((c & 0xE0) == 0xC0)
					{
						if (i < in.size() - 1)
						{
							unsigned char cc = in[i + 1];
							if ((cc & 0xC0) == 0x80)
							{
								// Yes, it is a UTF8 character
								isAUtf8Char = true;
								unsigned char r = cc & 0x3F;
								r |= (c & 0x3) << 6;
								ret[cnt] = r;
								i++;
							}
						}
					}

				}
				if (!isAUtf8Char)
				{
					ret[cnt] = c;
				}
				cnt++;
			}
			return ret;
		}

		jvxInt32 hash(std::string str)
		{
			jvxSize i;
			jvxInt32 c = 0;
			jvxInt32 h = 0;

			for (i = 0; i < str.size(); i++)
			{
				c = toupper(str[i]);
				h = ((h << 5) + h) ^ c;
			}
			return h;
		}
	}
}

void jvx_AllocateOneRcParameter(jvxRCOneParameter* ptr, jvxDataFormat form, jvxSize numChans, jvxSize bSize, jvxBool allocateBuf )
{
	jvxSize i;
	ptr->description.float_precision_id = JVX_FLOAT_DATAFORMAT_ID;
	ptr->description.format = form;
	ptr->description.seg_length_y = JVX_SIZE_UINT16(numChans);
	ptr->description.seg_length_x = JVX_SIZE_UINT16(bSize);
	ptr->content = NULL;
	JVX_SAFE_NEW_FLD(ptr->content, jvxHandle*, ptr->description.seg_length_y);
	if (allocateBuf)
	{
		for (i = 0; i < ptr->description.seg_length_y; i++)
		{
			jvxSize numBytes = jvxDataFormat_size[ptr->description.format] * ptr->description.seg_length_x;
			assert(numBytes > 0);

			JVX_SAFE_NEW_FLD(ptr->content[i], jvxByte, sizeof(jvxByte) *numBytes);
			memset(ptr->content[i], 0, sizeof(jvxByte) * numBytes);
		}
	}
	else
	{
		memset(ptr->content, 0, sizeof(jvxHandle*) * ptr->description.seg_length_y);
	}
}

void jvx_DeallocateOneRcParameter(jvxRCOneParameter* ptr)
{
	jvxSize i;
	for (i = 0; i < ptr->description.seg_length_y; i++)
	{
		JVX_SAFE_DELETE_FLD(ptr->content[i], jvxByte);		
	}
	JVX_SAFE_DELETE_FLD(ptr->content, jvxHandle*);

	ptr->description.float_precision_id = JVX_FLOAT_DATAFORMAT_ID;
	ptr->description.format = JVX_DATAFORMAT_NONE;
	ptr->description.seg_length_y = 0;
	ptr->description.seg_length_x = 0;
	ptr->content = NULL;
}

void
jvx_runAllActiveMainComponents(IjvxHost* hostRef, std::function<void(IjvxObject* theOwner, IjvxHiddenInterface* iface)> func)
{
	jvxSize i;
	for (i = JVX_COMPONENT_UNKNOWN + 1; i < JVX_MAIN_COMPONENT_LIMIT; i++)
	{
		jvxSize j, k;
		// IjvxNode* mComp = nullptr;
		jvxBool runSlots = false;
		jvxBool runSublots = false;
		jvxComponentIdentification tpL;
		tpL.tp = (jvxComponentType)i;

		jvxComponentTypeClass clsTp = jvxComponentTypeClass::JVX_COMPONENT_TYPE_NONE;
		hostRef->role_component_system(tpL.tp, nullptr, nullptr, &clsTp);
		hostRef->number_slots_component_system(tpL, nullptr, nullptr, nullptr, nullptr);
		std::string txtCt = jvxComponentTypeClass_txt(clsTp);
		switch (clsTp)
		{
		case jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY:
			runSlots = true;
			runSublots = true;
			break;
		case jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE:
			runSlots = true;
			break;
		default:
			break;
		}
		
		if (runSlots)
		{
			jvxSize szSlots = 0;
			hostRef->number_slots_component_system(tpL, &szSlots, nullptr, nullptr, nullptr);
			for (j = 0; j < szSlots; j++)
			{
				IjvxObject* obj = nullptr;
				IjvxHiddenInterface* iface = nullptr;
				jvxHandle* theHdl = nullptr;
				IjvxConfigurationDone* conf = nullptr;
				tpL.slotid = j;
				tpL.slotsubid = 0;
				hostRef->request_object_selected_component(tpL, &obj);
				if (obj)
				{
					IjvxObject* theOwner = nullptr;
					IjvxNode* theNode = castFromObject<IjvxNode>(obj);
					IjvxTechnology* theTechnology = castFromObject<IjvxTechnology>(obj);
	
					if (theNode)
					{
						iface = static_cast<IjvxHiddenInterface*>(theNode);
						theNode->owner(&theOwner);
					}
					if (theTechnology)
					{
						iface = static_cast<IjvxHiddenInterface*>(theTechnology);
						theTechnology->owner(&theOwner);
					}

					/*
					obj->request_specialization(&theHdl, nullptr, nullptr);
					switch (tpL.tp)
					{
#include "codeFragments/components/Hjvx_caseStatement_technologies.h"
						iface = static_cast<IjvxHiddenInterface*>((IjvxTechnology*)theHdl);
						((IjvxTechnology*)theHdl)->owner(&theOwner);

						break;
#include "codeFragments/components/Hjvx_caseStatement_nodes.h"
						iface = static_cast<IjvxHiddenInterface*>((IjvxNode*)theHdl);
						((IjvxNode*)theHdl)->owner(&theOwner);
						break;
					}
					*/
					func(theOwner, iface);
					hostRef->return_object_selected_component(tpL, obj);
				}

				if (runSublots)
				{
					jvxComponentIdentification tpLL = tpL;
					tpLL.tp = (jvxComponentType)(tpLL.tp + 1); // <- from technology to device
					jvxSize szSubSlots = 0;
					tpLL.slotid = tpL.slotid;

					hostRef->number_slots_component_system(tpLL, nullptr, &szSubSlots, nullptr, nullptr);
					for (k = 0; k < szSubSlots; k++)
					{
						tpLL.slotsubid = k;
						hostRef->request_object_selected_component(tpLL, &obj);
						if (obj)
						{
							IjvxObject* theOwner = nullptr;
							obj->request_specialization(&theHdl, nullptr, nullptr);
							switch (tpLL.tp)
							{
#include "codeFragments/components/Hjvx_caseStatement_devices.h"
								iface = static_cast<IjvxHiddenInterface*>((IjvxDevice*)theHdl);
								((IjvxDevice*)theHdl)->owner(&theOwner);
								break;
							}
							func(theOwner, iface);
						}
					}
				}
			}
		}
	}
}

jvxErrorType
jvx_activateObjectInModule(
	IjvxHost* hHostRef, 
	jvxComponentIdentification& tp, 
	const std::string& refModuleName, 
	IjvxObject* theOwner, 
	jvxBool extend_if_necessary,
	const std::string& attach_name)
{
	jvxSize id = JVX_SIZE_UNSELECTED;
	jvxSize i;
	jvxApiString astr;
	jvxErrorType res = JVX_NO_ERROR;

	// Activate the syncronization nodes
	// std::cout << "Activate the synchronization instances" << std::endl;
	jvxSize numC = 0;
	hHostRef->number_components_system(tp, &numC);
	for (i = 0; i < numC; i++)
	{
		hHostRef->module_reference_component_system(tp, i, &astr, nullptr);
		if (astr.std_str() == refModuleName)
		{
			id = i;
			break;
		}
	}
	if (JVX_CHECK_SIZE_SELECTED(id))
	{
		tp.slotid = JVX_SIZE_DONTCARE;
		res = hHostRef->select_component(tp, id, theOwner, extend_if_necessary);
		if (res == JVX_NO_ERROR)
		{
			if (!attach_name.empty())
			{
				IjvxObject* obj = nullptr;
				IjvxManipulate* man = nullptr;
				hHostRef->request_object_selected_component(tp, &obj);
				man = reqInterfaceObj<IjvxManipulate>(obj);
				if (man)
				{
					jvxApiString astr;
					jvxVariant var;
					var.assign(&astr);
					jvxErrorType resL = man->get_manipulate_value(JVX_MANIPULATE_DESCRIPTION, &var);
					if (resL == JVX_NO_ERROR)
					{
						astr.assign(astr.std_str() + attach_name);
						resL = man->set_manipulate_value(JVX_MANIPULATE_DESCRIPTION, &var);
					}
					retInterfaceObj<IjvxManipulate>(obj, man);
				}
			}

			res = hHostRef->activate_selected_component(tp);
			if (res != JVX_NO_ERROR)
			{
				hHostRef->unselect_selected_component(tp);
			}
		}	
	}
	return res;
}

jvxErrorType
jvx_deactivateObjectInModule(IjvxHost* hHostRef, jvxComponentIdentification& tp)
{
	jvxErrorType res = JVX_NO_ERROR;

	// Component may have been deactivated before..
	res = hHostRef->deactivate_selected_component(tp);
	res = hHostRef->unselect_selected_component(tp);
	res = JVX_NO_ERROR;
	return res;
}

void 
jvx_parseNumericExpression(std::string txt, jvxBool& err, std::vector<std::vector<jvxData> >& returnMe)
{
	jvxSize i;
	int state = 0;
	std::vector<jvxData> myRow;
	std::string oneToken;
	jvxData val;
	err = false;
	for (i = 0; i < txt.size(); i++)
	{
		char oneChar = txt[i];

		switch (state)
		{
		case 0:
			if (
				(oneChar != ' ') && (oneChar != '\t'))
			{
				if (oneChar == '[')
				{
					state = 1;
					oneToken = "";
				}
				else
				{
					state = -1;
				}
			}
			break;

		case 1:
			if (
				(oneChar != ' ') && (oneChar != '\t'))
			{
				state = 2;
				oneToken = "";
			}
			else
			{
				break;
			}
		case 2:
			if ((oneChar == ',') || (oneChar == ' ') || (oneChar == '\t'))
			{
				jvxBool errL = false;
				//val = atof(oneToken.c_str());
				if (
					(oneToken.size() >= 2) &&
					((oneToken.substr(0, 2) == "0x") || (oneToken.substr(0, 2) == "0X")))
				{
					val = (jvxData)jvx_string2Int64(oneToken, errL);
				}
				else
				{
					val = jvx_string2Data(oneToken, errL);
				}
				if (errL)
					err = true;
				myRow.push_back(val);
				oneToken = "";
				state = 1;
			}
			else if (oneChar == ';')
			{
				if (!oneToken.empty())
				{
					jvxBool errL = false;
					if (
						(oneToken.size() >= 2) &&
						((oneToken.substr(0, 2) == "0x") || (oneToken.substr(0, 2) == "0X")))
					{
						val = (jvxData)jvx_string2Int64(oneToken, errL);
					}
					else
					{
						val = jvx_string2Data(oneToken, errL);
					}
					if (errL)
						err = true;
					myRow.push_back(val);
				}
				oneToken = "";
				returnMe.push_back(myRow);
				myRow.clear();
				state = 1;
			}
			else if (oneChar == ']')
			{
				if (!oneToken.empty())
				{
					jvxBool errL = false;
					if (
						(oneToken.size() >= 2) &&
						((oneToken.substr(0, 2) == "0x") || (oneToken.substr(0, 2) == "0X")))
					{
						val = (jvxData)jvx_string2Int64(oneToken, errL);
					}
					else
					{
						val = jvx_string2Data(oneToken, errL);
					}
					if (errL)
						err = true;
					myRow.push_back(val);
				}
				if (!myRow.empty())
				{
					returnMe.push_back(myRow);
				}
				state = 0;
			}
			else
			{
				oneToken += oneChar;
			}
			break;
		}
	}	
}

void
jvx_parseNumericExpression(std::string txt, jvxBool& err, std::vector<std::vector<jvxInt64> >& returnMe)
{
	jvxSize i;
	int state = 0;
	std::vector<jvxInt64> myRow;
	std::string oneToken;
	jvxInt64 valI64;
	err = false;
	for (i = 0; i < txt.size(); i++)
	{
		char oneChar = txt[i];

		switch (state)
		{
		case 0:
			if (
				(oneChar != ' ') && (oneChar != '\t'))
			{
				if (oneChar == '[')
				{
					state = 1;
					oneToken = "";
				}
				else
				{
					state = -1;
				}
			}
			break;

		case 1:
			if (
				(oneChar != ' ') && (oneChar != '\t'))
			{
				state = 2;
				oneToken = "";
			}
			else
			{
				break;
			}
		case 2:
			if ((oneChar == ',') || (oneChar == ' ') || (oneChar == '\t'))
			{
				jvxBool errL = false;
				//val = atof(oneToken.c_str());
				if (
					(oneToken.size() >= 2) &&
					((oneToken.substr(0, 2) == "0x") || (oneToken.substr(0, 2) == "0X")))
				{
					valI64 = jvx_string2Int64(oneToken, errL);
				}
				else
				{
					valI64 = (jvxInt64)jvx_string2Data(oneToken, errL);
				}
				if (errL)
					err = true;
				myRow.push_back(valI64);
				oneToken = "";
				state = 1;
			}
			else if (oneChar == ';')
			{
				if (!oneToken.empty())
				{
					jvxBool errL = false;
					//val = atof(oneToken.c_str());
					if (
						(oneToken.size() >= 2) &&
						((oneToken.substr(0, 2) == "0x") || (oneToken.substr(0, 2) == "0X")))
					{
						valI64 = jvx_string2Int64(oneToken, errL);
					}
					else
					{
						valI64 = (jvxInt64)jvx_string2Data(oneToken, errL);
					}
					if (errL)
						err = true;
					myRow.push_back(valI64);
				}
				oneToken = "";
				returnMe.push_back(myRow);
				myRow.clear();
				state = 1;
			}
			else if (oneChar == ']')
			{
				if (!oneToken.empty())
				{
					jvxBool errL = false;
					//val = atof(oneToken.c_str());
					if (
						(oneToken.size() >= 2) &&
						((oneToken.substr(0, 2) == "0x") || (oneToken.substr(0, 2) == "0X")))
					{
						valI64 = jvx_string2Int64(oneToken, errL);
					}
					else
					{
						valI64 = (jvxInt64)jvx_string2Data(oneToken, errL);
					}
					if (errL)
						err = true;
					myRow.push_back(valI64);
				}
				if (!myRow.empty())
				{
					returnMe.push_back(myRow);
				}
				state = 0;
			}
			else
			{
				oneToken += oneChar;
			}
			break;
		}
	}
}

void
jvx_parseNumericExpression(std::string txt, jvxBool& err, std::vector<std::vector<jvxUInt64> >& returnMe)
{
	jvxSize i;
	int state = 0;
	std::vector<jvxUInt64> myRow;
	std::string oneToken;
	jvxUInt64 valUI64;
	err = false;
	for (i = 0; i < txt.size(); i++)
	{
		char oneChar = txt[i];

		switch (state)
		{
		case 0:
			if (
				(oneChar != ' ') && (oneChar != '\t'))
			{
				if (oneChar == '[')
				{
					state = 1;
					oneToken = "";
				}
				else
				{
					state = -1;
				}
			}
			break;

		case 1:
			if (
				(oneChar != ' ') && (oneChar != '\t'))
			{
				state = 2;
				oneToken = "";
			}
			else
			{
				break;
			}
		case 2:
			if ((oneChar == ',') || (oneChar == ' ') || (oneChar == '\t'))
			{
				jvxBool errL = false;
				//val = atof(oneToken.c_str());
				if (
					(oneToken.size() >= 2) &&
					((oneToken.substr(0, 2) == "0x") || (oneToken.substr(0, 2) == "0X")))
				{
					valUI64 = jvx_string2UInt64(oneToken, errL);
				}
				else
				{
					valUI64 = (jvxInt64)jvx_string2Data(oneToken, errL);
				}
				if (errL)
					err = true;
				myRow.push_back(valUI64);
				oneToken = "";
				state = 1;
			}
			else if (oneChar == ';')
			{
				if (!oneToken.empty())
				{
					jvxBool errL = false;
					//val = atof(oneToken.c_str());
					if (
						(oneToken.size() >= 2) &&
						((oneToken.substr(0, 2) == "0x") || (oneToken.substr(0, 2) == "0X")))
					{
						valUI64 = jvx_string2UInt64(oneToken, errL);
					}
					else
					{
						valUI64 = (jvxInt64)jvx_string2Data(oneToken, errL);
					}
					if (errL)
						err = true;
					myRow.push_back(valUI64);
				}
				oneToken = "";
				returnMe.push_back(myRow);
				myRow.clear();
				state = 1;
			}
			else if (oneChar == ']')
			{
				if (!oneToken.empty())
				{
					jvxBool errL = false;
					//val = atof(oneToken.c_str());
					if (
						(oneToken.size() >= 2) &&
						((oneToken.substr(0, 2) == "0x") || (oneToken.substr(0, 2) == "0X")))
					{
						valUI64 = jvx_string2UInt64(oneToken, errL);
					}
					else
					{
						valUI64 = (jvxUInt64)jvx_string2Data(oneToken, errL);
					}
					if (errL)
						err = true;
					myRow.push_back(valUI64);
				}
				if (!myRow.empty())
				{
					returnMe.push_back(myRow);
				}
				state = 0;
			}
			else
			{
				oneToken += oneChar;
			}
			break;
		}
	}
}

std::vector<jvxData> jvx_parseCsvExpression(std::string txt, jvxBool& err, char sep_char)
{
	jvxSize i;
	std::vector<jvxData> myRow;
	std::string oneToken;
	jvxData val;
	err = false;
	for (i = 0; i < txt.size(); i++)
	{
		char oneChar = txt[i];

		if (oneChar == sep_char)
		{
			if (oneToken.empty())
			{
				val = 0.0;
			}
			else
			{
				jvxBool errL = false;
				val = jvx_string2Data(oneToken, errL);
				myRow.push_back(val);
			}
			oneToken = "";
		}
		else
		{
			oneToken += oneChar;
		}
	}
	if (!oneToken.empty())
	{
		jvxBool errL = false;
		val = jvx_string2Data(oneToken, errL);
		myRow.push_back(val);
	}
	return(myRow);
}

std::vector<std::string> jvx_parseCsvStringExpression(std::string txt, jvxBool& err)
{
	jvxSize i;
	std::vector<std::string> myRow;
	std::string oneToken;
	err = false;
	for (i = 0; i < txt.size(); i++)
	{
		char oneChar = txt[i];

		if (oneChar == ';')
		{
			myRow.push_back(oneToken);
			oneToken = "";
		}
		else
		{
			oneToken += oneChar;
		}
	}
	if (!oneToken.empty())
	{
		myRow.push_back(oneToken);
	}
	return(myRow);
}
std::string jvx_data2NumericExpressionString(std::vector<std::vector<jvxData> > & lst, int numDigs)
{
	std::string txt;
	jvxSize i, j;
	txt = "[";
	for (i = 0; i < lst.size(); i++)
	{
		if (i > 0)
		{
			txt += ";";
		}
		for (j = 0; j < lst[i].size(); j++)
		{
			if (j > 0)
			{
				txt += ",";
			}
			txt += jvx_data2String(lst[i][j], numDigs);
		}
	}
	txt += "]";
	return(txt);
}

std::string jvx_data2CsvExpressionString(std::vector<jvxData>& lst, int numDigs)
{
	std::string txt;
	jvxSize  j;
	
	for (j = 0; j < lst.size(); j++)
	{
		if (j > 0)
		{
			txt += ";";
		}
		txt += jvx_data2String(lst[j], numDigs);
	}

	return(txt);
}

std::string jvx_string2CsvExpressionString(std::vector<std::string>& lst)
{
	std::string txt;
	jvxSize  j;

	for (j = 0; j < lst.size(); j++)
	{
		if (j > 0)
		{
			txt += ";";
		}
		txt += lst[j];
	}

	return(txt);
}

std::string jvx_stringNumericExpression(std::vector<jvxData> & lst, int numDigs)
{
	std::string txt;
	jvxSize j;
	txt = "[";

	for (j = 0; j < lst.size(); j++)
	{
		if (j > 0)
		{
			txt += ",";
		}
		txt += jvx_data2String(lst[j], numDigs);
	}
	txt += "]";
	return(txt);
}

jvxCBitField jvx_string2CBitField(std::string token)
{
	jvxCBitField myField = 0;
	jvxSize i;
	unsigned long out;
	if (token.size() >= 2)
	{
		std::string prefix = token.substr(0, 2);
		
		if (prefix == "0b")
		{
			// Remove leading "0b"
			token = token.substr(2, std::string::npos);
			// From LSB to MSB
			assert(token.size() <= sizeof(jvxCBitField));
			for (i = 0; i < token.size(); i++)
			{
				if (token[token.size() - i - 1] == '1')
				{
					myField |= ((jvxFlagTag)1 << i);
				}
				else if (token[token.size() - i - 1] != '0')
				{
					assert(0);
				}
			}
		}
		else if (prefix == "0x")
		{
			token = token.substr(2, std::string::npos);
			jvxUInt32 numEntries = (jvxUInt32)ceil(token.size() / (sizeof(jvxCBitField) * 8.0 / 4.0));
			if (numEntries > sizeof(jvxCBitField)/4)
			{
				std::cout << "Warning: Overload in bitfield expression, value is 0x" << token << std::endl;
				numEntries = (sizeof(jvxCBitField) * 8 / 4);
			}

			for (i = 0; i < token.size(); i++)
			{
				std::string ltok = token.substr(i, 1);
				out = strtoul(ltok.c_str(), NULL, 16);
				myField = myField << 4;
				myField |= (out & 0xF);
			}

		}
	}	
	return(myField);
}

std::string jvx_CBitField2String(jvxCBitField myField)
{
	std::string token;
	
	// From LSB to MSB
	while(myField)
	{
		if(myField & 0x1)
		{
			token += '1';
		}
		else
		{
			token += '0';
		}
		myField = myField >> 1;
	}
	// Add leading "0b"
	token = "0b" + token;
	return(token);
}

intmax_t jvx_string2IntMax(const std::string& in, jvxBool& err)
{
	const char* cin = in.c_str();
	char* cc = NULL;
	intmax_t ret = 0;
	jvxData retdata = 0.0;

	ret = strtoimax(cin, &cc, 0);

	intptr_t cc1 = intptr_t(cc);
	intptr_t cc2 = intptr_t(cin) + in.size();
	if (cc1 == cc2)
	{
		err = false;
		return ret;
	}
	retdata = JVX_C_STR2DATA(cin, &cc);
	cc1 = intptr_t(cc);
	cc2 = intptr_t(cin) + in.size();
	if (cc1 == cc2)
	{
		ret = intmax_t(retdata);
		err = false;
		return ret;
	}

	err = true;
	return 0;
}

jvxSize jvx_string2Size(const std::string& in, jvxBool& err)
{
	const char* cin = in.c_str();
	char* cc = NULL;
	jvxSize ret = 0;
	intmax_t retI = 0;

	retI = strtoumax(cin, &cc, 0);
	ret = (jvxSize)retI;

	intptr_t cc1 = intptr_t(cc);
	intptr_t cc2 = intptr_t(cin) + in.size();
	if (cc1 == cc2)
	{
		err = false;		
		return ret;
	}

	// se 1:
	// "JVX_SIZE_UNSELECTED"
	// "none";
	// "-1"

	// "JVX_SIZE_DONTCARE"
	// "dontcare";
	// "-2"

	// "JVX_SIZE_SLOT_RETAIN"
	// "retain"
	// "-3"

	ret = jvxSizeSpecialValues_decode(in.c_str());
	if (ret == 0)
	{
		jvxCBool errb = c_false;
		jvxData valD = jvx_txt2Data(in.c_str(), &errb);
		if (errb == c_false)
		{
			ret = JVX_DATA2SIZE(valD);
			err = false;
			return ret;
		}
		err = true;
	}
	return ret;
}

jvxData jvx_string2Data(const std::string& in, jvxBool& err)
{
	jvxCBool errl = c_false;
	jvxData res = jvx_txt2Data(in.c_str(), &errl);
	err = (errl == c_true);
	return res;
	/*
	const char* cin = in.c_str();
	char* cc = NULL;
	jvxData ret = 0.0;

#ifdef JVX_DSP_BASE_USE_DATA_FORMAT_FLOAT
	ret = strtof(in.c_str(), &cc);
#else
	ret = strtod(in.c_str(), &cc);
#endif

	intptr_t cc1 = intptr_t(cc);
	intptr_t cc2 = intptr_t(cin) + in.size();
	if (cc1 == cc2)
	{
		err = false;
		return ret;
	}
	err = true;
	return 0;
	*/
}

jvxInt64 jvx_string2Int64(const std::string& in, jvxBool& err)
{
	const char* cin = in.c_str();
	char* cc = NULL;
	jvxInt64 ret = 0;

	ret = strtoll(in.c_str(), &cc, 0);

	intptr_t cc1 = intptr_t(cc);
	intptr_t cc2 = intptr_t(cin) + in.size();
	if (cc1 == cc2)
	{
		err = false;
		return ret;
	}
	err = true;
	return 0;
}

jvxUInt64 jvx_string2UInt64(const std::string& in, jvxBool& err)
{
	jvxCBool errl = c_false;
	jvxUInt64 res = jvx_txt2UInt64(in.c_str(), &errl);
	err = (errl == c_true);
	return res;
	/*
	const char* cin = in.c_str();
	char* cc = NULL;
	jvxUInt64 ret = 0.0;

	ret = strtoull(in.c_str(), &cc, 0);

	intptr_t cc1 = intptr_t(cc);
	intptr_t cc2 = intptr_t(cin) + in.size();
	if (cc1 == cc2)
	{
		err = false;
		return ret;
	}
	err = true;
	return 0;
	*/
}

jvxUInt16 jvx_string2UInt16(const std::string& in, jvxBool& err)
{
	jvxCBool errl = c_false;
	jvxUInt16 res = jvx_txt2UInt16(in.c_str(), &errl);
	err = (errl == c_true);
	return res;
	/*
	const char* cin = in.c_str();
	char* cc = NULL;
	jvxUInt64 ret = 0.0;

	ret = strtoull(in.c_str(), &cc, 0);

	intptr_t cc1 = intptr_t(cc);
	intptr_t cc2 = intptr_t(cin) + in.size();
	if (cc1 == cc2)
	{
		err = false;
		return ret;
	}
	err = true;
	return 0;
	*/
}
std::string jvx_prepareStringForLogfile(const std::string& oneMessage)
{
	jvxSize i;
	std::string newMessage;
	for (i = 0; i < oneMessage.size(); i++)
	{
		switch (oneMessage[i])
		{
		case '\n':
			newMessage += "\\n";
			break;
		case '\r':
			newMessage += "\\r";
			break;
		default:
			newMessage += oneMessage[i];
		}
	}
	return newMessage;
}

jvxErrorType jvx_string2Context(const std::string ctxtStr, jvxContext* ctxt)
{
	jvxBool err = false;
	if (ctxt)
	{
		ctxt->id = jvx_string2Size(ctxtStr, err);
	}
	if (!err)
	{
		ctxt->valid = true;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_PARSE_ERROR;
}

jvxErrorType jvx_context2String(std::string& ctxtStr, jvxContext* ctxt)
{
	ctxtStr.clear();
	if (ctxt)
	{
		ctxtStr = jvx_size2String(ctxt->id);
	}
	return JVX_NO_ERROR;
}

std::string
jvx_textMessagePrioToString(std::string message, jvxReportPriority prio)
{
	std::string prefix;
	switch (prio)
	{
	case JVX_REPORT_PRIORITY_ERROR:
		prefix = " ==> <ERROR>";
		break;
	case JVX_REPORT_PRIORITY_INFO:
		prefix = " ==> <INFO>";
		break;
	case JVX_REPORT_PRIORITY_SUCCESS:
		prefix = " ==> <SUCCESS>";
		break;
	case JVX_REPORT_PRIORITY_WARNING:
		prefix = " ==> <WARNING>";
		break;
	default:
		prefix = " == >";
		break;
	}
	prefix = prefix + message;
	return prefix;
}

std::string jvx_create_text_seq_report(const std::string& intro, jvxSize sequenceId,
	jvxSize stepId, jvxSequencerQueueType tp,
	jvxSequencerElementType stp, jvxSize fId,
	jvxSize operation_state, const std::string& description)
{
	std::string txt = intro;
	txt += ", sequence = <" + jvx_size2String(sequenceId) + ">";
	txt += ", step = <" + jvx_size2String(stepId) + ">";
	txt += ", function id = <" + jvx_size2String(stepId) + ">";
	txt += ", oper state = <" + jvx_size2String(operation_state) + ">";
	switch (tp)
	{
	case JVX_SEQUENCER_QUEUE_TYPE_RUN:
		txt += ", <run queue>";
		break;
	case JVX_SEQUENCER_QUEUE_TYPE_LEAVE:
		txt += ", <leave queue>";
		break;
	default:
		txt += ", <no queue>";
		break;
	}

	txt += ", message = ";
	txt += description;
	return txt;
}
void jvx_getFilesFolderPath(std::vector<std::string>& lst, const std::string& pathName, const std::string& extension)
{
	JVX_HANDLE_DIR searchHandle = INVALID_HANDLE_VALUE;
	JVX_DIR_FIND_DATA searchResult = INIT_SEARCH_DIR_DEFAULT;
	lst.clear();

	// Search for directory
	searchHandle = JVX_FINDFIRSTFILE_WC(searchHandle, pathName.c_str(), extension.c_str(), searchResult);

	// If entry exists
	if (searchHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			std::string fileName = pathName;
			fileName += JVX_SEPARATOR_DIR;
			fileName += JVX_GETFILENAMEDIR(searchResult);//searchResult.cFileName;

			lst.push_back(fileName);
		} while (JVX_FINDNEXTFILE(searchHandle, searchResult, JVX_DLL_EXTENSION));//FindNextFile( searchHandle, &searchResult )
		JVX_FINDCLOSE(searchHandle);//FindClose( searchHandle );
	}
}

std::string jvx_time_string()
{
	std::string strDate = JVX_TIME();
	strDate = jvx_replaceCharacter(strDate, '/', '_');
	strDate = jvx_replaceCharacter(strDate, ':', '_');
	return(strDate);
}

std::string jvx_date_string()
{
	std::string strDate = JVX_DATE();
	strDate = jvx_replaceCharacter(strDate, '/', '_');
	strDate = jvx_replaceCharacter(strDate, ':', '_');
	return(strDate);
}

// =================================================================================

jvxComponentIdentification jvxCreateComponentIdentificationUnlocated(jvxComponentType tp)
{
	jvxComponentIdentification theNewId;
	theNewId.tp = tp;
	theNewId.slotid = JVX_SIZE_UNSELECTED;
	theNewId.slotsubid = JVX_SIZE_UNSELECTED;
	return theNewId;
};

jvxComponentIdentification jvxCreateComponentIdentification(jvxComponentType tp, jvxSize slotid, jvxSize slotsubid)
{
	jvxComponentIdentification theNewId;
	theNewId.tp = tp;
	theNewId.slotid = slotid;
	theNewId.slotsubid = slotsubid;
	return theNewId;
};

std::string jvxComponentIdentification_txt(jvxComponentIdentification cpTp, bool expl)
{
	std::string txt;
	txt += jvxComponentType_txt(cpTp.tp);
	if (!expl)
	{
		if ((cpTp.slotid == 0) && (cpTp.slotsubid == 0))
		{
			return txt;
		}
	}

	txt += "<";
	txt += jvx_size2String(cpTp.slotid, 1);
	/*
	switch (cpTp.slotid)
	{
	case JVX_SIZE_UNSELECTED:
	case JVX_SIZE_DONTCARE:
		txt += "dont-care";
		break;
	case JVX_SIZE_SLOT_RETAIN:
		txt += "slot-retain";
		break;
	case JVX_SIZE_SLOT_OFF_SYSTEM:
		txt += "off-sys";
		break;
	default:
		txt += jvx_size2String(cpTp.slotid);
	}*/

	if (!expl)
	{
		if (cpTp.slotsubid == 0)
		{
			txt += ">";
			return txt;
		}
	}

	txt += ",";

	txt += jvx_size2String(cpTp.slotsubid, 1);
	/*
	switch (cpTp.slotsubid)
	{
	case JVX_SIZE_UNSELECTED:
	case JVX_SIZE_DONTCARE:
		txt += "dont-care";
		break;
	case JVX_SIZE_SLOT_RETAIN:
		txt += "slot-retain";
		break;
	case JVX_SIZE_SLOT_OFF_SYSTEM:
		txt += "off-sys";
		break;
	default:
		txt += jvx_size2String(cpTp.slotsubid);
	}
	*/
	txt += ">";
	return txt;
}

jvxErrorType 
jvxComponentIdentification_decode(jvxComponentIdentification& cpTp, const std::string& in)
{
	std::string tp;
	std::string token;
	std::string sslotid;
	std::string sslotsid;
	jvxSize pos = std::string::npos;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool err = false;
	jvxSize slotid = JVX_SIZE_UNSELECTED;
	jvxSize slotsid = JVX_SIZE_UNSELECTED;
	cpTp.tp = JVX_COMPONENT_UNKNOWN;
	//cpTp.slotid = JVX_SIZE_UNSELECTED;
	//cpTp.slotsubid = JVX_SIZE_UNSELECTED;
	cpTp.slotid = 0;
	cpTp.slotsubid = 0;

	pos = in.find("<");
	if (pos == std::string::npos)
	{
		res = jvxComponentType_decode(&cpTp.tp, in);
		return res;
	}

	tp = in.substr(0, pos);
	token = in.substr(pos + 1, std::string::npos);

	pos = token.find(",");
	if (pos != std::string::npos)
	{
		sslotid = token.substr(0, pos);
		token = token.substr(pos + 1, std::string::npos);

		pos = token.find(">");
		if (pos != std::string::npos)
		{
			sslotsid = token.substr(0, pos);

			err = false;
			sslotid = jvx_removeSpacesLR(sslotid);
			slotid = jvx_string2Size(sslotid, err);
			if (err)
			{
				res = JVX_ERROR_PARSE_ERROR;
			}
			err = false;
			sslotsid = jvx_removeSpacesLR(sslotsid);
			slotsid = jvx_string2Size(sslotsid, err);
			if (err)
			{
				res = JVX_ERROR_PARSE_ERROR;
			}
			if (!err)
			{
				tp = jvx_removeSpacesLR(tp);
				res = jvxComponentType_decode(&cpTp.tp, tp);
				cpTp.slotid = slotid;
				cpTp.slotsubid = slotsid;
				return JVX_NO_ERROR;
			}
		}
	}
	else
	{
		pos = token.find(">");
		if (pos != std::string::npos)
		{
			sslotid = token.substr(0, pos);
			err = false;

			sslotid = jvx_removeSpacesLR(sslotid);
			slotid = jvx_string2Size(sslotid, err);
			if (err)
			{
				res = JVX_ERROR_PARSE_ERROR;
			}
			else
			{
				slotsid = 0;
				tp = jvx_removeSpacesLR(tp);
				res = jvxComponentType_decode(&cpTp.tp, tp);
				cpTp.slotid = slotid;
				cpTp.slotsubid = slotsid;
				return JVX_NO_ERROR;
			}
		}
	}

	return res;
}

jvxErrorType 
jvxSequencerElementType_decode(jvxSequencerElementType& tp, const std::string& txt)
{
	jvxSize i;
	tp = JVX_SEQUENCER_TYPE_COMMAND_NONE;
	jvxErrorType res = JVX_ERROR_PARSE_ERROR;
	for (i = 0; i < JVX_SEQUENCER_TYPE_COMMAND_LIMIT; i++)
	{
		if (
			(txt == jvxSequencerElementType_str[i].friendly) ||
			(txt == jvxSequencerElementType_str[i].full))
		{
			tp = (jvxSequencerElementType)i;
			res = JVX_NO_ERROR;
			break;
		}
	}
	return res;
}

jvxErrorType
jvxSequencerQueueType_decode(jvxSequencerQueueType& tp, const std::string& txt)
{
	jvxSize i;
	tp = JVX_SEQUENCER_QUEUE_TYPE_RUN;
	jvxErrorType res = JVX_ERROR_PARSE_ERROR;
	for (i = 0; i < JVX_SEQUENCER_QUEUE_TYPE_LIMIT; i++)
	{
		if (
			(txt == jvxSequencerQueueType_str[i].friendly) ||
			(txt == jvxSequencerQueueType_str[i].full))
		{
			tp = (jvxSequencerQueueType)i;
			res = JVX_NO_ERROR;
			break;
		}
	}
	return res;
}

void
jvx_request_interfaceToObject(IjvxInterfaceReference* toadd, IjvxObject** theObj, jvxComponentIdentification* tp, jvxApiString* descror, jvxApiString* descrion)
{
	IjvxObject* locObj = NULL;

	if (toadd)
	{
		toadd->request_reference_object(&locObj);
		if (locObj)
		{
			if (tp)
			{
				(locObj)->request_specialization(NULL, tp, NULL, NULL);
			}
			if (descror)
			{
				(locObj)->descriptor(descror);
			}
			if (descrion)
			{
				(locObj)->description(descrion);
			}
			if (theObj)
			{
				*theObj = locObj;
			}
			else
			{
				// It makes sense to return the interface if no pointer reference is passed outside. Otherwise there would be no
				// chance to return it later!
				toadd->return_reference_object(locObj);
			}
		}
	}
	else
	{
		if (tp)
		{
			*tp = JVX_COMPONENT_UNKNOWN;
		}
		if (descror)
		{
			descror->assign("internal");
		}
		if (descrion)
		{
			descrion->assign("Additional connector used for internal processes");
		}
		if (theObj)
		{
			*theObj = NULL;
		}
	}
}

void
jvx_return_interfaceToObject(IjvxInterfaceReference* toadd, IjvxObject* theObj)
{
	if (toadd)
	{
		toadd->return_reference_object(theObj);
	}
}
	
void 
jvx_commonConnectorToObjectDescription(IjvxCommonConnector* in_con, std::string& txt, const std::string& str)
{
	IjvxObject* theObj = NULL;
	jvxComponentIdentification tp;
	jvxApiString name;

	IjvxConnectorFactory* theParentFac = NULL;
	in_con->parent_factory(&theParentFac);
	
	jvx_request_interfaceToObject(static_cast<IjvxInterfaceReference*>(theParentFac), &theObj, &tp, &name, NULL);
	if (theObj)
	{
		txt = "[ Object <" + name.std_str() + "> - <" + jvxComponentIdentification_txt(tp) + "> - connector <" + str + ">]";
	}
	else
	{
		txt = "[ No Object - connector <" + str + ">]";
	}
	jvx_return_interfaceToObject(static_cast<IjvxInterfaceReference*>(theParentFac), theObj);
}

void 
jvx_connectionmasterToObjectDescription(IjvxConnectionMaster* mas, std::string& txt, const std::string& str)
{
	IjvxObject* theObj = NULL;
	jvxComponentIdentification tp;
	jvxApiString name;

	IjvxConnectionMasterFactory* theParentFac = NULL;
	mas->parent_master_factory(&theParentFac);

	jvx_request_interfaceToObject(static_cast<IjvxInterfaceReference*>(theParentFac), &theObj, &tp, &name, NULL);
	txt = "[ Object <" + name.std_str() + "> - <" + jvxComponentIdentification_txt(tp) + "> - master <" + str + ">]";
	jvx_return_interfaceToObject(static_cast<IjvxInterfaceReference*>(theParentFac), theObj);
}

jvxErrorType jvx_parseHttpQueryIntoPieces(std::vector<std::string>& qList, std::string in)
{
	std::string token;
	size_t posQ = std::string::npos;
	while (1)
	{
		posQ = in.find("&");
		if (posQ != std::string::npos)
		{
			token = in.substr(0, posQ);
			in = in.substr(posQ + 1, std::string::npos);
			qList.push_back(token);
		}
		else
		{
			break;
		}
	}
	if (in.size())
	{
		qList.push_back(in);
	}
	return JVX_NO_ERROR;
}

jvxErrorType jvx_findValueHttpQuery(std::vector<std::string>& qList, std::string& out, const std::string& lookfor)
{
	jvxSize i;
	size_t pos = std::string::npos;
	std::string token, tokenKey;
	out = "";
	for (i = 0; i < qList.size(); i++)
	{
		token = qList[i];
		pos = token.find("=");
		if (pos != std::string::npos)
		{
			tokenKey = token.substr(0, pos);
			if (tokenKey == lookfor)
			{
				out = token.substr(pos + 1, std::string::npos);
				return JVX_NO_ERROR;
			}
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

std::string jvx_getCallProtStringShort(IjvxCallProt* fdb)
{
	std::string ret;
	jvxApiString str1; 
	jvxApiString str2; 
	jvxBool err = false;
	if (fdb)
	{
		fdb->getErrorShort(&err, &str1, &str2);
		if (err)
		{
			ret = str2.std_str();
			ret += ":";
			ret += str1.std_str();
		}
		else
		{
			ret = "NO ERROR";
		}
	}
	return ret;
}

void jvx_getCallProtObject(IjvxCallProt* fdb, IjvxObject* object, const char* conn, const char* origin, const char* context, const char* comment)
{
	if (fdb)
	{
		jvxComponentIdentification idTp;
		jvxApiString str;
		if (object)
		{
			object->request_specialization(NULL, &idTp, NULL, NULL);
			object->description(&str);
			
			fdb->setIdentification(idTp, str.c_str(), conn, origin, context, comment);
		}
		else
		{
			fdb->setIdentification(idTp, context, conn, origin, context, comment);
		}
	}
}

void jvx_getCallProtConnector(IjvxCallProt* fdb, IjvxCommonConnector* conn, const char* origin)
{
	IjvxObject* theObj = NULL;
	jvxApiString descr;
	IjvxConnectorFactory* par = NULL;
	if (conn)
	{
		conn->descriptor_connector(&descr);
		conn->parent_factory(&par);
		if (par)
		{
			jvx_request_interfaceToObject(par, &theObj, NULL, NULL, NULL);
		}
	}
	jvx_getCallProtObject(fdb, theObj, descr.c_str(), origin);
}

void jvx_getCallProtNoLink(IjvxCallProt* fdb, const std::string& txt_orig, const char* origin, const char* ctxt, const char* comment)
{
	jvx_getCallProtObject(fdb, NULL, txt_orig.c_str(), origin, ctxt, comment);
}

void jvx_getCallProtFactoryMaster(IjvxCallProt* fdb, IjvxConnectionMasterFactory* fac, const char* origin)
{
	IjvxObject* theObj = NULL;
	jvx_request_interfaceToObject(fac, &theObj, NULL, NULL, NULL);
	jvx_getCallProtObject(fdb, theObj, "", origin);
}

std::string 
jvx_linkDataParams2String(jvxLinkDataDescriptor* lData)
{
	std::string txt;
	txt += "b=" + jvx_size2String(lData->con_params.buffersize);
	txt += ";r=" + jvx_size2String(lData->con_params.rate);
	txt += ";c=" + jvx_size2String(lData->con_params.number_channels);
	txt += ";f=";
	txt += jvxDataFormat_txt(lData->con_params.format);
	txt += ";sx=" + jvx_size2String(lData->con_params.segmentation.x);
	txt += ";sy=" + jvx_size2String(lData->con_params.segmentation.y);
	txt += ";sf=";
	txt += jvxDataFormatGroup_txt(lData->con_params.format_group);
	return txt;
}

jvxErrorType jvx_eventloop_copy_message(jvxHandle** ptr, jvxSize paramType, jvxSize szFld)
{
	std::string* newStr = NULL;
	std::string* oldStr = NULL;
	jvxByte* allocFld = NULL;
	jvxByte* inFld = NULL;
	jvxErrorType res = JVX_NO_ERROR;

	switch (paramType)
	{
	case JVX_EVENTLOOP_DATAFORMAT_STDSTRING:
		if (ptr)
		{
			oldStr = (std::string*)*ptr;
			newStr = NULL;
			JVX_DSP_SAFE_ALLOCATE_OBJECT(newStr, std::string);
			*newStr = *oldStr;
			*ptr = newStr;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case JVX_EVENTLOOP_DATAFORMAT_MEMORY_BUF:
		if (ptr)
		{
			inFld = (jvxByte*)*ptr;
			assert(szFld);
			assert(inFld);
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(allocFld, jvxByte, szFld);
			memcpy(allocFld, inFld, szFld);
			*ptr = allocFld;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case JVX_EVENTLOOP_DATAFORMAT_REQUEST_COMMAND_REQUEST:
		if (ptr)
		{
			const CjvxReportCommandRequest* request = (const CjvxReportCommandRequest*)*ptr;
			assert(request);
			switch (request->datatype())
			{
			case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SCHEDULE:
				*ptr = new CjvxReportCommandRequest_rm(*request);
				break;
			case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_IDENT:
				*ptr = new CjvxReportCommandRequest_id(*request);
				break;
			case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_UID:
				*ptr = new CjvxReportCommandRequest_uid(*request);
				break;
			case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS:
				*ptr = new CjvxReportCommandRequest_ss(*request);
				break;
			default:
				*ptr = new CjvxReportCommandRequest(*request);
				break;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;

	default:
		res = JVX_ERROR_INVALID_SETTING;
	}
	return res;
}

jvxErrorType jvx_eventloop_delete_message(jvxHandle** ptr, jvxSize paramType, jvxSize szFld)
{
	std::string* oldStr = NULL;
	jvxByte* inFld = NULL;
	jvxErrorType res = JVX_NO_ERROR;

	switch (paramType)
	{
	case JVX_EVENTLOOP_DATAFORMAT_STDSTRING:
		if (ptr)
		{
			oldStr = (std::string*)*ptr;
			JVX_DSP_SAFE_DELETE_OBJECT(oldStr);
			*ptr = NULL;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case JVX_EVENTLOOP_DATAFORMAT_MEMORY_BUF:
		if (ptr)
		{
			inFld = (jvxByte*)*ptr;			
			JVX_DSP_SAFE_DELETE_FIELD(inFld);
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case JVX_EVENTLOOP_DATAFORMAT_REQUEST_COMMAND_REQUEST:
		if (ptr)
		{
			CjvxReportCommandRequest* request = (CjvxReportCommandRequest*)*ptr;
			JVX_DSP_SAFE_DELETE_OBJECT( request);
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	default:
		res = JVX_ERROR_INVALID_SETTING;
	}
	return res;
}

void
jvx_request_test_chain(jvxLinkDataDescriptor* data, CjvxObject* theObj, jvxCBitField reportFlags)
{
	jvxSize idConn = 0;
	if (data)
	{
		if (data->con_link.master)
		{
			IjvxDataConnectionProcess* refProc = NULL;
			data->con_link.master->associated_process(&refProc);
			if (refProc)
			{
				refProc->unique_id_connections(&idConn);
				theObj->_report_command_request(reportFlags, (jvxHandle*)(intptr_t)idConn);
			}
		}
	}
}

// =================================================================
// class jvxostream : public std::streambuf
// =================================================================

jvxostream::jvxostream()
{
	theLog = NULL;
	theStat = JVX_STATE_NONE;
	JVX_INITIALIZE_MUTEX(theLock);
	threadId_lasttime = 0;
	lockThreadId = 0;;
	refCnt = 0;
}

jvxostream::~jvxostream()
{
	JVX_TERMINATE_MUTEX(theLock);
}

void 
jvxostream::setActive(jvxBool activeArg)
{
	active = activeArg;
}

jvxErrorType 
jvxostream::setReference(IjvxTextLog* theRef, char* theBufferP, jvxSize szBufP)
{
	if (theStat != JVX_STATE_NONE)
	{
		return JVX_ERROR_WRONG_STATE;
	}

	assert(szBufP > 0);

	theLog = theRef;
	theBuffer = theBufferP;

	//theBuffer = new char[szBuf];
	//memset(theBuffer, 0, szBuf);

	// Set the input buffer bounds
#ifdef JVX_OS_WINDOWS
	setp(theBuffer, theBuffer, theBuffer + szBufP);
#else
	setp(theBuffer, theBuffer + szBufP);
#endif
	theStat = JVX_STATE_INIT;
	return JVX_NO_ERROR;

}

void
jvxostream::set_module_name(const std::string& modnm)
{
	this->moduleName = modnm;
}

jvxErrorType 
jvxostream::unsetReference()
{
	if (theStat != JVX_STATE_INIT)
	{
		return JVX_ERROR_WRONG_STATE;
	}

	theLog = NULL;
	theBuffer = NULL;
	theStat = JVX_STATE_NONE;
	return JVX_NO_ERROR;
}

int 
jvxostream::sync() 
{
	if (active)
	{
		if (refCnt == 0)
		{
			if (threadId_lasttime != 0)
			{
				if (threadId_lasttime != JVX_GET_CURRENT_THREAD_ID())
				{
					std::cout << "Unlocked multithread access in " << __FUNCTION__ << "," << __LINE__ << std::endl;
				}
			}
		}
		threadId_lasttime = JVX_GET_CURRENT_THREAD_ID();
		if (theStat)
		{
			char* p1 = pbase();
			char* p2 = pptr();
			char* p3 = epptr();

			jvxSize sz = (int)((uintptr_t)p2 - (uintptr_t)p1);

			std::string text(theBuffer, sz);
			pbump(-((int)sz));

			if (theLog)
			{
				if (moduleName.size())
				{
					theLog->addEntry_buffered(text.c_str(), moduleName.c_str());
				}
				else
				{
					theLog->addEntry_buffered(text.c_str());
				}
			}
			else
			{
				std::cout << text << std::flush;
			}
			return 0;
		}
	}
	return EOF;
}

int 
jvxostream::overflow(int ch)
{
	if (active)
	{
		if (refCnt == 0)
		{
			if (threadId_lasttime != 0)
			{
				if (threadId_lasttime != JVX_GET_CURRENT_THREAD_ID())
				{
					std::cout << "Unlocked multithread access in " << __FUNCTION__ << "," << __LINE__ << std::endl;
				}
			}
		}
		threadId_lasttime = JVX_GET_CURRENT_THREAD_ID();
		if (theStat)
		{
			// Write out to log file
			sync();

			char* p1 = pbase();
			char* p2 = pptr();
			char* p3 = epptr();

			assert(p2 == p1);
			*p1 = ch;
			pbump(1);

			return 0;
		}
	}
	return EOF;
}

void 
jvxostream::lock()
{
	if (active)
	{
		JVX_LOCK_MUTEX(theLock);
		if (lockThreadId == 0)
		{
			lockThreadId = JVX_GET_CURRENT_THREAD_ID();
			refCnt = 1;
		}
		else
		{
			if (lockThreadId == JVX_GET_CURRENT_THREAD_ID())
			{
				refCnt++;
			}
			else
			{
				assert(0); // <- this should never happen!
			}

		}
	}
}

void 
jvxostream::unlock()
{
	if (active)
	{
		if (lockThreadId != JVX_GET_CURRENT_THREAD_ID())
		{
			std::cout << "Invalid thread unlock operation, thread id is " << JVX_GET_CURRENT_THREAD_ID() << " but it should be " << threadId_lasttime << std::endl;
		}
		refCnt--;
		if (refCnt == 0)
		{
			lockThreadId = 0;
		}
		JVX_UNLOCK_MUTEX(theLock);
	}
}

bool 
jvxostream::try_lock()
{
	if (active)
	{
		JVX_TRY_LOCK_MUTEX_RESULT_TYPE res = JVX_TRY_LOCK_MUTEX_NO_SUCCESS;
		JVX_TRY_LOCK_MUTEX(res, theLock);
		if (res == JVX_TRY_LOCK_MUTEX_SUCCESS)
			return true;
	}
	return false;	
}

// =================================================================

// Do this before:  jvxrtst(&jvxos)

void jvx_init_text_log(jvxrtst_backup& bkp)
{
	bkp.dbgLevel = 0;
	bkp.dbgModule = true;
	bkp.jvxlst_buf = NULL;
	bkp.jvxlst_buf_sz = 0;
	bkp.theModuleName = "no-module-name";
	bkp.theTextLogger_hdl = NULL;
	bkp.theTextLogger_obj = NULL;
	bkp.theToolsHost = NULL;
}

void jvx_terminate_text_log(jvxrtst_backup& bkp)
{
	bkp.dbgLevel = 0;
	bkp.dbgModule = false;
	bkp.jvxlst_buf = NULL;
	bkp.jvxlst_buf_sz = 0;
	bkp.theModuleName = "no-module-name";
	bkp.theTextLogger_hdl = NULL;
	bkp.theTextLogger_obj = NULL;
	bkp.theToolsHost = NULL;
}

void 
jvx_request_text_log(jvxrtst_backup& bkp)
{
	jvxErrorType resL = JVX_NO_ERROR;

	if (bkp.jvxlst_buf_sz == 0)
	{
		std::cout << "Error: configuration of text log stream with zero character buffer size." << std::endl;
		return;
	}

	if (bkp.theToolsHost)
	{
		resL = bkp.theToolsHost->reference_tool(JVX_COMPONENT_SYSTEM_TEXT_LOG, &bkp.theTextLogger_obj, 0, "jvxTSystemTextLog"); // <- main text logger object
		if ((resL == JVX_NO_ERROR) && bkp.theTextLogger_obj)
		{
			resL = bkp.theTextLogger_obj->request_specialization(reinterpret_cast<jvxHandle**>(&bkp.theTextLogger_hdl), NULL, NULL);
		}
		else
		{
			std::cout << "Error: Failed to open logger object, reason: " << jvxErrorType_txt(resL) << std::endl;
		}
	}
	else
	{
		std::cout << "Error: Failed to open logger object for component< " << bkp.theModuleName << "> since tools host reference is not vailable." << std::endl;
	}

	if (bkp.theTextLogger_hdl)
	{
		bkp.theTextLogger_hdl->debug_config(&bkp.dbgLevel, bkp.theModuleName.c_str(), &bkp.dbgModule);

		if (!bkp.dbgModule)
		{
			// Only if the options are setup rpoperly, we will hold a nonzero reference
			resL = bkp.theToolsHost->return_reference_tool(JVX_COMPONENT_SYSTEM_TEXT_LOG, bkp.theTextLogger_obj);
			bkp.theTextLogger_obj = NULL;
			bkp.theTextLogger_hdl = NULL;
			bkp.jvxos.setActive(false);
		}
		else
		{
			if (bkp.dbgLevel)
			{
				bkp.jvxos.setReference(bkp.theTextLogger_hdl, bkp.jvxlst_buf, bkp.jvxlst_buf_sz);
				bkp.jvxos.set_module_name(bkp.theModuleName);
			}
		}
	}
};

void jvx_return_text_log(jvxrtst_backup& bkp)
{
	jvxErrorType resL = JVX_NO_ERROR;
	if (bkp.theTextLogger_hdl)
	{
		bkp.jvxos.unsetReference();
		resL = bkp.theToolsHost->return_reference_tool(JVX_COMPONENT_SYSTEM_TEXT_LOG, bkp.theTextLogger_obj);
		bkp.theTextLogger_obj = NULL;
		bkp.theTextLogger_hdl = NULL;
	}
}

bool 
jvx_try_lock_text_log(jvxrtst_backup& bkp)
{
	return bkp.jvxos.try_lock();
}

void 
jvx_lock_text_log(jvxrtst_backup& bkp)
{
	bkp.jvxos.lock();
}

void 
jvx_unlock_text_log(jvxrtst_backup& bkp)
{
	bkp.jvxos.unlock();
}

CjvxReportCommandRequest* jvx_command_request_copy_alloc(const CjvxReportCommandRequest& in)
{
	CjvxReportCommandRequest* ret = nullptr;
	jvxReportCommandDataType tp = in.datatype();
	switch (tp)
	{
	case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_IDENT:
		JVX_DSP_SAFE_ALLOCATE_OBJECT(ret, CjvxReportCommandRequest_id(in));
		break;
	case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SCHEDULE:
		JVX_DSP_SAFE_ALLOCATE_OBJECT(ret, CjvxReportCommandRequest_rm(in));
		break;
	case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_UID:
		JVX_DSP_SAFE_ALLOCATE_OBJECT(ret, CjvxReportCommandRequest_uid(in));
		break;
	case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SEQ:
		JVX_DSP_SAFE_ALLOCATE_OBJECT(ret, CjvxReportCommandRequest_seq(in));
		break;
	case jvxReportCommandDataType::JVX_REPORT_COMMAND_TYPE_SS:
		JVX_DSP_SAFE_ALLOCATE_OBJECT(ret, CjvxReportCommandRequest_ss(in));
		break;
	default:
		JVX_DSP_SAFE_ALLOCATE_OBJECT(ret, CjvxReportCommandRequest(in));
	}
	return ret;
}

void jvx_command_request_copy_dealloc(CjvxReportCommandRequest* in)
{
	JVX_DSP_SAFE_DELETE_OBJECT(in);
}