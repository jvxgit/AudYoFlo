#ifndef __CJVXPYTHONTOCCONVERTER_H__
#define __CJVXPYTHONTOCCONVERTER_H__

#include "jvx.h"
#include "localPybindIncludes.h"

// ==============================================================================
// Python analog of CjvxMatlabToCConverter: converts pybind11 objects into
// C/jvx values. Argument lists are passed as std::vector<pybind11::object>
// (analog of const mxArray** thePointer, jvxSize idx, jvxSize numEntries) so
// that the existing "read the n-th argument" call pattern used throughout
// CjvxPropertiesToMatlabConverter (e.g. multi-argument selection lists) can
// be mirrored 1:1.
// ==============================================================================

#define JVX_PY_READ_SINGLE_FROM_STRUCT(res, token, curPtr, value) \
	{ \
		res = JVX_ERROR_ELEMENT_NOT_FOUND; \
		std::vector<jvxValue> vals;\
		pybind11::object val = CjvxPythonToCConverter::jvx_py_read_single_reference(curPtr, token); \
		if (!val.is_none()) \
		{ \
			vals = CjvxPythonToCConverter::jvx_py_2_numeric(val); \
			if (vals.size()) \
			{ \
				vals[0].toContent(&value); \
				res = JVX_NO_ERROR; \
			} \
		} \
	}

#define JVX_PY_READ_STRING_FROM_STRUCT(res, token, curPtr, value) \
	{ \
		res = JVX_ERROR_ELEMENT_NOT_FOUND; \
		pybind11::object val = CjvxPythonToCConverter::jvx_py_read_single_reference(curPtr, token); \
		if (!val.is_none()) \
		{ \
			if(pybind11::isinstance<pybind11::str>(val)) \
			{ \
				value.assign(CjvxPythonToCConverter::jvx_py_2_cstring(val)); \
				res = JVX_NO_ERROR; \
			} \
			else \
			{ \
				res = JVX_ERROR_INVALID_SETTING; \
			} \
		} \
	}

#define JVX_PY_READ_SELECTION_FROM_STRUCT(res, token, curPtr, value, nOptions, cType) \
	{ \
		jvxBool err = false; \
		jvxApiString astr; \
		JVX_PY_READ_STRING_FROM_STRUCT(res, token, curPtr, astr); \
		if(res != JVX_NO_ERROR) \
		{ \
			std::string tokenP = token; \
			tokenP += "/jvxSelection_value"; \
			JVX_PY_READ_STRING_FROM_STRUCT(res, tokenP, curPtr, astr); \
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

class CjvxPythonToCConverter
{
public:
	template<typename T>
	static jvxErrorType pyArgument2Type(T& value, const std::vector<pybind11::object>& thePointer, jvxSize idx, jvxSize numEntries, T defVal, jvxSize idxLim, jvxTextHelpers* strPtr)
	{
		jvxErrorType res = JVX_NO_ERROR;
		std::string cpName = "unknown";
		jvxInt32 valI = -1;
		value = defVal;

		res = pyArgument2Index<jvxInt32>(valI, thePointer, idx, numEntries);
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
			res = pyArgument2String(cpName, thePointer, idx, numEntries);
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

	//! Read the idx-th argument and interpret it as an integral index. Accepts
	//! Python int/float/bool/numpy-scalars via pybind11's built-in numeric casts.
	template<typename T>
	static jvxErrorType pyArgument2Index(T& value, const std::vector<pybind11::object>& thePointer, jvxSize idx, jvxSize numEntries)
	{
		jvxErrorType res = JVX_NO_ERROR;

		if (idx < numEntries)
		{
			if (!thePointer[idx].is_none())
			{
				try
				{
					value = thePointer[idx].cast<T>();
				}
				catch (const pybind11::cast_error&)
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
	};

	//! Returns a non-owning pointer into the numpy array's buffer (valid only
	//! as long as prhs stays alive), analog of mexArgumentToBufPtr<T>.
	template<typename T> static T* pyArgumentToBufPtr(jvxDataFormat* format, jvxSize* N, jvxSize* M, const pybind11::object& prhs, jvxBool* isCplx = nullptr)
	{
		if (!pybind11::isinstance<pybind11::array>(prhs))
		{
			if (format) *format = JVX_DATAFORMAT_NONE;
			return nullptr;
		}

		pybind11::array arr = prhs.cast<pybind11::array>();

		if (M)
		{
			*M = (arr.ndim() == 2) ? (jvxSize)arr.shape(0) : 1;
		}
		if (N)
		{
			*N = (arr.ndim() == 2) ? (jvxSize)arr.shape(1) : (jvxSize)arr.size();
		}

		if (isCplx)
		{
			// Complex numpy dtypes are not auto-detected here; handled explicitly
			// by callers that know they expect complex data (see pyReturnGenericNumeric).
		}

		if (format)
		{
			if (arr.dtype().is(pybind11::dtype::of<double>()))
			{
#ifdef JVX_DATA_FORMAT_DOUBLE
				*format = JVX_DATAFORMAT_DATA;
#else
				*format = JVX_DATAFORMAT_DOUBLE;
#endif
			}
			else if (arr.dtype().is(pybind11::dtype::of<float>()))
			{
#ifdef JVX_DATA_FORMAT_DOUBLE
				*format = JVX_DATAFORMAT_DOUBLE;
#else
				*format = JVX_DATAFORMAT_DATA;
#endif
			}
			else if (arr.dtype().is(pybind11::dtype::of<jvxInt64>()))
			{
				*format = JVX_DATAFORMAT_64BIT_LE;
			}
			else if (arr.dtype().is(pybind11::dtype::of<jvxInt32>()))
			{
				*format = JVX_DATAFORMAT_32BIT_LE;
			}
			else if (arr.dtype().is(pybind11::dtype::of<jvxInt16>()))
			{
				*format = JVX_DATAFORMAT_16BIT_LE;
			}
			else if (arr.dtype().is(pybind11::dtype::of<jvxInt8>()))
			{
				*format = JVX_DATAFORMAT_8BIT;
			}
			else if (arr.dtype().is(pybind11::dtype::of<jvxUInt64>()))
			{
				*format = JVX_DATAFORMAT_U64BIT_LE;
			}
			else if (arr.dtype().is(pybind11::dtype::of<jvxUInt32>()))
			{
				*format = JVX_DATAFORMAT_U32BIT_LE;
			}
			else if (arr.dtype().is(pybind11::dtype::of<jvxUInt16>()))
			{
				*format = JVX_DATAFORMAT_U16BIT_LE;
			}
			else if (arr.dtype().is(pybind11::dtype::of<jvxUInt8>()))
			{
				*format = JVX_DATAFORMAT_U8BIT;
			}
			else
			{
				*format = JVX_DATAFORMAT_NONE;
				return nullptr;
			}
		}

		return (T*)arr.mutable_data();
	};

	static jvxErrorType convert_py_buf_c_buf_1_x_N(jvxHandle* data_setprops, jvxDataFormat format, jvxSize N, const pybind11::object& prhs);

	static std::string jvx_py_2_cstring(const pybind11::object& phs);
	static std::vector<jvxValue> jvx_py_2_numeric(const pybind11::object& phs, jvxSize lineNo = 0);
	static intptr_t jvx_py_2_intptr(const pybind11::object& phs);
	static pybind11::object jvx_py_lookup_dictfield_core(const pybind11::object& strEntry, const std::string& key, jvxSize curLevel, int levelMax, jvxBool& moreLevels);
	static pybind11::object jvx_py_lookup_dictfield(const pybind11::object& strEntry, const std::string& key, jvxSize levelMax = JVX_SIZE_UNSELECTED);
	static pybind11::object jvx_py_read_single_reference(const pybind11::object& fld, const std::string& expr);

	static jvxErrorType pyArgument2Data(jvxData& value, const std::vector<pybind11::object>& thePointer, jvxSize idx, jvxSize numEntries);
	static jvxErrorType pyArgument2Bool(bool& value, const std::vector<pybind11::object>& thePointer, jvxSize idx, jvxSize numEntries);
	static jvxErrorType pyArgument2String(std::string& value, const std::vector<pybind11::object>& thePointer, jvxSize idx, jvxSize numEntries);
	static jvxErrorType pyArgument2ComponentIdentification(jvxComponentIdentification& value, const std::vector<pybind11::object>& thePointer, jvxSize idx, jvxSize numEntries);
	static jvxErrorType pyArgument2String(std::vector<std::string>& valueList, jvxSize expectedNumberEntries, const std::vector<pybind11::object>& thePointer, jvxSize idx, jvxSize numEntries);
	static jvxErrorType pyArgument2StringList(std::vector<std::string>& valueList, const std::vector<pybind11::object>& thePointer, jvxSize idx, jvxSize numEntries);
};

#endif
