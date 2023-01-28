#ifndef __IJVXSHAREDMEMORY_H__
#define __IJVXSHAREDMEMORY_H__

typedef enum
{
	JVX_SHARED_MEMORY_NO_ACCESS = 0,
	JVX_SHARED_MEMORY_READ_ACCESS = 0x1,
	JVX_SHARED_MEMORY_WRITE_ACCESS = 0x2,
	JVX_SHARED_MEMORY_READ_AND_WRITE_ACCESS = 0x3
} jvxSharedMemoryAccessMode;

typedef enum
{
	JVX_SHARED_MEMORY_DESTROYED = 0,
	JVX_SHARED_MEMORY_CONTENT_CHANGED = 1,
	JVX_SHARED_MEMORY_CONTENT_FIRST_ACCESS = 2
} jvxSharedMemoryChangeType;

JVX_INTERFACE IjvxSharedMemoryOwner
{
public:
	virtual ~IjvxSharedMemoryOwner(){};
	
	// ====================================================================================

	virtual jvxErrorType identify_owner(jvxApiString* astr, jvxComponentIdentification* cpId) = 0;
	virtual jvxErrorType owned_content_has_changed(jvxSize uId) = 0;
};
	
JVX_INTERFACE IjvxSharedMemoryListener
{
public:
	virtual ~IjvxSharedMemoryListener(){};
	
	// ====================================================================================

	virtual jvxErrorType identify_listener(jvxApiString* astr, jvxComponentIdentification* cpId) = 0;
	virtual jvxErrorType observed_content_has_changed(jvxSize uId, jvxSharedMemoryChangeType tp) = 0;

	virtual jvxErrorType content_has_been_added(jvxSize uId, const char* name, const char* description, jvxComponentIdentification id_cp) = 0;
	virtual jvxErrorType content_has_been_removed(jvxSize uId) = 0;
};

/**
 * Interface to access shared memory object 
 *///=====================================================================================
JVX_INTERFACE IjvxSharedMemory: public IjvxObject
{
public:

	virtual ~IjvxSharedMemory() {};
	
	// ====================================================================================

	virtual jvxErrorType create_area(
		const char* name, 
		const char* description, 
		jvxSize* uId, 
		jvxDataFormat format, 
		jvxSize numElements, 
		IjvxSharedMemoryOwner* theOwner, 
		jvxComponentIdentification origin, 
		jvxSharedMemoryAccessMode mode) = 0;
	virtual jvxErrorType destroy_area(jvxSize uId, 
		IjvxSharedMemoryOwner* theOwner) = 0;
	
	// ====================================================================================
	
	virtual jvxErrorType number_areas(jvxSize* num) = 0;
	virtual jvxErrorType ident_area(jvxSize id, jvxSize* uId) = 0;
	virtual jvxErrorType description_area(jvxSize uId, jvxApiString* nm, jvxApiString* description, jvxDataFormat* format, jvxSize* numElements) = 0;

	// ====================================================================================

	virtual jvxErrorType start_observe_shared_memory(
		IjvxSharedMemoryListener* theOwner, 
		const char* filter_wc_desciption, 
		jvxComponentIdentification filter_cp_id) = 0;
	virtual jvxErrorType stop_observe_shared_memory(IjvxSharedMemoryListener* theOwner) = 0;

	// ====================================================================================

	virtual jvxErrorType start_observe_area(jvxSize uId, IjvxSharedMemoryListener* theOwner) = 0;
	virtual jvxErrorType stop_observe_area(jvxSize uId, IjvxSharedMemoryListener* theOwner) = 0;

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
		jvxSharedMemoryAccessMode* mode = NULL) = 0;

	virtual jvxErrorType unlock_area(jvxSize uId, 
		IjvxSharedMemoryListener* theListener, 
		jvxBool content_written) = 0;
};

#endif


	