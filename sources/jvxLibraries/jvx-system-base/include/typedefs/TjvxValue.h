#ifndef __JVXVALUE_H__
#define __JVXVALUE_H__

#include "jvx_system.h"

class jvxValue
{
public:
	union
	{
		jvxSize valS;
		jvxData valD;
		jvxInt16 valI16;
		jvxInt32 valI32;
		jvxInt64 valI64;
		jvxInt8 valI8;
		jvxUInt16 valUI16;
		jvxUInt32 valUI32;
		jvxUInt64 valUI64;
		jvxUInt8 valUI8;
	} ct;

	jvxDataFormat tp;
  virtual ~jvxValue(){};
	jvxValue(const jvxValue& elm)
	{
		tp = elm.tp;
		switch (elm.tp)
		{
		case JVX_DATAFORMAT_DATA:
			ct.valD = elm.ct.valD;
			break;
		case JVX_DATAFORMAT_SIZE:
			ct.valS = elm.ct.valS;
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			ct.valI16 = elm.ct.valI16;
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			ct.valI32 = elm.ct.valI32;
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			ct.valI64 = elm.ct.valI64;
			break;
		case JVX_DATAFORMAT_8BIT:
			ct.valI8 = elm.ct.valI8;
			break;

		case JVX_DATAFORMAT_U16BIT_LE:
			ct.valI16 = elm.ct.valUI16;
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			ct.valI32 = elm.ct.valUI32;
			break;
		case JVX_DATAFORMAT_U64BIT_LE:
			ct.valI64 = elm.ct.valUI64;
			break;
		case JVX_DATAFORMAT_U8BIT:
			ct.valI8 = elm.ct.valUI8;
			break;
		default:
			assert(0);
		}
	};
	jvxValue()
	{
		tp = JVX_DATAFORMAT_DATA;
		ct.valD = 0;
	};
	jvxValue(jvxData val)
	{
		tp = JVX_DATAFORMAT_DATA;
		ct.valD = val;
	};

	jvxValue(jvxSize val)
	{
#ifdef JVX_SIZE_IS_UINT32
		assert(sizeof(jvxSize) == sizeof(jvxUInt32));
#endif
#ifdef JVX_SIZE_IS_UINT64
		assert(sizeof(jvxSize) == sizeof(jvxUInt64));
#endif
		tp = JVX_DATAFORMAT_SIZE;
		ct.valS = val;
	};

	jvxValue(jvxInt16 valI16)
	{
		tp = JVX_DATAFORMAT_16BIT_LE;
		ct.valI16 = valI16;
	};

	jvxValue(jvxInt32 valI32)
	{
		tp = JVX_DATAFORMAT_32BIT_LE;
		ct.valI32 = valI32;
	};
	jvxValue(jvxInt64 valI64)
	{
		tp = JVX_DATAFORMAT_64BIT_LE;
		ct.valI64 = valI64;
	};
	jvxValue(jvxInt8 valI8)
	{
		tp = JVX_DATAFORMAT_8BIT;
		ct.valI8 = valI8;
	};

	jvxValue(jvxUInt16 valUI16)
	{
		tp = JVX_DATAFORMAT_U16BIT_LE;
		ct.valUI16 = valUI16;
	};

#ifdef JVX_SIZE_IS_UINT32
	jvxValue(jvxUInt32 valUI32, jvxBool isUInt32)
#else
	jvxValue(jvxUInt32 valUI32)
#endif
	{
		tp = JVX_DATAFORMAT_U32BIT_LE;
		ct.valUI32 = valUI32;
	};

#ifdef JVX_SIZE_IS_UINT64
	jvxValue(jvxUInt64 valUI64, jvxBool isUInt64)
#else
	jvxValue(jvxUInt64 valUI64)
#endif
	{
		tp = JVX_DATAFORMAT_U64BIT_LE;
		ct.valUI64 = valUI64;
	};

	jvxValue(jvxUInt8 valUI8)
	{
		tp = JVX_DATAFORMAT_U8BIT;
		ct.valI8 = valUI8;
	};

	jvxValue(jvxBool valB)
	{
		tp = JVX_DATAFORMAT_16BIT_LE;
		ct.valI16 = 0;
		if (valB)
			ct.valI16 = 1;
	};

#ifndef JVX_CBOOL_IS_UINT16
	jvxValue(jvxCBool valB)
	{
		tp = JVX_DATAFORMAT_16BIT_LE;
		ct.valI16 = 0;
		if (valB == c_true)
			ct.valI16 = 1;
	};
#endif

	void assign(const jvxData& val)
	{
		tp = JVX_DATAFORMAT_DATA;
		ct.valD = val;
	};

	void assign(const jvxSize val)
	{
		tp = JVX_DATAFORMAT_SIZE;
		ct.valS = val;
	};

	void assign(const jvxInt16& valI16)
	{
		tp = JVX_DATAFORMAT_16BIT_LE;
		ct.valI16 = valI16;
	};

	void assign(const jvxInt32& valI32)
	{
		tp = JVX_DATAFORMAT_32BIT_LE;
		ct.valI32 = valI32;
	};
	void assign(const jvxInt64& valI64)
	{
		tp = JVX_DATAFORMAT_64BIT_LE;
		ct.valI64 = valI64;
	};
	void assign(const jvxInt8& valI8)
	{
		tp = JVX_DATAFORMAT_8BIT;
		ct.valI8 = valI8;
	};

	void assign(const jvxUInt16& valUI16)
	{
		tp = JVX_DATAFORMAT_U16BIT_LE;
		ct.valUI16 = valUI16;
	};

#ifdef JVX_SIZE_IS_UINT32
	void assign(const jvxUInt32& valUI32, jvxBool isUInt32)
#else
	void assign(const jvxUInt32& valUI32)
#endif
	{
		tp = JVX_DATAFORMAT_U32BIT_LE;
		ct.valUI32 = valUI32;
	};

#ifdef JVX_SIZE_IS_UINT64
	void assign(const jvxUInt64& valUI64, jvxBool isUInt64)
#else
	void assign(const jvxUInt64& valUI64)
#endif
	{
		tp = JVX_DATAFORMAT_U64BIT_LE;
		ct.valUI64 = valUI64;
	};

	void assign(const jvxUInt8& valUI8)
	{
		tp = JVX_DATAFORMAT_U8BIT;
		ct.valUI8 = valUI8;
	};


	void assign(const jvxBool& valB)
	{
		tp = JVX_DATAFORMAT_16BIT_LE;
		ct.valI16 = 0;
		if (valB)
			ct.valI16 = 1;
	};

#ifndef JVX_CBOOL_IS_UINT16

	void assign(const jvxCBool& valB)
	{
		tp = JVX_DATAFORMAT_16BIT_LE;
		ct.valI16 = 0;
		if (valB == c_true)
			ct.valI16 = 1;
	};
#endif

	virtual jvxErrorType toContent(jvxData* val)
	{
		if (val)
		{
			switch (tp)
			{
			case JVX_DATAFORMAT_SIZE:
				*val = (jvxData)ct.valS;
				break;
			case JVX_DATAFORMAT_DATA:
				*val = ct.valD;
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				*val = (jvxData)ct.valI16;
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				*val = (jvxData)ct.valI32;
				break;
			case JVX_DATAFORMAT_64BIT_LE:
				*val = (jvxData)ct.valI64;
				break;
			case JVX_DATAFORMAT_8BIT:
				*val = (jvxData)ct.valI8;
				break;
			case JVX_DATAFORMAT_U16BIT_LE:
				*val = (jvxData)ct.valUI16;
				break;
			case JVX_DATAFORMAT_U32BIT_LE:
				*val = (jvxData)ct.valUI32;
				break;
			case JVX_DATAFORMAT_U64BIT_LE:
				*val = (jvxData)ct.valUI64;
				break;
			case JVX_DATAFORMAT_U8BIT:
				*val = (jvxData)ct.valUI8;
				break;
			default:
				assert(0);
				return(JVX_ERROR_INVALID_SETTING);
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	};

	jvxErrorType toContent(jvxSize* val) const
	{
		if (val)
		{
			switch (tp)
			{
			case JVX_DATAFORMAT_SIZE:
				*val = ct.valS;
				break;
			case JVX_DATAFORMAT_DATA:
				*val = JVX_DATA2SIZE(ct.valD);
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				*val = (jvxSize)ct.valI16;
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				*val = (jvxSize)ct.valI32;
				break;
			case JVX_DATAFORMAT_64BIT_LE:
				*val = (jvxSize)ct.valI64;
				break;
			case JVX_DATAFORMAT_8BIT:
				*val = (jvxSize)ct.valI8;
				break;
			case JVX_DATAFORMAT_U16BIT_LE:
				*val = (jvxSize)ct.valUI16;
				break;
			case JVX_DATAFORMAT_U32BIT_LE:
				*val = (jvxSize)ct.valUI32;
				break;
			case JVX_DATAFORMAT_U64BIT_LE:
				*val = (jvxSize)ct.valUI64;
				break;
			case JVX_DATAFORMAT_U8BIT:
				*val = (jvxSize)ct.valUI8;
				break;
			default:
				assert(0);
				return(JVX_ERROR_INVALID_SETTING);
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	};

	// ===================================================================================

	virtual jvxErrorType toContent(jvxInt16* val) const
	{
		if (val)
		{
			switch (tp)
			{
			case JVX_DATAFORMAT_SIZE:
				*val = (jvxInt16)ct.valS;
				break;
			case JVX_DATAFORMAT_DATA:
				*val = JVX_DATA2INT16(ct.valD);
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				*val = (jvxInt16)ct.valI16;
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				*val = (jvxInt16)ct.valI32;
				break;
			case JVX_DATAFORMAT_64BIT_LE:
				*val = (jvxInt16)ct.valI64;
				break;
			case JVX_DATAFORMAT_8BIT:
				*val = (jvxInt16)ct.valI8;
				break;
			case JVX_DATAFORMAT_U16BIT_LE:
				*val = (jvxInt16)ct.valUI16;
				break;
			case JVX_DATAFORMAT_U32BIT_LE:
				*val = (jvxInt16)ct.valUI32;
				break;
			case JVX_DATAFORMAT_U64BIT_LE:
				*val = (jvxInt16)ct.valUI64;
				break;
			case JVX_DATAFORMAT_U8BIT:
				*val = (jvxInt16)ct.valUI8;
				break;
			default:
				assert(0);
				return(JVX_ERROR_INVALID_SETTING);
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	};

	virtual jvxErrorType toContent(jvxInt32* val) const
	{
		if (val)
		{
			switch (tp)
			{
			case JVX_DATAFORMAT_SIZE:
				*val = (jvxInt32)ct.valS;
				break;
			case JVX_DATAFORMAT_DATA:
				*val = JVX_DATA2INT32(ct.valD);
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				*val = (jvxInt32)ct.valI16;
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				*val = (jvxInt32)ct.valI32;
				break;
			case JVX_DATAFORMAT_64BIT_LE:
				*val = (jvxInt32)ct.valI64;
				break;
			case JVX_DATAFORMAT_8BIT:
				*val = (jvxInt32)ct.valI8;
				break;

			case JVX_DATAFORMAT_U16BIT_LE:
				*val = (jvxInt32)ct.valUI16;
				break;
			case JVX_DATAFORMAT_U32BIT_LE:
				*val = (jvxInt32)ct.valUI32;
				break;
			case JVX_DATAFORMAT_U64BIT_LE:
				*val = (jvxInt32)ct.valUI64;
				break;
			case JVX_DATAFORMAT_U8BIT:
				*val = (jvxInt32)ct.valUI8;
				break;

			default:
				assert(0);
				return(JVX_ERROR_INVALID_SETTING);
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	};
	virtual jvxErrorType toContent(jvxInt64* val) const
	{
		if (val)
		{
			switch (tp)
			{
			case JVX_DATAFORMAT_SIZE:
				*val = (jvxInt64)ct.valS;
				break;
			case JVX_DATAFORMAT_DATA:
				*val = JVX_DATA2INT64(ct.valD);
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				*val = (jvxInt64)ct.valI16;
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				*val = (jvxInt64)ct.valI32;
				break;
			case JVX_DATAFORMAT_64BIT_LE:
				*val = (jvxInt64)ct.valI64;
				break;
			case JVX_DATAFORMAT_8BIT:
				*val = (jvxInt64)ct.valI8;
				break;

			case JVX_DATAFORMAT_U16BIT_LE:
				*val = (jvxInt64)ct.valUI16;
				break;
			case JVX_DATAFORMAT_U32BIT_LE:
				*val = (jvxInt64)ct.valUI32;
				break;
			case JVX_DATAFORMAT_U64BIT_LE:
				*val = (jvxInt64)ct.valUI64;
				break;
			case JVX_DATAFORMAT_U8BIT:
				*val = (jvxInt64)ct.valUI8;
				break;

			default:
				assert(0);
				return(JVX_ERROR_INVALID_SETTING);
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	};

	virtual jvxErrorType toContent(jvxInt8* val) const
	{
		if (val)
		{
			switch (tp)
			{
			case JVX_DATAFORMAT_SIZE:
				*val = (jvxInt8)ct.valS;
				break;
			case JVX_DATAFORMAT_DATA:
				*val = JVX_DATA2INT8(ct.valD);
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				*val = (jvxInt8)ct.valI16;
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				*val = (jvxInt8)ct.valI32;
				break;
			case JVX_DATAFORMAT_64BIT_LE:
				*val = (jvxInt8)ct.valI64;
				break;
			case JVX_DATAFORMAT_8BIT:
				*val = (jvxInt8)ct.valI8;
				break;

			case JVX_DATAFORMAT_U16BIT_LE:
				*val = (jvxInt8)ct.valUI16;
				break;
			case JVX_DATAFORMAT_U32BIT_LE:
				*val = (jvxInt8)ct.valUI32;
				break;
			case JVX_DATAFORMAT_U64BIT_LE:
				*val = (jvxInt8)ct.valUI64;
				break;
			case JVX_DATAFORMAT_U8BIT:
				*val = (jvxInt8)ct.valUI8;
				break;

			default:
				assert(0);
				return(JVX_ERROR_INVALID_SETTING);
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	};

	// =============================================================

	virtual jvxErrorType toContent(jvxUInt16* val) const
	{
		if (val)
		{
			switch (tp)
			{
			case JVX_DATAFORMAT_SIZE:
				*val = (jvxInt16)ct.valS;
				break;
			case JVX_DATAFORMAT_DATA:
				*val = JVX_DATA2INT16(ct.valD);
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				*val = (jvxUInt16)ct.valI16;
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				*val = (jvxUInt16)ct.valI32;
				break;
			case JVX_DATAFORMAT_64BIT_LE:
				*val = (jvxUInt16)ct.valI64;
				break;
			case JVX_DATAFORMAT_8BIT:
				*val = (jvxUInt16)ct.valI8;
				break;
			case JVX_DATAFORMAT_U16BIT_LE:
				*val = (jvxUInt16)ct.valUI16;
				break;
			case JVX_DATAFORMAT_U32BIT_LE:
				*val = (jvxUInt16)ct.valUI32;
				break;
			case JVX_DATAFORMAT_U64BIT_LE:
				*val = (jvxUInt16)ct.valUI64;
				break;
			case JVX_DATAFORMAT_U8BIT:
				*val = (jvxUInt16)ct.valUI8;
				break;
			default:
				assert(0);
				return(JVX_ERROR_INVALID_SETTING);
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	};

#ifdef JVX_SIZE_IS_UINT32
	virtual jvxErrorType toContent(jvxUInt32* val, jvxBool isUInt32) const
#else
	virtual jvxErrorType toContent(jvxUInt32* val) const
#endif
	{
		if (val)
		{
			switch (tp)
			{
			case JVX_DATAFORMAT_SIZE:
				*val = (jvxInt32)ct.valS;
				break;
			case JVX_DATAFORMAT_DATA:
				*val = JVX_DATA2INT32(ct.valD);
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				*val = (jvxUInt32)ct.valI16;
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				*val = (jvxUInt32)ct.valI32;
				break;
			case JVX_DATAFORMAT_64BIT_LE:
				*val = (jvxUInt32)ct.valI64;
				break;
			case JVX_DATAFORMAT_8BIT:
				*val = (jvxUInt32)ct.valI8;
				break;

			case JVX_DATAFORMAT_U16BIT_LE:
				*val = (jvxUInt32)ct.valUI16;
				break;
			case JVX_DATAFORMAT_U32BIT_LE:
				*val = (jvxUInt32)ct.valUI32;
				break;
			case JVX_DATAFORMAT_U64BIT_LE:
				*val = (jvxUInt32)ct.valUI64;
				break;
			case JVX_DATAFORMAT_U8BIT:
				*val = (jvxUInt32)ct.valUI8;
				break;

			default:
				assert(0);
				return(JVX_ERROR_INVALID_SETTING);
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	};

#ifdef JVX_SIZE_IS_UINT64
	virtual jvxErrorType toContent(jvxUInt64* val, jvxBool isUInt64) const
#else
	virtual jvxErrorType toContent(jvxUInt64* val) const
#endif
	{
		if (val)
		{
			switch (tp)
			{
			case JVX_DATAFORMAT_SIZE:
				*val = (jvxInt64)ct.valS;
				break;
			case JVX_DATAFORMAT_DATA:
				*val = JVX_DATA2INT64(ct.valD);
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				*val = (jvxUInt64)ct.valI16;
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				*val = (jvxUInt64)ct.valI32;
				break;
			case JVX_DATAFORMAT_64BIT_LE:
				*val = (jvxUInt64)ct.valI64;
				break;
			case JVX_DATAFORMAT_8BIT:
				*val = (jvxUInt64)ct.valI8;
				break;

			case JVX_DATAFORMAT_U16BIT_LE:
				*val = (jvxUInt64)ct.valUI16;
				break;
			case JVX_DATAFORMAT_U32BIT_LE:
				*val = (jvxUInt64)ct.valUI32;
				break;
			case JVX_DATAFORMAT_U64BIT_LE:
				*val = (jvxUInt64)ct.valUI64;
				break;
			case JVX_DATAFORMAT_U8BIT:
				*val = (jvxUInt64)ct.valUI8;
				break;

			default:
				assert(0);
				return(JVX_ERROR_INVALID_SETTING);
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	};

	virtual jvxErrorType toContent(jvxUInt8* val) const
	{
		if (val)
		{
			switch (tp)
			{
			case JVX_DATAFORMAT_SIZE:
				*val = (jvxUInt8)ct.valS;
				break;
			case JVX_DATAFORMAT_DATA:
				*val = JVX_DATA2INT8(ct.valD);
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				*val = (jvxUInt8)ct.valI16;
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				*val = (jvxUInt8)ct.valI32;
				break;
			case JVX_DATAFORMAT_64BIT_LE:
				*val = (jvxUInt8)ct.valI64;
				break;
			case JVX_DATAFORMAT_8BIT:
				*val = (jvxUInt8)ct.valI8;
				break;

			case JVX_DATAFORMAT_U16BIT_LE:
				*val = (jvxUInt8)ct.valUI16;
				break;
			case JVX_DATAFORMAT_U32BIT_LE:
				*val = (jvxUInt8)ct.valUI32;
				break;
			case JVX_DATAFORMAT_U64BIT_LE:
				*val = (jvxUInt8)ct.valUI64;
				break;
			case JVX_DATAFORMAT_U8BIT:
				*val = (jvxUInt8)ct.valUI8;
				break;

			default:
				assert(0);
				return(JVX_ERROR_INVALID_SETTING);
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	};

	virtual jvxErrorType toContent(jvxBool* val) const
	{
		if (val)
		{
			switch (tp)
			{
			case JVX_DATAFORMAT_SIZE:
				*val = (ct.valS != 0);
				break;
			case JVX_DATAFORMAT_DATA:
				*val = (ct.valD > 0.5);
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				*val = (ct.valI16 != 0);
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				*val = (ct.valI32 != 0);
				break;
			case JVX_DATAFORMAT_64BIT_LE:
				*val = (ct.valI64 != 0);
				break;
			case JVX_DATAFORMAT_8BIT:
				*val = (ct.valI8 != 0);
				break;

			case JVX_DATAFORMAT_U16BIT_LE:
				*val = (ct.valUI16 != 0);
				break;
			case JVX_DATAFORMAT_U32BIT_LE:
				*val = (ct.valUI32 != 0);
				break;
			case JVX_DATAFORMAT_U64BIT_LE:
				*val = (ct.valUI64 != 0);
				break;
			case JVX_DATAFORMAT_U8BIT:
				*val = (ct.valUI8 != 0);
				break;
			default:
				assert(0);
				return(JVX_ERROR_INVALID_SETTING);
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	};

#ifndef JVX_CBOOL_IS_UINT16
	virtual jvxErrorType toContent(jvxCBool* val) const
	{
		if (val)
		{
			*val = c_false;
			switch (tp)
			{
			case JVX_DATAFORMAT_SIZE:
				if (ct.valS != 0)
				{
					*val = c_true;
				}
				break;
			case JVX_DATAFORMAT_DATA:
				if (ct.valD > 0.5)
				{
					*val = c_true;
				}
				break;
			case JVX_DATAFORMAT_16BIT_LE:
				if (ct.valI16 != 0)
				{
					*val = c_true;
				}
				break;
			case JVX_DATAFORMAT_32BIT_LE:
				if (ct.valI32 != 0)
				{
					*val = c_true;
				}
				break;
			case JVX_DATAFORMAT_64BIT_LE:
				if (ct.valI64 != 0)
				{
					*val = c_true;
				}
				break;
			case JVX_DATAFORMAT_8BIT:
				if (ct.valI8 != 0)
				{
					*val = c_true;
				}
				break;

			case JVX_DATAFORMAT_U16BIT_LE:
				if (ct.valUI16 != 0)
				{
					*val = c_true;
				}
				break;
			case JVX_DATAFORMAT_U32BIT_LE:
				if (ct.valUI32 != 0)
				{
					*val = c_true;
				}
				break;
			case JVX_DATAFORMAT_U64BIT_LE:
				if (ct.valUI64 != 0)
				{
					*val = c_true;
				}
				break;
			case JVX_DATAFORMAT_U8BIT:
				if (ct.valUI8 != 0)
				{
					*val = c_true;
				}
				break;
			default:
				assert(0);
				return(JVX_ERROR_INVALID_SETTING);
			}
			return(JVX_NO_ERROR);
		}
		return(JVX_ERROR_INVALID_ARGUMENT);
	};
#endif

};

#endif
