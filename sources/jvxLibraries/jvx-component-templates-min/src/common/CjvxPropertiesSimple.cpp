#include "common/CjvxPropertiesSimple.h"

CjvxPropertiesSimple::CjvxPropertiesSimple()
{
	// set pointers in triple-node to NULL
	thePropRef = NULL;
	//tpRef = JVX_COMPONENT_UNKNOWN;
}

 jvxErrorType CjvxPropertiesSimple::get_property_id_handle(std::string name, jvxSize *idx,
		 jvxCallManagerProperties& callGate)
	{
		jvxErrorType res;
		jvx::propertyDescriptor::CjvxPropertyDescriptorCore theDescr;
		jvx::propertyAddress::CjvxPropertyAddressLinear ident(0);

		res = refresh_properties();
		if(res != JVX_NO_ERROR)
		{
			reportErrorGet(name, res);
			return res;
		}

		if (thePropRef)
		{
			// retrieve & check property description
			ident.idx = propidmap[name];
			thePropRef->get_descriptor_property(callGate, theDescr, ident);
			if (res == JVX_NO_ERROR)
			{
				if (idx)
				{
					*idx = theDescr.globalIdx;
				}
			}
		}
		return(res);
	}

	jvxErrorType 
		CjvxPropertiesSimple::get_property(std::string name, jvxInt16 *val, jvxSize num , jvxSize offset ,
			jvxCallManagerProperties& callGate)
	{
		return get_property_typesafe(name, val, JVX_DATAFORMAT_16BIT_LE, num, offset, true, callGate);
	}

	jvxErrorType
		CjvxPropertiesSimple::get_property(std::string name, jvxCBool* val, jvxSize num, jvxSize offset,
			jvxCallManagerProperties& callGate)
	{
		return get_property_typesafe(name, val, JVX_DATAFORMAT_BOOL, num, offset, true, callGate);
	}

	jvxErrorType
		CjvxPropertiesSimple::get_property(std::string name, jvxInt32 *val, jvxSize num , jvxSize offset ,
			jvxCallManagerProperties& callGate)
	{
		return get_property_typesafe(name, val, JVX_DATAFORMAT_32BIT_LE, num, offset, true, callGate);
	}

	jvxErrorType 
		CjvxPropertiesSimple::get_property(std::string name, jvxSize *val,
			jvxCallManagerProperties& callGate)
	{
		return get_property_typesafe(name, val, JVX_DATAFORMAT_SIZE, 1, 0, true, callGate);
	}

	jvxErrorType 
		CjvxPropertiesSimple::get_property_number_selected(std::string name, jvxSize *val,
			jvxCallManagerProperties& callGate)
	{
		jvxSelectionList tmp;
		jvxErrorType res;
		res = get_property_typesafe(name, &tmp, JVX_DATAFORMAT_SELECTION_LIST, 1, 0, true, callGate);
		if (res == JVX_NO_ERROR)
		{
			*val = 0;
			while (JVX_EVALUATE_BITFIELD(tmp.bitFieldSelected()))
			{
				tmp.bitFieldSelected() >>= 1;
				(*val)++;
			}
		}
		return res;
	}

	jvxErrorType 
		CjvxPropertiesSimple::get_property(std::string name, std::string &val,
			jvxCallManagerProperties& callGate)
	{
		jvxApiString tmp;
		jvxErrorType res;
		res = get_property_typesafe(name, &tmp, JVX_DATAFORMAT_STRING, 1, 0, true, callGate);
		if(res != JVX_NO_ERROR)
			return res;
		val = tmp.std_str();
		return res;
	}

	jvxErrorType 
		CjvxPropertiesSimple::get_property(std::string name, jvxData *val, jvxSize num , jvxSize offset,
			jvxCallManagerProperties& callGate)
	{
		return get_property_typesafe(name, val, JVX_DATAFORMAT_DATA, num, offset, true, callGate);
	}

	jvxErrorType 
		CjvxPropertiesSimple::set_property(std::string name, jvxInt16 *val, jvxSize num , jvxSize offset,
			jvxCallManagerProperties& callGate)
	{
		return set_property_typesafe(name, val, JVX_DATAFORMAT_16BIT_LE, num, offset, true, callGate);
	}

	jvxErrorType
		CjvxPropertiesSimple::set_property(std::string name, jvxCBool* val, jvxSize num, jvxSize offset,
			jvxCallManagerProperties& callGate)
	{
		return set_property_typesafe(name, val, JVX_DATAFORMAT_BOOL, num, offset, true, callGate);
	}

	jvxErrorType 
		CjvxPropertiesSimple::set_property(std::string name, jvxInt32 *val, jvxSize num , jvxSize offset,
			jvxCallManagerProperties& callGate)
	{
		return set_property_typesafe(name, val, JVX_DATAFORMAT_32BIT_LE, num, offset, true, callGate);
	}

	jvxErrorType 
		CjvxPropertiesSimple::set_property(std::string name, jvxSize *val,
			jvxCallManagerProperties& callGate)
	{
		jvxSelectionList tmp;

		tmp.bitFieldSelected() = 1 << *val;
		return set_property_typesafe(name, &tmp, JVX_DATAFORMAT_SELECTION_LIST, 1 ,0, true, callGate);
	}

	jvxErrorType 
		CjvxPropertiesSimple::set_property(std::string name, jvxApiString *val,
			jvxCallManagerProperties& callGate)
	{
		return set_property_typesafe(name, val, JVX_DATAFORMAT_STRING, 1, 0, true, callGate);
	}

	jvxErrorType 
		CjvxPropertiesSimple::set_property(std::string name, jvxData *val, jvxSize num , jvxSize offset,
			jvxCallManagerProperties& callGate)
	{
		return set_property_typesafe(name, val, JVX_DATAFORMAT_DATA, num, offset, true, callGate);
	}

	jvxErrorType 
	 CjvxPropertiesSimple::get_property_typesafe(std::string name,
		jvxHandle *val,
		jvxDataFormat format,
		jvxSize num ,
		jvxSize offset ,
		jvxBool contentOnly,
		 jvxCallManagerProperties& callGate)
	{
		jvxErrorType res;
		std::map<std::string,int>::iterator it;

		jvx::propertyDescriptor::CjvxPropertyDescriptorCore theDescr;
		jvx::propertyAddress::CjvxPropertyAddressLinear ident(0);

		res = refresh_properties();
		if(res != JVX_NO_ERROR)
		{
			reportErrorGet(name, res);
			return res;
		}

		it = propidmap.find(name);
		if(it == propidmap.end())
		{
			reportErrorGet(name, JVX_ERROR_INVALID_ARGUMENT, "unknown property "+name);
			return JVX_ERROR_INVALID_ARGUMENT;
		}

		if (thePropRef)
		{
			// retrieve & check property description			
			ident.idx = propidmap[name];
			thePropRef->get_descriptor_property(callGate, theDescr, ident);

			if (!JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_ID(ident.idx), thePropRef))
			{
				reportErrorGet(name, JVX_ERROR_WRONG_STATE);
				return JVX_ERROR_WRONG_STATE;
			}

			// num == -1 means get all (val must contain enough space)
			if (num == -1)
				num = theDescr.num;

			if (res != JVX_NO_ERROR)
			{
				reportErrorGet(name, res);
				return res;
			}
			if (theDescr.num < num)
			{
				reportErrorGet(name, JVX_ERROR_SIZE_MISMATCH,
					"requested " + jvx_size2String(num) +
					", but stored only " + jvx_size2String(theDescr.num));
				return JVX_ERROR_SIZE_MISMATCH;
			}
			if (format != theDescr.format)
			{
				reportErrorGet(name, JVX_ERROR_INVALID_ARGUMENT,
					"requested format " + std::string(jvxDataFormat_txt(format)) +
					", but stored format is " + std::string(jvxDataFormat_txt(theDescr.format)));
				return JVX_ERROR_INVALID_ARGUMENT;
			}
			

			// retrieve the property
			res = thePropRef->get_property(callGate, 
				jPRG(val, num, format), jPAGID(theDescr.globalIdx,
				theDescr.category), 
				jPD(contentOnly, offset));
			reportErrorGet(name, res);
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		return res;
	}

	jvxErrorType 
		CjvxPropertiesSimple::set_property_typesafe(std::string name,
		jvxHandle *val,
		jvxDataFormat format,
		jvxSize num ,
		jvxSize offset ,
		jvxBool contentOnly ,
			jvxCallManagerProperties& callGate)
	{
		jvxErrorType res;
		std::map<std::string,int>::iterator it;
		jvx::propertyDescriptor::CjvxPropertyDescriptorCore theDescr;
		jvx::propertyAddress::CjvxPropertyAddressLinear ident(0);

		res = refresh_properties();
		if(res != JVX_NO_ERROR)
		{
			reportErrorSet(name, res);
			return res;
		}

		it = propidmap.find(name);
		if(it == propidmap.end())
		{
			reportErrorSet(name, JVX_ERROR_INVALID_ARGUMENT, "unknown property "+name);
			return JVX_ERROR_INVALID_ARGUMENT;
		}

		// retrieve & check property description
		ident.idx = propidmap[name];
		res = thePropRef->get_descriptor_property(callGate, theDescr, ident);
		if (!JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_ID(propidmap[name]), thePropRef))
		{
			reportErrorSet(name, res);
			return res;
		}

		if(theDescr.num < num)
		{
			reportErrorSet(name, JVX_ERROR_SIZE_MISMATCH,
				"requested "+ jvx_size2String(num) +
				", but stored only "+ jvx_size2String(theDescr.num));
			return JVX_ERROR_SIZE_MISMATCH;
		}
		if(format != theDescr.format)
		{
			reportErrorSet(name, JVX_ERROR_INVALID_ARGUMENT,
				"requested format "+std::string(jvxDataFormat_txt(format)) +
				", but stored format is "+std::string(jvxDataFormat_txt(theDescr.format)) );
			return JVX_ERROR_INVALID_ARGUMENT;
		}
		

		// set the property
		jvx::propertyDetail::CjvxTranferDetail detail(contentOnly, offset);
		jvx::propertyAddress::CjvxPropertyAddressGlobalId addr(theDescr.globalIdx,
			theDescr.category);
		res = thePropRef->set_property(callGate, 
			jPRG(val, num, format), 
			addr, detail);


		reportErrorSet(name, res);
		return res;
	}

	jvxBool CjvxPropertiesSimple::is_ready_properties()
	{
		return (thePropRef != nullptr);
	}

	jvxErrorType 
		CjvxPropertiesSimple::init_properties(IjvxAccessProperties* thePropRefLoc)
	{
		jvxErrorType res = JVX_NO_ERROR;;

		if(!thePropRefLoc)
			return JVX_ERROR_INVALID_ARGUMENT;

		thePropRef = thePropRefLoc;

		savedPropRevision = -1;

		res = refresh_properties();

		return res;
	}

	jvxErrorType CjvxPropertiesSimple::terminate_properties()
	{
		jvxErrorType res = JVX_NO_ERROR;

		propidmap.clear();

		thePropRef = NULL;

		savedPropRevision = -1;
		return(res);
	}


	void 
		CjvxPropertiesSimple::reportErrorGet(std::string name, jvxErrorType err, std::string info)
	{
		if(err != JVX_NO_ERROR)
		{
			std::cerr << "ERROR: failed to get property '"<<name<<"': " << jvxErrorType_txt(err) << std::endl;
			if(info != "")
				std::cerr << "       "<<info<< std::endl;
		}
	}

	void 
		CjvxPropertiesSimple::reportErrorSet(std::string name, jvxErrorType err, std::string info)
	{
		if(err != JVX_NO_ERROR)
		{
			std::cerr << "ERROR: failed to set property '"<<name<<"': " << jvxErrorType_txt(err) << std::endl;
			if(info != "")
				std::cerr << "       "<<info<< std::endl;
		}
	}

	jvxErrorType 
		CjvxPropertiesSimple::refresh_properties()
	{
		jvxErrorType res;
		jvxSize i;
		jvxSize num = 0;
		jvxSize propRevision;
		jvxCallManagerProperties callGate;
		jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
		jvx::propertyAddress::CjvxPropertyAddressLinear ident(0);

		if (thePropRef)
		{
			thePropRef->get_revision(&propRevision, callGate);
			if (propRevision == savedPropRevision)
				return JVX_NO_ERROR; // no refresh needed

			savedPropRevision = propRevision;

			propidmap.clear();

#ifdef CJVXPROPERTIESSIMPLIFY_VERBOSE_OUTPUT
			std::cout << "COMPONENT " << jvxComponentType_txt(tpRef) << std::endl;
#endif

			thePropRef->get_number_properties(callGate, &num);

			for (i = 0; i < num; i++)
			{
				ident.idx = i;
				res = thePropRef->get_descriptor_property(callGate, theDescr, ident);
				if (res != JVX_NO_ERROR)
					return res;

				propidmap[theDescr.descriptor.std_str()] = (jvxInt32)i;

#ifdef CJVXPROPERTIESSIMPLIFY_VERBOSE_OUTPUT
				std::cout << "---> PROPERTY " << str;
				if (accessType == JVX_PROPERTY_ACCESS_READ_ONLY)
					std::cout << " (read only)";
				std::cout << ", idx: " << i << ", hdlIdx: " << hdlIdx << ", format: " << format << std::endl;
#endif
			}
		
		}
		return JVX_NO_ERROR;
	}
