#ifndef __CJVXMEXCALLS_H__
#define __CJVXMEXCALLS_H__

#include "jvx.h"
#include "common/CjvxProperties.h"
#include "CjvxMexCallsProfiler.h"


/**
 * Class to encapsulate mexcall functoionality for integration in various classes
 *///===============================================================================

class CjvxMexCalls: public IjvxExternalCallTarget, public CjvxMexCallsProfiler
{

private:

	jvxHandle* theSubclass;

protected:

public:
	
	CjvxMexCalls();

	~CjvxMexCalls();

	jvxErrorType select(IjvxHiddenInterface* theHost, CjvxProperties* theProps, const char* c_postfix = "", const char* component_specified = NULL);

	jvxErrorType unselect();

	jvxErrorType activate();

	jvxErrorType deactivate();
	
	jvxErrorType put_configuration(jvxCallManagerConfiguration* callConf,
		IjvxConfigProcessor* processor, jvxState theState, jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename, jvxInt32 lineno );
	
	jvxErrorType get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections);

	jvxErrorType prepare();
	
	jvxErrorType prepare_sender_to_receiver(jvxLinkDataDescriptor* theData);
	jvxErrorType prepare_connect_icon_enter(jvxLinkDataDescriptor* theData);

	jvxErrorType prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData, jvxLinkDataDescriptor* theData_out, const char** hintsTags = NULL, const char** hintsValues = NULL, jvxSize numHints = 0);
	jvxErrorType prepare_connect_icon_leave(jvxLinkDataDescriptor* theData, jvxLinkDataDescriptor* theData_out, const char** hintsTags = NULL, const char** hintsValues = NULL, jvxSize numHints = 0);

	jvxErrorType is_extcall_reference_present(jvxBool* isPresent);

	jvxErrorType is_matlab_processing_engaged(jvxBool* isEngaged);

	jvxErrorType process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender, jvxLinkDataDescriptor* theData_out = NULL);
	jvxErrorType process_buffers_icon(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out);

	jvxErrorType postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData);
	jvxErrorType postprocess_connect_icon_leave(jvxLinkDataDescriptor* theData);

	jvxErrorType before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData);
	jvxErrorType postprocess_connect_icon_enter(jvxLinkDataDescriptor* theData);

	jvxErrorType postprocess();

	jvxErrorType extcall_lasterror(jvxApiError* theErr);

	jvxErrorType extcall_set(const char* descr, int level, int id);

};

// ===========================================================================
// Some valuable macros for Matlab entries
// ===========================================================================

#define JVX_MEXCALL_DEFINE_COMPONENT_NAME std::string _theExtCallObjectName;

#define JVX_MEXCALL_SELECT_SIMPLE(classname, baseclassname) \
	jvxErrorType classname::select(IjvxObject* theOwner) \
	{ \
		jvxErrorType res = JVX_NO_ERROR; \
		res = baseclassname::select(theOwner); \
		if(res == JVX_NO_ERROR) \
		{ \
			res = CjvxMexCalls::select(_common_set.theHostRef, static_cast<CjvxProperties*>(this), "", #classname); \
		} \
		return(res); \
	}

#define JVX_MEXCALL_SELECT(classname, baseclassname, compNameStr) \
	jvxErrorType classname::select(IjvxObject* theOwner) \
	{ \
		jvxErrorType res = JVX_NO_ERROR; \
		res = baseclassname::select(theOwner); \
		if(res == JVX_NO_ERROR) \
		{ \
			res = CjvxMexCalls::select(_common_set_min.theHostRef, static_cast<CjvxProperties*>(this), "", compNameStr.c_str()); \
		} \
		return(res); \
	}

#define JVX_MEXCALL_SELECT_REGISTER(classname, baseclassname, compNameStr) \
	jvxErrorType classname::select(IjvxObject* theOwner) \
	{ \
		jvxErrorType res = JVX_NO_ERROR; \
		res = baseclassname::select(theOwner); \
		if(res == JVX_NO_ERROR) \
		{ \
			requestExternalCallHandle(_common_set_min.theHostRef, &_theExtCallHandler); \
			initExternalCallhandler(_theExtCallHandler, static_cast<IjvxExternalCallTarget*>(this), compNameStr); \
			res = CjvxMexCalls::select(_common_set_min.theHostRef, static_cast<CjvxProperties*>(this), "", compNameStr.c_str()); \
		} \
		return(res); \
	}


#define JVX_MEXCALL_UNSELECT(classname, baseclassname, compNameStr) jvxErrorType \
	classname::unselect() \
	{ \
		jvxErrorType res = JVX_NO_ERROR; \
		res = baseclassname::unselect(); \
		if(res == JVX_NO_ERROR) \
		{ \
			res = CjvxMexCalls::unselect(); \
			compNameStr = ""; \
		} \
	return(res); \
}

#define JVX_MEXCALL_UNSELECT_SIMPLE(classname, baseclassname) jvxErrorType \
	classname::unselect() \
	{ \
		jvxErrorType res = JVX_NO_ERROR; \
		res = baseclassname::unselect(); \
		if(res == JVX_NO_ERROR) \
		{ \
			res = CjvxMexCalls::unselect(); \
		} \
	return(res); \
}

#define JVX_MEXCALL_UNSELECT_UNREGISTER(classname, baseclassname, compNameStr) jvxErrorType \
	classname::unselect() \
	{ \
		jvxErrorType res = JVX_NO_ERROR; \
		res = baseclassname::unselect(); \
		if(res == JVX_NO_ERROR) \
		{ \
			res = CjvxMexCalls::unselect(); \
			terminateExternalCallhandler(_theExtCallHandler); \
			returnExternalCallHandle(_common_set_min.theHostRef, _theExtCallHandler); \
			compNameStr = ""; \
		} \
		return(res); \
	}

#define JVX_MEXCALL_ACTIVATE(classname, baseclassname) jvxErrorType \
	classname::activate() \
	{ \
		jvxErrorType res = baseclassname::activate(); \
		if(res == JVX_NO_ERROR) \
		{ \
			res = CjvxMexCalls::activate(); \
		} \
		return(res); \
	}

#define JVX_MEXCALL_DEACTIVATE(classname, baseclassname) jvxErrorType \
	classname::deactivate() \
	{ \
		jvxErrorType res = baseclassname::deactivate(); \
		if(res == JVX_NO_ERROR) \
		{ \
			res = CjvxMexCalls::deactivate(); \
		} \
		return(res); \
	}


#define JVX_MEXCALL_PREPARE(classname, baseclassname) jvxErrorType \
	classname::prepare() \
	{ \
		jvxErrorType res = baseclassname::prepare(); \
		if(res == JVX_NO_ERROR) \
		{ \
			res = CjvxMexCalls::prepare(); \
		} \
		return(res); \
	}

#define JVX_MEXCALL_PREPARE_SENDER_TO_RECEIVER(classname, baseclassname) jvxErrorType \
	classname::prepare_sender_to_receiver(jvxLinkDataDescriptor* theData) \
	{ \
		jvxErrorType res = baseclassname::prepare_sender_to_receiver(theData); \
		if(res == JVX_NO_ERROR) \
		{ \
			res = CjvxMexCalls::prepare_sender_to_receiver(theData); \
		} \
		return(res); \
	}

#define JVX_MEXCALL_PREPARE_COMPLETE_RECEIVER_TO_SENDER(classname, baseclassname, theData_out) jvxErrorType \
	classname::prepare_complete_receiver_to_sender(jvxLinkDataDescriptor* theData) \
	{ \
		jvxErrorType res = baseclassname::prepare_complete_receiver_to_sender(theData); \
		if(res == JVX_NO_ERROR) \
		{ \
			res = CjvxMexCalls::prepare_complete_receiver_to_sender(theData, theData_out); \
		} \
		return(res); \
	}

#define JVX_MEXCALL_POSTPROCESS_SENDER_TO_RECEIVER(classname, baseclassname) jvxErrorType  \
	classname::postprocess_sender_to_receiver(jvxLinkDataDescriptor* theData) \
	{ \
		jvxErrorType res = baseclassname::postprocess_sender_to_receiver(theData); \
		if(res == JVX_NO_ERROR) \
		{ \
			res = CjvxMexCalls::postprocess_sender_to_receiver(theData); \
		} \
		return(res); \
	}

#define JVX_MEXCALL_BEFORE_POSTPROCESS_RECEIVER_TO_SENDER(classname, baseclassname) jvxErrorType \
	classname::before_postprocess_receiver_to_sender(jvxLinkDataDescriptor* theData) \
	{ \
		jvxErrorType res = baseclassname::before_postprocess_receiver_to_sender(theData); \
		if(res == JVX_NO_ERROR) \
		{ \
			res = CjvxMexCalls::before_postprocess_receiver_to_sender(theData); \
		} \
		return(res); \
	}

#define JVX_MEXCALL_POSTPROCESS(classname, baseclassname) jvxErrorType \
	classname::postprocess() \
	{ \
		jvxErrorType res = baseclassname::postprocess(); \
		if(res == JVX_NO_ERROR) \
		{ \
			res = CjvxMexCalls::postprocess(); \
		} \
		return(res); \
	} 

#define JVX_MEXCALL_PUT_CONFIGURATION(classname, baseclassname) jvxErrorType \
	classname::put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor, \
		jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename,jvxInt32 lineno) \
	{ \
		std::vector<std::string> warnings; \
		jvxErrorType res =  baseclassname::put_configuration(callMan, processor, sectionToContainAllSubsectionsForMe, filename, lineno); \
		if(res != JVX_NO_ERROR) \
			return res; \
		res = CjvxMexCalls::put_configuration(callMan, processor, this->_common_set_min.theState, sectionToContainAllSubsectionsForMe, filename, lineno); \
		return res; \
	}

#define JVX_MEXCALL_PUT_CONFIGURATION_NO_BASE(classname) jvxErrorType \
	classname::put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor, \
		jvxHandle* sectionToContainAllSubsectionsForMe,const char* filename,jvxInt32 lineno) \
	{ \
		jvxErrorType res = JVX_NO_ERROR; \
		std::vector<std::string> warnings; \
		res = CjvxMexCalls::put_configuration(callMan, processor, this->_common_set_min.theState, sectionToContainAllSubsectionsForMe, \
			filename, lineno ); \
		return res; \
	}

#define JVX_MEXCALL_GET_CONFIGURATION(classname, baseclassname) jvxErrorType \
	classname::get_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor, jvxHandle* sectionToWriteTo ) \
	{ \
		std::vector<std::string> warnings; \
		jvxErrorType res =  baseclassname::get_configuration(callMan, processor, sectionToWriteTo); \
		if(res != JVX_NO_ERROR) \
			return res; \
		res = CjvxMexCalls::get_configuration(callMan, processor, sectionToWriteTo); \
		return res; \
	}

#define JVX_MEXCALL_GET_CONFIGURATION_NO_BASE(classname) jvxErrorType \
	classname::get_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor, \
		jvxHandle* sectionWhereToAddAllSubsections) \
	{ \
		jvxErrorType res = JVX_NO_ERROR; \
		res = CjvxMexCalls::get_configuration(callMan, processor, sectionWhereToAddAllSubsections); \
		return res; \
	}

#define JVX_MEXCALL_PROCESS(classname, baseclassname) jvxErrorType \
	classname::process_st(jvxLinkDataDescriptor* theData, jvxSize idx_sender_to_receiver, jvxSize idx_receiver_to_sender) \
	{ \
		jvxErrorType res = JVX_NO_ERROR; \
		res = CjvxMexCalls::process_st(theData, idx_sender_to_receiver, idx_receiver_to_sender); \
		return res; \
	}

#endif