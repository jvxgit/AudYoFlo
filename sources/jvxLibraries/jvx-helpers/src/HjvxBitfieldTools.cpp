#include "jvx-helpers.h"

#ifdef JVX_NUMBER_32BITS_BITFIELD

jvxSize jvx_bitFieldSize()
{
	return(jvxBitField::numberBits());
}

jvxSize jvx_bitNumSelections(jvxBitField entry)
{
	return entry.bit_setcount();
}
jvxBool jvx_bitTest(jvxBitField entry, jvxSize position)
{
	return(entry.bit_test(position));
}

jvxSize jvx_bitCountFilled(jvxBitField entry)
{
	return entry.bit_count_filled();
}

jvxBool jvx_cbitTest(jvxCBitField entry, jvxSize position)
{
	return( (entry & ((jvxCBitField)1 << position)) != 0);
}

void jvx_bitSet(jvxBitField& entry, jvxSize position)
{
	entry.bit_set(position);
}

void jvx_bitZero(jvxBitField& entry)
{
	entry.zero();
}

void jvx_bitFull(jvxBitField& entry)
{
	entry.full();
}

void jvx_cbitSet(jvxCBitField& entry, jvxSize position)
{
	entry |= ((jvxCBitField)1 << position);
}

void jvx_bitZSet(jvxBitField& entry, jvxSize position)
{
	entry.bit_zset(position);
}

void jvx_cbitZSet(jvxCBitField& entry, jvxSize position)
{
	entry = ((jvxCBitField)1 << position);
}

void jvx_bitClear(jvxBitField& entry, jvxSize position)
{
	entry.bit_clear(position);
}

void jvx_cbitClear(jvxCBitField& entry, jvxSize position)
{
	entry &= ~((jvxCBitField)1 << position);
}

void jvx_bitFClear(jvxBitField& entry)
{
	entry.bit_fclear();
}

void jvx_bitFSet(jvxBitField& entry)
{
	entry.bit_fset();
}

void jvx_bitFSet(jvxCBitField& entry)
{
	entry = (jvxCBitField)-1;
}

void jvx_bitFClear(jvxCBitField& entry)
{
	entry = 0;
}

void jvx_bitToggle(jvxBitField& entry, jvxSize position)
{
	assert(JVX_CHECK_SIZE_SELECTED(position));
	if (jvx_bitTest(entry, position))
	{
		jvx_bitClear(entry, position);
	}
	else
	{
		jvx_bitSet(entry, position);
	}
}

void jvx_cbitToggle(jvxCBitField& entry, jvxSize position)
{
	assert(JVX_CHECK_SIZE_SELECTED(position));
	if (jvx_cbitTest(entry, position))
	{
		jvx_cbitClear(entry, position);
	}
	else
	{
		jvx_cbitSet(entry, position);
	}
}

jvxUInt32 jvx_bitFieldValue32(jvxBitField entry)
{
	return(entry.toUInt32());
}

jvxCBitField jvx_bitFieldCBitField(jvxBitField entry)
{
	return(entry.toCBitField());
}

/*
jvxUInt32 jvx_bitFieldValue64(jvxBitField& entry)
{
	return(entry.toUInt64());
}
*/

#ifndef JVX_COMPILE_SMALL
jvxBitField jvx_string2BitField(const std::string& str, jvxBool& err)
{
	jvxBitField retVal;
	err = (retVal.fromString(str) == false);
	return(retVal);
}

jvxBitField jvx_string2BitField2SelId(const std::string& str, jvxSize ll, jvxBool& err)
{
	jvxBitField retVal;
	err = (retVal.fromStringSelId(str, ll) == false);
	return(retVal);
}

std::string jvx_bitField2String(jvxBitField& entry)
{
	return(entry.toString());
}

#endif

int jvx_maxNumberSelections()
{
	return(JVX_NUMBER_32BITS_BITFIELD * sizeof(jvxUInt32) * 8);
}

// ==========================================================
// Some C bitfield operations
// ==========================================================

jvxBool jvx_bitTest(jvxCBitField entry, jvxSize position)
{
	assert(JVX_CHECK_SIZE_SELECTED(position));
	return((entry & ((jvxCBitField)1<<position)) != 0);
}

void jvx_bitSet(jvxCBitField& entry, jvxSize position)
{
	assert(JVX_CHECK_SIZE_SELECTED(position));
	entry |= ((jvxCBitField)1<<position);
}

void jvx_bitZSet(jvxCBitField& entry, jvxSize position)
{
	assert(JVX_CHECK_SIZE_SELECTED(position));
	entry = ((jvxCBitField)1<<position);
}

void jvx_bitClear(jvxCBitField& entry, jvxSize position)
{
	assert(JVX_CHECK_SIZE_SELECTED(position));
	entry &= ~((jvxCBitField)1 << position);
}

void jvx_bitToggle(jvxCBitField& entry, jvxSize position)
{
	assert(JVX_CHECK_SIZE_SELECTED(position));
	if (jvx_bitTest(entry, position))
	{
		jvx_bitClear(entry, position);
	}
	else
	{
		jvx_bitSet(entry, position);
	}
}

jvxUInt32 jvx_bitFieldValue32(jvxCBitField entry)
{
	return((jvxUInt32) entry);
}
// ==========================================================


#else

jvxSize jvx_bitFieldSize()
{
	return(sizeof(jvxBitField));
}

jvxBool jvx_bitTest(jvxBitField entry, jvxSize position)
{
	assert(JVX_CHECK_SIZE_SELECTED(position));
	return((entry & ((jvxBitField)1<<position)) != 0);
}

void jvx_bitSet(jvxBitField& entry, jvxSize position)
{
	assert(JVX_CHECK_SIZE_SELECTED(position));
	entry |= ((jvxBitField)1<<position);
}

void jvx_bitZSet(jvxBitField& entry, jvxSize position)
{
	assert(JVX_CHECK_SIZE_SELECTED(position));
	entry = ((jvxBitField)1<<position);
}

void jvx_bitClear(jvxBitField& entry, jvxSize position)
{
	assert(JVX_CHECK_SIZE_SELECTED(position));
	entry &= ~((jvxBitField)1 << position);
}

void jvx_bitToggle(jvxBitField& entry, jvxSize position)
{
	assert(JVX_CHECK_SIZE_SELECTED(position));
	if (jvx_bitTest(entry, position))
	{
		jvx_bitClear(entry, position);
	}
	else
	{
		jvx_bitSet(entry, position);
	}
}

jvxUInt32 jvx_bitFieldValue32(jvxBitField entry)
{
	return((jvxUInt32) entry);
}

jvxCBitField jvx_bitFieldCBitField(jvxBitField entry)
{
	return(entry);
}

#ifndef JVX_COMPILE_SMALL
jvxBitField jvx_string2BitField(std::string& str, jvxBool& err)
{
	jvxBitField out = 0;
	err = false;
	jvxSize szFld = str.size();
	jvxSize bfSize = jvx_bitFieldSize();
	if(szFld > 2*bfSize + 2)
	{
		std::cout << "Warning: Bitfield string " << str << " for selection is too large, " << szFld << " in string whereas system is configured for " << bfSize + 2 << std::endl;
	}
	out = JVX_CONVERT_BITFIELD_STRING_FUNC(str.c_str(), NULL, 0);
	return(out);
}

std::string jvx_bitField2String(jvxBitField& entry)
{
	char myBuf[2*sizeof(jvxBitField)+3];
	memset(myBuf, 0, 2*sizeof(jvxBitField)+3);
	std::string prodToken = "0x%0" + jvx_int2String(sizeof(jvxBitField)) + "x";
#if _MSC_VER >= 1600
	sprintf_s(myBuf, 2*sizeof(jvxBitField)+3,prodToken.c_str(), entry);
#else
	sprintf(myBuf, prodToken.c_str(), entry);
#endif
	std::string retVal = myBuf;
	return(retVal);
}
#endif

int jvx_maxNumberSelections()
{
	return(sizeof(jvxBitField) * 8);
}

#endif
