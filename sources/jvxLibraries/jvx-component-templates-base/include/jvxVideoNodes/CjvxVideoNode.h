#ifndef __CJVXVIDEONODE_H__
#define __CJVXVIDEONODE_H__

#include "jvx.h"
// #include "jvxNodes/CjvxBareNode1ioRearrange.h"
#include "jvxNodes/CjvxBareNode1io.h"
#include "pcg_CjvxVideoNode_pcg.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxVideoNode : public CjvxBareNode1io, public CjvxVideoNode_genpcg
{
protected:

public:

	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxVideoNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxVideoNode();

	jvxErrorType activate() override;
	jvxErrorType deactivate() override;
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
