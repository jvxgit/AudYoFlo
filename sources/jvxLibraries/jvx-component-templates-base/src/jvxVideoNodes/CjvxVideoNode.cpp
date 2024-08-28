#include "jvx.h"
#include "jvxVideoNodes/CjvxVideoNode.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

CjvxVideoNode::CjvxVideoNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE): CjvxBareNode1io(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
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

	CjvxVideoNode::~CjvxVideoNode()
{
}

jvxErrorType
CjvxVideoNode::activate()
{
	jvxErrorType res = CjvxBareNode1io::activate();
	if(res == JVX_NO_ERROR)
	{
		CjvxVideoNode_genpcg::init_all();
		CjvxVideoNode_genpcg::allocate_all();
		CjvxVideoNode_genpcg::register_all(static_cast<CjvxProperties*>(this));
	}
	return(res);
};

jvxErrorType
CjvxVideoNode::deactivate()
{
	jvxErrorType res = CjvxBareNode1io::deactivate();
	if(res == JVX_NO_ERROR)
	{
		CjvxVideoNode_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxVideoNode_genpcg::deallocate_all();
	}
	return(res);
};


#ifdef JVX_PROJECT_NAMESPACE
}
#endif
