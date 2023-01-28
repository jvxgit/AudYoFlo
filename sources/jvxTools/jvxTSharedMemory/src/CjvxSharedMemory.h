#ifndef __CJVXTHREADCONTROLLER_H__
#define __CJVXTHREADCONTROLLER_H__

#include <string>
#include <vector>

#include "jvx.h"
#include "common/CjvxObject.h"

class oneAreaShared
{
public:
	std::string name;
	std::string description;
	IjvxSharedMemoryOwner* owner;
	std::list<IjvxSharedMemoryListener*> listeners;
	jvxComponentIdentification origin;

	IjvxSharedMemoryListener* theListener;

	jvxByte* fld;
	jvxDataFormat format;
	jvxSize num;
	jvxSize szOneElm;
	jvxSize szAllElms;
	jvxSize uId;
	jvxSize version_id;
	jvxSharedMemoryAccessMode mode;

	oneAreaShared()
	{
		origin.tp = JVX_COMPONENT_UNKNOWN;
		theListener = NULL;
		owner = NULL;

		fld = NULL;
		format = JVX_DATAFORMAT_NONE;
		num = 0;
		szOneElm = 0;
		szAllElms = 0;
		uId = JVX_SIZE_UNSELECTED;
		version_id = 0;
		mode = JVX_SHARED_MEMORY_READ_AND_WRITE_ACCESS;
	};
};

class oneMemoryListener
{
public:
	std::string wc_memory;
	jvxComponentIdentification id;
	IjvxSharedMemoryListener* theListener;
	oneMemoryListener()
	{
		id = JVX_COMPONENT_UNKNOWN;
		theListener = NULL;
	};
};

// =====================================================================================

class CjvxSharedMemory : public IjvxSharedMemory, public IjvxGlobalInstance, public CjvxObject
{
public:

private:

	JVX_MUTEX_HANDLE safeAccess;

	std::map<jvxSize, oneAreaShared> allocated_areas;

	std::map<IjvxSharedMemoryListener*, oneMemoryListener> global_listener;

	jvxSize myUniqueSeed;

public:

	//! Constructor: Do nothing
	CjvxSharedMemory(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	//! Destructor
	~CjvxSharedMemory();

	virtual jvxErrorType initialize();
	virtual jvxErrorType terminate(); 

	// =====================================================================================
	// Interface API
	// =====================================================================================
	virtual jvxErrorType create_area(const char* name, 
		const char* description, 
		jvxSize* uId, 
		jvxDataFormat format, 
		jvxSize numElements, 
		IjvxSharedMemoryOwner* theOwner,
		jvxComponentIdentification origin,
		jvxSharedMemoryAccessMode mode) override;

	virtual jvxErrorType destroy_area(jvxSize uId, IjvxSharedMemoryOwner* theOwner) override;

	// ====================================================================================

	virtual jvxErrorType number_areas(jvxSize* num) override;
	virtual jvxErrorType ident_area(jvxSize id, jvxSize* uId) override;
	virtual jvxErrorType description_area(jvxSize uId, jvxApiString* nm, jvxApiString* description, jvxDataFormat* format, jvxSize* numElements) override;

	// ====================================================================================

	virtual jvxErrorType start_observe_shared_memory(IjvxSharedMemoryListener* theOwner, const char* wc_report, jvxComponentIdentification cp_id)override;
	virtual jvxErrorType stop_observe_shared_memory(IjvxSharedMemoryListener* theOwner)override;

	virtual jvxErrorType start_observe_area(jvxSize uId, IjvxSharedMemoryListener* theOwner)override;
	virtual jvxErrorType stop_observe_area(jvxSize uId, IjvxSharedMemoryListener* theOwner) override;

	// ====================================================================================

	virtual jvxErrorType try_lock_area(jvxSize uId, 
		IjvxSharedMemoryListener* listener, 
		jvxSharedMemoryAccessMode acc,
		IjvxSharedMemoryOwner** owner,
		jvxHandle** mem,
		jvxDataFormat* form = NULL,
		jvxSize* num = NULL,
		jvxSize* version_id = NULL,
		jvxComponentIdentification* origin = NULL,
		jvxSharedMemoryAccessMode* mode = NULL) override;

	virtual jvxErrorType unlock_area(jvxSize uId, 
		IjvxSharedMemoryListener* theListener, 
		jvxBool content_written) override;

	// ===========================================================================================

#define JVX_OBJECT_SKIP_GLOBALPREPOST_CALLBACKS
#include "codeFragments/simplify/jvxObjects_simplify.h"
#undef JVX_OBJECT_SKIP_GLOBALPREPOST_CALLBACKS

	virtual jvxErrorType JVX_CALLINGCONVENTION single_instance_post_bootup(IjvxHiddenInterface* hostRef) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION single_instance_pre_shutdown() override;

	// ===========================================================================================

	void report_all_areas_start(oneMemoryListener* oneList);
	void report_all_areas_stop(oneMemoryListener* oneList);
	jvxBool check_match_area(oneMemoryListener* oneList, const std::string& descr, const jvxComponentIdentification& cpId);
};

#endif
