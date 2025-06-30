#ifndef __TJVXTYPES_CPP_H__
#define __TJVXTYPES_CPP_H__

#include "jvx_platform.h"
#include "TjvxValue.h"

enum class jvxCheckAccessEnum
{
	JVX_CHECK_ACCESS_NONE,
	JVX_CHECK_ACCESS_LOCK,
	JVX_CHECK_ACCESS_READ,
	JVX_CHECK_ACCESS_CREATE,
	JVX_CHECK_ACCESS_WRITE,
	JVX_CHECK_ACCESS_DESTROY
};

class jvxApiString
{
private:
	jvxSize lString;
	char* bString;
	jvxBool removeOnExit;

public:
	jvxApiString();
	
	jvxApiString(const jvxApiString& tocopy);

#ifndef JVX_COMPILE_SMALL
	jvxApiString(const std::string& tocopy);
#endif
	jvxApiString(const char* tocopy);


	virtual ~jvxApiString();

	jvxApiString& operator=(const jvxApiString& other);
	bool operator == (const jvxApiString& other);
	bool operator != (const jvxApiString& other);

	virtual void assert_valid();
	virtual bool empty();

	// =========================================================

#ifdef JVX_COMPILE_SMALL
	virtual const char* const_char_ref();
#else

	virtual std::string std_str() const;
#endif

	virtual const char* c_str() const;
	virtual void clear();
				
#ifndef JVX_COMPILE_SMALL
	virtual void assign(const char* str); 
	virtual void assign(const std::string& str);
	virtual void assign(const jvxApiString& str);
#endif

	virtual void assign_const(const char* str, jvxSize ll);

};
	
// ==================================================================================================

class jvxApiStringList
{
private:
	jvxSize lStrings;
	jvxApiString* bStrings;
	jvxBool removeOnExit;

public:
	jvxApiStringList();
	
	jvxApiStringList(const jvxApiStringList& tocopy);
	
	jvxApiStringList& operator=(const jvxApiStringList& other);

	virtual ~jvxApiStringList();

	bool content_only() const
	{
		return (bStrings == nullptr);
	}

	// =========================================================

	virtual jvxSize ll() const;
	
	virtual const char* c_str_at(jvxSize idx) const;
#ifndef JVX_COMPILE_SMALL
	virtual std::string std_str_at(jvxSize idx) const;
#endif	
	virtual void clear();
				
#ifndef JVX_COMPILE_SMALL
	virtual void assign(const std::vector<std::string>& str_list);
	virtual void assign(const std::list<std::string>& str_list);
	virtual void add(const std::string& str_entry);
#endif
	
	virtual void assign_empty(jvxSize ll);

	virtual void assign_const(const jvxApiString*  lst, jvxSize ll);
};	

// ==================================================================================================

class jvxContext
{
public:
	jvxContext()
	{
		id = JVX_SIZE_UNSELECTED;
		valid = false;
	};

	jvxSize id;
	jvxBool valid;
};

// ===============================================================================

/*
jvxCBitField request = 0;
jvxHandle* caseSpecificData = nullptr;
jvxSize szSpecificData = 0;
jvxHandle* userData = nullptr;
jvxSize szUserData = 0;

jvxReportCommandRequest(
jvxCBitField requestArg = 0,
jvxHandle* caseSpecificDataArg = nullptr,
jvxSize szSpecificDataArg = 0,
jvxHandle* userData = nullptr;
jvxSize szUserData = 0
)
jvxReportCommandRequest* duplicate()
{
};
}
*/

typedef jvxCBitField16 jvxAccessRightFlags;
typedef jvxCBitField16 jvxConfigModeFlags;

typedef jvxCBitField64 jvxAccessRightFlags_rwcd;

namespace jvx
{
	class callManager
	{
	public:

		// 16 different roles allowed
		jvxAccessRightFlags accessFlags = JVX_ACCESS_ROLE_DEFAULT;

		// On return, contains the result of function call related to access rights
		jvxAccessProtocol access_protocol = JVX_ACCESS_PROTOCOL_OK;

		jvxSize origin_call_id = JVX_SIZE_UNSELECTED;

		jvxApiString last_err_hint;

		struct
		{
			jvxAccessProtocol access_protocol = JVX_ACCESS_PROTOCOL_OK;
		} internal_stack;

		void reset()
		{
			accessFlags = JVX_ACCESS_ROLE_DEFAULT;
			access_protocol = JVX_ACCESS_PROTOCOL_OK;
			origin_call_id = JVX_SIZE_UNSELECTED;
		};

		void p_push()
		{
			internal_stack.access_protocol = access_protocol;
		}

		void p_pop()
		{
			access_protocol = internal_stack.access_protocol;
		}

	};
};
typedef jvx::callManager jvxCallManager;

// ==================================================================================================
// Component identification class
// ==================================================================================================
class jvxComponentIdentification
{
public:
	jvxComponentType tp = JVX_COMPONENT_UNKNOWN;
	jvxSize slotid = JVX_SIZE_UNSELECTED;
	jvxSize slotsubid = JVX_SIZE_UNSELECTED;
	jvxUniqueId uId = JVX_UNIQUE_ID_UNSPECIFIED;

	jvxComponentIdentification()
	{
		tp = JVX_COMPONENT_UNKNOWN;
		slotid = JVX_SIZE_UNSELECTED;
		slotsubid = JVX_SIZE_UNSELECTED;
		uId = JVX_UNIQUE_ID_UNSPECIFIED;
	};

	jvxComponentIdentification(const jvxComponentIdentification& elm)
	{
		tp = elm.tp;
		slotid = elm.slotid;
		slotsubid = elm.slotsubid;
		uId = elm.uId;
	};

	void unselected(jvxComponentType tpI)
	{
		tp = tpI;
		slotid = JVX_SIZE_UNSELECTED;
		slotsubid = JVX_SIZE_UNSELECTED;
		uId = JVX_UNIQUE_ID_UNSPECIFIED;
	};

	void reset(jvxComponentType tpI = JVX_COMPONENT_UNKNOWN, 
		jvxSize slotidI = 0, jvxSize slotsubidI = 0,
		jvxSize uIdI = JVX_SIZE_UNSELECTED)
	{
		tp = tpI;
		slotid = slotidI;
		slotsubid = slotsubidI;
		uId = uIdI;

		if (tp == JVX_COMPONENT_UNKNOWN)
		{
			slotid = JVX_SIZE_UNSELECTED;
			slotsubid = JVX_SIZE_UNSELECTED;
		}
	}

	// Constructor to only address the slots and uid, not (necessarily) the type
	jvxComponentIdentification(jvxSize slotidI,
		jvxSize slotsubidI = 0,
		jvxSize uIdI = JVX_SIZE_UNSELECTED)
	{
		tp = JVX_COMPONENT_UNKNOWN;
		slotid = slotidI;
		slotsubid = slotsubidI;
		uId = uIdI;
	}

	jvxComponentIdentification(jvxComponentType tpI, 
		jvxSize slotidI = 0, 
		jvxSize slotsubidI = 0,
		jvxSize uIdI = JVX_SIZE_UNSELECTED)
	{
		tp = tpI;
		slotid = slotidI;
		slotsubid = slotsubidI;
		uId = uIdI;

		if (tp == JVX_COMPONENT_UNKNOWN)
		{
			slotid = JVX_SIZE_UNSELECTED;
			slotsubid = JVX_SIZE_UNSELECTED;
		}
	}

	bool operator < (const jvxComponentIdentification& idC) const
	{
		if (tp < idC.tp)
		{
			return true;
		}
		else if (tp == idC.tp)
		{
			if (slotid < idC.slotid)
			{
				return true;
			}
			else if (slotid == idC.slotid)
			{
				if (slotsubid < idC.slotsubid)
				{
					return true;
				}
			}
		}
		return false;
	}
};

// ===============================================================================

enum class jvxReportCommandBroadcastType
{
	JVX_REPORT_COMMAND_BROADCAST_NONE,
	JVX_REPORT_COMMAND_BROADCAST_NO_FURTHER,
	JVX_REPORT_COMMAND_BROADCAST_AUTOMATION,
	JVX_REPORT_COMMAND_BROADCAST_RESCHEDULED
};

enum class jvxReportCommandDataType
{
	JVX_REPORT_COMMAND_TYPE_BASE, // IjvxReportCommandRequest
	JVX_REPORT_COMMAND_TYPE_SCHEDULE, // IjvxReportCommandRequest_rm
	JVX_REPORT_COMMAND_TYPE_IDENT, // IjvxReportCommandRequest_id
	JVX_REPORT_COMMAND_TYPE_UID, // IjvxReportCommandRequest_uid
	JVX_REPORT_COMMAND_TYPE_SS, // IjvxReportCommandRequest_ss
	JVX_REPORT_COMMAND_TYPE_SEQ, // IjvxReportCommandRequest_seq
	JVX_REPORT_COMMAND_TYPE_SS_ID, // IjvxReportCommandRequest_ss_id
	JVX_REPORT_COMMAND_TYPE_LIMIT
};

enum class jvxReportCommandRequest
{
	JVX_REPORT_COMMAND_REQUEST_UNSPECIFIC, // placeholder for invalid
	JVX_REPORT_COMMAND_REQUEST_UPDATE_AVAILABLE_COMPONENT_LIST, // IjvxReportCommandRequest
	JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT_LIST, // IjvxReportCommandRequest
	JVX_REPORT_COMMAND_REQUEST_UPDATE_STATUS_COMPONENT, // IjvxReportCommandRequest
	JVX_REPORT_COMMAND_REQUEST_SYSTEM_STATUS_CHANGED, // IjvxReportCommandRequest
	JVX_REPORT_COMMAND_REQUEST_TRIGGER_SEQUENCER_IMMEDIATE, // IjvxReportCommandRequest
	JVX_REPORT_COMMAND_REQUEST_UPDATE_ALL_PROPERTIES, // IjvxReportCommandRequest

	JVX_REPORT_COMMAND_REQUEST_RESCHEDULE_MAIN, // IjvxReportCommandRequest_rm

	JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_SUBDEVICE, // IjvxReportCommandRequest_id
	JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_SUBDEVICE, // IjvxReportCommandRequest_id

	JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_COMPONENT, // IjvxReportCommandRequest_id
	JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_COMPONENT, // IjvxReportCommandRequest_id

	JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_CONNECTED, // IjvxReportCommandRequest_uid -- typically reported immediately
	JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_TO_BE_DISCONNECTED, // IjvxReportCommandRequest_uid -- typically reported immediately

	JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN, // IjvxReportCommandRequest_uid

	JVX_REPORT_COMMAND_REQUEST_REPORT_COMPONENT_STATESWITCH,

	JVX_REPORT_COMMAND_REQUEST_REPORT_CONFIGURATION_COMPLETE,

	JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_EVENT, // IjvxReportCommandRequest_seq
	JVX_REPORT_COMMAND_REQUEST_REPORT_SEQUENCER_CALLBACK, // IjvxReportCommandRequest_seq

	JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_DISCONNECT_COMPLETE, // IjvxReportCommandRequest_uid

	JVX_REPORT_COMMAND_REQUEST_UPDATE_PROPERTY, // IjvxReportCommandRequest_id

	JVX_REPORT_COMMAND_REQUEST_REPORT_TEST_SUCCESS, // IjvxReportCommandRequest_uid - contains tested process uid

	JVX_REPORT_COMMAND_REQUEST_TEST_CHAIN_RUN,

	JVX_REPORT_COMMAND_REQUEST_COMPONENT_STATESWITCH, // JVX_REPORT_COMMAND_TYPE_SS

	JVX_REPORT_COMMAND_REQUEST_REPORT_SYSTEM_READY,

	JVX_REPORT_COMMAND_REQUEST_LIMIT
};

/*
JVX_INTERFACE IjvxReportCommandRequest
{
public:
	virtual ~IjvxReportCommandRequest() {};

	virtual jvxReportCommandRequest request() = 0;
	virtual jvxComponentIdentification origin() = 0;
	virtual jvxReportCommandBroadcastType broadcast() = 0;
	virtual jvxReportCommandDataType datatype() = 0;
	virtual jvxHandle* user_data() = 0;
	virtual jvxBool immediate() = 0;

	virtual jvxErrorType specialization(jvxHandle**, jvxReportCommandDataType tp) = 0;

	virtual void modify_broadcast(jvxReportCommandBroadcastType broadArg) = 0;
};
*/

/*
JVX_INTERFACE IjvxReportCommandRequest_rm
{
public:
	virtual ~IjvxReportCommandRequest_rm() {};
	virtual jvxSize schedule_id() = 0;
};


JVX_INTERFACE IjvxReportCommandRequest_id
{
public:
	virtual ~IjvxReportCommandRequest_id() {};
	virtual void ident(jvxApiString* astr) = 0;
};


JVX_INTERFACE IjvxReportCommandRequest_uid
{
public:
	virtual ~IjvxReportCommandRequest_uid() {};
	virtual void uid(jvxSize* uid) = 0;
};


JVX_INTERFACE IjvxReportCommandRequest_ss
{
public:
	virtual ~IjvxReportCommandRequest_ss() {};
	virtual void sswitch(jvxStateSwitch* ssRet) = 0;
};

// Forward declaration, this type is defined in TjvxSequencer.h

class TjvxSequencerEvent;
JVX_INTERFACE IjvxReportCommandRequest_seq
{
public:
	virtual ~IjvxReportCommandRequest_seq() {};
	virtual void seq_event(TjvxSequencerEvent* ev) = 0;
};
*/

class jvxSyncTimeStamp_data
{
public:
	jvxData timestamp_device = -1;
	jvxData timestamp_software = -1;
	jvxSyncTimeStamp_data(jvxData timestamp_device_arg = -1, jvxData timestamp_software_arg = -1) : timestamp_device(timestamp_device_arg), timestamp_software(timestamp_software_arg)
	{
	};
	void reset() 
	{
		timestamp_device = -1;
		timestamp_software = -1;
	}
};

// ===============================================================================

namespace jvx
{
	// Gneric base class for class typecasts
	template <typename T >
	JVX_INTERFACE IjvxSpecialzation
	{
	public:
		virtual jvxErrorType specialization(const jvxHandle * *ret, T tp) const = 0;
		virtual T type() const = 0;
	};

	namespace configurationCall
	{
		class manager : public jvx::callManager
		{
		public:

			// Current mode
			jvxConfigModeFlags configModeFlags;
			jvxConfigurationCallPurpose call_purpose;
			jvxComponentIdentification call_origin;

			struct
			{
				jvxConfigModeFlags configModeFlags;
				jvxConfigurationCallPurpose call_purpose;
				jvxComponentIdentification call_origin;
			} internal_stack;

			manager(jvxConfigModeFlags mode_flags_arg = JVX_CONFIG_MODE_DEFAULT,
				jvxConfigurationCallPurpose call_purpose_arg = JVX_CONFIGURATION_PURPOSE_CONFIG_ON_SYSTEM_STARTSTOP,
				jvxAccessRightFlags access_flags_arg = JVX_ACCESS_ROLE_DEFAULT, jvxAccessProtocol access_protocol_arg = JVX_ACCESS_PROTOCOL_OK) 
			{
				configModeFlags = mode_flags_arg;
				call_purpose = call_purpose_arg;
				s_push();
			};

			void reset()
			{
				configModeFlags = JVX_CONFIG_MODE_DEFAULT;
			};

			void s_push()
			{
				internal_stack.configModeFlags = configModeFlags;
				internal_stack.call_purpose = call_purpose;
			};
			void s_pop()
			{
				configModeFlags = internal_stack.configModeFlags;
				call_purpose = internal_stack.call_purpose;
			};
		};
	};
};

typedef jvx::configurationCall::manager jvxCallManagerConfiguration;

// ===============================================================================

//! List of string elements
// extern std::list<jvxHandle*>* lstMem;

template <class T>
class jvxMultiEntriesSingle
{
public:
	jvxBool reqDealloc = false;
	T* fld = nullptr;
	jvxSize num = 0;

	jvxMultiEntriesSingle(jvxSize numArg = 1)
	{
		this->resize(numArg);
	}

	~jvxMultiEntriesSingle()
	{
		this->dealloc();
	}

	jvxMultiEntriesSingle(const jvxMultiEntriesSingle& copythis)
	{
		// Assignments always make a copy of the content!
		this->resize(copythis.num, false);
		this->content_copy(copythis);
	}

	jvxMultiEntriesSingle& operator =(const jvxMultiEntriesSingle& copythis)
	{
		if (reqDealloc)
		{
			// Auto resize
			this->resize(copythis.num, false);
		}
		
		// Operat = does only copy the content and does not touch the allocated field
		this->content_copy(copythis);
		return *this;
	}

	virtual void resize(jvxSize numArg, jvxBool copyOld = true, jvxBool force = false)
	{
		jvxSize i;		
		T* fldNew = nullptr;
		if( (numArg != num) || force)
		{
			if (numArg)
			{
				JVX_SAFE_ALLOCATE_FIELD(fldNew, T, numArg);
				/*
				if (lstMem == nullptr)
				{
					lstMem = new std::list<jvxHandle*>;
				}
				lstMem->push_back((jvxHandle*)fldNew);
				*/
				//std::cout << "Allocated: " << fldNew << std::endl;
				jvxSize minNum = JVX_MIN(num, numArg);
				i = 0;

				if (copyOld)
				{
					for (; i < minNum; i++)
					{
						fldNew[i] = fld[i];
					}
				}
				for (; i < numArg; i++)
				{
					fldNew[i] = 0;
				}
			}

			// Allocate old field
			dealloc();

			// Store new pointer instead
			fld = fldNew;
			num = numArg;
			reqDealloc = true;
		}
	}

	virtual void set_all(T value)
	{
		jvxSize i;
		for (i = 0; i < num; i++)
		{
			fld[i] = value;
		}
	}

	void assign(T* ext_ptr, jvxSize ext_num)
	{
		this->dealloc();
		fld = ext_ptr;
		num = ext_num;
		reqDealloc = false;
	}

	void dealloc()
	{
		if (reqDealloc)
		{
			// std::cout << "Deallocate: " << fld << std::endl;
			/*
			auto elm = std::find(lstMem->begin(), lstMem->end(), (jvxHandle*)fld);
			if (elm == lstMem->end())
			{
				assert(0);
			}
			lstMem->erase(elm);
			*/
			JVX_SAFE_DELETE_FIELD(fld);

		}
		fld = nullptr;
		num = 0;
	}

	virtual void content_copy(const jvxMultiEntriesSingle& copyFrom, jvxSize offsetTo = 0, jvxSize offsetFrom = 0, jvxSize numCopy = JVX_SIZE_UNSELECTED, jvxBool* anyChangePtr = nullptr)
	{
		jvxSize i;
		jvxBool anyChange = false;

		// Copy to field starting with element 0, copy from element at offset
		jvxSize numMin = JVX_MIN(num - offsetTo, copyFrom.num - offsetFrom);
		if (JVX_CHECK_SIZE_SELECTED(numCopy))
		{
			numMin = JVX_MIN(numMin, numCopy);
		}
		for (i = 0; i < numMin; i++)
		{
			if (fld[i+ offsetTo] != copyFrom.fld[i+offsetFrom])
			{
				anyChange = true;
				fld[i+ offsetTo] = copyFrom.fld[i+offsetFrom];
			}
		}
		if (anyChangePtr)
			*anyChangePtr = anyChange;
	}
};

class jvxSelectionList: public jvxMultiEntriesSingle<jvxBitField>
{
public:
	jvxSelectionList(jvxSize numElms = 1) : jvxMultiEntriesSingle<jvxBitField>(numElms) {};
	jvxApiStringList strList;
	jvxBitField bitFieldSelectable;
	jvxBitField bitFieldExclusive;

	jvxBitField& bitFieldSelected(jvxSize idx = 0);
	const jvxBitField& bitFieldSelected(jvxSize idx = 0) const;

};
/*
class jvxSelectionList
{
public:
	jvxApiStringList strList;
	jvxBitField bitFieldSelected;
	jvxBitField bitFieldSelectable;
	jvxBitField bitFieldExclusive;
};
*/

class jvxValueInRange : public jvxMultiEntriesSingle<jvxData>
{
public:
	jvxValueInRange(jvxSize numElms = 1):jvxMultiEntriesSingle<jvxData>(numElms) {};
	jvxData minVal = 0;
	jvxData maxVal = 1.0;
	jvxData init_value = 0;

	jvxData& val(jvxSize idx = 0);
	const jvxData& val(jvxSize idx = 0) const;
};


/*
class jvxValueInRange
{
public:
	jvxData val;
	jvxData minVal;
	jvxData maxVal;

};
*/

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
		jvxSize i;
		bList = NULL;
		lList = 0;
		removeOnExit = false;

		lList = tocopy.lList;
		if (lList > 0)
		{
			bList = new jvxValue[lList];
			for (i = 0; i < lList; i++)
			{
				bList[i] = tocopy.bList[i];
			}
			removeOnExit = true;
		}
	};

	jvxApiValueList& operator = (const jvxApiValueList& tocopy)
	{
		jvxSize i;
		bList = NULL;
		lList = 0;
		removeOnExit = false;

		lList = tocopy.lList;
		if (lList > 0)
		{
			bList = new jvxValue[lList];
			for (i = 0; i < lList; i++)
			{
				bList[i] = tocopy.bList[i];
			}
			removeOnExit = true;
		}

		return *this;
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

#ifndef JVX_COMPILE_SMALL

// Some C++ datatypes
class jvxSelectionList_cpp: public jvxMultiEntriesSingle<jvxBitField>
{
public:
	jvxSelectionList_cpp(jvxSize numEntries = 0) : jvxMultiEntriesSingle<jvxBitField>(numEntries) {};
	std::vector<std::string> entries;
	jvxBitField exclusive;
	jvxBitField selectable;
	jvxBitField init_selection;

	jvxBitField& selection(jvxSize id = 0);
	const jvxBitField& selection(jvxSize id = 0) const;
} ;

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
