#ifndef __ALLHOSTSSTATIC_COMMON_H__
#define __ALLHOSTSSTATIC_COMMON_H__

class oneAddedStaticComponent
{
public:
	IjvxObject* theStaticObject = nullptr;
	IjvxGlobalInstance* theStaticGlobal = nullptr;
	jvxInitObject_tp funcInit = nullptr;
	jvxTerminateObject_tp funcTerm = nullptr;
	jvxBool run_init;
	IjvxObject* packageRef = nullptr;
	oneAddedStaticComponent()
	{
		reset();
	}
	void reset()
	{
		theStaticObject = NULL;
		theStaticGlobal = NULL;
		funcInit = NULL;
		funcTerm = NULL;
		run_init = false;
	};
};

#define LOAD_ONE_MODULE_LIB_SIMPLE(FUNC, DESRIPTION, HOST_REF)		\
	addMe = NULL;							\
	res = FUNC(&addMe);						\
	assert(res == JVX_NO_ERROR);					\
	assert(addMe);							\
	res = HOST_REF->add_external_component(addMe, "static host component"); \
	assert(res == JVX_NO_ERROR);

#define LOAD_ONE_MODULE_LIB_FULL(FUNCI, FUNCT, DESRIPTION, LST_COMP, HOST_REF) \
	comp.funcInit = FUNCI;						\
	comp.funcTerm = FUNCT;						\
	comp.theStaticObject = NULL;					\
	res = FUNCI(&comp.theStaticObject, &comp.theStaticGlobal, NULL);				\
	assert(res == JVX_NO_ERROR);					\
	assert(comp.theStaticObject);					\
	res = HOST_REF->add_external_component(comp.theStaticObject, comp.theStaticGlobal, DESRIPTION, true, comp.funcInit, comp.funcTerm); \
	assert(res == JVX_NO_ERROR);					\
	LST_COMP.push_back(comp);

#define UNLOAD_ONE_MODULE_LIB_FULL(INST, HOST_REF) \
	HOST_REF->remove_external_component(INST.theStaticObject); \
	INST.funcTerm(INST.theStaticObject); \
	INST.theStaticObject = NULL; \
	INST.funcInit = NULL; \
	INST.funcTerm = NULL;

// ##################################################################################
#define FIND_PROPERTY_ID(theT, idx, TOKEN, isV, found, callGate)			\
	{								\
		jvxSize i;						\
		jvxSize num = 0;					\
		std::string str;					\
		found = false; \
	    if(theT) \
		{ \
			theT->get_number_properties(callGate, &num);			\
			for(i = 0; i < num; i++)				\
			{						\
				jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr; \
				jvx::propertyAddress::CjvxPropertyAddressLinear ident(i); \
				theT->get_descriptor_property(callGate, theDescr, ident); \
				str = theDescr.descriptor.std_str();			\
				if(str == TOKEN)			\
					{				\
						found = true;		\
						break;			\
					}				\
			}						\
		} \
	}

#define FIND_PROPERTY_ID_SZ(theT, idx, hdlSz, TOKEN, isV, found)	\
	{								\
		jvxSize i;						\
		jvxSize num = 0;					\
		jvxApiString descror;				\
		std::string str;					\
		found = false; \
		if(theT) \
		{ \
			theT->theProps->number_properties(&num);			\
			for(i = 0; i < num; i++)				\
			{						\
				theT->theProps->description_property(i, NULL, NULL, NULL, NULL, hdlSz, NULL, NULL, &idx, NULL, NULL, NULL, &descror, &isV); \
				str = descror.std_str();			\
				if(str == TOKEN)			\
					{				\
						found = true;		\
						break;			\
					}				\
			}						\
		} \
	}

#define FIND_PROPERTY(theT, TOKEN, isV, found, callGate)				\
	{								\
		jvxSize i;						\
		jvxSize num = 0;					\
		jvxApiString descror;				\
		std::string str;					\
		found = false; \
		if(theT) \
		{ \
			theT->number_properties(callGate, &num);			\
			for(i = 0; i < num; i++)				\
			{	\
				theT->description_property(callGate, i, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &hdlIdx, NULL, NULL, NULL, &descror, &isV, NULL); \
				str = descror.std_str();			\
				if(str == TOKEN)			\
					{				\
						found = true;		\
						break;			\
					}				\
			}						\
		} \
	}

#endif
