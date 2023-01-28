#ifdef JVX_COMPILE_SMALL

#include "jvx_small.h"
#else

#include "jvx.h"

#endif

jvxApiString::jvxApiString()
{
	bString = NULL;
	lString = 0;
	removeOnExit = false;
}

jvxApiString::jvxApiString(const jvxApiString& tocopy)
{
	std::string nm = tocopy.std_str();

	bString = NULL;
	lString = 0;
	removeOnExit = false;

	if (!nm.empty())
	{
		this->assign(nm);
	}
	// assert(0);
}

jvxApiString::jvxApiString(const std::string& tocopy)
{
	bString = NULL;
	lString = 0;
	removeOnExit = false;

	if (!tocopy.empty())
	{
		this->assign(tocopy);
	}
}

jvxApiString::jvxApiString(const char* tocopy)
{
	bString = NULL;
	lString = 0;
	removeOnExit = false;

	if (tocopy)
	{
		this->assign(tocopy);
	}
}

jvxApiString::~jvxApiString()
{
	clear();
	assert(bString == NULL);
}

bool 
jvxApiString::empty()
{
	return (bString == nullptr);
}

void
jvxApiString::assert_valid()
{
	assert(bString != NULL);
}

jvxApiString& 
jvxApiString::operator=(const jvxApiString& other)
{
	this->clear();
	if (other.bString)
	{
		this->assign(other.std_str());
	}
	return *this;
}

bool 
jvxApiString::operator == (const jvxApiString& other)
{
	return this->std_str() == other.std_str();
}

bool
jvxApiString::operator != (const jvxApiString& other)
{
	return this->std_str() != other.std_str();
}

// =========================================================

#ifdef JVX_COMPILE_SMALL
const char*
jvxApiString::const_char_ref()
{
	return bString;
}
#else

// This return a new string. Be careful: NEVER use std_str().c_str() - will give you an invald 
// char pointer as the std::string as returned from std_str will be immediately be invalidated after return.
std::string jvxApiString::std_str() const
{
	std::string ret;
	if (bString)
	{
		ret = bString;
	}
	return ret;
}
#endif

const char* 
jvxApiString::c_str() const
{
	return bString;
}

void
jvxApiString::clear()
{
	if (removeOnExit)
	{
#ifdef JVX_COMPILE_SMALL
		assert(0);
#else
		JVX_DSP_SAFE_DELETE_FIELD(bString);
#endif
	}
	bString = NULL;
	lString = 0;
	removeOnExit = false;
}

#ifndef JVX_COMPILE_SMALL
void
jvxApiString::assign(const std::string& str)
{
	clear();

	lString = str.length() + 1;
	JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(bString, char, lString);
	memcpy(bString, str.c_str(), sizeof(char) * (lString - 1));
	removeOnExit = true;
}

void 
jvxApiString::assign(const char* str)
{
	std::string txt = str;
	this->assign(txt);
}

void
jvxApiString::assign(const jvxApiString& str)
{
	this->assign(str.std_str());
}

#endif

void
jvxApiString::assign_const(const char* str, jvxSize ll)
{

	clear();

	lString = ll;
	bString = (char*)str;
}





// ==================================================================================================

jvxApiStringList::jvxApiStringList()
{
	bStrings = NULL;
	lStrings = 0;
	removeOnExit = false;
}

jvxApiStringList::jvxApiStringList(const jvxApiStringList& tocopy)
{
	jvxSize i;
	bStrings = NULL;
	lStrings = 0;
	removeOnExit = false;

	lStrings = tocopy.lStrings;
	if (lStrings > 0)
	{
		bStrings = new jvxApiString[lStrings];
		for (i = 0; i < lStrings; i++)
		{
			bStrings[i].assign(tocopy.bStrings[i].std_str());
		}
		removeOnExit = true;
	}
}

jvxApiStringList& 
jvxApiStringList::operator=(const jvxApiStringList& other)
{
	jvxSize i;
	clear();

	lStrings = other.lStrings;
	if (lStrings > 0)
	{
		bStrings = new jvxApiString[lStrings];
		for (i = 0; i < lStrings; i++)
		{
			bStrings[i].assign(other.bStrings[i].std_str());
		}
		removeOnExit = true;
	}
	return *this;
}

jvxApiStringList::~jvxApiStringList()
{
	clear();
	assert(bStrings == NULL);
}

// =========================================================

jvxSize jvxApiStringList::ll() const
{
	return lStrings;
}

#ifdef JVX_COMPILE_SMALL
const char*
jvxApiStringList::const_char_at(jvxSize idx) const
{
	const char* ret = NULL;;
	if (idx < lStrings)
	{
		assert(bStrings);
		ret = bStrings[idx].const_char_ref();
	}
	else
	{
		assert(0);
	}
	return ret;
}
#else
std::string
jvxApiStringList::std_str_at(jvxSize idx) const
{
	std::string ret;
	if (idx < lStrings)
	{
		if (!bStrings)
		{
			ret = "INVALID_STRING_POINTER";
		}
		else
		{
			ret = bStrings[idx].std_str();
		}
	}
	else
	{
		ret = "STRING_ID_OUT_OF_BOUNDS";
	}
	return ret;
}
#endif	
void
jvxApiStringList::clear()
{
	jvxSize i;
	if (removeOnExit)
	{
		for (i = 0; i < lStrings; i++)
		{
			bStrings[i].clear();
		}
		delete[](bStrings);
	}
	bStrings = NULL;
	lStrings = 0;
	removeOnExit = false;
}

#ifndef JVX_COMPILE_SMALL
void
jvxApiStringList::assign(const std::vector<std::string>& str_list)
{
	jvxSize i;
	clear();

	lStrings = str_list.size();
	if (lStrings > 0)
	{
		bStrings = new jvxApiString[lStrings];
		for (i = 0; i < lStrings; i++)
		{
			bStrings[i].assign(str_list[i]);
		}
		removeOnExit = true;
	}
}

void
jvxApiStringList::assign(const std::list<std::string>& str_list)
{
	jvxSize i = 0;
	clear();

	lStrings = str_list.size();
	if (lStrings > 0)
	{
		bStrings = new jvxApiString[lStrings];
		for (auto elm : str_list)
		{
			bStrings[i].assign(elm);
			i++;
		}
		removeOnExit = true;
	}
}
void
jvxApiStringList::add(const std::string& str)
{
	jvxSize i;
	jvxSize lStringsNew = lStrings + 1;
	jvxApiString* bStringsNew = new jvxApiString[lStringsNew];
	for (i = 0; i < lStrings; i++)
	{
		bStringsNew[i].assign(bStrings[i].std_str());
	}
	bStringsNew[lStringsNew - 1].assign(str);

	clear();

	bStrings = bStringsNew;
	lStrings = lStringsNew;

	removeOnExit = true;
}
#endif

void
jvxApiStringList::assign_empty(jvxSize ll)
{
	clear();
	lStrings = ll;
}

void
jvxApiStringList::assign_const(const jvxApiString*  lst, jvxSize ll)
{
	clear();

	lStrings = ll;
	bStrings = (jvxApiString*)lst;
}


#if 0
//! List of string elements
typedef struct
{
	jvxApiStringList strList;
	jvxBitField bitFieldSelected;
	jvxBitField bitFieldSelectable;
	jvxBitField bitFieldExclusive;
} jvxSelectionList;

class jvxApiValueList
{
private:
	jvxSize lList;
	jvxValue* bList;
	jvxBool removeOnExit;
	
public:
	jvxApiValueList()
	{
		bList = NULL;
		lList = 0;
		removeOnExit = false;
	};
	
	jvxApiValueList(const jvxApiValueList& tocopy)
	{
		assert(0);
	};
		
	virtual ~jvxApiValueList()
	{
		clear();
		assert(bList == NULL);
	};

	// =========================================================

	virtual jvxSize ll() const
	{
		return lList;
	};
	
	virtual jvxValue elm_at(jvxSize idx)
	{
		jvxValue ret;
		if(idx < lList)
		{
			assert(bList);
			ret = bList[idx];
		}
		else
		{
			assert(0);
		}
		return ret;
	};
	
	virtual void set_elm_at(jvxSize idx, jvxValue elm)
	{
		jvxValue ret;
		if(idx < lList)
		{
			assert(bList);
			bList[idx] = elm;
		}
		else
		{
			assert(0);
		}
	};
		
	virtual void clear()
	{
		if(removeOnExit)
		{
			delete[](bList);
		}
		bList = NULL;
		lList = 0;
		removeOnExit = false;
	};
				
#ifndef JVX_COMPILE_SMALL
	virtual void assign(const std::vector<jvxValue>& lst)
	{
		jvxSize i;
		clear();
	
		lList = lst.size();
		if(lList > 0)
		{
			bList = new jvxValue[lList];
		}
		for(i = 0; i < lList; i++)
		{
			bList[i] = lst[i];
		}
		removeOnExit = true;
	};
#endif

	virtual void assign(jvxValue* lst, jvxSize ll)
	{
		jvxSize i;
		jvxData* ptrDt = (jvxData*)lst;
		clear();
	
		lList = ll;
		if (lList > 0)
		{
			bList = new jvxValue[lList];
		}
		for (i = 0; i < lList; i++)
		{
			bList[i] = lst[i];
		}
		removeOnExit = true; 
	};

	virtual void assign(jvxHandle* lst, jvxDataFormat form, jvxSize ll)
	{
		jvxSize i;
		jvxData* ptrDt = (jvxData*)lst;
		clear();
	
		lList = ll;
		if (lList > 0)
		{
			bList = new jvxValue[lList];
		}
		switch (form)
		{
		case JVX_DATAFORMAT_DATA:
			for (i = 0; i < lList; i++)
			{
				bList[i].assign(ptrDt[i]);
			}
			break;
		default:
			assert(0);
		}
		removeOnExit = true; 
	};
	
	virtual void assign_const(jvxValue* lst, jvxSize ll)
	{

		clear();
	
		lList = ll;
		bList = lst;
	};
	virtual void create(jvxSize ll)
	{
		jvxSize i;
		clear();
		lList = ll;
		jvxData dat = 0;
		if(lList > 0)
		{
			bList = new jvxValue[lList];
		}
		for(i = 0; i < lList; i++)
		{
			bList[i].assign(dat);
		}
		removeOnExit = true;
	};
};

//! Error code error type
class jvxApiError
{
public:	
	jvxInt32 errorCode;
	jvxApiString errorDescription;
	jvxInt32 errorLevel;
	
	jvxApiError()
	{
	};
	
	~jvxApiError()
	{
	};
};

// ==================================================================================================
// Component identification class
// ==================================================================================================
class jvxComponentIdentification
{
public:
	jvxComponentType tp;
	jvxSize slotid;
	jvxSize slotsubid;

	jvxComponentIdentification()
	{
		tp = JVX_COMPONENT_UNKNOWN;
		slotid = JVX_SIZE_UNSELECTED;
		slotsubid = JVX_SIZE_UNSELECTED;
	}

	void reset(jvxComponentType tpI = JVX_COMPONENT_UNKNOWN, jvxSize slotidI = 0, jvxSize slotsubidI = 0)
	{
		tp = tpI;
		slotid = slotidI;
		slotsubid = slotsubidI;

		if (tp == JVX_COMPONENT_UNKNOWN)
		{
			slotid = JVX_SIZE_UNSELECTED;
			slotsubid = JVX_SIZE_UNSELECTED;
		}
	}

	jvxComponentIdentification(jvxComponentType tpI, jvxSize slotidI = 0, jvxSize slotsubidI = 0)
	{
		tp = tpI;
		slotid = slotidI;
		slotsubid = slotsubidI;

		if (tp == JVX_COMPONENT_UNKNOWN)
		{
			slotid = JVX_SIZE_UNSELECTED;
			slotsubid = JVX_SIZE_UNSELECTED;
		}
	}
};


#ifndef JVX_COMPILE_SMALL

// Some C++ datatypes
typedef struct
{
	std::vector<std::string> entries;
	jvxBitField selection;
	jvxBitField exclusive;
	jvxBitField selectable;
} jvx_oneSelectionOption_cpp;

	typedef struct
	{
		jvxBool isWildcard;
		std::string token;
	} jvx_oneWildcardEntry;


	typedef struct
	{
		jvxSize lValList;
		jvxValue* bValList;
	} jvxValueList_;

	/*
	typedef struct
	{
		jvxComponentType tp;
		jvxSize slotid;
		jvxSize slotsubid;
	} jvxComponentIdentification;
	*/


	template<typename TS, typename TC> class jvxMatrix
{
	std::vector<TS> selectorX;
	std::vector<TS> selectorY;

	std::vector<TC> content;

public:
	jvxMatrix() {};
	~jvxMatrix() {};

	jvxSize dimX()
	{
		return(selectorX.size());
	};

	jvxSize dimY()
	{
		return(selectorY.size());
	};

	void resize(std::vector<TS>& newX, std::vector<TS>& newY, TC setme_diag, TC setme_offdiag)
	{
		jvxSize i, j;
		std::vector<TC> content_new;
		content_new.resize(newX.size()*newY.size());
		for (i = 0; i < newX.size(); i++)
		{
			for (j = 0; j < newY.size(); j++)
			{
				jvxSize idxOldx = JVX_SIZE_UNSELECTED;
				jvxSize idxOldy = JVX_SIZE_UNSELECTED;
				translate(newX[i], newY[j], idxOldx, idxOldy);
				if (
					JVX_CHECK_SIZE_SELECTED(idxOldx) && JVX_CHECK_SIZE_SELECTED(idxOldy))
				{
					content_new[j*newX.size() + i] = content[idxOldy*selectorX.size() + idxOldx];
				}
				else
				{
					if(i == j)
					{
						content_new[j*newX.size() + i] = setme_diag;						
					}
					else
					{
						content_new[j*newX.size() + i] = setme_offdiag;
					}
				}
			}
		}
		content = content_new;
		selectorX = newX;
		selectorY = newY;
	};

	void clear()
	{
		selectorX.clear();
		selectorY.clear();
		content.clear();
	};

	void translate(const TS& selx, const TS& sely, jvxSize& idxx, jvxSize& idxy)
	{
		jvxSize i;
		
		idxx = JVX_SIZE_UNSELECTED;
		idxy = JVX_SIZE_UNSELECTED;

		for (i = 0; i < selectorX.size(); i++)
		{
			if (selectorX[i] == selx)
			{
				idxx = i;
				break;
			}
		}

		for (i = 0; i < selectorY.size(); i++)
		{
			if (selectorY[i] == sely)
			{
				idxy = i;
				break;
			}
		}
	}

	TC value(jvxSize idxx, jvxSize idxy)
	{
		assert(idxx < selectorX.size());
		assert(idxy < selectorY.size());
		return(content[idxy*selectorX.size() + idxx]);
	};

	void setValue(jvxSize idxx, jvxSize idxy, TC val)
	{
		assert(idxx < selectorX.size());
		assert(idxy < selectorY.size());

		content[idxy*selectorX.size() + idxx] = val;
	};
};

static inline bool operator == (const jvxComponentIdentification& one, const jvxComponentIdentification& other)
{
	int id1, id2;
	if (one.tp != other.tp)
	{
		return false;
	}

	id1 = JVX_SIZE_INT(one.slotid);
	id2 = JVX_SIZE_INT(other.slotid);

	if (
		(id1 < 0) || (id2 < 0) || (id1 == id2))
	{
		id1 = JVX_SIZE_INT(one.slotsubid);
		id2 = JVX_SIZE_INT(other.slotsubid);
		if (
			(id1 < 0) || (id2 < 0) || (id1 == id2))
		{
			return true;
		}
	}
	return false;
}

static inline bool operator != (const jvxComponentIdentification& one, const jvxComponentIdentification& other)
{
	bool res = (one == other);
	return !res;
}

/*
typedef struct
{
	jvxComponentType tp;
	jvxSize slotid;
	jvxSize slotsubid;
} jvxComponentIdentification;
*/

// ==================================================================================================
// ==================================================================================================
#endif
#endif
