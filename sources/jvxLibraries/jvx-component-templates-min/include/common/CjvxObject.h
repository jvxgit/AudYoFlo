#ifndef __CJVXOBJECT_H__
#define __CJVXOBJECT_H__

#include "jvx.h"
#include "jvx-helpers.h"
#include <map>
#include "common/CjvxObjectMin.h"
#include "common/CjvxObjectLog.h"

#ifdef JVX_OBJECTS_WITH_TEXTLOG
#ifndef JVX_COBJECT_OS_BUF_SIZE 
#define JVX_COBJECT_OS_BUF_SIZE 128
#endif
#endif

#ifndef JVX_OBJECTS_WITH_TEXTLOG
#define jvxrtst std::cout
#endif

class CjvxObjectCore : public CjvxObjectMin
{
public:
	CjvxObjectCore(const std::string& descr) : CjvxObjectMin(descr) 
	{
	};

	jvxErrorType _prepare();
	jvxErrorType _start();
	jvxErrorType _stop();
	jvxErrorType _postprocess();

};

class CjvxObject: public CjvxObjectCore
#ifdef JVX_OBJECTS_WITH_TEXTLOG
	, public CjvxObjectLog
#endif
{
protected:

	struct commonSetType
	{		
		std::string theName;
		std::string theDescriptor;
		std::string theModuleName;
		jvxComponentAccessType theComponentAccessTp = JVX_COMPONENT_ACCESS_UNKNOWN;
		jvxBitField theFeatureClass;
		jvxComponentIdentification theComponentType;
		std::string theComponentSubTypeDescriptor;

		std::string compileOptions;
		
		jvxHandle* theObjectSpecialization = nullptr;
		bool theMultipleObjects = false;

		jvxInt32 theErrorid = -1;	// -1 = no info
		jvxInt32 theErrorlevel = -1; // -1 = no info
		std::string theErrordescr;

		//IjvxHost* theHostRef;		
		IjvxToolsHost* theToolsHost = nullptr;	
		IjvxUniqueId* theUniqueId = nullptr;
		IjvxReport* theReport = nullptr;

		IjvxObject* thisisme = nullptr;
		IjvxObject* templ = nullptr;

		std::vector<std::string> theInformation;
		struct
		{
			JVX_MUTEX_HANDLE the_lock;
		} _safeAccessStateBound;

		struct
		{
			jvxCBitField prepare_flags;
			jvxCBitField start_flags;
		} stateChecks;

		std::map<jvxSize, jvxHandle*> registeredUserData;
		jvxSize register_host_id = JVX_SIZE_UNSELECTED;
		jvxBool iamready = false;
		std::string reasonnotready;
		jvxBool iamalive = false;
		
		IjvxInterfaceFactory* theInterfaceFactory = nullptr;
	};
	
	commonSetType _common_set;
	jvxSize refCnt = 0;

	/*
	struct
	{
		jvxSize theNumberEndpoints;
		IjvxDataProcessor** theEndpoints;
	} _common_set_processing;
	*/

public:

	//CjvxObject(const char* descr, bool multipleObjects, const char* module_name, const char* descriptor = NULL, jvxBitField featClass = (jvxBitField)JVX_FEATURE_CLASS_NONE)
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	~CjvxObject();

	// =========================================================================
	// Map interface IjvxObject
	// =========================================================================

	jvxErrorType _descriptor(jvxApiString* str, jvxApiString* subComponentDescriptor);

	jvxErrorType _set_location_info(const jvxComponentIdentification& tp);
	jvxErrorType _location_info(jvxComponentIdentification& tp);
	jvxErrorType _name(jvxApiString*);
	jvxErrorType _description(jvxApiString* str);
	jvxErrorType _module_reference(jvxApiString* str, jvxComponentAccessType* acTp);
	 jvxErrorType _feature_class(jvxBitField* ft);
	 jvxErrorType _version(jvxApiString* str);
	 jvxErrorType _lasterror(jvxApiError* err_argout);

	 jvxErrorType _requestComponentSpecialization(jvxHandle** obj, jvxComponentIdentification* compTp, jvxBool* multipleObjects, jvxApiString* subComponentDescriptor);

	 // =========================================================================
	  // Interface IjvxHiddenInterface
	 // =========================================================================
	 jvxErrorType _request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl);
	 jvxErrorType _return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl);

	 // =========================================================================
	// Interface IjvxSystemStatus
	// =========================================================================
	 jvxErrorType _system_ready();
	 jvxErrorType _system_about_to_shutdown();


	 void _seterror(jvxErrorType tp);

	 jvxErrorType _return_seterr(jvxErrorType tp, jvxInt32 errId = -1, jvxInt32 errLvl = -1);


	 jvxErrorType _compile_options(jvxApiString* str);


	jvxErrorType _set_user_data(jvxSize idUserData, jvxHandle* floating_pointer);
	
	jvxErrorType _user_data(jvxSize idUserData, jvxHandle** floating_pointer);

	// =========================================================================

	jvxErrorType _interface_factory(IjvxInterfaceFactory** ref);


	// ===========================================================================

	// State handling functions
	//inline jvxErrorType _initialize(IjvxHost* hostRef = NULL)
	

	jvxErrorType _select(IjvxObject* dependsOn = NULL) override;

	// ===========================================================================

	 jvxErrorType _activate() override;
	 jvxErrorType _activate_no_text_log();
	 jvxErrorType _activate_lock();	 

	// ===========================================================================

	jvxErrorType _is_ready(jvxBool* suc, jvxApiString* reasonIfNot);

	// ===========================================================================

	jvxErrorType _is_alive(jvxBool* alive);

	// ===========================================================================

	 jvxErrorType _prepare_lock();
	 jvxErrorType _start_lock();
	 jvxErrorType _stop_lock();
	 jvxErrorType _postprocess_lock();

	// ===========================================================================

	 jvxErrorType _deactivate()override;
	 jvxErrorType _deactivate_no_text_log();
	 jvxErrorType _deactivate_lock();

	// ===========================================================================

	 jvxErrorType _unselect()override;

	 jvxErrorType _lock_state();

	 jvxErrorType _try_lock_state();

	 jvxErrorType _unlock_state();

	jvxErrorType _report_text_message(const char* txt, jvxReportPriority prio);

	jvxErrorType _report_command_request(jvxCBitField req, 
		jvxHandle* caseSpecificData = NULL, jvxSize szSpecificData = 0);

	jvxErrorType _request_command(const CjvxReportCommandRequest& request);

	jvxErrorType _report_property_has_changed(jvxPropertyCategoryType cat, jvxSize idProp, jvxBool onlyCont,
		jvxSize offset_start, jvxSize numElements,
			const jvxComponentIdentification& cpTo, jvxPropertyCallPurpose callpurp = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC);

	jvxErrorType _request_test_chain_master(jvxSize processId);

	jvxErrorType _report_take_over_property(jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent, 
		jvxPropertyCategoryType cat, jvxSize idProp, const jvxComponentIdentification& cpTo);

	virtual jvxErrorType _number_info_tokens(jvxSize* num);

	virtual jvxErrorType _info_token(jvxSize idx, jvxApiString* fldStr);

	 void _request_text_log();

	 void _return_text_log();

	 void _append_compile_option(const std::string& oneOption);

	 //jvxErrorType _check_access_property_inst(jvxFlagTag flagtag, jvxAccessProtocol* accProt);

	 jvxErrorType _request_unique_object_id(jvxSize* uId);

     IjvxReport* _request_report();

     void _release_report(IjvxReport* theReport);

	 jvxErrorType _increment_reference();
	 jvxErrorType _current_reference(jvxSize* cnt);
	 jvxErrorType _decrement_reference();

	 virtual jvxErrorType JVX_CALLINGCONVENTION report_properties_modified(const char* props_set) override;
};

#define JVX_DECLARE_OBJECT_REFERENCES(COMP_TP, COMP_IF) \
	_common_set.theComponentType.unselected(COMP_TP); \
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<COMP_IF*>(this)); \
	_common_set.theInterfaceFactory = static_cast<IjvxInterfaceFactory*>(this); \
	_common_set.thisisme = static_cast<IjvxObject*>(this)

#endif
