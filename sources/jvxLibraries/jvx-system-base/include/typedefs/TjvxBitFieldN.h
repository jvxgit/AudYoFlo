#ifndef __CJVXBITFIELD_H__
#define __CJVXBITFIELD_H__

#include <iostream>
#include <cmath>

class CjvxBitFieldN
{
	// Storage : [MSW .... LSW] (MOST SIGNIFICANT WORD FIRST)
	jvxUInt32 listFieldsBits[JVX_NUMBER_32BITS_BITFIELD];

public:
	CjvxBitFieldN()
	{
		memset(listFieldsBits, 0, sizeof(jvxUInt32)*JVX_NUMBER_32BITS_BITFIELD);
	};

	CjvxBitFieldN(jvxInt32 initVal)
	{
		jvxSize i;
		jvxUInt32 carry = 1;
		jvxUInt64 accu = 0;
		jvxUInt32 val2 = 0;
		memset(listFieldsBits, 0, sizeof(jvxUInt32)*JVX_NUMBER_32BITS_BITFIELD);
		if(initVal < 0)
		{
			listFieldsBits[JVX_NUMBER_32BITS_BITFIELD-1] = -initVal;
			for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
			{
				val2 = listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i];
				val2 = ~val2;
				accu = val2  + carry;
				listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i] = accu & 0xFFFFFFFF;
				carry = (accu & 0xFFFFFFFF00000000) >> 32;
			}
		}
		else
		{
			listFieldsBits[JVX_NUMBER_32BITS_BITFIELD-1] = initVal;
		}
	};

	CjvxBitFieldN(jvxUInt32 initVal)
	{
		memset(listFieldsBits, 0, sizeof(jvxUInt32)*JVX_NUMBER_32BITS_BITFIELD);
		listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1] = initVal;
	};

	CjvxBitFieldN(jvxInt64 initVal)
	{
		jvxSize i;
		jvxUInt32 carry = 1;
		jvxUInt64 accu = 0;
		jvxUInt32 val2 = 0;
		memset(listFieldsBits, 0, sizeof(jvxUInt32)*JVX_NUMBER_32BITS_BITFIELD);
		if (initVal < 0)
		{
			listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1] = ((jvxUInt64)(-initVal) & 0xFFFFFFFF);
			listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 2] = (((jvxUInt64)(-initVal) >> 32) & 0xFFFFFFFF);
			for (i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
			{
				val2 = listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i];
				val2 = ~val2;
				accu = val2 + carry;
				listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i] = accu & 0xFFFFFFFF;
				carry = (accu & 0xFFFFFFFF00000000) >> 32;
			}
		}
		else
		{
			listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1] = ((jvxUInt64)initVal & 0xFFFFFFFF);
			listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 2] = (((jvxUInt64)initVal >> 32) & 0xFFFFFFFF);
		}
	};

	CjvxBitFieldN(jvxUInt64 initVal)
	{
		//jvxUInt32 carry = 1;
		//jvxUInt64 accu = 0;
		//jvxUInt32 val2 = 0;
		memset(listFieldsBits, 0, sizeof(jvxUInt32)*JVX_NUMBER_32BITS_BITFIELD);
		assert(JVX_NUMBER_32BITS_BITFIELD >= 2);

		listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1] = (initVal & 0xFFFFFFFF);
		listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 2] = ((initVal >> 32) & 0xFFFFFFFF);
	};

	~CjvxBitFieldN(){};

	jvxUInt32* raw() {
		return (jvxUInt32*)listFieldsBits;

	}
	void zero()
	{
		memset(listFieldsBits, 0, sizeof(jvxUInt32)*JVX_NUMBER_32BITS_BITFIELD);
	};

	void full()
	{
		jvxSize i;
		for (i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			listFieldsBits[i] = (jvxUInt32)-1;
		}
	};

	void setValue(jvxUInt64 initVal)
	{
		assert(JVX_NUMBER_32BITS_BITFIELD > 1);

		memset(listFieldsBits, 0, sizeof(jvxUInt32)*JVX_NUMBER_32BITS_BITFIELD);
		listFieldsBits[JVX_NUMBER_32BITS_BITFIELD-1] = initVal & 0xFFFFFFFF;
		listFieldsBits[JVX_NUMBER_32BITS_BITFIELD-2] = (initVal >> 32) & 0xFFFFFFFF;
	};

	void setValueC(jvxCBitField initVal)
	{
		assert(JVX_NUMBER_32BITS_BITFIELD > 1);

		memset(listFieldsBits, 0, sizeof(jvxUInt32)*JVX_NUMBER_32BITS_BITFIELD);
		if (sizeof(jvxCBitField) == sizeof(jvxUInt64))
		{
			listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1] = initVal & 0xFFFFFFFF;
			listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 2] = (initVal >> 32) & 0xFFFFFFFF;
		}
		else
		{
			assert(0);
		}
	};

	jvxSize bit_setcount()
	{
		jvxSize i, j;
		jvxSize ret = 0;
		assert(JVX_NUMBER_32BITS_BITFIELD > 1);
		for (i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			if (listFieldsBits[i] != 0)
			{
				jvxUInt32 oneVal = listFieldsBits[i];
				for (j = 0; j < sizeof(jvxUInt32) * 8; j++)
				{
					if (oneVal & 0x1)
					{
						ret++;
					}
					oneVal = oneVal >> 1;
				}
			}
		}
		return ret;
	}

	void bit_set(jvxSize position)
	{
		jvxSize idxWord = position / (8* sizeof(jvxUInt32));
		jvxSize bit_posi = position - idxWord * 8 * sizeof(jvxUInt32);
		assert(JVX_CHECK_SIZE_SELECTED(position));
		if(idxWord < JVX_NUMBER_32BITS_BITFIELD)
		{
			listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - idxWord] |= (jvxUInt32)1 << bit_posi;
		}
		else
		{
			assert(0);
		}
	};

	void bit_fclear()
	{
		memset(listFieldsBits, 0, sizeof(jvxUInt32)*JVX_NUMBER_32BITS_BITFIELD);
	};

	void bit_fset()
	{
		jvxSize i;
		for (i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			listFieldsBits[i] = 0xFFFFFFFF;
		}
	};

	void bit_zset(jvxSize position)
	{
		jvxSize idxWord = position / (8* sizeof(jvxUInt32));
		jvxSize bit_posi = position - idxWord * 8 * sizeof(jvxUInt32);
		memset(listFieldsBits, 0, sizeof(jvxUInt32)*JVX_NUMBER_32BITS_BITFIELD);
		assert(JVX_CHECK_SIZE_SELECTED(position));
		if(idxWord < JVX_NUMBER_32BITS_BITFIELD)
		{
			listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - idxWord] |= (jvxUInt32)1 << bit_posi;
		}
		else
		{
			assert(0);
		}
	};
	
 	void bit_clear(jvxSize position)
	{
		if (JVX_CHECK_SIZE_SELECTED(position))
		{
			jvxSize idxWord = position / (8 * sizeof(jvxUInt32));
			jvxSize bit_posi = position - idxWord * 8 * sizeof(jvxUInt32);
			if (idxWord < JVX_NUMBER_32BITS_BITFIELD)
			{
				listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - idxWord] &= ~((jvxUInt32)1 << bit_posi);
			}
			else
			{
				assert(0);
			}
		}
		else
		{
			jvxSize i;
			for (i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
			{
				// Rest all bits
				listFieldsBits[i] = 0;
			}
		}
	};

	bool bit_test(jvxSize position)
	{
		jvxBool retVal = false;
		jvxSize idxWord = position / (8*sizeof(jvxUInt32));
		jvxSize bit_posi = position - idxWord * 8 * sizeof(jvxUInt32);
		assert(JVX_CHECK_SIZE_SELECTED(position));
		if(idxWord < JVX_NUMBER_32BITS_BITFIELD)
		{
			jvxUInt32 andVal = (listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - idxWord] & ((jvxUInt32)1 << bit_posi));
			retVal = (andVal != 0);
		}
		else
		{
			assert(0);
		}
		return(retVal);
	};

	static jvxSize numberBits()
	{
		return(8*sizeof(jvxUInt32)*JVX_NUMBER_32BITS_BITFIELD);
	}

	static jvxSize numberUInt32()
	{
		return JVX_NUMBER_32BITS_BITFIELD;
	}

	//explicit operator bool () const <- does not work in VS 2012 since it is C++ 11
	bool evaluateBool() const
	{
		jvxSize i;
		for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			// Start with LSW to minimize runtime effort
			if(listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i])
			{
				return(true);
			}
		}
		return(false);
	};
		
	jvxUInt32 toUInt32() const
	{
		return(listFieldsBits[JVX_NUMBER_32BITS_BITFIELD-1]);
	};

	jvxCBitField toCBitField()
	{
		jvxCBitField val = 0;
		jvxSize i;
		jvxSize numTokensCopy = sizeof(jvxCBitField) / sizeof(jvxUInt32);
		for (i = 0; i < numTokensCopy; i++)
		{
			val |= ((listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i] & 0xFFFFFFFF) << (sizeof(jvxUInt32)*i));
		}
		return(val);
	};

	jvxSize bit_count_filled()
	{
		jvxSize cnt = 0;
		jvxSize i, j;
		for (i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			jvxUInt32 val = listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i];
			for (j = 0; j < sizeof(jvxUInt32); j++)
			{
				if (val & (jvxUInt32)1 << j)
				{
					cnt++;
				}
				else
				{
					return cnt;
				}
			}
		}
		return cnt;
	}
	// ==================================================================

	CjvxBitFieldN& operator |= (CjvxBitFieldN in)
	{
		jvxSize i;
		for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			listFieldsBits[i] = listFieldsBits[i] | in.listFieldsBits[i];
		}
		return *this;
	};

	CjvxBitFieldN& operator ~()
	{
		jvxSize i;
		for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			listFieldsBits[i] = ~listFieldsBits[i];
		}
		return *this;
	};

	CjvxBitFieldN& operator &= (CjvxBitFieldN in)
	{
		jvxSize i;
		for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			listFieldsBits[i] = listFieldsBits[i] & in.listFieldsBits[i];
		}
		return *this;
	};

	CjvxBitFieldN& operator ^= (CjvxBitFieldN in)
	{
		jvxSize i;
		for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			listFieldsBits[i] = listFieldsBits[i] ^ in.listFieldsBits[i];
		}
		return *this;
	};

	CjvxBitFieldN& operator += (jvxUInt32 addme)
	{
		jvxSize i;
		jvxUInt32 carry = 0;
		jvxUInt64 accu = 0;

		accu = listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1];
		accu += addme;
		listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1] = accu & 0xFFFFFFFF;
		carry = (jvxUInt32)((accu & 0xFFFFFFFF00000000) >> 32);

		for(i = 1; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			accu = listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i] + carry;
			listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i] = accu & 0xFFFFFFFF;
			carry = (accu & 0xFFFFFFFF00000000) >> 32;
		}
		return *this;
	};

	CjvxBitFieldN& operator >>= (jvxUInt32 shft)
	{
		// Need a copy of element due to order when processing
		CjvxBitFieldN tmp = *this;
		*this = tmp >> shft;
		return *this;
	};

	CjvxBitFieldN& operator <<= (jvxUInt32 shft)
	{
		// Need a copy of element due to order when processing
		CjvxBitFieldN tmp = *this;
		*this = tmp << shft;
		return *this;
	};

	std::string toString()
	{
		jvxSize i;
		char myBuf[2*sizeof(jvxUInt32)+1];
		char prodToken_int[3];
		std::string prodToken;
		jvxUInt32 val = 0;
		jvxBool firstNonZero = false;
		std::string retVal = "0x";


		memset(prodToken_int, 0, 3);


#if _MSC_VER >= 1600
		sprintf_s(prodToken_int, 3, "%d", (int)(2*sizeof(jvxUInt32)));
#else
		sprintf(prodToken_int, "%d", (int)(2*sizeof(jvxUInt32)));
#endif

		prodToken = "%0";
		prodToken += prodToken_int;
		prodToken += "x";
			
		memset(myBuf, 0, 2*sizeof(jvxUInt32)+ 1);
		for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD-1; i++)
		{
			val = listFieldsBits[i];
			if(firstNonZero == false)
			{
				if(val)
				{
					firstNonZero = true;
				}
			}
			if(firstNonZero)
			{
#if _MSC_VER >= 1600
				sprintf_s(myBuf, 2*sizeof(jvxUInt32)+1, prodToken.c_str(), val);
#else
				sprintf(myBuf, prodToken.c_str(), val);
#endif
				retVal = retVal + myBuf;
			}
		}		

		// We want at least ONE token in the string
		val = listFieldsBits[i];
#if _MSC_VER >= 1600
		sprintf_s(myBuf, 2*sizeof(jvxUInt32)+1, prodToken.c_str(), val);
#else
		sprintf(myBuf, prodToken.c_str(), val);
#endif
		retVal = retVal + myBuf;
		return(retVal);
	};

	jvxBool fromString(std::string  str_in)
	{
		jvxCBool err = c_false;
		jvxSize i;
		jvxUInt64 out;
		jvxBool retVal = true;
		unsigned long outul;
		std::string token, prefix, str_short;
		memset(listFieldsBits, 0, sizeof(jvxUInt32)*JVX_NUMBER_32BITS_BITFIELD);
		jvxSize szFld = str_in.size();
		if (szFld >= 2)
		{
			prefix = str_in.substr(0, 2);
			szFld = szFld - 2;
			str_short = str_in.substr(2, std::string::npos);
			if (prefix == "0x")
			{
				jvxUInt32 numEntries = (jvxUInt32)ceil(szFld / 8.0);
				if (numEntries > JVX_NUMBER_32BITS_BITFIELD)
				{
					std::cout << "Warning: Overload in bitfield expression, value is 0x" << str_short << std::endl;
					numEntries = JVX_NUMBER_32BITS_BITFIELD;
				}

				if (numEntries > 0)
				{
					for (i = 0; i < numEntries - 1; i++)
					{
						token = str_short.substr(str_short.size() - 2 * sizeof(jvxUInt32), 2 * sizeof(jvxUInt32));
						if (token == "ffffffff")
						{
							listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i] = (jvxUInt32)-1;
						}
						else
						{
							outul = strtoul(token.c_str(), NULL, 16);

							listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i] = JVX_ULONG_UINT32(outul);
							str_short = str_short.substr(0, str_short.size() - 2 * sizeof(jvxUInt32));
						}
					}
					if (token == "ffffffff")
					{
						listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i] = (jvxUInt32)-1;
					}
					else
					{
						outul = strtoul(str_short.c_str(), NULL, 16);
						listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i] = JVX_ULONG_UINT32(outul);
					}
				}
			}
			else if (prefix == "0b")
			{
				jvxUInt32 numEntries = (jvxUInt32)ceil(szFld);
				if (numEntries > JVX_NUMBER_32BITS_BITFIELD * 8)
				{
					std::cout << "Warning: Overload in bitfield expression, value is 0b" << str_short << std::endl;
					numEntries = JVX_NUMBER_32BITS_BITFIELD * 8;
				}

				if (numEntries > 0)
				{
					for (i = 0; i < numEntries; i++)
					{
						jvxSize idxEntry = i / sizeof(jvxUInt32);
						jvxSize inEntry = i % sizeof(jvxUInt32);

						if (token[i] == '1')
						{
							listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - idxEntry] |= (1 << (sizeof(jvxUInt32) - 1 - inEntry));
						}
					}
				}
			}
			else
			{
				if (str_in == "yes")
				{
					// In a selectionlist, option "0" is related to a "true"
					out = 0x1;
				}
				else if (str_in == "no")
				{
					// In a selectionlist, option "1" is related to a "false"
					out = 0x2;
				}
				else
				{
					out = jvx_txt2UInt64(str_in.c_str(), &err);
					if (err)
					{
						jvxData outD = jvx_txt2Data(str_in.c_str(), &err);
						if (err == c_false)
						{
							out = JVX_DATA2UINT64(outD);
						}
						else
						{
							retVal = false;
							//std::cout << __FUNCTION__ << ": Conversion if property string <" << str_in << "> into a selection failed due to conversion error." << std::endl;
						}
					}
				}
				this->setValue(out);
				//return false;
			}
		}
		else
		{
			
			out = jvx_txt2UInt64(str_in.c_str(), &err);
			if (err)
			{
				jvxData outD = jvx_txt2Data(str_in.c_str(), &err);
				if (err == c_false)
				{
					out = JVX_DATA2UINT64(outD);
				}
				else
				{
					retVal = false;
					// std::cout << __FUNCTION__ << ": Conversion if property string <" << str_in << "> into a selection failed due to conversion error." << std::endl;
				}
			}
			this->setValue(out);
		}
		return retVal;
	};

	// ===================================================================================

	bool fromStringSelId(std::string  str, jvxSize ll)
	{
		jvxSize i;
		jvxUInt64 out;
		jvxCBool err = false;
		jvxBool foundit = false;
		std::string token, prefix;
		memset(listFieldsBits, 0, sizeof(jvxUInt32)*JVX_NUMBER_32BITS_BITFIELD);
		// jvxSize szFld = str.size();
		out = jvx_txt2UInt64(str.c_str(), &err);
		if (err == c_true)
		{
			err = false;
			jvxData outD = jvx_txt2Data(str.c_str(), &err);
			if(err == c_false)
			{
				out = (jvxUInt64)outD;
			}
		}

		this->bit_fclear();
		for(i = 0; i < ll; i++)
		{
			if (i == out)
			{
				this->bit_set(i);
				foundit = true;
				break;
			}
		}

		if (!foundit)
		{
			this->bit_fset();
		}
		return true;
	};

	// ===================================================================================

	friend CjvxBitFieldN operator + (CjvxBitFieldN in1, CjvxBitFieldN in2)
	{
		jvxSize i;
		CjvxBitFieldN retVal;
		jvxUInt32 carry = 0;
		jvxUInt64 accu = 0;
		for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			accu = in1.listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i];
			accu += in2.listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i];
			accu += carry;
			retVal.listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i] = accu & 0xFFFFFFFF;
			carry = (accu & 0xFFFFFFFF00000000) >> 32;
		}
		return(retVal);
	};

	friend CjvxBitFieldN operator - (CjvxBitFieldN in1, CjvxBitFieldN in2)
	{
		jvxSize i;
		CjvxBitFieldN retVal;
		jvxUInt32 carry = 1;
		jvxUInt64 accu = 0;
		jvxUInt32 val2 = 0;
		for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			val2 = in2.listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i];
			val2 = ~val2;
			accu = in1.listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i];
			accu += val2;
			accu += carry;
			retVal.listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i] = accu & 0xFFFFFFFF;
			carry = (accu & 0xFFFFFFFF00000000) >> 32;
		}
		return(retVal);
	};

	friend CjvxBitFieldN operator << (CjvxBitFieldN in1, jvxUInt32 shft)
	{
		jvxSize i;
		CjvxBitFieldN retVal;
		jvxUInt32 carry = 0;
		if(shft > (JVX_NUMBER_32BITS_BITFIELD * sizeof(jvxUInt32) * 8))
		{
			shft = (JVX_NUMBER_32BITS_BITFIELD * sizeof(jvxUInt32) * 8);
		}

		jvxUInt32 shftWords = shft / (sizeof(jvxUInt32) * 8);
		shft = shft - shftWords * (sizeof(jvxUInt32) * 8);
		jvxUInt32 onemshift = sizeof(jvxUInt32) * 8 - shft;
		jvxUInt64 accu1 = 0;
		jvxUInt64 accu2 = 0;		
		jvxSize idxWrite = shftWords;
		jvxUInt64 mask1 = ( (jvxUInt64)1 << shft) - 1;
		jvxUInt64 mask2 = ( (jvxUInt64)1 << onemshift ) - 1;
		jvxUInt64 mask1s = mask1 << onemshift;

		for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD - shftWords; i++, idxWrite++)
		{
			accu1 = in1.listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i];
			accu2 = (accu1 & mask2) << shft;
			accu1 = (accu1 & mask1s) >> onemshift;

			retVal.listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - idxWrite] = ((jvxUInt32)(accu2 & 0xFFFFFFFF)) | carry;
			carry = (jvxUInt32)(accu1 & 0xFFFFFFFF);
		}
		return(retVal);	
	};

	friend CjvxBitFieldN operator >> (CjvxBitFieldN in1, jvxUInt32 shft)
	{
		jvxSize i;
		CjvxBitFieldN retVal;
		jvxUInt32 carry = 0;
		jvxUInt32 shftWords = shft / (sizeof(jvxUInt32) * 8);
		shft = shft - shftWords * (sizeof(jvxUInt32) * 8);
		jvxUInt32 onemshift = sizeof(jvxUInt32) * 8 - shft;
		jvxUInt64 accu1 = 0;
		jvxUInt64 accu2 = 0;		
		jvxSize idxWrite = shftWords;
		jvxUInt64 mask1 = ( (jvxUInt64)1 << onemshift ) - 1;
		jvxUInt64 mask2 = ( (jvxUInt64)1 << shft) - 1;
		jvxUInt64 mask1s = mask1 << shft;

		for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD - shftWords; i++, idxWrite++)
		{
			accu1 = in1.listFieldsBits[i];
			accu2 = (accu1 & mask1s) >> shft;
			accu1 = (accu1 & mask2) << onemshift;

			retVal.listFieldsBits[idxWrite] = ((jvxUInt32)(accu2 & 0xFFFFFFFF)) | carry;
			carry = (jvxUInt32)(accu1 & 0xFFFFFFFF);
		}
		return(retVal);	
	};

	friend CjvxBitFieldN operator & (CjvxBitFieldN in1, CjvxBitFieldN in2)
	{
		CjvxBitFieldN retVal;
		jvxSize i;
		for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			retVal.listFieldsBits[i] = in1.listFieldsBits[i] & in2.listFieldsBits[i];
		}
		return(retVal);
	};
	friend CjvxBitFieldN operator | (CjvxBitFieldN in1, CjvxBitFieldN in2)
	{
		CjvxBitFieldN retVal;
		jvxSize i;
		for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			retVal.listFieldsBits[i] = in1.listFieldsBits[i] | in2.listFieldsBits[i];
		}
		return(retVal);
	};
	friend CjvxBitFieldN operator ^(CjvxBitFieldN in1, CjvxBitFieldN in2)
	{
		CjvxBitFieldN retVal;
		jvxSize i;
		for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			retVal.listFieldsBits[i] = in1.listFieldsBits[i] ^ in2.listFieldsBits[i];
		}
		return(retVal);
	};

	friend jvxBool operator == (CjvxBitFieldN in1, CjvxBitFieldN in2)
	{
		jvxBool retVal = true;
		jvxSize i;
		for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			if(in1.listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i] != in2.listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 -i])
			{
				retVal = false;
				break;
			}
		}
		return(retVal);
	};

	friend jvxBool operator != (CjvxBitFieldN in1, CjvxBitFieldN in2)
	{
		jvxBool retVal = false;
		jvxSize i;
		for(i = 0; i < JVX_NUMBER_32BITS_BITFIELD; i++)
		{
			if(in1.listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i] != in2.listFieldsBits[JVX_NUMBER_32BITS_BITFIELD - 1 - i])
			{
				retVal = true;
				break;
			}
		}
		return(retVal);
	};
};

#endif
