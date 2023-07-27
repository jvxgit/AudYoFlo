#ifndef _CJVXCOMPONENTHOSTTOOLS_H__
#define _CJVXCOMPONENTHOSTTOOLS_H__

#include "jvxHosts/CjvxComponentHost.h"

class CjvxComponentHostTools : public CjvxComponentHost
{
public:
	CjvxComponentHostTools();
	~CjvxComponentHostTools();

	virtual jvxErrorType JVX_CALLINGCONVENTION _number_tools(const jvxComponentIdentification& tp, jvxSize* num);
	virtual jvxErrorType JVX_CALLINGCONVENTION _identification_tool(const jvxComponentIdentification&,
		jvxSize idx, jvxApiString* description,
		jvxApiString* descriptor, jvxBool* multipleInstances);
	virtual jvxErrorType JVX_CALLINGCONVENTION _reference_tool(const jvxComponentIdentification& tp, IjvxObject** theObject, jvxSize filter_id,
		const char* filter_descriptor, jvxBitField filter_stateMask = (jvxBitField)JVX_STATE_DONTCARE,
		IjvxReferenceSelector* decider = nullptr) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION _return_reference_tool(const jvxComponentIdentification&, IjvxObject* theObject) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION _instance_tool(jvxComponentType, IjvxObject** theObject, jvxSize filter_id, const char* filter_descriptor);
	virtual jvxErrorType JVX_CALLINGCONVENTION _return_instance_tool(jvxComponentType tp, IjvxObject* theObject, jvxSize filter_id, const char* filter_descriptor);

	// File CjvxHost-tpl.cpp
	template <class T> jvxErrorType t_reference_tool(
		std::vector<oneObjType<T>>& registeredTypes,
		const jvxComponentIdentification& tp,
		IjvxObject** theObject, jvxSize filter_id,
		const char* filter_descriptor,
		jvxBitField filter_stateMask,
		IjvxReferenceSelector* decider = nullptr);

	template <class T> jvxErrorType t_return_reference_tool(
		std::vector<oneObjType<T>>& registeredTypes,
		jvxComponentType tp,
		IjvxObject* theObject);
};

#endif
