#ifndef __CJVXVIDEONODETPL_H__
#define __CJVXVIDEONODETPL_H__

#include "jvx.h"
#include "pcg_CjvxVideoNode_pcg.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

// CjvxBareNode1io

template <class T>
class CjvxVideoNodeTpl : public T, public CjvxVideoNode_genpcg
{
protected:

public:

	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxVideoNodeTpl(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
		T(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
	{
		if (tpComp == JVX_COMPONENT_UNKNOWN)
		{
			tpComp = JVX_COMPONENT_VIDEO_NODE;
		}
		_common_set.theComponentType.unselected(tpComp);
		_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxNode*>(this));
		_common_set.thisisme = static_cast<IjvxObject*>(this);

		_common_set.theComponentSubTypeDescriptor = "signal_processing_node/video_node";

		// Allow zerocopy passthrough
		zeroCopyBuffering_rt = true;

		// Preset for video
		neg_input.negBehavior = CjvxNegotiate_common::negBehaviorType::JVX_BEHAVIOR_VIDEO;
	}

	virtual JVX_CALLINGCONVENTION ~CjvxVideoNodeTpl()
	{
	};

	jvxErrorType activate() override
	{
		jvxErrorType res = T::activate();
		if (res == JVX_NO_ERROR)
		{
			CjvxVideoNode_genpcg::init_all();
			CjvxVideoNode_genpcg::allocate_all();
			CjvxVideoNode_genpcg::register_all(static_cast<CjvxProperties*>(this));
		}
		return(res);
	};

	jvxErrorType deactivate() override
	{
		jvxErrorType res = T::deactivate();
		if (res == JVX_NO_ERROR)
		{
			CjvxVideoNode_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
			CjvxVideoNode_genpcg::deallocate_all();
		}
		return(res);
	};
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
