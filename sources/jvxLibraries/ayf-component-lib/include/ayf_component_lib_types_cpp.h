#ifndef __AYF_COMPONENT_LIB_TYPES_CPP_H__
#define __AYF_COMPONENT_LIB_TYPES_CPP_H__

struct ayfConnectArgAllocateDeallocate
{
	IjvxObject*& entryObj;
	std::list< IjvxObject*>& subsequentComponents;
	void_pvoid_callback ptr_callback_multipurpose = NULL;
	void* prv_callback_multipurpose = NULL;
	jvxHandle* embeddingInfo = NULL;

	ayfConnectArgAllocateDeallocate(IjvxObject*& mainObjRef, std::list< IjvxObject*>& subsequentComponentsRef,
		void_pvoid_callback ptr_callback_multipurposeArg = NULL, void* prv_callback_multipurposeArg = NULL,
		jvxHandle* embeddingInfoArg = NULL) :
		entryObj(mainObjRef), subsequentComponents(subsequentComponentsRef),
		ptr_callback_multipurpose(ptr_callback_multipurposeArg), prv_callback_multipurpose(prv_callback_multipurposeArg), embeddingInfo(embeddingInfoArg) {
	};
};

#endif
