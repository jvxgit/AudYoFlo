#ifndef __CJVXPAFFMPEG_H__
#define __CJVXPAFFMPEG_H__

#include "common/CjvxObject.h"

class CjvxPaFfmpeg : public IjvxInterfaceFactory, public IjvxPackage, public CjvxObject
{
public:
	CjvxPaFfmpeg(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override;
	jvxErrorType object_hidden_interface(IjvxObject** objRef) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION number_components(jvxSize* numOnReturn) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION request_entries_component(jvxSize idx,
		jvxApiString* description, jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION release_entries_component(jvxSize idx) override;

#include "codeFragments/simplify/jvxObjects_simplify.h"
// #include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
};

#endif

