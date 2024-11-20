#ifndef __TJVXPROPERTYRAWPOINTER_H__
#define __TJVXPROPERTYRAWPOINTER_H__

JVX_INTERFACE IjvxLocalTextLog;
JVX_INTERFACE IjvxPropertyExtender;

namespace jvx
{
	namespace propertyRawPointerType
	{
		enum class apiType
		{
			JVX_RAW_POINTER_API_TYPE_LOCAL = 0x1, /* LOCAL means that this variable is typically not passed over DLL bounds, e.g., std::string*/
			JVX_RAW_POINTER_API_TYPE_DLL = 0x2, /* LOCAL means that this variable is typically passed over DLL bounds, e.g., jvxApiString*/
			JVX_RAW_POINTER_API_TYPE_NONE_SPECIFIC = 0x3
		};

		enum class elementType
		{
			JVX_FIELD_RAW_POINTER_NONE,
			JVX_FIELD_RAW_POINTER_GENERIC,
			JVX_FIELD_RAW_POINTER_SINGLE_VALUE,
			JVX_FIELD_RAW_POINTER_LINEAR_FIELD,
			JVX_FIELD_RAW_POINTER_EXTERNAL_BUFFER,
			JVX_FIELD_RAW_POINTER_EXTERNAL_BUFFER_POINTER,
			JVX_FIELD_RAW_POINTER_CALLBACK,
			JVX_FIELD_RAW_POINTER_CALLBACK_POINTER,
			JVX_FIELD_RAW_POINTER_VALID_BOOL,
			JVX_FIELD_RAW_POINTER_TEXT_LOG,
			JVX_FIELD_RAW_POINTER_TEXT_LOG_POINTER,
			JVX_FIELD_RAW_POINTER_EXTENSION_INTERFACE,
			JVX_FIELD_RAW_POINTER_EXTENSION_INTERFACE_POINTER
		};

		// ===========================================================================

		JVX_INTERFACE IjvxRawPointerType
		{
		public:
			virtual ~IjvxRawPointerType() {};
			virtual elementType etype() const = 0;
			virtual apiType atype() const = 0;
			virtual jvxDataFormat ftype() const = 0;
			virtual jvxErrorType specialization(const jvxHandle** ret, elementType etp, apiType atp, jvxDataFormat ftp) const = 0;
		};

		class CjvxRawPointerType : public IjvxRawPointerType
		{
		protected:

			elementType eTp = elementType::JVX_FIELD_RAW_POINTER_NONE;
			apiType aTp = apiType::JVX_RAW_POINTER_API_TYPE_NONE_SPECIFIC;

			jvxHandle* fld = nullptr;
			jvxDataFormat form = JVX_DATAFORMAT_NONE;
			jvxSize sz = 0;

		public:

			CjvxRawPointerType(jvxHandle* fldArg = nullptr, jvxSize szArg = 1, jvxDataFormat formArg = JVX_DATAFORMAT_NONE)
			{
				reset(fldArg, szArg, formArg);
			};			

			CjvxRawPointerType(jvxData& varArg)
			{
				reset(&varArg, 1, JVX_DATAFORMAT_DATA);
			};

			void reset(jvxHandle* fldArg = nullptr, jvxSize szArg = 1, jvxDataFormat formArg = JVX_DATAFORMAT_NONE)
			{
				fld = fldArg;
				form = formArg;
				sz = szArg;
				eTp = elementType::JVX_FIELD_RAW_POINTER_GENERIC;
				aTp = apiType::JVX_RAW_POINTER_API_TYPE_NONE_SPECIFIC;
			};

			virtual elementType etype() const override
			{
				return eTp;
			}
			virtual apiType atype() const override
			{
				return aTp;
			}
			virtual jvxDataFormat ftype() const  override
			{
				return form;
			}

			virtual jvxHandle*  raw() const
			{
				return fld;
			}
			virtual jvxSize size() const
			{
				return sz;
			}

			virtual jvxErrorType specialization(const jvxHandle** ret, elementType eTpArg, apiType aTpArg, jvxDataFormat ftp) const override
			{
				if (eTpArg == elementType::JVX_FIELD_RAW_POINTER_GENERIC)
				{
					// apitype and format not checked
					if (ret)
					{
						*ret = reinterpret_cast<const jvxHandle*>(static_cast<const CjvxRawPointerType*>(this));
					}
					return JVX_NO_ERROR;
				}
				return JVX_ERROR_INVALID_SETTING;
			}

			virtual void atype(apiType aTpArg)  
			{
				aTp = aTpArg;
			}
		};

		template <class T>
		class CjvxRawPointerTypeSimple : public CjvxRawPointerType
		{
		public:		

			CjvxRawPointerTypeSimple(T& varArg) 
			{
				jvxDataFormat form = template_get_type_enum<T>();
				reset(&varArg, 1, form);
			};
		};

		template <class T>
		class CjvxRawPointerTypeSimpleI : public CjvxRawPointerType
		{
		public:
			T in;
		public:
			CjvxRawPointerTypeSimpleI(const T& varArg): in(varArg) // no call to parent constructor since we should not link the argument in at this time!!
			{		
				jvxDataFormat form = template_get_type_enum<T>();
				reset(&in, 1, form);
			};
		};

		// ====================================================================
		// =========================================================================

		template <class T>
		class CjvxRawPointerTypeObject : public CjvxRawPointerType
		{
		public:
			CjvxRawPointerTypeObject(T& ptrArg) : CjvxRawPointerType((jvxHandle*)&ptrArg)
			{
				findTypes();
			}

			CjvxRawPointerTypeObject(T* ptrArg) : CjvxRawPointerType((jvxHandle*)ptrArg)
			{
				findTypes();
			}

			virtual void findTypes()
			{
				if (std::is_same<T, jvxSelectionList_cpp>::value)
				{
					CjvxRawPointerType::aTp = apiType::JVX_RAW_POINTER_API_TYPE_LOCAL;
					CjvxRawPointerType::form = JVX_DATAFORMAT_SELECTION_LIST;
					CjvxRawPointerType::eTp = elementType::JVX_FIELD_RAW_POINTER_SINGLE_VALUE;
				}
				else if (std::is_same<T, jvxSelectionList>::value)
				{
					CjvxRawPointerType::aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
					CjvxRawPointerType::form = JVX_DATAFORMAT_SELECTION_LIST;
					CjvxRawPointerType::eTp = elementType::JVX_FIELD_RAW_POINTER_SINGLE_VALUE;
				}
				else if (std::is_same<T, jvxValueInRange>::value)
				{
					CjvxRawPointerType::aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
					CjvxRawPointerType::form = JVX_DATAFORMAT_VALUE_IN_RANGE;
					CjvxRawPointerType::eTp = elementType::JVX_FIELD_RAW_POINTER_SINGLE_VALUE;
				}				
				else if (std::is_same<T, std::string>::value)
				{
					CjvxRawPointerType::aTp = apiType::JVX_RAW_POINTER_API_TYPE_LOCAL;
					CjvxRawPointerType::form = JVX_DATAFORMAT_STRING;
					CjvxRawPointerType::eTp = elementType::JVX_FIELD_RAW_POINTER_SINGLE_VALUE;
				}
				else if (std::is_same<T, jvxApiString>::value)
				{
					CjvxRawPointerType::aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
					CjvxRawPointerType::form = JVX_DATAFORMAT_STRING;
					CjvxRawPointerType::eTp = elementType::JVX_FIELD_RAW_POINTER_SINGLE_VALUE;
				}
				else if (std::is_same<T, std::vector<std::string> >::value)
				{
					CjvxRawPointerType::aTp = apiType::JVX_RAW_POINTER_API_TYPE_LOCAL;
					CjvxRawPointerType::form = JVX_DATAFORMAT_STRING_LIST;
					CjvxRawPointerType::eTp = elementType::JVX_FIELD_RAW_POINTER_SINGLE_VALUE;
				}
				else if (std::is_same<T, jvxApiStringList>::value)
				{
					CjvxRawPointerType::aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
					CjvxRawPointerType::form = JVX_DATAFORMAT_STRING_LIST;
					CjvxRawPointerType::eTp = elementType::JVX_FIELD_RAW_POINTER_SINGLE_VALUE;
				}
			}
			
			T* typedPointer() const
			{
				return (T*)fld;
			}

			virtual jvxErrorType specialization(const jvxHandle** ret, elementType eTpArg, apiType aTpArg, jvxDataFormat ftp) const override
			{
				if (
					(eTp == eTpArg) && (aTp == aTpArg) && (ftp == form))
				{
					if (ret)
					{
						*ret = reinterpret_cast<const jvxHandle*>(static_cast<const CjvxRawPointerTypeObject<T>*>(this));
					}
					return JVX_NO_ERROR;
				}
				return CjvxRawPointerType::specialization(ret, eTpArg, aTpArg, ftp);
			}
		};

		// =========================================================================

		template <class T>
		class CjvxRawPointerTypeObjectField : public CjvxRawPointerTypeObject<T>
		{
		public:
			CjvxRawPointerTypeObjectField(T& arg) : CjvxRawPointerTypeObject<T>(arg) 
			{ 
				CjvxRawPointerType::sz = CjvxRawPointerTypeObject<T>::typedPointer()->num;
			};			
		};

		// =========================================================================

		template <class T> 
		class CjvxRawPointerTypeField : public CjvxRawPointerType
		{
		public:
			CjvxRawPointerTypeField(T* ptrArg, jvxSize szArg = 1) :
				CjvxRawPointerType((jvxHandle*)ptrArg, szArg, JVX_DATAFORMAT_NONE)
			{
				findTypes();
			}

			CjvxRawPointerTypeField(T& ptrArg) :
				CjvxRawPointerType((jvxHandle*)&ptrArg, 1, JVX_DATAFORMAT_NONE)
			{
				findTypes();
			}

			void findTypes()
			{
				if (std::is_same<T, jvxData>::value)
				{
					form = JVX_DATAFORMAT_DATA;
					eTp = elementType::JVX_FIELD_RAW_POINTER_LINEAR_FIELD;
					aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
				}
				else if (std::is_same<T, jvxInt64>::value)
				{
					form = JVX_DATAFORMAT_64BIT_LE;
					eTp = elementType::JVX_FIELD_RAW_POINTER_LINEAR_FIELD;
					aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
				}
				else if (std::is_same<T, jvxUInt64>::value)
				{
					form = JVX_DATAFORMAT_U64BIT_LE;
					eTp = elementType::JVX_FIELD_RAW_POINTER_LINEAR_FIELD;
					aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
				}
				else if (std::is_same<T, jvxInt32>::value)
				{
					form = JVX_DATAFORMAT_32BIT_LE;
					eTp = elementType::JVX_FIELD_RAW_POINTER_LINEAR_FIELD;
					aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
				}
				else if (std::is_same<T, jvxUInt32>::value)
				{
					form = JVX_DATAFORMAT_U32BIT_LE;
					eTp = elementType::JVX_FIELD_RAW_POINTER_LINEAR_FIELD;
					aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
				}
				else if (std::is_same<T, jvxInt16>::value)
				{
					form = JVX_DATAFORMAT_16BIT_LE;
					eTp = elementType::JVX_FIELD_RAW_POINTER_LINEAR_FIELD;
					aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
				}
				else if (std::is_same<T, jvxUInt16>::value)
				{
					form = JVX_DATAFORMAT_U16BIT_LE;
					eTp = elementType::JVX_FIELD_RAW_POINTER_LINEAR_FIELD;
					aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
				}
				else if (std::is_same<T, jvxInt8>::value)
				{
					form = JVX_DATAFORMAT_8BIT;
					eTp = elementType::JVX_FIELD_RAW_POINTER_LINEAR_FIELD;
					aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
				}
				else if (std::is_same<T, jvxUInt8>::value)
				{
					form = JVX_DATAFORMAT_8BIT;
					eTp = elementType::JVX_FIELD_RAW_POINTER_LINEAR_FIELD;
					aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
				}
				else if (std::is_same<T, jvxSize>::value)
				{
					form = JVX_DATAFORMAT_SIZE;
					eTp = elementType::JVX_FIELD_RAW_POINTER_LINEAR_FIELD;
					aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
				}

			};
			T* typedPointer() const
			{
				return (T*)fld;
			}

			virtual jvxErrorType specialization(const jvxHandle** ret, elementType eTpArg, apiType aTpArg, jvxDataFormat ftp) const override
			{
				if (
					(eTp == eTpArg) && (aTp == aTpArg) && (ftp == form))
				{
					if (ret)
					{
						*ret = reinterpret_cast<const jvxHandle*>(static_cast<const CjvxRawPointerTypeField<T>*>(this));
					}
					return JVX_NO_ERROR;
				}
				return CjvxRawPointerType::specialization(ret, eTpArg, aTpArg, ftp);
			}
		};

		//#ifdef JVX_SIZE_IS_UINT64
		class CjvxRawPointerTypeFieldSize : public CjvxRawPointerTypeField<jvxSize>
		{
		public:
			CjvxRawPointerTypeFieldSize(jvxSize* ptrArg, jvxSize szArg = 1) : CjvxRawPointerTypeField<jvxSize>(ptrArg, szArg)
			{
				form = JVX_DATAFORMAT_SIZE;
				eTp = elementType::JVX_FIELD_RAW_POINTER_LINEAR_FIELD;
				aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
			}
		};

		// ===============================================================		

		template <class T>
		class CjvxRawPointerTypeExternal : public CjvxRawPointerType
		{
			bool* isValid = nullptr;
		public:
			CjvxRawPointerTypeExternal(T* fldArg = nullptr, bool* isValidArg = nullptr, jvxDataFormat formArg = JVX_DATAFORMAT_INTERFACE) :
				CjvxRawPointerType(fldArg, 1, formArg)
			{
				reset(fldArg, isValidArg, formArg);
			}

			void reset(T* fldArg = nullptr, bool* isValidArg = nullptr, jvxDataFormat formArg = JVX_DATAFORMAT_INTERFACE)
			{
				CjvxRawPointerType::reset(fldArg, 1, formArg);
				isValid = isValidArg;
				aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
				if (std::is_same<T, jvxExternalBuffer>::value)
				{
					eTp = elementType::JVX_FIELD_RAW_POINTER_EXTERNAL_BUFFER;
					// form = JVX_DATAFORMAT_POINTER; <- taken from reset/constructor
				}
				if (std::is_same<T, IjvxLocalTextLog>::value)
				{
					eTp = elementType::JVX_FIELD_RAW_POINTER_TEXT_LOG;
					form = JVX_DATAFORMAT_STRING;
				}
				if (std::is_same<T, IjvxPropertyExtender>::value)
				{
					eTp = elementType::JVX_FIELD_RAW_POINTER_EXTENSION_INTERFACE;
					form = JVX_DATAFORMAT_INTERFACE;
				}
				if (std::is_same<T, jvxCallbackPrivate>::value)
				{
					eTp = elementType::JVX_FIELD_RAW_POINTER_CALLBACK;
					form = JVX_DATAFORMAT_CALLBACK;
				}
			}

			virtual jvxErrorType specialization(const jvxHandle** ret, elementType eTpArg, apiType aTpArg, jvxDataFormat ftp) const override
			{
				if (
					(eTp == eTpArg) && (aTp == aTpArg) && (ftp == form))
				{
					if (ret)
					{
						*ret = reinterpret_cast<const jvxHandle*>(static_cast<const CjvxRawPointerTypeExternal<T>*>(this));
					}
					return JVX_NO_ERROR;
				}
				return CjvxRawPointerType::specialization(ret, eTpArg, aTpArg, ftp);
			}			

			T* typedPointer() const
			{
				return (T*)fld;
			}
			bool* isValidPtr() const
			{
				return isValid;
			}
		};

		template <class T>
		class CjvxRawPointerTypeExternalPointer : public CjvxRawPointerType
		{
		public:	

			CjvxRawPointerTypeExternalPointer(T** fldArg, jvxDataFormat formArg = JVX_DATAFORMAT_POINTER) :
				CjvxRawPointerType(fldArg, 1, formArg)
			{
				aTp = apiType::JVX_RAW_POINTER_API_TYPE_NONE_SPECIFIC;
				if (std::is_same<T, jvxExternalBuffer>::value)
				{
					eTp = elementType::JVX_FIELD_RAW_POINTER_EXTERNAL_BUFFER_POINTER;
					//form = JVX_DATAFORMAT_POINTER; <- taken from reset/constructor
				}
				if (std::is_same<T, IjvxLocalTextLog>::value)
				{
					eTp = elementType::JVX_FIELD_RAW_POINTER_TEXT_LOG_POINTER;
					form = JVX_DATAFORMAT_STRING;
				}
				if (std::is_same<T, IjvxPropertyExtender>::value)
				{
					eTp = elementType::JVX_FIELD_RAW_POINTER_EXTENSION_INTERFACE_POINTER;
					form = JVX_DATAFORMAT_INTERFACE;
				}
			}

			virtual jvxErrorType specialization(const jvxHandle** ret, elementType eTpArg, apiType aTpArg, jvxDataFormat ftp) const override
			{
				if (
					(eTp == eTpArg) && (ftp == form)) // && (aTp == aTpArg) <- there is no difference between DLL and LOCAL, HENCE NON_SPECIFIC
				{
					if (ret)
					{
						*ret = reinterpret_cast<const jvxHandle*>(static_cast<const CjvxRawPointerTypeExternalPointer<T>*>(this));
					}
					return JVX_NO_ERROR;
				}
				return CjvxRawPointerType::specialization(ret, eTpArg, aTpArg, ftp);
			}
			
			T** typedPointer() const
			{
				return (T**)fld;
			}
		};

		// =========================================================================
		
		class CjvxRawPointerTypeCallback : public CjvxRawPointerType
		{
		public:

			CjvxRawPointerTypeCallback(jvxCallbackPrivate* fldArg):
				CjvxRawPointerType(fldArg)
			{
				eTp = elementType::JVX_FIELD_RAW_POINTER_CALLBACK;
				form = JVX_DATAFORMAT_CALLBACK;
				aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
			}

			virtual jvxErrorType specialization(const jvxHandle** ret, elementType eTpArg, apiType aTpArg, jvxDataFormat ftp) const override
			{
				if (
					(eTp == eTpArg) && (aTp == aTpArg) && (ftp == form))
				{
					if (ret)
					{
						*ret = reinterpret_cast<const jvxHandle*>(static_cast<const CjvxRawPointerTypeCallback*>(this));
					}
					return JVX_NO_ERROR;
				}
				return CjvxRawPointerType::specialization(ret, eTpArg, aTpArg, ftp);
			}

			jvxCallbackPrivate* typedPointer() const
			{
				return (jvxCallbackPrivate*)fld;
			}
		};

		class CjvxRawPointerTypeCallbackPointer : public CjvxRawPointerType
		{
		public:

			CjvxRawPointerTypeCallbackPointer(jvxCallbackPrivate** fldArg) :
				CjvxRawPointerType(fldArg)
			{
				eTp = elementType::JVX_FIELD_RAW_POINTER_CALLBACK_POINTER;
				form = JVX_DATAFORMAT_CALLBACK;
				aTp = apiType::JVX_RAW_POINTER_API_TYPE_LOCAL;
			}

			virtual jvxErrorType specialization(const jvxHandle** ret, elementType eTpArg, apiType aTpArg, jvxDataFormat ftp) const override
			{
				if (
					(eTp == eTpArg) && (aTp == aTpArg) && (ftp == form))
				{
					if (ret)
					{
						*ret = reinterpret_cast<const jvxHandle*>(static_cast<const CjvxRawPointerTypeCallbackPointer*>(this));
					}
					return JVX_NO_ERROR;
				}
				return CjvxRawPointerType::specialization(ret, eTpArg, aTpArg, ftp);
			}

			jvxCallbackPrivate** typedPointer() const
			{
				return (jvxCallbackPrivate**)fld;
			}
		};
		
		// ================================================================================

		class CjvxRawPointerTypeValid : public CjvxRawPointerType
		{
		public:
			CjvxRawPointerTypeValid(jvxBool* isValidPtrArg):
				CjvxRawPointerType(isValidPtrArg)
			{
				eTp = elementType::JVX_FIELD_RAW_POINTER_VALID_BOOL;
				aTp = apiType::JVX_RAW_POINTER_API_TYPE_DLL;
			}

			virtual jvxErrorType specialization(const jvxHandle** ret, elementType eTpArg, apiType aTpArg, jvxDataFormat ftp) const override
			{
				if (
					(eTp == eTpArg) && (aTp == aTpArg) && (ftp == form))
				{
					if (ret)
					{
						*ret = reinterpret_cast<const jvxHandle*>(static_cast<const CjvxRawPointerTypeValid*>(this));
						return JVX_NO_ERROR;
					}
				}
				return CjvxRawPointerType::specialization(ret, eTpArg, aTpArg, ftp);
			}

			jvxBool* typedPointer()
			{
				return (jvxBool*)fld;
			}
		};
	};
};

typedef jvx::propertyRawPointerType::CjvxRawPointerType jPRG;

typedef jvx::propertyRawPointerType::CjvxRawPointerTypeObject<jvxApiStringList> jPROSTRL;
typedef jvx::propertyRawPointerType::CjvxRawPointerTypeObjectField<jvxSelectionList> jPROSL;
typedef jvx::propertyRawPointerType::CjvxRawPointerTypeObjectField<jvxValueInRange> jPROVR;
typedef jvx::propertyRawPointerType::CjvxRawPointerTypeObject<jvxApiString> jPROS;

typedef jvx::propertyRawPointerType::CjvxRawPointerTypeField<jvxData> jPRFD;
typedef jvx::propertyRawPointerType::CjvxRawPointerTypeField<jvxInt64> jPRFI64;
typedef jvx::propertyRawPointerType::CjvxRawPointerTypeField<jvxInt32> jPRFI32;
typedef jvx::propertyRawPointerType::CjvxRawPointerTypeField<jvxInt16> jPRFI16;
typedef jvx::propertyRawPointerType::CjvxRawPointerTypeField<jvxInt8> jPRFI8;
typedef jvx::propertyRawPointerType::CjvxRawPointerTypeField<jvxUInt64> jPRFU64;
typedef jvx::propertyRawPointerType::CjvxRawPointerTypeField<jvxUInt32> jPRFU32;
typedef jvx::propertyRawPointerType::CjvxRawPointerTypeField<jvxUInt16> jPRFU16;
typedef jPRFU16 jPRFCBOOL;
typedef jvx::propertyRawPointerType::CjvxRawPointerTypeField<jvxUInt8> jPRFU8;
typedef jvx::propertyRawPointerType::CjvxRawPointerTypeFieldSize jPRFS;

// We need to add the template argument here
#define jPRIO jvx::propertyRawPointerType::CjvxRawPointerTypeSimple
#define jPRI jvx::propertyRawPointerType::CjvxRawPointerTypeSimpleI

typedef jvx::propertyRawPointerType::CjvxRawPointerTypeSimpleI<jvxData> jPRD;

#endif

