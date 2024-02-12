#ifndef __CJVXMATLABTOCCONVERTER_H__
#define __CJVXMATLABTOCCONVERTER_H__

#include "jvx.h"
#include "localMexIncludes.h"

#define JVX_SIZE_2_MAT_IDX(idx) (mwIndex) idx

#define JVX_MEX_READ_SINGLE_FROM_STRUCT(res, token, curPtr, value) \
	{ \
		res = JVX_ERROR_ELEMENT_NOT_FOUND; \
		std::vector<jvxValue> vals;\
		if (val = CjvxMatlabToCConverter::jvx_mex_read_single_reference(curPtr, token)) \
		{ \
			vals = CjvxMatlabToCConverter::jvx_mex_2_numeric(val); \
			if (vals.size()) \
			{ \
				vals[0].toContent(&value); \
				res = JVX_NO_ERROR; \
			} \
		} \
	}

#define JVX_MEX_READ_STRING_FROM_STRUCT(res, token, curPtr, value) \
	{ \
		res = JVX_ERROR_ELEMENT_NOT_FOUND; \
		if (val = CjvxMatlabToCConverter::jvx_mex_read_single_reference(curPtr, token)) \
		{ \
			if(mxIsChar(val)) \
			{ \
				value.assign(CjvxMatlabToCConverter::jvx_mex_2_cstring(val)); \
				res = JVX_NO_ERROR; \
			} \
			else \
			{ \
				res = JVX_ERROR_INVALID_SETTING; \
			} \
		} \
	}

#define JVX_MEX_READ_SELECTION_FROM_STRUCT(res, token, curPtr, value, nOptions, cType) \
	{ \
		jvxBool err = false; \
		jvxApiString astr; \
		JVX_MEX_READ_STRING_FROM_STRUCT(res, token, curPtr, astr); \
		if(res != JVX_NO_ERROR) \
		{ \
			std::string tokenP = token; \
			tokenP += "/jvxSelection_value"; \
			JVX_MEX_READ_STRING_FROM_STRUCT(res, tokenP, curPtr, astr); \
		} \
		if(res == JVX_NO_ERROR) \
		{ \
			jvxBitField bf = jvx_string2BitField(astr.std_str(), err); \
			if(!err) \
			{ \
				jvxSize idx = jvx_bitfieldSelection2Id(bf, nOptions); \
				if (JVX_CHECK_SIZE_SELECTED(idx)) \
				{ \
					value = (cType)idx; \
				} \
				else \
				{ \
					res = JVX_ERROR_ID_OUT_OF_BOUNDS; \
				} \
			} \
			else \
			{ \
				res = JVX_ERROR_PARSE_ERROR; \
			} \
		} \
	}

class CjvxMatlabToCConverter
{
public:
	template<typename T>
	static jvxErrorType mexArgument2Type(T& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries, T defVal, jvxSize idxLim, jvxTextHelpers* strPtr)
	{
		jvxErrorType res = JVX_NO_ERROR;
		std::string cpName = "unknown";
		jvxInt32 valI = -1;
		value = defVal;

		res = mexArgument2Index<jvxInt32>(valI, thePointer, idx, numEntries);
		if (res == JVX_NO_ERROR)
		{
			if ((valI >= 0) && (valI < JVX_COMPONENT_ALL_LIMIT))
			{
				value = (T)valI;
			}
			else
			{
				res = JVX_ERROR_ID_OUT_OF_BOUNDS;
			}
		}
		else
		{
			res = mexArgument2String(cpName, thePointer, idx, numEntries);
			if (res == JVX_NO_ERROR)
			{
				jvxSize idx_entry = 0;
				while (idx_entry < JVX_COMPONENT_ALL_LIMIT)
				{
					if (
						(strPtr[idx_entry].friendly == cpName) ||
						(strPtr[idx_entry].full == cpName))
					{
						break;
					}
					idx_entry++;
				}
				if (idx_entry == JVX_COMPONENT_ALL_LIMIT)
				{
					res = JVX_ERROR_INVALID_SETTING;
				}
				else
				{
					value = (T)idx_entry;
				}
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
		}
		return(res);
	};

	template<typename T>
	static jvxErrorType mexArgument2Index(T& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries)
	{
		jvxInt64 valueT = -1;
		jvxErrorType res = JVX_NO_ERROR;

		if (idx < numEntries)
		{
			if (thePointer[idx])
			{
				if (mxIsDouble(thePointer[idx]))
				{
					valueT = JVX_DDOUBLE2INT64(*((double*)mxGetData(thePointer[idx])));
				}
				else if (mxIsSingle(thePointer[idx]))
				{
					valueT = JVX_DFLOAT2INT64(*((float*)mxGetData(thePointer[idx])));
				}
				else if (mxIsInt64(thePointer[idx]))
				{
					valueT = (*((jvxInt64*)mxGetData(thePointer[idx])));
				}
				else if (mxIsInt32(thePointer[idx]))
				{
					valueT = (*((jvxInt32*)mxGetData(thePointer[idx])));
				}
				else if (mxIsInt16(thePointer[idx]))
				{
					valueT = (*((jvxInt16*)mxGetData(thePointer[idx])));
				}
				else if (mxIsInt8(thePointer[idx]))
				{
					valueT = (*((jvxInt8*)mxGetData(thePointer[idx])));
				}
				else if (mxIsUint64(thePointer[idx]))
				{
					valueT = (*((jvxUInt64*)mxGetData(thePointer[idx])));
				}
				else if (mxIsUint32(thePointer[idx]))
				{
					valueT = (*((jvxUInt32*)mxGetData(thePointer[idx])));
				}
				else if (mxIsUint16(thePointer[idx]))
				{
					valueT = (*((jvxUInt16*)mxGetData(thePointer[idx])));
				}
				else if (mxIsUint8(thePointer[idx]))
				{
					valueT = (*((jvxUInt8*)mxGetData(thePointer[idx])));
				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}

				value = (T)valueT;
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
	};

	static jvxErrorType convert_mat_buf_c_buf_1_x_N(jvxHandle* data_setprops, jvxDataFormat format, jvxSize N, const mxArray* prhs);

	static std::string jvx_mex_2_cstring(const mxArray* phs);
	static std::vector<jvxValue> jvx_mex_2_numeric(const mxArray* phs, jvxSize lineNo = 0);
	static const mxArray* jvx_mex_lookup_strfield_core(const mxArray* strEntry, const std::string& key, jvxSize curLevel, int levelMax, jvxBool& moreLevels);
	static const mxArray* jvx_mex_lookup_strfield(const mxArray* strEntry, const std::string& key, jvxSize levelMax = JVX_SIZE_UNSELECTED);
	static const mxArray* jvx_mex_read_single_reference(const mxArray* fld, const std::string& expr);
	
	static jvxErrorType mexArgument2Data(jvxData& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries);
	static jvxErrorType mexArgument2Bool(bool& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries);
	static jvxErrorType mexArgument2String(std::string& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries);
	static jvxErrorType mexArgument2ComponentIdentification(jvxComponentIdentification& value, const mxArray** thePointer, jvxSize idx, jvxSize numEntries);
	static jvxErrorType mexArgument2String(std::vector<std::string>& valueList, jvxSize expectedNumberEntries, const mxArray** thePointer, jvxSize idx, jvxSize numEntries);
	static jvxErrorType mexArgument2StringList(std::vector<std::string>& valueList, const mxArray** thePointer, jvxSize idx, jvxSize numEntries);
};

#endif