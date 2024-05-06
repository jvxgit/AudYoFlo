#ifndef __CJVXHRTFDISPENSER__H__
#define __CJVXHRTFDISPENSER__H__

#include "jvxNodes/CjvxNodeBase.h"
#include "ayf-hrtf.h"
#include "pcg_exports_node.h"

class CjvxHrtfDispenser : public CjvxNodeBase, public genHrtfDispenser_node,
	public IjvxPropertyExtender
{
public:
	JVX_CALLINGCONVENTION CjvxHrtfDispenser(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxHrtfDispenser() {};

	jvxErrorType select(IjvxObject* owner) override;
	jvxErrorType unselect() override;

	jvxErrorType activate() override;
	jvxErrorType deactivate() override;

	jvxErrorType supports_prop_extender_type(jvxPropertyExtenderType tp) override;
	jvxErrorType prop_extender_specialization(jvxHandle** prop_extender, jvxPropertyExtenderType tp) override;

	jvxErrorType put_configuration(
		jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno) override;
	jvxErrorType get_configuration(
		jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(select_sofa_file);
	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(select_hrtf_slot);

protected:
	ayfHrtfDispenser* theDispenser = nullptr;
};

#endif