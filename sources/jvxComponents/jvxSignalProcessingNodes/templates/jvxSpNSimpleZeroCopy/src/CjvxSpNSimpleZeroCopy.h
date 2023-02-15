
#include "jvxNodes/CjvxBareNode1io.h"
#include "pcg_exports_node.h"

class CjvxSpNSimpleZeroCopy : public CjvxBareNode1io, public genSimpleZeroCopy_node
{
	 
private:
public:

	JVX_CALLINGCONVENTION CjvxSpNSimpleZeroCopy(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxSpNSimpleZeroCopy();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	// ===============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	// ===============================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe,
		const char* filename,
		jvxInt32 lineno)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections)override;

	// ===============================================================

	JVX_PROPERTIES_FORWARD_C_CALLBACK_DECLARE(set_bypass_mode);
}; 