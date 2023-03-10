#ifndef __CJVXVINOPENGLVIEWER_H__
#define __CJVXVINOPENGLVIEWER_H__

#include "jvxVideoNodes/CjvxVideoNode.h"
#include "pcg_exports_node.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxViNOpenGLViewer: public CjvxVideoNode, public CjvxViNOpenGLViewer_genpcg
{
private:
protected:
public:

	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxViNOpenGLViewer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxViNOpenGLViewer();

};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
